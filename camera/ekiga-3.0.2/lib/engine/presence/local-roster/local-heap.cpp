
/*
 * Ekiga -- A VoIP and Video-Conferencing application
 * Copyright (C) 2000-2007 Damien Sandras

 * This program is free software; you can  redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version. This program is distributed in the hope
 * that it will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * Ekiga is licensed under the GPL license and as a special exception, you
 * have permission to link or otherwise combine this program with the
 * programs OPAL, OpenH323 and PWLIB, and distribute the combination, without
 * applying the requirements of the GNU GPL to the OPAL, OpenH323 and PWLIB
 * programs, as long as you do follow the requirements of the GNU GPL for all
 * the rest of the software thus combined.
 */


/*
 *                         local-heap.cpp  -  description
 *                         ------------------------------------------
 *   begin                : written in 2007 by Julien Puydt
 *   copyright            : (c) 2007 by Julien Puydt
 *   description          : implementation of the heap of the local roster
 *
 */

#include <iostream>
#include <set>

#include "config.h"

#include "gmconf.h"
#include "form-request-simple.h"

#include "local-heap.h"

#define KEY "/apps/" PACKAGE_NAME "/contacts/roster"


/*
 * Public API
 */
Local::Heap::Heap (Ekiga::ServiceCore &_core): core (_core), doc (NULL)
{
  xmlNodePtr root;

  presence_core = dynamic_cast<Ekiga::PresenceCore*>(core.get ("presence-core"));

  gchar *c_raw = gm_conf_get_string (KEY);

  // Build the XML document representing the contacts list from the configuration
  if (c_raw != NULL) {

    const std::string raw = c_raw;
    doc = xmlRecoverMemory (raw.c_str (), raw.length ());

    root = xmlDocGetRootElement (doc);
    if (root == NULL) {

      root = xmlNewDocNode (doc, NULL, BAD_CAST "list", NULL);
      xmlDocSetRootElement (doc, root);
    }

    for (xmlNodePtr child = root->children; child != NULL; child = child->next)
      if (child->type == XML_ELEMENT_NODE
	  && child->name != NULL
	  && xmlStrEqual (BAD_CAST ("entry"), child->name))
	add (child);

    g_free (c_raw);

    // Or create a new XML document
  }
  else {

    doc = xmlNewDoc (BAD_CAST "1.0");
    root = xmlNewDocNode (doc, NULL, BAD_CAST "list", NULL);
    xmlDocSetRootElement (doc, root);

    {
      // add 500 and 501 at ekiga.net in this case!
      std::set<std::string> groups;

      groups.insert (_("Services"));
      add (_("Echo test"), "sip:500@ekiga.net", groups);
      add (_("Conference room"), "sip:501@ekiga.net", groups);
    }
  }
}


Local::Heap::~Heap ()
{
  if (doc != NULL)
    xmlFreeDoc (doc);
}


const std::string
Local::Heap::get_name () const
{
  return _("Local roster");
}


bool
Local::Heap::populate_menu (Ekiga::MenuBuilder &builder)
{
  builder.add_action ("new", _("New contact"),
		      sigc::bind (sigc::mem_fun (this, &Local::Heap::new_presentity), "", ""));
  return true;
}


bool
Local::Heap::populate_menu_for_group (const std::string name,
				      Ekiga::MenuBuilder& builder)
{
  builder.add_action ("rename_group", _("Rename"),
		      sigc::bind (sigc::mem_fun (this, &Local::Heap::on_rename_group), name));
  return true;
}


bool
Local::Heap::has_presentity_with_uri (const std::string uri) const
{
  bool result = false;

  for (const_iterator iter = begin ();
       iter != end () && result != true;
       iter++)
    result = (iter->get_uri () == uri);

  return result;
}


const std::set<std::string>
Local::Heap::existing_groups () const
{
  std::set<std::string> result;

  for (const_iterator iter = begin ();
       iter != end ();
       iter++) {

    std::set<std::string> groups = iter->get_groups ();
    result.insert (groups.begin (), groups.end ());
  }

  return result;
}


