
/* Ekiga -- A VoIP and Video-Conferencing application
 * Copyright (C) 2000-2007 Damien Sandras
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * Ekiga is licensed under the GPL license and as a special exception,
 * you have permission to link or otherwise combine this program with the
 * programs OPAL, OpenH323 and PWLIB, and distribute the combination,
 * without applying the requirements of the GNU GPL to the OPAL, OpenH323
 * and PWLIB programs, as long as you do follow the requirements of the
 * GNU GPL for all the rest of the software thus combined.
 */


/*
 *                         ldap-book.cpp  -  description
 *                         ------------------------------------------
 *   begin                : written in 2007 by Julien Puydt
 *   copyright            : (c) 2007 by Julien Puydt
 *   description          : implementation of a LDAP book
 *
 */

#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <sys/time.h>
#include <string.h>

#include <ldap.h>
#include <glib.h>

#include "config.h"

#include "ldap-book.h"
#include "form-request-simple.h"
#include "robust-xml.h"

/* little helper function... can probably be made more complete */
static const std::string
fix_to_utf8 (const std::string str)
{
  gchar *utf8_str = NULL;
  std::string result;

  if (g_utf8_validate (str.c_str (), -1, NULL))
    utf8_str = g_strdup (str.c_str ());
  else
    utf8_str =  g_convert (str.c_str (), -1,
                           "UTF-8", "ISO-8859-1",
                           0, 0, 0);

  result = std::string (utf8_str);

  g_free (utf8_str);

  return result;
}

/* parses a message to construct a nice contact */
static OPENLDAP::Contact *
parse_result (Ekiga::ServiceCore &core,
	      struct ldap *context,
	      LDAPMessage *message)
{
  OPENLDAP::Contact *result = NULL;
  BerElement *ber = NULL;
  char *attribute = NULL;
  struct berval **values = NULL;
  std::string name;
  std::string call_address;

  attribute = ldap_first_attribute (context, message, &ber);

  while (attribute != NULL) {

    if (strncmp("givenName",attribute, 9) == 0) {

      values = ldap_get_values_len (context, message, attribute);
      if (values[0] != NULL) {
	name = std::string (values[0]->bv_val, values[0]->bv_len);
        ldap_value_free_len (values);
      }
    }
    if (strncmp ("telephoneNumber", attribute, 15) == 0) {

      values = ldap_get_values_len (context, message, attribute);
      if (values[0] != NULL) {
	call_address = std::string (values[0]->bv_val, values[0]->bv_len);
        ldap_value_free_len (values);
      }
    }
    ldap_memfree (attribute);
    attribute = ldap_next_attribute (context, message, ber);
  }
  ber_free (ber, 0);

  if (!name.empty () && !call_address.empty ()) {

    result = new OPENLDAP::Contact (core, fix_to_utf8 (name),
				    /* FIXME: next line is annoying */
				    std::string ("sip:") + call_address);
  }

  return result;
}


/* this allows us to do the refreshing in a thread: we put all needed
 * data in this structure, let everything happen elsewhere, then push back
 * into the main thread
 */
struct RefreshData
{

  RefreshData (Ekiga::ServiceCore &_core,
	       const std::string _name,
	       const std::string _hostname,
	       int _port,
	       const std::string _base,
	       const std::string _scope,
	       const std::string _call_attribute,
	       const std::string _password,
	       const std::string _search_string,
	       sigc::slot<void, std::vector<OPENLDAP::Contact *> > _publish_results):
    core(_core), name(_name), hostname(_hostname), port(_port),
    base(_base), scope(_scope), call_attribute(_call_attribute),
    password(_password), search_string(_search_string),
    error(false), publish_results (_publish_results)
  {
  }

  /* search data */
  Ekiga::ServiceCore &core;
  std::string name;
  std::string hostname;
  int port;
  std::string base;
  std::string scope;
  std::string call_attribute;
  std::string password;
  std::string search_string;

  /* result data */
  bool error;
  std::vector<OPENLDAP::Contact *> contacts;
  std::string error_message;

