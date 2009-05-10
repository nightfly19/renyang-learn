
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
 *                         engine.cpp  -  description
 *                         ------------------------------------------
 *   begin                : written in 2007 by Damien Sandras
 *   copyright            : (c) 2007 by Damien Sandras
 *   description          : Vroom.
 *
 */

#include "config.h"

#include "engine.h"

#include "services.h"

#include "presence-core.h"
#include "account-core.h"
#include "contact-core.h"
#include "call-core.h"
#include "chat-core.h"
#include "videooutput-core.h"
#include "videoinput-core.h"
#include "audioinput-core.h"
#include "audiooutput-core.h"
#include "hal-core.h"
#include "history-main.h"
#include "local-roster-main.h"
#include "local-roster-bridge.h"
#include "gtk-core-main.h"
#include "gtk-frontend.h"
#include "gmconf-personal-details-main.h"

#ifndef WIN32
#include "videooutput-main-x.h"
#endif

#ifdef HAVE_DX
#include "videooutput-main-dx.h"
#endif

#include "videoinput-main-mlogo.h"
#include "audioinput-main-null.h"
#include "audiooutput-main-null.h"

#include "videoinput-main-ptlib.h"
#include "audioinput-main-ptlib.h"
#include "audiooutput-main-ptlib.h"

#ifdef HAVE_DBUS
#include "hal-main-dbus.h"
#endif

#include "opal-main.h"

#ifdef HAVE_AVAHI
#include "avahi-main.h"
#include "avahi-publisher-main.h"
#endif

#ifdef HAVE_EDS
#include "evolution-main.h"
#endif

#ifdef HAVE_LDAP
#include "ldap-main.h"
#endif


void
engine_init (int argc,
             char *argv [],
             Ekiga::Runtime *runtime,
             Ekiga::ServiceCore * &core)
{
  core = new Ekiga::ServiceCore; 

  /* VideoInputCore depends on VideoOutputCore and must this              *
   * be constructed thereafter                                      */

  Ekiga::AccountCore *account_core = new Ekiga::AccountCore;
  Ekiga::PresenceCore *presence_core = NULL;
  Ekiga::ContactCore *contact_core = new Ekiga::ContactCore;
  Ekiga::CallCore *call_core = new Ekiga::CallCore;
  Ekiga::ChatCore *chat_core = new Ekiga::ChatCore;
  Ekiga::VideoOutputCore *videooutput_core = new Ekiga::VideoOutputCore;
  Ekiga::VideoInputCore *videoinput_core = new Ekiga::VideoInputCore(*runtime, *videooutput_core);
  Ekiga::AudioOutputCore *audiooutput_core = new Ekiga::AudioOutputCore(*runtime);  
  Ekiga::AudioInputCore *audioinput_core = new Ekiga::AudioInputCore(*runtime, *audiooutput_core);
  Ekiga::HalCore *hal_core = new Ekiga::HalCore;


  /* The last item in the following list will be destroyed first.   *
   * - VideoInputCore must be destroyed before VideoOutputCore since its  *
   *   PreviewManager may call functions of VideoOutputCore.            *
   * - The runtime should be destroyed last since other core        *
   *   components may still call runtime functions until destroyed  *
   *   (e.g. VideoOutputCore).                                          */
   
  core->add (*runtime);
  core->add (*account_core);
  core->add (*contact_core);
  core->add (*chat_core);
  core->add (*videooutput_core);
  core->add (*videoinput_core);
  core->add (*audiooutput_core);
  core->add (*audioinput_core);
  core->add (*hal_core);
  core->add (*call_core);

  if (!gmconf_personal_details_init (*core, &argc, &argv)) {
    delete core;
    return;
  }

  presence_core = new Ekiga::PresenceCore (*core);
  core->add (*presence_core);

#ifndef WIN32
  if (!videooutput_x_init (*core, &argc, &argv)) {
    delete core;
    return;
  }
#endif

#ifdef HAVE_DX
  if (!videooutput_dx_init (*core, &argc, &argv)) {
    delete core;
    return;
  }
#endif

  if (!videoinput_mlogo_init (*core, &argc, &argv)) {
    delete core;
    return;
  }

  if (!audioinput_null_init (*core, &argc, &argv)) {
    delete core;
    return;
  }

  if (!audiooutput_null_init (*core, &argc, &argv)) {
    delete core;
    return;
  }

  if (!videoinput_ptlib_init (*core, &argc, &argv)) {
    delete core;
    return;
  }

  if (!audioinput_ptlib_init (*core, &argc, &argv)) {
    delete core;
    return;
  }

  if (!audiooutput_ptlib_init (*core, &argc, &argv)) {
    delete core;
    return;
  }

#ifdef HAVE_DBUS
  if (!hal_dbus_init (*core, &argc, &argv)) {
    delete core;
    return;
  }
#endif

  if (!opal_init (*core, &argc, &argv)) {
    delete core;
    return;
  }

#ifdef HAVE_AVAHI
  if (!avahi_init (*core, &argc, &argv)) {
    delete core;
    return;
  }
  
  if (!avahi_publisher_init (*core, &argc, &argv)) {
    delete core;
    return;
  }
#endif

#ifdef HAVE_EDS
  if (!evolution_init (*core, &argc, &argv)) {
    delete core;
    return;
  }
#endif

#ifdef HAVE_LDAP
  if (!ldap_init (*core, &argc, &argv)) {
    delete core;
    return;
  }
#endif

  if (!history_init (*core, &argc, &argv)) {

    delete core;
    return;
  }

  if (!gtk_core_init (*core, &argc, &argv)) {
    delete core;
    return;
  }

  if (!gtk_frontend_init (*core, &argc, &argv)) {
    delete core;
    return;
  }

  if (!local_roster_init (*core, &argc, &argv)) {
    delete core;
    return;
  }

  if (!local_roster_bridge_init (*core, &argc, &argv)) {
    delete core;
    return;
  }

  videooutput_core->setup_conf_bridge();
  videoinput_core->setup_conf_bridge();
  audiooutput_core->setup_conf_bridge();
  audioinput_core->setup_conf_bridge();

  sigc::connection conn;
  conn = hal_core->videoinput_device_added.connect (sigc::mem_fun (videoinput_core, &Ekiga::VideoInputCore::add_device));
  conn = hal_core->videoinput_device_removed.connect (sigc::mem_fun (videoinput_core, &Ekiga::VideoInputCore::remove_device));
  conn = hal_core->audiooutput_device_added.connect (sigc::mem_fun (audiooutput_core, &Ekiga::AudioOutputCore::add_device));
  conn = hal_core->audiooutput_device_removed.connect (sigc::mem_fun (audiooutput_core, &Ekiga::AudioOutputCore::remove_device));
  conn = hal_core->audioinput_device_added.connect (sigc::mem_fun (audioinput_core, &Ekiga::AudioInputCore::add_device));
  conn = hal_core->audioinput_device_removed.connect (sigc::mem_fun (audioinput_core, &Ekiga::AudioInputCore::remove_device));
  // std::vector<sigc::connection> connections;
  //connections.push_back (conn);
}