void
Local::Heap::new_presentity (const std::string name,
			     const std::string uri)
{
  if (!has_presentity_with_uri (uri)) {

    Ekiga::FormRequestSimple request;
    std::set<std::string> groups = existing_groups ();

    request.title (_("Add to local roster"));
    request.instructions (_("Please fill in this form to add a new contact "
			    "to ekiga's internal roster"));
    request.text ("name", _("Name:"), name);
    if (presence_core->is_supported_uri (uri)) {

      request.hidden ("good-uri", "yes");
      request.hidden ("uri", uri);
    } else {

      request.hidden ("good-uri", "no");
      if ( !uri.empty ())
	request.text ("uri", _("Address:"), uri);
      else
	request.text ("uri", _("Address:"), "sip:"); // let's put a default
    }

    request.editable_set ("groups",
			  _("Put contact in groups:"),
			  std::set<std::string>(), groups);

    request.submitted.connect (sigc::mem_fun (this, &Local::Heap::new_presentity_form_submitted));

    if (!questions.handle_request (&request)) {

    // FIXME: better error reporting
#ifdef __GNUC__
      std::cout << "Unhandled form request in "
		<< __PRETTY_FUNCTION__ << std::endl;
#endif
    }
  }
}


/*
 * Private API
 */
void
Local::Heap::add (xmlNodePtr node)
{
  Presentity *presentity = NULL;

  presentity = new Presentity (core, node);

  common_add (*presentity);
}


void
Local::Heap::add (const std::string name,
		  const std::string uri,
		  const std::set<std::string> groups)
{
  Presentity *presentity = NULL;
  xmlNodePtr root = NULL;

  root = xmlDocGetRootElement (doc);
  presentity = new Presentity (core, name, uri, groups);

  xmlAddChild (root, presentity->get_node ());

  save ();
  common_add (*presentity);
}


void
Local::Heap::common_add (Presentity &presentity)
{
  // Add the presentity to this Heap
  add_presentity (presentity);

  // Fetch presence
  presence_core->fetch_presence (presentity.get_uri ());

  // Connect the Local::Presentity signals.
  presentity.trigger_saving.connect (sigc::mem_fun (this, &Local::Heap::save));
}


void
Local::Heap::save () const
{
  xmlChar *buffer = NULL;
  int size = 0;

  xmlDocDumpMemory (doc, &buffer, &size);

  gm_conf_set_string (KEY, (const char *)buffer);

  xmlFree (buffer);
}


void
Local::Heap::new_presentity_form_submitted (Ekiga::Form &result)
{
  try {

    const std::string name = result.text ("name");
    const std::string good_uri = result.hidden ("good-uri");
    std::string uri;
    const std::set<std::string> groups = result.editable_set ("groups");

    if (good_uri == "yes")
      uri = result.hidden ("uri");
    else
      uri = result.text ("uri");

    if (presence_core->is_supported_uri (uri)
	&& !has_presentity_with_uri (uri)) {

      add (name, uri, groups);
      save ();
    } else {

      Ekiga::FormRequestSimple request;

      result.visit (request);
      if (!presence_core->is_supported_uri (uri))
	request.error (_("You supplied an unsupported address"));
      else
	request.error (_("You already have a contact with this address!"));
      request.submitted.connect (sigc::mem_fun (this, &Local::Heap::new_presentity_form_submitted));
      if (!questions.handle_request (&request)) {

	// FIXME: better error handling
#ifdef __GNUC__
	std::cout << "Unhandled form request in "
		  << __PRETTY_FUNCTION__ << std::endl;
#endif
      }
    }
  } catch (Ekiga::Form::not_found) {

#ifdef __GNUC__
    std::cerr << "Invalid form submitted to "
	      << __PRETTY_FUNCTION__ << std::endl;
#endif
  }
}

void
Local::Heap::on_rename_group (std::string name)
{
  Ekiga::FormRequestSimple request;

  request.title (_("Rename group"));
  request.instructions (_("Please edit this group name"));
  request.text ("name", _("Name:"), name);

  request.submitted.connect (sigc::bind<0>(sigc::mem_fun (this, &Local::Heap::rename_group_form_submitted), name));

  if (!questions.handle_request (&request)) {

    // FIXME: better error reporting
#ifdef __GNUC__
    std::cout << "Unhandled form request in "
	      << __PRETTY_FUNCTION__ << std::endl;
#endif
  }
}

void
Local::Heap::rename_group_form_submitted (std::string old_name,
					  Ekiga::Form& result)
{
  try {
    const std::string new_name = result.text ("name");

    if ( !new_name.empty () && new_name != old_name) {

      for (iterator iter = begin ();
	   iter != end ();
	   ++iter) {

	iter->rename_group (old_name, new_name);
      }
    }
  } catch (Ekiga::Form::not_found) {

#ifdef __GNUC__
    std::cerr << "Invalid form submitted to "
	      << __PRETTY_FUNCTION__ << std::endl;
#endif
  }
}
