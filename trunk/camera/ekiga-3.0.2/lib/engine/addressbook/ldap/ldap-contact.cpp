
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
 *                         ldap-contact.cpp  -  description
 *                         ------------------------------------------
 *   begin                : written in 2007 by Julien Puydt
 *   copyright            : (c) 2007 by Julien Puydt
 *   description          : implementation of a LDAP contact
 *
 */

#include <iostream>

#include "ldap-contact.h"


OPENLDAP::Contact::Contact (Ekiga::ServiceCore &_core,
			    const std::string _name,
			    const std::string _uri)
  : core(_core), name(_name), uri(_uri)
{
}


OPENLDAP::Contact::~Contact ()
{
}

const std::string
OPENLDAP::Contact::get_name () const
{
  return name;
}

const std::set<std::string>
OPENLDAP::Contact::get_groups () const
{
  return std::set<std::string>();
}

bool
OPENLDAP::Contact::populate_menu (Ekiga::MenuBuilder &builder)
{
  Ekiga::ContactCore *contact_core
    = dynamic_cast<Ekiga::ContactCore *>(core.get ("contact-core"));
  /* FIXME: add here the specific actions we want to allow
   * (before or after the uri-specific actions)
   */

  return contact_core->populate_contact_menu (*this, uri, builder);
}

bool
OPENLDAP::Contact::is_found (const std::string /*test*/) const
{
  return false;
}
