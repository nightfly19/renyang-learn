
/* Ekiga -- A VoIP and Video-Conferencing application
 * Copyright (C) 2000-2008 Damien Sandras
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
 *                         avahi-publisher-main.cpp  -  description
 *                         ------------------------------------------
 *   begin                : written in 2008 by Damien Sandras
 *   copyright            : (c) 2008 Damien Sandras
 *   description          : code to hook the avahi publisher into the main program
 *
 */

#include "avahi-publisher-main.h"
#include "avahi-publisher.h"

#include <iostream>

class Service: public Ekiga::Service
{
public:

  Service (Ekiga::PresenceCore& core_,
	   Avahi::PresencePublisher* publisher_):
    core(core_), publisher(publisher_)
  {
    core.add_presence_publisher (*publisher);
  }

  ~Service ()
  {
    core.remove_presence_publisher (*publisher);
    delete publisher;
  }

  const std::string get_name () const
  { return "avahi-presence-publisher"; }

  const std::string get_description () const
  { return "\tObject bringing in Avahi presence publishing"; }

private:
  Ekiga::PresenceCore& core;
  Avahi::PresencePublisher* publisher;
};

bool
avahi_publisher_init (Ekiga::ServiceCore &core,
                      int* /*argc*/,
                      char* */*argv*/[])
{
  bool result = false;
  Ekiga::PresenceCore* presence_core = NULL;
  Ekiga::CallCore* call_core = NULL;
  Ekiga::PersonalDetails* details = NULL;

  presence_core
    = dynamic_cast<Ekiga::PresenceCore*>(core.get ("presence-core"));
  call_core
    = dynamic_cast<Ekiga::CallCore*>(core.get ("call-core"));
  details
    = dynamic_cast<Ekiga::PersonalDetails*>(core.get ("personal-details"));

  if (presence_core != NULL
      && call_core != NULL
      && details != NULL) {

    Avahi::PresencePublisher* publisher = NULL;
    Service* service = NULL;
    publisher = new Avahi::PresencePublisher (core, *details, *call_core);
    service = new Service (*presence_core, publisher);
    core.add (*service);
    result = true;
  }

  return result;
}
