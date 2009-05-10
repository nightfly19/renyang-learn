
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
 *                         ldap-source.h  -  description
 *                         ------------------------------------------
 *   begin                : written in 2007 by Julien Puydt
 *   copyright            : (c) 2007 by Julien Puydt
 *   description          : interface of a LDAP source
 *
 */

#ifndef __LDAP_SOURCE_H__
#define __LDAP_SOURCE_H__

#include "services.h"
#include "form.h"
#include "contact-core.h"
#include "source-impl.h"

#include "ldap-book.h"

namespace OPENLDAP
{

/**
 * @addtogroup contacts
 * @internal
 * @{
 */

  class Source:
    public Ekiga::SourceImpl<Book>,
    public Ekiga::Service
  {
  public:

    Source (Ekiga::ServiceCore &_core);

    ~Source ();

    bool populate_menu (Ekiga::MenuBuilder &builder);

    const std::string get_name () const
    { return "ldap-source"; }

    const std::string get_description () const
    { return "\tComponent bringing in LDAP addressbooks"; }

  private:

    Ekiga::ServiceCore &core;
    xmlDocPtr doc;

    void add (xmlNodePtr node);

    void add (const std::string name,
	      const std::string hostname,
	      int port,
	      const std::string base,
	      const std::string scope,
	      const std::string call_attribute,
	      const std::string password);

    void common_add (Book &book);

    void on_remove_me (Book *book);

    void save ();

    void new_book ();
    
    void new_ekiga_net_book ();

    void on_new_book_form_submitted (Ekiga::Form &result);
  };

/**
 * @}
 */

};

#endif
