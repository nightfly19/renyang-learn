
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
 *                         echo-main.cpp  -  description
 *                         ------------------------------------------
 *   begin                : written in 2008 by Julien Puydt
 *   copyright            : (c) 2008 by Julien Puydt
 *   description          : code to hook the echo chat to the main program
 *
 */

#include "echo-main.h"
#include "chat-core.h"
#include "echo-dialect.h"

namespace Echo
{
  class Service: public Ekiga::Service
  {
  public:

    Service (Dialect *dialect_): dialect(dialect_)
    {}

    ~Service ()
    { delete dialect; }

    const std::string get_name () const
    { return "echo-dialect"; }

    const std::string get_description () const
    { return "\tProvides an echo chat for testing purposes"; }

  private:

    Dialect *dialect;
  };
};

bool
echo_init (Ekiga::ServiceCore &core,
	   int */*argc*/,
	   char **/*argv*/[])
{
  bool result = false;
  Ekiga::ChatCore *chat_core = NULL;
  Echo::Service *service = NULL;

  chat_core
    = dynamic_cast<Ekiga::ChatCore*>(core.get ("chat-core"));

  if (chat_core != NULL) {

    Echo::Dialect* dialect = new Echo::Dialect;
    service = new Echo::Service (dialect);
    core.add (*service);
    chat_core->add_dialect (*dialect);
    result = true;
  }

  return result;
}