  /* callback */
  sigc::slot<void, std::vector<OPENLDAP::Contact *> > publish_results;
};

/* actual implementation */

OPENLDAP::Book::Book (Ekiga::ServiceCore &_core,
		      xmlNodePtr _node):
  core(_core), node(_node), name_node(NULL), hostname_node(NULL),
  port_node(NULL), base_node(NULL), scope_node(NULL), call_attribute_node(NULL),
  password_node(NULL), ldap_context(NULL), patience(0),
  runtime (*(dynamic_cast<Ekiga::Runtime *>(core.get ("runtime"))))
{
  xmlChar *xml_str;

  contact_core = dynamic_cast<Ekiga::ContactCore *>(core.get ("contact-core"));

  for (xmlNodePtr child = node->children ;
       child != NULL;
       child = child->next) {

    if (child->type == XML_ELEMENT_NODE
	&& child->name != NULL) {

      if (xmlStrEqual (BAD_CAST ("name"), child->name)) {

	xml_str = xmlNodeGetContent (child);
	name = (const char *)xml_str;
	name_node = child;
	xmlFree (xml_str);
      }

      if (xmlStrEqual (BAD_CAST ("hostname"), child->name)) {

	xml_str = xmlNodeGetContent (child);
	hostname = (const char *)xml_str;
	hostname_node = child;
	xmlFree (xml_str);
      }

      if (xmlStrEqual (BAD_CAST ("port"), child->name)) {

	xml_str = xmlNodeGetContent (child);
	port = std::atoi ((const char *)xml_str);
	port_node = child;
	xmlFree (xml_str);
      }

      if (xmlStrEqual (BAD_CAST ("base"), child->name)) {

	xml_str = xmlNodeGetContent (child);
	base = (const char *)xml_str;
	base_node = child;
	xmlFree (xml_str);
      }

      if (xmlStrEqual (BAD_CAST ("scope"), child->name)) {

	xml_str = xmlNodeGetContent (child);
	scope = (const char *)xml_str;
	scope_node = child;
	xmlFree (xml_str);
      }

      if (xmlStrEqual (BAD_CAST ("call_attribute"), child->name)) {

	xml_str = xmlNodeGetContent (child);
	call_attribute = (const char *)xml_str;
	call_attribute_node = child;
	xmlFree (xml_str);
      }

      if (xmlStrEqual (BAD_CAST ("password"), child->name)) {

	xml_str = xmlNodeGetContent (child);
	password = (const char *)xml_str;
	password_node = child;
	xmlFree (xml_str);
      }
    }
  }
}

OPENLDAP::Book::Book (Ekiga::ServiceCore &_core,
		      const std::string _name,
		      const std::string _hostname,
		      int _port,
		      const std::string _base,
		      const std::string _scope,
		      const std::string _call_attribute,
		      const std::string _password):
  core(_core), name(_name), name_node(NULL),
  hostname(_hostname), hostname_node(NULL), port(_port), port_node(NULL),
  base(_base), base_node(NULL), scope(_scope), scope_node(NULL),
  call_attribute(_call_attribute), call_attribute_node(NULL),
  password (_password), password_node(NULL), ldap_context(NULL), patience(0),
  runtime (*(dynamic_cast<Ekiga::Runtime *>(core.get ("runtime"))))
{
  contact_core = dynamic_cast<Ekiga::ContactCore *>(core.get ("contact-core"));

  node = xmlNewNode (NULL, BAD_CAST "server");

  name_node = xmlNewChild (node, NULL,
			   BAD_CAST "name",
			   BAD_CAST robust_xmlEscape (node->doc,
						      name).c_str ());

  hostname_node = xmlNewChild (node, NULL,
			       BAD_CAST "hostname",
			       BAD_CAST robust_xmlEscape (node->doc,
							  hostname).c_str ());

  { // Snark hates C++ -- and there isn't only a reason for it -- but many
    std::stringstream strm;
    std::string port_string;
    strm << port;
    strm >> port_string;
    port_node = xmlNewChild (node, NULL,
			     BAD_CAST "port",
			     BAD_CAST port_string.c_str ());
  }

  base_node = xmlNewChild (node, NULL,
			   BAD_CAST "base",
			   BAD_CAST robust_xmlEscape (node->doc,
						      base).c_str ());

  scope_node = xmlNewChild (node, NULL,
			    BAD_CAST "scope",
			    BAD_CAST robust_xmlEscape (node->doc,
						       scope).c_str ());

  call_attribute_node = xmlNewChild (node, NULL,
				     BAD_CAST "call_attribute",
				     BAD_CAST robust_xmlEscape (node->doc,
								call_attribute).c_str ());

  password_node = xmlNewChild (node, NULL,
			       BAD_CAST "password",
			       BAD_CAST robust_xmlEscape (node->doc,
							  password).c_str ());
}

