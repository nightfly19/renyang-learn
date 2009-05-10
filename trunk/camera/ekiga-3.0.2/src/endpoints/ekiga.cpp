
/* Ekiga -- A VoIP and Video-Conferencing application
 * Copyright (C) 2000-2006 Damien Sandras
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
 *                         gnomemeeting.cpp  -  description
 *                         --------------------------------
 *   begin                : Sat Dec 23 2000
 *   copyright            : (C) 2000-2006 by Damien Sandras
 *   description          : This file contains the main class
 *
 */


#include "config.h"

#include "ekiga.h"
#include "callbacks.h"
#include "preferences.h"
#include "chat-window.h"
#include "assistant.h"
#include "statusicon.h"
#include "accounts.h"
#include "main.h"
#include "misc.h"

#ifdef HAVE_DBUS
#include "dbus.h"
#endif

#include "gmdialog.h"
#include "gmstockicons.h"
#include "gmconf.h"

#include "engine.h"

#define new PNEW


GnomeMeeting *GnomeMeeting::GM = NULL;

/* The main GnomeMeeting Class  */
GnomeMeeting::GnomeMeeting ()
  : PProcess("", "", MAJOR_VERSION, MINOR_VERSION, BUILD_TYPE, BUILD_NUMBER)

{
  GM = this;
  
  call_number = 0;

  service_core = NULL;
  assistant_window = NULL;
  prefs_window = NULL;
}


GnomeMeeting::~GnomeMeeting()
{ 
  Exit ();
}


void
GnomeMeeting::Exit ()
{
  PWaitAndSignal m(ep_var_mutex);

  if (prefs_window)
    gtk_widget_destroy (prefs_window);
  prefs_window = NULL;
  
  if (main_window)
    gtk_widget_destroy (main_window);
  main_window = NULL;
  
  if (assistant_window)
    gtk_widget_destroy (assistant_window);
  assistant_window = NULL;
  
  if (accounts_window)
    gtk_widget_destroy (accounts_window);
  accounts_window = NULL;

  if (statusicon)
    g_object_unref (statusicon);
  statusicon = NULL;
  
#ifdef HAVE_DBUS
  if (dbus_component)
    g_object_unref (dbus_component);
  dbus_component = NULL;
#endif

  if (service_core)
    delete service_core;
  service_core = NULL;
}


bool
GnomeMeeting::DetectInterfaces ()
{
  //TODO
  PString config_interface;
  PString iface_noip;
  PString ip;
  PIPSocket::InterfaceTable ifaces;

  PINDEX i = 0;
  PINDEX pos = 0;
  bool res = FALSE;

  gchar *conf_interface = NULL;

  PWaitAndSignal m(iface_access_mutex);

  /* Detect the valid interfaces */
  res = PIPSocket::GetInterfaceTable (ifaces);
  interfaces.RemoveAll ();

  conf_interface = gm_conf_get_string (PROTOCOLS_KEY "interface");
  config_interface = conf_interface;
  g_free (conf_interface);

  pos = config_interface.Find("[");
  if (pos != P_MAX_INDEX)
    iface_noip = config_interface.Left (pos).Trim ();
  while (i < ifaces.GetSize ()) {

    ip = " [" + ifaces [i].GetAddress ().AsString () + "]";

    if (ifaces [i].GetName () + ip == config_interface || ifaces [i].GetName () == iface_noip) 
      break;

    i++;
  }

  pos = i;
  i = 0;

  while (i < ifaces.GetSize ()) {

    ip = " [" + ifaces [i].GetAddress ().AsString () + "]";

    if (i != pos) {

      if (ifaces [i].GetAddress ().AsString () != "0.0.0.0") {

        if (ifaces [i].GetName ().Find ("ppp") != P_MAX_INDEX) {

          if (i > 0) {
            interfaces += interfaces [0];
            interfaces [0] = ifaces [i].GetName () + ip;     
          }
          else
            interfaces += ifaces [i].GetName () + ip;
        }
        else if (!ifaces [i].GetAddress ().IsLoopback ())
          interfaces += ifaces [i].GetName () + ip;
      }
    }
    else {

      if (!interfaces [0].IsEmpty ())
        interfaces += interfaces [0];
      interfaces [0] = ifaces [pos].GetName () + ip;
    }

    i++;
  }

  return res;
}


Ekiga::ServiceCore *
GnomeMeeting::GetServiceCore ()
{
  PWaitAndSignal m(ep_var_mutex);
  
  return service_core;
}


GnomeMeeting *
GnomeMeeting::Process ()
{
  return GM;
}


GtkWidget *
GnomeMeeting::GetMainWindow ()
{
  return main_window;
}


GtkWidget *
GnomeMeeting::GetPrefsWindow (bool create)
{
  if (!prefs_window && create)
    prefs_window = gm_prefs_window_new (service_core);
  return prefs_window;
}


GtkWidget *
GnomeMeeting::GetAssistantWindow (bool create)
{
  if (!assistant_window && create)
    assistant_window = ekiga_assistant_new (service_core);

  return assistant_window;
}


GtkWidget *
GnomeMeeting::GetAccountsWindow ()
{
  return accounts_window;
}


StatusIcon *
GnomeMeeting::GetStatusicon ()
{
  return statusicon;
}

void GnomeMeeting::Main ()
{
}


void GnomeMeeting::BuildGUI ()
{
  /* Init the stock icons */
  gnomemeeting_stock_icons_init ();
  
  /* Build the GUI */
  gtk_window_set_default_icon_name (GM_ICON_LOGO);
  accounts_window = gm_accounts_window_new (*service_core);

  statusicon = statusicon_new (*service_core);
  main_window = gm_main_window_new (*service_core);

  /* GM is started */
  PTRACE (1, "Ekiga version "
	  << MAJOR_VERSION << "." << MINOR_VERSION << "." << BUILD_NUMBER);
  PTRACE (1, "OPAL version " << OPAL_VERSION);
  PTRACE (1, "PTLIB version " << PTLIB_VERSION);
#ifdef HAVE_GNOME
  PTRACE (1, "GNOME support enabled");
#else
  PTRACE (1, "GNOME support disabled");
#endif
#if defined HAVE_XV || defined HAVE_DX
  PTRACE (1, "Accelerated rendering support enabled");
#else
  PTRACE (1, "Accelerated rendering support disabled");
#endif
#ifdef HAVE_DBUS
  PTRACE (1, "DBUS support enabled");
#else
  PTRACE (1, "DBUS support disabled");
#endif
#ifdef HAVE_GCONF
  PTRACE (1, "GConf support enabled");
#else
  PTRACE (1, "GConf support disabled");
#endif
#ifdef HAVE_ESD
  PTRACE (1, "ESound support enabled");
#else
  PTRACE (1, "ESound support disabled");
#endif
}

PStringArray 
GnomeMeeting::GetInterfaces ()
{
  PWaitAndSignal m(iface_access_mutex);

  return interfaces;
}


void
GnomeMeeting::InitEngine ()
{
  PWaitAndSignal m(ep_var_mutex);

  Ekiga::Runtime *runtime = new Ekiga::GlibRuntime;
  engine_init (1, NULL, runtime, service_core);

  if (!service_core)
    std::cout << "engine couldn't init!" << std::endl;
}
