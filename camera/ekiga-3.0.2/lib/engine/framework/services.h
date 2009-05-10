
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
 *                         services.h  -  description
 *                         ------------------------------------------
 *   begin                : written in 2007 by Julien Puydt
 *   copyright            : (c) 2007 by Julien Puydt
 *   description          : declaration of a service object
 *
 */

#ifndef __SERVICES_H__
#define __SERVICES_H__

/* Ok, the principle is simple : we want to do "foo = new Foo (whatever)"
 * somewhere, and hook it somewhere for the whole program life ; so we give
 * the pointer to a ServiceCore object, which will end it cleanly.
 * Why not use smart pointers? Because it would be nice for debugging purposes
 * to be able to describe what exactly is enabled.
 */

#include <list>
#include <string>
#include <sigc++/sigc++.h>

namespace Ekiga
{

/**
 * @defgroup services Services
 * @{
 */

  struct Service
  {
    virtual ~Service () {}

    virtual const std::string get_name () const = 0;

    virtual const std::string get_description () const = 0;
  };


  class ServiceCore
  {
  public:

    ServiceCore () {}

    ~ServiceCore ();

    bool add (Service &service);

    Service *get (const std::string name);

    void dump (std::ostream &stream) const;

   sigc::signal<void, Service &> service_added;

  private:

    std::list<Service *> services;

  };

/**
 * @}
 */

};

#endif