OPENLDAP::Book::~Book ()
{
}

const std::string
OPENLDAP::Book::get_name () const
{
  return name;
}

bool
OPENLDAP::Book::populate_menu (Ekiga::MenuBuilder &builder)
{
  builder.add_action ("refresh", _("_Refresh"),
		      sigc::mem_fun (this, &OPENLDAP::Book::refresh));
  builder.add_separator ();
  builder.add_action ("remove", _("_Remove"),
		      sigc::mem_fun (this, &OPENLDAP::Book::remove));
  builder.add_action ("properties", _("_Properties"),
		      sigc::mem_fun (this, &OPENLDAP::Book::edit));

  return true;
}

void
OPENLDAP::Book::set_search_filter (std::string _search_filter)
{
  search_filter = _search_filter;
  refresh ();
}

const std::string
OPENLDAP::Book::get_status () const
{
  return status;
}

const std::string
OPENLDAP::Book::get_type () const
{
  return "remote";
}


void
OPENLDAP::Book::refresh ()
{
  /* we flush */
  iterator iter = begin ();
  while (iter != end ()) {

    remove_contact (*iter);
    iter = begin ();
  }

  if (ldap_context == NULL)
    refresh_start ();
}

void
OPENLDAP::Book::remove ()
{
  xmlUnlinkNode (node);
  xmlFreeNode (node);

  trigger_saving.emit ();
  removed.emit ();
}

xmlNodePtr
OPENLDAP::Book::get_node ()
{
  return node;
}

void
OPENLDAP::Book::refresh_start ()
{
  int msgid = -1;
  int result = LDAP_SUCCESS;
  int ldap_version = LDAP_VERSION3;
  char *ldap_uri = NULL;

  status = std::string (_("Refreshing"));
  updated.emit ();

  ldap_uri = g_strdup_printf ("ldap://%s:%d", hostname.c_str (), port);
  result = ldap_initialize (&ldap_context, ldap_uri);
  g_free (ldap_uri);
  if (result != LDAP_SUCCESS) {

    status = std::string (_("Could not initialize server"));
    updated.emit ();
    return;
  }

  /* the openldap code shows I don't have to check the result of this
   * (see for example tests/prog/slapd-search.c)
   */
  (void)ldap_set_option (ldap_context,
			 LDAP_OPT_PROTOCOL_VERSION, &ldap_version);

  if (password.empty ()) {

    result = ldap_sasl_bind (ldap_context, NULL,
			     LDAP_SASL_SIMPLE, NULL,
			     NULL, NULL,
			     &msgid);
  } else {

    struct berval passwd = { 0, NULL };
    passwd.bv_val = g_strdup (password.c_str ());
    passwd.bv_len = strlen (password.c_str ());

    result = ldap_sasl_bind (ldap_context, NULL,
			     LDAP_SASL_SIMPLE, &passwd,
			     NULL, NULL,
			     &msgid);

    g_free (passwd.bv_val);
  }

  if (result != LDAP_SUCCESS) {

    status = std::string (_("Could not contact server"));
    updated.emit ();

    ldap_unbind_ext (ldap_context, NULL, NULL);
    ldap_context = NULL;
    return;
  }

  status = std::string (_("Contacted server"));
  updated.emit ();

  patience = 3;
  runtime.run_in_main (sigc::mem_fun (this, &OPENLDAP::Book::refresh_bound), 0);
}

