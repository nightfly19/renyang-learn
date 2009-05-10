
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
 *                         avahi-cluster.cpp  -  description
 *                         ------------------------------------------
 *   begin                : written in 2007 by Julien Puydt
 *   copyright            : (c) 2007 by Julien Puydt
 *   description          : implementation of the avahi cluster
 *
 */

#include "avahi-cluster.h"

Avahi::Cluster::Cluster (Ekiga::ServiceCore &_core): core(_core)
{
  Ekiga::PresenceCore* presence_core = NULL;

  heap = new Heap (core);

  add_heap (*heap);

  presence_core
    = dynamic_cast<Ekiga::PresenceCore*>(core.get ("presence-core"));
  /* don't check the dynamic cast: it has been checked already by avahi-main!*/
  presence_core->add_presence_fetcher (*heap);
}

Avahi::Cluster::~Cluster ()
{
}

void
Avahi::Cluster::visit_heaps (sigc::slot<bool, Heap &> visitor)
{
  (void)visitor (*heap);
}

bool
Avahi::Cluster::populate_menu (Ekiga::MenuBuilder &/*builder*/)
{
  /* FIXME */
  return false;
}
