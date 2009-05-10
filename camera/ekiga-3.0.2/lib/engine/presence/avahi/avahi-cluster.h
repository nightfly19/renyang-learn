
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
 *                         avahi-cluster.h  -  description
 *                         ------------------------------------------
 *   begin                : written in 2007 by Julien Puydt
 *   copyright            : (c) 2007 by Julien Puydt
 *   description          : declaration of the avahi cluster
 *
 */

#ifndef __AVAHI_CLUSTER_H__
#define __AVAHI_CLUSTER_H__

#include "cluster-impl.h"
#include "avahi-heap.h"

namespace Avahi
{

/**
 * @addtogroup presence
 * @internal
 * @{
 */

  class Cluster:
    public Ekiga::ClusterImpl<Heap>
  {
  public:

    Cluster (Ekiga::ServiceCore &_core);

    ~Cluster ();

    void visit_heaps (sigc::slot<bool, Heap &> visitor);

    bool populate_menu (Ekiga::MenuBuilder &builder);

  private:

    Ekiga::ServiceCore &core;
    Heap *heap;
  };

/**
 * @}
 */

};

#endif