void
OPENLDAP::Book::refresh_bound ()
{
  int result = LDAP_SUCCESS;
  struct timeval timeout = { 1, 0}; /* block 1s */
  LDAPMessage *msg_entry = NULL;
  int msgid;
  std::vector<std::string> attributes_vector;
  char **attributes = NULL;
  int iscope = LDAP_SCOPE_SUBTREE;
  std::string filter;

  result = ldap_result (ldap_context, LDAP_RES_ANY, LDAP_MSG_ALL,
			&timeout, &msg_entry);

  if (result <= 0) {

    if (patience == 3) {
      patience--;
      runtime.run_in_main (sigc::mem_fun (this, &OPENLDAP::Book::refresh_bound), 12);
    } else if (patience == 2) {

      patience--;
      runtime.run_in_main (sigc::mem_fun (this, &OPENLDAP::Book::refresh_bound), 21);
    } else if (patience == 1) {

      patience--;
      runtime.run_in_main (sigc::mem_fun (this, &OPENLDAP::Book::refresh_bound), 30);
    } else { // patience == 0

      status = std::string (_("Could not connect to server"));
      updated.emit ();

      ldap_unbind_ext (ldap_context, NULL, NULL);
      ldap_context = NULL;
    }

    if (msg_entry != NULL)
      ldap_msgfree (msg_entry);

    return;
  }
  (void) ldap_msgfree (msg_entry);

  attributes_vector.push_back ("givenname");
  attributes_vector.push_back (call_attribute);

  if (scope == "sub")
    iscope = LDAP_SCOPE_SUBTREE;
  else
    iscope = LDAP_SCOPE_ONELEVEL;

  attributes = (char **)malloc ((attributes_vector.size ()+1)*sizeof(char *));
  for (unsigned int i = 0; i < attributes_vector.size (); i++)
    attributes[i] = strdup (attributes_vector[i].c_str ());
  attributes[attributes_vector.size ()] = NULL;// don't ask how I remembered...

  /* FIXME: http://bugzilla.gnome.org/show_bug.cgi?id=424459
   * here things should be done with givenname=..., but for this bug
   */
  if (!search_filter.empty ())
    filter = "(cn=*" + search_filter + "*)";
  else
    filter = "(cn=*)";

  msgid = ldap_search_ext (ldap_context,
			   base.c_str (),
			   iscope,
			   filter.c_str (),
			   attributes,
			   0, /* attrsonly */
			   NULL, NULL,
			   NULL, 0, &msgid);

  for (unsigned int i = 0; i < attributes_vector.size (); i++)
    free (attributes[i]);
  free (attributes);

  if (msgid == -1) {

    status = std::string (_("Could not search"));
    updated.emit ();

    ldap_unbind_ext (ldap_context, NULL, NULL);
    ldap_context = NULL;
    return;
  } else {

    status = std::string (_("Waiting for search results"));
    updated.emit ();
  }

  patience = 3;
  runtime.run_in_main (sigc::mem_fun (this, &OPENLDAP::Book::refresh_result), 0);

}

