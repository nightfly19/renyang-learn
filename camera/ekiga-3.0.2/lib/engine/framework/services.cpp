
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
 *                         services.cpp  -  description
 *                         ------------------------------------------
 *   begin                : written in 2007 by Julien Puydt
 *   copyright            : (c) 2007 by Julien Puydt
 *   description          : implementation of a service object
 *
 */

#include <iostream>

#include "services.h"

Ekiga::ServiceCore::~ServiceCore ()
{
  for (std::list<Service *>::iterator iter = services.begin ();
       iter != services.end ();
       iter++)
    delete *iter;
}

bool
Ekiga::ServiceCore::add (Service &service)
{
  bool result = false;

  if (get (service.get_name ()) == NULL) {

    services.push_front (&service);
    service_added (service);
    result = true;
  } else {

    result = false;
  }

  return result;
}

Ekiga::Service *
Ekiga::ServiceCore::get (const std::string name)
{
  Service *result = NULL;
  bool found = false;

  for (std::list<Service *>::iterator iter = services.begin ();
       iter != services.end () && !found;
       iter++)
    if (name == (*iter)->get_name ()) {

      result = *iter;
      found = true;
    }
  return result;
}

void
Ekiga::ServiceCore::dump (std::ostream &stream) const
{
  for (std::list<Service *>::const_reverse_iterator iter
	 = services.rbegin ();
       iter != services.rend ();
       iter++)
    stream << (*iter)->get_name ()
	   << ":"
	   << std::endl
	   << (*iter)->get_description ()
	   << std::endl;
}
