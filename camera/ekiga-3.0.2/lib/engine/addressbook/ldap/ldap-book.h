
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
 *                         ldap-book.h  -  description
 *                         ------------------------------------------
 *   begin                : written in 2007 by Julien Puydt
 *   copyright            : (c) 2007 by Julien Puydt
 *   description          : declaration of a LDAP book
 *
 */

#ifndef __LDAP_BOOK_H__
#define __LDAP_BOOK_H__

#include <vector>
#include <libxml/tree.h>

#include "runtime.h"
#include "book-impl.h"
#include "form.h"

#include "ldap-contact.h"

namespace OPENLDAP
{

/**
 * @addtogroup contacts
 * @internal
 * @{
 */

  class Book:
    public Ekiga::BookImpl<Contact>
  {
  public:

    Book (Ekiga::ServiceCore &_core,
	  xmlNodePtr node);

    Book (Ekiga::ServiceCore &_core,
	  const std::string _name,
	  const std::string _hostname,
	  int _port,
	  const std::string _base,
	  const std::string _scope,
	  const std::string _call_attribute,
	  const std::string _password);

    ~Book ();

    const std::string get_name () const;

    bool populate_menu (Ekiga::MenuBuilder &builder);

    void set_search_filter (std::string search_string);

    const std::string get_status () const;

    const std::string get_type () const;

    void refresh ();

    void remove ();

    xmlNodePtr get_node ();

    sigc::signal<void> trigger_saving;

  private:

    void refresh_start ();
    void refresh_bound ();
    void refresh_result ();

    void edit ();
    void on_edit_form_submitted (Ekiga::Form &);

    Ekiga::ServiceCore &core;
    Ekiga::ContactCore *contact_core;
    xmlNodePtr node;

    std::string name;
    xmlNodePtr name_node;

    std::string hostname;
    xmlNodePtr hostname_node;

    int port;
    xmlNodePtr port_node;

    std::string base;
    xmlNodePtr base_node;

    std::string scope;
    xmlNodePtr scope_node;

    std::string call_attribute;
    xmlNodePtr call_attribute_node;

    std::string password;
    xmlNodePtr password_node;

    struct ldap *ldap_context;
    unsigned int patience;

    std::string status;
    std::string search_filter;

    Ekiga::Runtime & runtime;
  };

/**
 * @}
 */

};

#endif
