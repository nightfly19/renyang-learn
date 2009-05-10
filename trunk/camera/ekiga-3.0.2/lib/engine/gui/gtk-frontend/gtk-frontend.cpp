
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
 *                         gtk-frontend.cpp  -  description
 *                         ------------------------------------------
 *   begin                : written in 2007 by Julien Puydt
 *   copyright            : (c) 2007 by Julien Puydt
 *   description          : code to hook a gtk+ user interface to
 *                          the main program
 *
 */

#include <iostream>

#include <gtk/gtk.h>

#include "config.h"

#include "gtk-frontend.h"

#include "chat-core.h"
#include "contact-core.h"
#include "presence-core.h"
#include "addressbook-window.h"
#include "chat-window.h"
#include "roster-view-gtk.h"
#include "history-source.h"
#include "book-view-gtk.h"
#include "call-history-view-gtk.h"

#include "gmwindow.h"


GtkFrontend::GtkFrontend (Ekiga::ServiceCore &core)
{
  sigc::connection conn;

  Ekiga::PresenceCore *presence_core = NULL;
  Ekiga::ContactCore *contact_core = NULL;
  Ekiga::ChatCore *chat_core = NULL;
  History::Source *history_source = NULL;
  History::Book *history_book = NULL;

  contact_core = dynamic_cast<Ekiga::ContactCore *>(core.get ("contact-core"));
  presence_core = dynamic_cast<Ekiga::PresenceCore *>(core.get ("presence-core"));
  chat_core = dynamic_cast<Ekiga::ChatCore *>(core.get ("chat-core"));
  history_source = dynamic_cast<History::Source*>(core.get ("call-history-store"));
  history_book = history_source->get_book ();

  roster_view = roster_view_gtk_new (*presence_core);
  call_history_view = call_history_view_gtk_new (*history_book);
  addressbook_window =
    addressbook_window_new_with_key (*contact_core, "/apps/ekiga/general/user_interface/addressbook_window");
  chat_window =
    chat_window_new (*chat_core,
		     "/apps/ekiga/general/user_interface/chat_window");
}


GtkFrontend::~GtkFrontend ()
{
  gtk_widget_destroy (addressbook_window);
  gtk_widget_destroy (chat_window);
}


const std::string GtkFrontend::get_name () const
{
  return "gtk-frontend";
}


const std::string GtkFrontend::get_description () const
{
  return "\tGtk+ frontend support";
}


const GtkWidget *GtkFrontend::get_roster_view () const
{
  return roster_view;
}


const GtkWidget *GtkFrontend::get_addressbook_window () const
{
  return addressbook_window;
}


const GtkWidget *GtkFrontend::get_chat_window () const
{
  return chat_window;
}


const GtkWidget *
GtkFrontend::get_call_history_view () const
{
  return call_history_view;
}


bool
gtk_frontend_init (Ekiga::ServiceCore &core,
		   int * /*argc*/,
		   char ** /*argv*/[])
{
  GtkFrontend *gtk_frontend = new GtkFrontend (core);
  core.add (*gtk_frontend);

  return true;
}