void
OPENLDAP::Book::refresh_result ()
{
  int result = LDAP_SUCCESS;
  int nbr = 0;
  struct timeval timeout = { 1, 0}; /* block 1s */
  LDAPMessage *msg_entry = NULL;
  LDAPMessage *msg_result = NULL;
  OPENLDAP::Contact *contact = NULL;
  gchar* c_status = NULL;

  result = ldap_result (ldap_context, LDAP_RES_ANY, LDAP_MSG_ALL,
			&timeout, &msg_entry);


  if (result <= 0) {

    if (patience == 3) {

      patience--;
      runtime.run_in_main (sigc::mem_fun (this, &OPENLDAP::Book::refresh_result), 12);
    } else if (patience == 2) {

      patience--;
      runtime.run_in_main (sigc::mem_fun (this, &OPENLDAP::Book::refresh_result), 21);
    } else if (patience == 1) {

      patience--;
      runtime.run_in_main (sigc::mem_fun (this, &OPENLDAP::Book::refresh_result), 30);
    } else { // patience == 0

      status = std::string (_("Could not search"));
      updated.emit ();

      ldap_unbind_ext (ldap_context, NULL, NULL);
      ldap_context = NULL;
    }

    if (msg_entry != NULL)
      ldap_msgfree (msg_entry);

    return;
  }

  msg_result = ldap_first_message (ldap_context, msg_entry);
  do {

    if (ldap_msgtype (msg_result) == LDAP_RES_SEARCH_ENTRY) {

      contact = parse_result (core, ldap_context, msg_result);
      if (contact != NULL) {
	add_contact (*contact);
        nbr++;
      }
    }
    msg_result = ldap_next_message (ldap_context, msg_result);
  } while (msg_result != NULL);

  c_status = g_strdup_printf (ngettext ("%d user found",
					"%d users found", nbr), nbr);
  status = c_status;
  g_free (c_status);

  updated.emit ();

  (void)ldap_msgfree (msg_entry);

  ldap_unbind_ext (ldap_context, NULL, NULL);
  ldap_context = NULL;
}

void
OPENLDAP::Book::edit ()
{
  Ekiga::FormRequestSimple request;

  request.title (_("Edit LDAP directory"));

  request.instructions (_("Please edit the following fields"));

  request.text ("name", _("_Name"), name);
  request.text ("hostname", _("_Hostname"), hostname);
  { // Snark hates C++ -- and there isn't only a reason for it -- but many
    std::stringstream strm;
    std::string port_string;
    strm << port;
    strm >> port_string;
    request.text ("port", _("_Port"), port_string);
  }
  request.text ("base", _("_Base DN"), base);

  {
    std::map<std::string, std::string> choices;

    choices["sub"] = _("Subtree");
    choices["single"] = _("Single Level");
    request.single_choice ("scope", _("_Scope"), scope, choices);
  }

  request.text ("call-attribute", _("Call _Attribute"), call_attribute);

  request.private_text ("password", _("_Password"), password);

  request.submitted.connect (sigc::mem_fun (this,
					    &OPENLDAP::Book::on_edit_form_submitted));

  if (!questions.handle_request (&request)) {

    // FIXME: better error reporting
#ifdef __GNUC__
    std::cout << "Unhandled form request in "
	      << __PRETTY_FUNCTION__ << std::endl;
#endif
  }
}

void
OPENLDAP::Book::on_edit_form_submitted (Ekiga::Form &result)
{
  try {

    std::string new_name = result.text ("name");
    std::string new_hostname = result.text ("hostname");
    std::string new_port_string = result.text ("port");
    std::string new_base = result.text ("base");
    std::string new_scope = result.single_choice ("scope");
    std::string new_call_attribute = result.text ("call-attribute");
    std::string new_password = result.private_text ("password");
    int new_port = std::atoi (new_port_string.c_str ());

    name = new_name;
    robust_xmlNodeSetContent (node, &name_node, "name", name);

    hostname = new_hostname;
    robust_xmlNodeSetContent (node, &hostname_node, "hostname", hostname);

    port = new_port;
    robust_xmlNodeSetContent (node, &port_node, "port", new_port_string);

    base = new_base;
    robust_xmlNodeSetContent (node, &base_node, "base", base);

    scope = new_scope;
    robust_xmlNodeSetContent (node, &scope_node, "scope", scope);

    call_attribute = new_call_attribute;
    robust_xmlNodeSetContent (node, &call_attribute_node,
			      "call_attribute", call_attribute);

    password = new_password;
    robust_xmlNodeSetContent (node, &password_node, "password", password);

    updated.emit ();
    trigger_saving.emit ();

  } catch (Ekiga::Form::not_found) {

    std::cerr << "Invalid result form" << std::endl; // FIXME: do better
  }
}
