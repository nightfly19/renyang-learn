
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
 *                         ldap-source.cpp  -  description
 *                         ------------------------------------------
 *   begin                : written in 2007 by Julien Puydt
 *   copyright            : (c) 2007 by Julien Puydt
 *   description          : implementation of a LDAP source
 *
 */

#include <cstdlib>
#include <iostream>
#include <string.h>

#include "config.h"

#include "gmconf.h"
#include "form-request-simple.h"

#include "ldap-source.h"

#define KEY "/apps/" PACKAGE_NAME "/contacts/ldap_servers"

OPENLDAP::Source::Source (Ekiga::ServiceCore &_core): core(_core), doc(NULL)
{
  xmlNodePtr root;
  gchar *c_raw = gm_conf_get_string (KEY);

  if (c_raw != NULL && strcmp (c_raw, "")) {

    const std::string raw = c_raw;

    doc = xmlRecoverMemory (raw.c_str (), raw.length ());
    if (doc == NULL)
      doc = xmlNewDoc (BAD_CAST "1.0");

    root = xmlDocGetRootElement (doc);

    if (root == NULL) {

      root = xmlNewNode (NULL, BAD_CAST "list");
      xmlDocSetRootElement (doc, root);
    }

    for (xmlNodePtr child = root->children ;
	 child != NULL ;
	 child = child->next)
      if (child->type == XML_ELEMENT_NODE
	  && child->name != NULL
	  && xmlStrEqual (BAD_CAST "server", child->name))
	add (child);

    g_free (c_raw);
  } else {

    doc = xmlNewDoc (BAD_CAST "1.0");
    root = xmlNewNode (NULL, BAD_CAST "list");
    xmlDocSetRootElement (doc, root);

    new_ekiga_net_book ();
  }
}

OPENLDAP::Source::~Source ()
{
  if (doc)
    xmlFreeDoc (doc);
}

void
OPENLDAP::Source::add (xmlNodePtr node)
{
  Book *book = NULL;

  book = new Book (core, node);

  common_add (*book);
}

void
OPENLDAP::Source::add (const std::string name,
		       const std::string hostname,
		       int port,
		       const std::string base,
		       const std::string scope,
		       const std::string call_attribute,
		       const std::string password)
{
  Book *book = NULL;
  xmlNodePtr root;

  root = xmlDocGetRootElement (doc);
  book = new Book (core, name, hostname, port, base,
		   scope, call_attribute, password);

  xmlAddChild (root, book->get_node ());

  common_add (*book);
}

void
OPENLDAP::Source::common_add (Book &book)
{
  book.trigger_saving.connect (sigc::mem_fun (this,
					      &OPENLDAP::Source::save));
  add_book (book);
}

bool
OPENLDAP::Source::populate_menu (Ekiga::MenuBuilder &builder)
{
  builder.add_action ("add", _("Add an LDAP Address Book"),
		      sigc::mem_fun (this,
				     &OPENLDAP::Source::new_book));
  builder.add_action ("add", _("Add the Ekiga.net Directory"),
		      sigc::mem_fun (this,
				     &OPENLDAP::Source::new_ekiga_net_book));
  return true;
}

void
OPENLDAP::Source::new_book ()
{
  Ekiga::FormRequestSimple request;

  request.title (_("Create LDAP directory"));

  request.instructions (_("Please edit the following fields"));

  request.text ("name", _("_Name:"), "");
  request.text ("hostname", _("_Hostname:"), "");
  request.text ("port", _("_Port:"), "389");
  request.text ("base", _("_Base DN:"), "dc=net");
  {
    std::map<std::string, std::string> choices;

    choices["sub"] = _("_Subtree");
    choices["single"] = _("Single _Level");
    request.single_choice ("scope", _("_Scope"), "sub", choices);
  }

  request.text ("call-attribute", _("Call _Attribute"), "telephoneNumber");
  request.private_text ("password", _("Password"), "");

  request.submitted.connect (sigc::mem_fun (this,
					    &OPENLDAP::Source::on_new_book_form_submitted));

  if (!questions.handle_request (&request)) {

    // FIXME: better error reporting
#ifdef __GNUC__
    std::cout << "Unhandled form request in "
	      << __PRETTY_FUNCTION__ << std::endl;
#endif
  }
}

void
OPENLDAP::Source::new_ekiga_net_book ()
{
  add (_("Ekiga.net Directory"),
       "ekiga.net", 389,
       "dc=ekiga,dc=net",
       "sub",
       "telephoneNumber", 
       "");
  save ();
}

void
OPENLDAP::Source::on_new_book_form_submitted (Ekiga::Form &result)
{
  try {

    std::string name = result.text ("name");
    std::string hostname = result.text ("hostname");
    std::string port_string = result.text ("port");
    std::string base = result.text ("base");
    std::string scope = result.single_choice ("scope");
    std::string call_attribute = result.text ("call-attribute");
    std::string password = result.private_text ("password");
    int port = atoi (port_string.c_str ());

    add (name, hostname, port, base, scope, call_attribute, password);
    save ();

  } catch (Ekiga::Form::not_found) {

    std::cerr << "Invalid result form" << std::endl; // FIXME: do better
  }
}

void
OPENLDAP::Source::save ()
{
  xmlChar *buffer = NULL;
  int size = 0;

  xmlDocDumpMemory (doc, &buffer, &size);

  gm_conf_set_string (KEY, (const char *)buffer);

  xmlFree (buffer);
}
