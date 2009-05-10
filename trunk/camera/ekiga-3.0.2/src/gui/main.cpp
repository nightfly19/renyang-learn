
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
 *                         main_window.cpp  -  description
 *                         -------------------------------
 *   begin                : Mon Mar 26 2001
 *   copyright            : (C) 2000-2006 by Damien Sandras
 *   description          : This file contains all the functions needed to
 *                          build the main window.
 */

#include "revision.h"

#include "config.h"

#include "main.h"

#include "ekiga.h"
#include "conf.h"
#include "misc.h"
#include "callbacks.h"
#include "statusicon.h"
#include "dialpad.h"
#include "statusmenu.h"

#include "gmdialog.h"
#include "gmentrydialog.h"
#include "gmstatusbar.h"
#include "gmconnectbutton.h"
#include "gmstockicons.h"
#include "gmconf.h"
#include "gmwindow.h"
#include "gmmenuaddon.h"
#include "gmlevelmeter.h"
#include "gmpowermeter.h"
#include "gmconfwidgets.h"
#include "trigger.h"
#include "menu-builder-gtk.h"

#include "platform/gm-platform.h"

#include <gdk/gdkkeysyms.h>

#ifdef HAVE_DBUS
#include "components/dbus.h"
#endif

#ifndef WIN32
#include <signal.h>
#include <gdk/gdkx.h>
#else
#include "platform/winpaths.h"
#include <gdk/gdkwin32.h>
#endif

#ifdef HAVE_GNOME
#undef _
#undef N_
#include <gnome.h>
#endif

#ifdef HAVE_NOTIFY
#include <libnotify/notify.h>
#endif

#if defined(P_FREEBSD) || defined (P_MACOSX)
#include <libintl.h>
#endif

#include <libxml/parser.h>

#include "videoinput-core.h"
#include "audioinput-core.h"
#include "audiooutput-core.h"

#include "call-core.h"
#include "account.h"
#include "gtk-frontend.h"
#include "services.h"
#include "form-dialog-gtk.h"

#include "../devices/videooutput.h"

enum CallingState {Standby, Calling, Connected, Called};

enum DeviceType { AudioInput, AudioOutput, VideoInput} ;
struct deviceStruct {
  char name[256];
  DeviceType deviceType;
};

/* Declarations */
struct _GmMainWindow
{
  _GmMainWindow (Ekiga::ServiceCore & _core) : core (_core) { }

  GtkWidget *input_signal;
  GtkWidget *output_signal;
  GtkObject *adj_input_volume;
  GtkObject *adj_output_volume;
  GtkWidget *audio_input_volume_frame;
  GtkWidget *audio_output_volume_frame;
  GtkWidget *audio_settings_window;
  GtkWidget *audio_settings_button;

  GtkObject *adj_whiteness;
  GtkObject *adj_brightness;
  GtkObject *adj_colour;
  GtkObject *adj_contrast;
  GtkWidget *video_settings_button;
  GtkWidget *video_settings_frame;
  GtkWidget *video_settings_window;

  GtkListStore *completion;

  GtkAccelGroup *accel;

  GtkWidget *main_menu;
  GtkWidget *main_toolbar;

  GtkWidget *window_vbox;

  GtkWidget *status_label_ebox;
  GtkWidget *info_text;
  GtkWidget *info_label;

  GtkWidget *statusbar;
  GtkWidget *statusbar_ebox;
  GtkWidget *qualitymeter;
  GtkWidget *entry;
  GtkWidget *main_notebook;
  GtkWidget *hpaned;
  GtkWidget *roster;
  GtkWidget *main_video_image;
  GtkWidget *local_video_image;
  GtkWidget *remote_video_image;
  GtkWidget *video_frame;
  GtkWidget *preview_button;
  GtkWidget *connect_button;
  GtkWidget *hold_button;
  GtkWidget *transfer_call_popup;
  GtkWidget *status_option_menu;
  Ekiga::Presentity* presentity;

#ifndef WIN32
  GdkGC* video_widget_gc;
#endif

  unsigned int calling_state;
  bool audio_transmission_active;
  bool audio_reception_active;
  bool video_transmission_active;
  bool video_reception_active;
  std::string transmitted_video_codec;
  std::string transmitted_audio_codec;
  std::string received_video_codec;
  std::string received_audio_codec;
  unsigned int timeout_id;
  unsigned int levelmeter_timeout_id;
  Ekiga::Call *current_call;
  Ekiga::ServiceCore & core;
  std::vector<sigc::connection> connections;
  std::list<std::string> accounts;
};

typedef struct _GmMainWindow GmMainWindow;


#define GM_MAIN_WINDOW(x) (GmMainWindow *) (x)

/* channel types */
enum {
  CHANNEL_FIRST,
  CHANNEL_AUDIO,
  CHANNEL_VIDEO,
  CHANNEL_LAST
};


/* GUI Functions */


/* DESCRIPTION  : / 
 * BEHAVIOR     : Frees a GmMainWindow and its content.
 * PRE          : A non-NULL pointer to a GmMainWindow.
 */
static void gm_mw_destroy (gpointer);


/* DESCRIPTION  : / 
 * BEHAVIOR     : Returns a pointer to the private GmMainWindow
 * 		  used by the main book GMObject.
 * PRE          : The given GtkWidget pointer must be the main window GMObject. 
 */
static GmMainWindow *gm_mw_get_mw (GtkWidget *);


/* DESCRIPTION  :  /
 * BEHAVIOR     :  Create the URI toolbar of the main window.
 *                 The toolbar is created in its initial state, with
 *                 required items being unsensitive.
 * PRE          :  The main window GMObject.
 */
static GtkWidget *gm_mw_init_uri_toolbar (GtkWidget *);


/* DESCRIPTION  :  /
 * BEHAVIOR     :  Create the status menu toolbar of the main window.
 * PRE          :  The main window GMObject.
 */
static GtkWidget *gm_mw_init_status_toolbar (GtkWidget *);


/* DESCRIPTION  :  /
 * BEHAVIOR     :  Create the menu of the main window.
 *                 The menu is created in its initial state, with
 *                 required items being unsensitive.
 * PRE          :  The main window GMObject. The statusbar must have been
 * 		   created.
 */
static void gm_mw_init_menu (GtkWidget *);


/* description  : /
 * behavior     : Builds the contacts list part of the main window.
 * pre          : The given GtkWidget pointer must be the main window GMObject. 
 */
static void gm_mw_init_contacts_list (GtkWidget *);


/* description  : /
 * behavior     : builds the dialpad part of the main window.
 * pre          : the given GtkWidget pointer must be the main window GMObject. 
 */
static void gm_mw_init_dialpad (GtkWidget *);

/* DESCRIPTION  : /
 * BEHAVIOR     : Builds the video settings popup of the main window.
 * PRE          : The given GtkWidget pointer must be the main window GMObject. 
 */
static GtkWidget *gm_mw_video_settings_window_new (GtkWidget *);


/* DESCRIPTION  : /
 * BEHAVIOR     : Builds the audio settings popup for the main window.
 * PRE          : The given GtkWidget pointer must be the main window GMObject. 
 */
static GtkWidget *gm_mw_audio_settings_window_new (GtkWidget *);


/* DESCRIPTION  : /
 * BEHAVIOR     : Builds the call part of the main window.
 * PRE          : The given GtkWidget pointer must be the main window GMObject. 
 */
static void gm_mw_init_call (GtkWidget *);


/* DESCRIPTION  : /
 * BEHAVIOR     : Builds the call history part of the main window.
 * PRE          : The given GtkWidget pointer must be the main window GMObject. 
 */
static void gm_mw_init_history (GtkWidget *);


/* DESCRIPTION  :  /
 * BEHAVIOR     :  enables/disables the zoom related menuitems according
 *                 to zoom factor
 * PRE          :  The main window GMObject.
 */
static void gm_mw_zooms_menu_update_sensitivity (GtkWidget *,
			      			 unsigned int);
static void gm_main_window_toggle_fullscreen (Ekiga::VideoOutputFSToggle toggle,
                                              GtkWidget   *main_window);

static void gm_main_window_show_call_panel (GtkWidget *self);

static void gm_main_window_hide_call_panel (GtkWidget *self);

void gm_main_window_clear_signal_levels (GtkWidget *main_window);

void gm_main_window_selected_presentity_build_menu (GtkWidget *main_window);

void gm_main_window_incoming_call_dialog_show (GtkWidget *main_window,
                                               Ekiga::Call & call);

#ifdef HAVE_NOTIFY
void gm_main_window_incoming_call_notify (GtkWidget *main_window, 
                                          Ekiga::Call & call);
#endif


/* Callbacks */
/* DESCRIPTION  :  This callback is called when the user selects a presentity
 *                 in the roster
 * BEHAVIOR     :  Populates the Chat->Contact submenu
 * PRE          :  /
 */
static void on_presentity_selected (GtkWidget* view,
				    Ekiga::Presentity* presentity,
				    gpointer data);

/* DESCRIPTION  :  This callback is called when the chat window alerts about
 *                 unread messages
 * BEHAVIOR     :  Plays a sound (if enabled)
 * PRE          :  /
 */
static void on_chat_unread_alert (GtkWidget*,
				  gpointer);

/* DESCRIPTION  :  This callback is called when the control panel 
 *                 section key changes.
 * BEHAVIOR     :  Sets the right page, and also sets 
 *                 the good value for the radio menu. 
 * PRE          :  /
 */
static void panel_section_changed_nt (gpointer id,
                                      GmConfEntry *entry,
                                      gpointer data);


/* DESCRIPTION  :  This callback is called when the call panel 
 *                 section key changes.
 * BEHAVIOR     :  Show it or hide it, resize the window appropriately.
 * PRE          :  /
 */
static void show_call_panel_changed_nt (G_GNUC_UNUSED gpointer id, 
                                        GmConfEntry *entry, 
                                        gpointer data);


/** Pull a trigger from a Ekiga::Service
 *
 * @param data is a pointer to the Ekiga::Trigger
 */
static void pull_trigger_cb (GtkWidget * /*widget*/,
                             gpointer data);


/** Show the widget passed as parameter
 *
 * @param data is a pointer to the widget to show
 */
static void  show_widget_cb (GtkWidget * /*widget*/,
                             gpointer data);


/* DESCRIPTION  :  /
 * BEHAVIOR     :  Set the current active call on hold.
 * PRE          :  /
 */
static void hold_current_call_cb (GtkWidget *,
				  gpointer);


/* DESCRIPTION  :  /
 * BEHAVIOR     :  Set the current active call audio channel on pause or not
 * PRE          :  a pointer to the main window 
 */
static void toggle_audio_stream_pause_cb (GtkWidget *, 
                                          gpointer);


/* DESCRIPTION  :  /
 * BEHAVIOR     :  Set the current active call video channel on pause or not
 * PRE          :  a pointer to the main window 
 */
static void toggle_video_stream_pause_cb (GtkWidget *, 
                                          gpointer);


/* DESCRIPTION  :  /
 * BEHAVIOR     :  Creates a dialog to transfer the current call and transfer
 * 		   it if required.
 * PRE          :  The parent window.
 */
static void transfer_current_call_cb (GtkWidget *,
				      gpointer);


/* DESCRIPTION  :  This callback is called when a video window is shown.
 * BEHAVIOR     :  Set the WM HINTS to stay-on-top if the config key is set
 *                 to true.
 * PRE          :  /
 */
static void video_window_shown_cb (GtkWidget *,
                                   gpointer);

/* DESCRIPTION  :  This callback is called when the user changes the
 *                 audio settings sliders in the main notebook.
 * BEHAVIOR     :  Update the volume of the choosen mixers. If the update
 *                 fails, the sliders are put back to 0.
 * PRE          :  The main window GMObject.
 */
static void audio_volume_changed_cb (GtkAdjustment *, 
				     gpointer);


/* DESCRIPTION  :  This callback is called when the user changes one of the 
 *                 video settings sliders in the main notebook.
 * BEHAVIOR     :  Updates the value in real time, if it fails, reset 
 * 		   all sliders to 0.
 * PRE          :  gpointer is a valid pointer to the main window GmObject.
 */
static void video_settings_changed_cb (GtkAdjustment *, 
				       gpointer);


/* DESCRIPTION  :  This callback is called when the user changes the
 *                 page in the main notebook.
 * BEHAVIOR     :  Update the config key accordingly.
 * PRE          :  A valid pointer to the main window GmObject.
 */
static void panel_section_changed_cb (GtkNotebook *, 
                                      GtkNotebookPage *,
                                      gint, 
                                      gpointer);


/* DESCRIPTION  :  This callback is called when the user 
 *                 clicks on the dialpad button.
 * BEHAVIOR     :  Generates a dialpad event.
 * PRE          :  A valid pointer to the main window GMObject.
 */
static void dialpad_button_clicked_cb (EkigaDialpad  *dialpad,
				       const gchar *button_text,
				       GtkWidget   *main_window);


/* DESCRIPTION  :  This callback is called when the user tries to close
 *                 the application using the window manager.
 * BEHAVIOR     :  Calls the real callback if the notification icon is 
 *                 not shown else hide GM.
 * PRE          :  A valid pointer to the main window GMObject.
 */
static gint window_closed_cb (GtkWidget *, 
			      GdkEvent *, 
			      gpointer);


/* DESCRIPTION  :  This callback is called when the user tries to close
 *                 the main window using the FILE-menu
 * BEHAVIOUR    :  Directly calls window_closed_cb (i.e. it's just a wrapper)
 * PRE          :  ---
 */

static void window_closed_from_menu_cb (GtkWidget *,
                                       gpointer);


/* DESCRIPTION  :  This callback is called when the user changes the zoom
 *                 factor in the menu, and chooses to zoom in.
 * BEHAVIOR     :  zoom *= 2.
 * PRE          :  The GConf key to update with the new zoom.
 */
static void zoom_in_changed_cb (GtkWidget *,
				gpointer);


/* DESCRIPTION  :  This callback is called when the user changes the zoom
 *                 factor in the menu, and chooses to zoom in.
 * BEHAVIOR     :  zoom /= 2.
 * PRE          :  The GConf key to update with the new zoom.
 */
static void zoom_out_changed_cb (GtkWidget *,
				 gpointer);


/* DESCRIPTION  :  This callback is called when the user changes the zoom
 *                 factor in the menu, and chooses to zoom in.
 * BEHAVIOR     :  zoom = 1.
 * PRE          :  The GConf key to update with the new zoom.
 */
static void zoom_normal_changed_cb (GtkWidget *,
				    gpointer);

void 
display_changed_cb (GtkWidget *widget,
		    gpointer data);

/* DESCRIPTION  :  This callback is called when the user toggles fullscreen
 *                 factor in the popup menu.
 * BEHAVIOR     :  Toggles the fullscreen configuration key. 
 * PRE          :  / 
 */
static void fullscreen_changed_cb (GtkWidget *,
				   gpointer);

/* DESCRIPTION  :  This callback is called when the user changes the URL
 * 		   in the URL bar.
 * BEHAVIOR     :  It udpates the tooltip with the new URL
 *                 and the completion cache.
 * PRE          :  A valid pointer to the main window GMObject. 
 */
static void url_changed_cb (GtkEditable *, 
			    gpointer);

/* DESCRIPTION  :  This callback is called when the user presses a
 *                 button in the toolbar. 
 *                 (See menu_toggle_changed)
 * BEHAVIOR     :  Updates the config cache.
 * PRE          :  data is the key.
 */
static void toolbar_toggle_button_changed_cb (GtkWidget *, 
					      gpointer);


/* DESCRIPTION  :  This callback is called when the status bar is clicked.
 * BEHAVIOR     :  Clear all info message, not normal messages.
 * PRE          :  The main window GMObject.
 */
static gboolean statusbar_clicked_cb (GtkWidget *,
				      GdkEventButton *,
				      gpointer);


/* DESCRIPTION  :  This callback is called if main window is focussed
 * BEHAVIOR     :  currently only: unset urgency hint
 * PRE          : /
 */
static gboolean main_window_focus_event_cb (GtkWidget *,
					    GdkEventFocus *,
					    gpointer);

static void audio_volume_window_shown_cb (GtkWidget *widget,
	                                  gpointer data);

static void audio_volume_window_hidden_cb (GtkWidget *widget,
	                                   gpointer data);



/* 
 * Engine Callbacks 
 */
static void on_ready_cb (gpointer self)
{
  gm_main_window_set_busy (GTK_WIDGET (self), false);
}


static void on_registration_event (const Ekiga::Account & account,
                                   Ekiga::AccountCore::RegistrationState state,
                                   std::string /*info*/,
                                   gpointer self)
{
  GmMainWindow *mw = NULL;

  gchar *msg = NULL;
  std::string aor = account.get_aor ();

  g_return_if_fail (GTK_WIDGET (self) != NULL);
  mw = gm_mw_get_mw (GTK_WIDGET (self));

  switch (state) {
  case Ekiga::AccountCore::Registered:
    /* Translators: Is displayed once an account "%s" is registered. */
    msg = g_strdup_printf (_("Registered %s"), aor.c_str ()); 
    mw->accounts.push_back (account.get_host ());
    break;

  case Ekiga::AccountCore::Unregistered:
    /* Translators: Is displayed once an account "%s" is unregistered. */
    msg = g_strdup_printf (_("Unregistered %s"), aor.c_str ());
    mw->accounts.remove (account.get_host ());
    break;

  case Ekiga::AccountCore::UnregistrationFailed:
    msg = g_strdup_printf (_("Could not unregister %s"), aor.c_str ());
    break;

  case Ekiga::AccountCore::RegistrationFailed:
    msg = g_strdup_printf (_("Could not register %s"), aor.c_str ());
    break;

  case Ekiga::AccountCore::Processing:
  default:
    break;
  }

  if (msg)
    gm_main_window_flash_message (GTK_WIDGET (self), "%s", msg);

  g_free (msg);
}


static void on_setup_call_cb (Ekiga::CallManager & /*manager*/,
                              Ekiga::Call & call,
                              gpointer self)
{
  GmMainWindow *mw = gm_mw_get_mw (GTK_WIDGET (self));
  Ekiga::AudioOutputCore *audiooutput_core = dynamic_cast<Ekiga::AudioOutputCore *> (mw->core.get ("audiooutput-core"));

  if (!call.is_outgoing ()) {
    gm_main_window_update_calling_state (GTK_WIDGET (self), Called);
    audiooutput_core->start_play_event("incoming_call_sound", 4000, 256);
#ifdef HAVE_NOTIFY
    gm_main_window_incoming_call_notify (GTK_WIDGET (self), call);
#else
    gm_main_window_incoming_call_dialog_show (GTK_WIDGET (self), call);
#endif
    mw->current_call = &call;
  }
  else {
    gm_main_window_update_calling_state (GTK_WIDGET (self), Calling);
    if (!call.get_remote_uri ().empty ())
      gm_main_window_set_call_url (GTK_WIDGET (self), call.get_remote_uri ().c_str());
    mw->current_call = &call;
  }
}


static void on_ringing_call_cb (Ekiga::CallManager & /*manager*/,
                                Ekiga::Call & call,
                                gpointer self)
{
  GmMainWindow *mw = gm_mw_get_mw (GTK_WIDGET (self));
  Ekiga::AudioOutputCore *audiooutput_core = dynamic_cast<Ekiga::AudioOutputCore *> (mw->core.get ("audiooutput-core"));

  if (call.is_outgoing ()) {
    audiooutput_core->start_play_event("ring_tone_sound", 3000, 256);
  }
}



static gboolean on_stats_refresh_cb (gpointer self) 
{
  gchar *msg = NULL;

  GmMainWindow *mw = gm_mw_get_mw (GTK_WIDGET (self));
  
  if (mw->calling_state == Connected && mw->current_call) {

    Ekiga::VideoOutputStats videooutput_stats;
    Ekiga::VideoOutputCore *videooutput_core = dynamic_cast<Ekiga::VideoOutputCore *> (mw->core.get ("videooutput-core"));
    videooutput_core->get_videooutput_stats(videooutput_stats);
  
    msg = g_strdup_printf (_("A:%.1f/%.1f   V:%.1f/%.1f   FPS:%d/%d"), 
                           mw->current_call->get_transmitted_audio_bandwidth (),
                           mw->current_call->get_received_audio_bandwidth (),
                           mw->current_call->get_transmitted_video_bandwidth (),
                           mw->current_call->get_received_video_bandwidth (),
                           videooutput_stats.tx_fps,
                           videooutput_stats.rx_fps);
    gm_main_window_flash_message (GTK_WIDGET (self), msg);
    gm_main_window_set_call_duration (GTK_WIDGET (self), mw->current_call->get_duration ().c_str ());
    g_free (msg);

    unsigned int jitter = mw->current_call->get_jitter_size ();
    double lost = mw->current_call->get_lost_packets ();
    double late = mw->current_call->get_late_packets ();
    double out_of_order = mw->current_call->get_out_of_order_packets ();

    gm_main_window_update_stats (GTK_WIDGET (self), lost, late, out_of_order, jitter, 
                                 0, 0, 0, 0,
                                 videooutput_stats.rx_width,
                                 videooutput_stats.rx_height,
                                 videooutput_stats.tx_width,
                                 videooutput_stats.tx_height);
  }
  return true;
}

static gboolean on_signal_level_refresh_cb (gpointer self) 
{
  GmMainWindow *mw = gm_mw_get_mw (GTK_WIDGET (self));

  Ekiga::AudioInputCore *audioinput_core = dynamic_cast<Ekiga::AudioInputCore *> (mw->core.get ("audioinput-core"));
  Ekiga::AudioOutputCore *audiooutput_core = dynamic_cast<Ekiga::AudioOutputCore *> (mw->core.get ("audiooutput-core"));

  gtk_levelmeter_set_level (GTK_LEVELMETER (mw->output_signal), audiooutput_core->get_average_level());
  gtk_levelmeter_set_level (GTK_LEVELMETER (mw->input_signal), audioinput_core->get_average_level());
  return true;
}

static void on_established_call_cb (Ekiga::CallManager & /*manager*/,
                                    Ekiga::Call & call,
                                    gpointer self)
{
  GmMainWindow *mw = gm_mw_get_mw (GTK_WIDGET (self));
  gchar* info = NULL;

  info = g_strdup_printf (_("Connected with %s"),
			  call.get_remote_party_name ().c_str ());

  if (!call.get_remote_uri ().empty ())
    gm_main_window_set_call_url (GTK_WIDGET (self), call.get_remote_uri ().c_str());
  if (gm_conf_get_bool (VIDEO_DISPLAY_KEY "stay_on_top"))
    gm_main_window_set_stay_on_top (GTK_WIDGET (self), TRUE);
  gm_main_window_set_status (GTK_WIDGET (self), info);
  gm_main_window_flash_message (GTK_WIDGET (self), "%s", info);
  if (!gm_conf_get_bool (USER_INTERFACE_KEY "main_window/show_call_panel"))
    gm_main_window_show_call_panel (GTK_WIDGET (self));
  gm_main_window_update_calling_state (GTK_WIDGET (self), Connected);

  mw->current_call = &call;

  mw->timeout_id = g_timeout_add (1000, on_stats_refresh_cb, self);

  Ekiga::AudioOutputCore *audiooutput_core = dynamic_cast<Ekiga::AudioOutputCore *> (mw->core.get ("audiooutput-core"));

  audiooutput_core->stop_play_event("incoming_call_sound");
  audiooutput_core->stop_play_event("ring_tone_sound");

  g_free (info);
}


static void on_cleared_call_cb (Ekiga::CallManager & /*manager*/,
                                Ekiga::Call & call,
                                std::string reason, 
                                gpointer self)
{
  GmMainWindow *mw = gm_mw_get_mw (GTK_WIDGET (self));

  if (gm_conf_get_bool (VIDEO_DISPLAY_KEY "stay_on_top"))
    gm_main_window_set_stay_on_top (GTK_WIDGET (self), FALSE);
  gm_main_window_update_calling_state (GTK_WIDGET (self), Standby);
  gm_main_window_set_status (GTK_WIDGET (self), _("Standby"));
  gm_main_window_set_call_duration (GTK_WIDGET (self), NULL);
  gm_main_window_set_call_info (GTK_WIDGET (self), NULL, NULL, NULL, NULL);
  if (!gm_conf_get_bool (USER_INTERFACE_KEY "main_window/show_call_panel"))
    gm_main_window_hide_call_panel (GTK_WIDGET (self));
  gm_main_window_clear_stats (GTK_WIDGET (self));
  gm_main_window_update_logo_have_window (GTK_WIDGET (self));
  gm_main_window_push_message (GTK_WIDGET (self), "%s", reason.c_str ());

  if (mw->current_call && mw->current_call->get_id () == call.get_id ()) {

    mw->current_call = NULL;
    g_source_remove (mw->timeout_id);
    mw->timeout_id = -1;
  }
  Ekiga::AudioOutputCore *audiooutput_core = dynamic_cast<Ekiga::AudioOutputCore *> (mw->core.get ("audiooutput-core"));

  audiooutput_core->stop_play_event("incoming_call_sound");
  audiooutput_core->stop_play_event("ring_tone_sound");

  gm_main_window_clear_signal_levels(GTK_WIDGET (self));
}


static void on_cleared_incoming_call_cb (std::string /*reason*/,
                                         gpointer self)
{
  GmMainWindow *mw = NULL;

  GtkWidget *main_window = NULL;
  
  main_window = GnomeMeeting::Process ()->GetMainWindow ();
  mw = gm_mw_get_mw (main_window);
  Ekiga::AudioOutputCore *audiooutput_core = dynamic_cast<Ekiga::AudioOutputCore *> (mw->core.get ("audiooutput-core"));
  audiooutput_core->stop_play_event("incoming_call_sound");
  audiooutput_core->stop_play_event("ring_tone_sound");

#ifdef HAVE_NOTIFY
  notify_notification_close (NOTIFY_NOTIFICATION (self), NULL);
#else
  gtk_widget_destroy (GTK_WIDGET (self));
#endif
}


static void on_missed_incoming_call_cb (gpointer self)
{
#ifdef HAVE_NOTIFY
  notify_notification_close (NOTIFY_NOTIFICATION (self), NULL);
#else
  gtk_widget_destroy (GTK_WIDGET (self));
#endif
}


void gm_main_window_add_device_dialog_show (GtkWidget *main_window,
                                            const Ekiga::Device & device,
                                            DeviceType deviceType);

static void on_held_call_cb (Ekiga::CallManager & /*manager*/,
                             Ekiga::Call & /*call*/,
                             gpointer self)
{
  gchar *info_string = NULL;

  gm_main_window_set_call_hold (GTK_WIDGET (self), true);
  info_string = g_strdup (_("Call on hold"));
  gm_main_window_flash_message (GTK_WIDGET (self), "%s", info_string);
  g_free (info_string);
}


static void on_retrieved_call_cb (Ekiga::CallManager & /*manager*/,
                                  Ekiga::Call & /*call*/,
                                  gpointer self)
{
  gchar *info_string = NULL;

  gm_main_window_set_call_hold (GTK_WIDGET (self), false);
  info_string = g_strdup (_("Call retrieved"));
  gm_main_window_flash_message (GTK_WIDGET (self), "%s", info_string);
  g_free (info_string);
}


static void on_missed_call_cb (Ekiga::CallManager & /*manager*/,
                               Ekiga::Call & call,
                               gpointer self)
{
  GmMainWindow *mw = gm_mw_get_mw (GTK_WIDGET (self));
  Ekiga::AudioOutputCore *audiooutput_core = dynamic_cast<Ekiga::AudioOutputCore *> (mw->core.get ("audiooutput-core"));

  audiooutput_core->stop_play_event("incoming_call_sound");
  audiooutput_core->stop_play_event("ring_tone_sound");

  gchar* info = NULL;
  info = g_strdup_printf (_("Missed call from %s"),
			  call.get_remote_party_name ().c_str ());
  gm_main_window_push_message (GTK_WIDGET (self), "%s", info);
  g_free (info);

  gm_main_window_update_calling_state (GTK_WIDGET (self), Standby);
}


static void on_stream_opened_cb (Ekiga::CallManager & /*manager*/,
                                 Ekiga::Call & /* call */,
                                 std::string name,
                                 Ekiga::Call::StreamType type,
                                 bool is_transmitting,
                                 gpointer self)
{
  GmMainWindow *mw = NULL;

  g_return_if_fail (GTK_WIDGET (self) != NULL);
  mw = gm_mw_get_mw (GTK_WIDGET (self));
  g_return_if_fail (mw != NULL);

  bool is_closing = false;
  bool is_encoding = is_transmitting;
  bool is_video = (type == Ekiga::Call::Video);

  /* FIXME: This should not be needed anymore */
  if (type == Ekiga::Call::Video) {
    
    is_closing ?
      (is_encoding ? mw->video_transmission_active = false : mw->video_reception_active = false)
      :(is_encoding ? mw->video_transmission_active = true : mw->video_reception_active = true);

    if (is_encoding)
      is_closing ? mw->transmitted_video_codec = "" : mw->transmitted_video_codec = name;
    else
      is_closing ? mw->received_video_codec = "" : mw->received_video_codec = name;
  }
  else {
    
    is_closing ?
      (is_encoding ? mw->audio_transmission_active = false : mw->audio_reception_active = false)
      :(is_encoding ? mw->audio_transmission_active = true : mw->audio_reception_active = true);

    if (is_encoding)
      is_closing ? mw->transmitted_audio_codec = "" : mw->transmitted_audio_codec = name;
    else
      is_closing ? mw->received_audio_codec = "" : mw->received_audio_codec = name;
  }

  gm_main_window_update_sensitivity (GTK_WIDGET (self),
                                     is_video,
                                     is_video ? mw->video_reception_active : mw->audio_reception_active,
                                     is_video ? mw->video_transmission_active : mw->audio_transmission_active);
  gm_main_window_set_call_info (GTK_WIDGET (self), 
                                mw->transmitted_audio_codec.c_str (), 
                                mw->received_audio_codec.c_str (),
                                mw->transmitted_video_codec.c_str (), 
                                mw->received_audio_codec.c_str ());
}


static void on_stream_closed_cb (Ekiga::CallManager & /*manager*/,
                                 Ekiga::Call & /* call */,
                                 std::string name,
                                 Ekiga::Call::StreamType type,
                                 bool is_transmitting,
                                 gpointer self)
{
  GmMainWindow *mw = NULL;

  g_return_if_fail (GTK_WIDGET (self) != NULL);
  mw = gm_mw_get_mw (GTK_WIDGET (self));
  g_return_if_fail (mw != NULL);

  bool is_closing = true;
  bool is_encoding = is_transmitting;
  bool is_video = (type == Ekiga::Call::Video);

  /* FIXME: This should not be needed anymore */
  if (type == Ekiga::Call::Video) {
    
    is_closing ?
      (is_encoding ? mw->video_transmission_active = false : mw->video_reception_active = false)
      :(is_encoding ? mw->video_transmission_active = true : mw->video_reception_active = true);

    if (is_encoding)
      is_closing ? mw->transmitted_video_codec = "" : mw->transmitted_video_codec = name;
    else
      is_closing ? mw->received_video_codec = "" : mw->received_video_codec = name;
  }
  else {
    
    is_closing ?
      (is_encoding ? mw->audio_transmission_active = false : mw->audio_reception_active = false)
      :(is_encoding ? mw->audio_transmission_active = true : mw->audio_reception_active = true);

    if (is_encoding)
      is_closing ? mw->transmitted_audio_codec = "" : mw->transmitted_audio_codec = name;
    else
      is_closing ? mw->received_audio_codec = "" : mw->received_audio_codec = name;
  }

  gm_main_window_update_sensitivity (GTK_WIDGET (self),
                                     is_video,
                                     is_video ? mw->video_reception_active : mw->audio_reception_active,
                                     is_video ? mw->video_transmission_active : mw->audio_transmission_active);
  gm_main_window_set_call_info (GTK_WIDGET (self), 
                                mw->transmitted_audio_codec.c_str (), 
                                mw->received_audio_codec.c_str (),
                                mw->transmitted_video_codec.c_str (), 
                                mw->received_audio_codec.c_str ());
}


static void on_stream_paused_cb (Ekiga::CallManager & /*manager*/,
                                 Ekiga::Call & /*call*/,
                                 std::string /*name*/,
                                 Ekiga::Call::StreamType type,
                                 gpointer self)
{
  gm_main_window_set_channel_pause (GTK_WIDGET (self), true, (type == Ekiga::Call::Video));
}


static void on_stream_resumed_cb (Ekiga::CallManager & /*manager*/,
                                  Ekiga::Call & /*call*/,
                                  std::string /*name*/,
                                  Ekiga::Call::StreamType type,
                                  gpointer self)
{
  gm_main_window_set_channel_pause (GTK_WIDGET (self), false, (type == Ekiga::Call::Video));
}


static bool on_handle_errors (std::string error,
                              gpointer data)
{
  g_return_val_if_fail (data != NULL, false);

  GtkWidget *dialog = gtk_message_dialog_new (GTK_WINDOW (data), 
                                              GTK_DIALOG_MODAL, 
                                              GTK_MESSAGE_ERROR,
                                              GTK_BUTTONS_OK, NULL);

  gtk_window_set_title (GTK_WINDOW (dialog), _("Error"));
  gtk_label_set_markup (GTK_LABEL (GTK_MESSAGE_DIALOG (dialog)->label), error.c_str ());
  
  g_signal_connect_swapped (GTK_OBJECT (dialog), "response",
                            G_CALLBACK (gtk_widget_destroy),
                            GTK_OBJECT (dialog));
  
  gtk_widget_show_all (dialog);

  return true;
}



/* 
 * Display Engine Callbacks 
 */

void 
on_videooutput_device_opened_cb (Ekiga::VideoOutputManager & /* manager */, 
                                 Ekiga::VideoOutputAccel /* accel */, 
                                 Ekiga::VideoOutputMode mode, 
                                 unsigned zoom, 
                                 bool both_streams,
                                 gpointer self)
{
  GmMainWindow *mw = gm_mw_get_mw (GTK_WIDGET (self));

  if (both_streams) {
       gtk_menu_section_set_sensitive (mw->main_menu, "local_video", TRUE);
       gtk_menu_section_set_sensitive (mw->main_menu, "fullscreen", TRUE);
  } 
  else {

    if (mode == Ekiga::VO_MODE_LOCAL)
      gtk_menu_set_sensitive (mw->main_menu, "local_video", TRUE);

    if (mode == Ekiga::VO_MODE_REMOTE)
       gtk_menu_set_sensitive (mw->main_menu, "remote_video", TRUE);
  }

  gtk_radio_menu_select_with_id (mw->main_menu, "local_video", mode);

  gtk_menu_set_sensitive (mw->main_menu, "zoom_in", (zoom == 200) ? FALSE : TRUE);
  gtk_menu_set_sensitive (mw->main_menu, "zoom_out", (zoom == 50) ? FALSE : TRUE);
  gtk_menu_set_sensitive (mw->main_menu, "normal_size", (zoom == 100) ? FALSE : TRUE);
}

void 
on_videooutput_device_closed_cb (Ekiga::VideoOutputManager & /* manager */, gpointer self)
{
  GmMainWindow *mw = gm_mw_get_mw (GTK_WIDGET (self));

  gtk_menu_section_set_sensitive (mw->main_menu, "local_video", FALSE);

  gtk_menu_section_set_sensitive (mw->main_menu, "fullscreen", TRUE);

  gtk_menu_section_set_sensitive (mw->main_menu, "zoom_in", FALSE);
}

void 
on_videooutput_device_error_cb (Ekiga::VideoOutputManager & /* manager */, 
                                Ekiga::VideoOutputErrorCodes /* error_code */, 
                                gpointer /*self*/)
{
  PTRACE(1, "Error opening the output device");
}

void 
on_fullscreen_mode_changed_cb (Ekiga::VideoOutputManager & /* manager */, Ekiga::VideoOutputFSToggle toggle,  gpointer self)
{
  gm_main_window_toggle_fullscreen (toggle, GTK_WIDGET (self));
}

void 
on_size_changed_cb (Ekiga::VideoOutputManager & /* manager */, unsigned width, unsigned height,  gpointer self)
{
  GmMainWindow *mw = gm_mw_get_mw (GTK_WIDGET (self));
  GtkRequisition req;
  int x, y;

  gtk_widget_size_request (GTK_WIDGET (mw->main_video_image), &req);
  gtk_window_get_size (GTK_WINDOW (self), &x, &y);
  gtk_widget_set_size_request (GTK_WIDGET (mw->main_video_image), width, height);
  gtk_window_resize (GTK_WINDOW (self), width, height);
  
  GdkRectangle rect;
  rect.x = mw->main_video_image->allocation.x;
  rect.y = mw->main_video_image->allocation.y;
  rect.width = mw->main_video_image->allocation.width;
  rect.height = mw->main_video_image->allocation.height;

  gdk_window_invalidate_rect (GDK_WINDOW (GTK_WIDGET (self)->window), &rect , TRUE);
}

void
on_videoinput_device_opened_cb (Ekiga::VideoInputManager & /* manager */,
                                Ekiga::VideoInputDevice & /* device */,
                                Ekiga::VideoInputSettings & settings,
                                gpointer self)
{
  GmMainWindow *mw = NULL;
  g_return_if_fail (self != NULL);
  mw = gm_mw_get_mw (GTK_WIDGET (self));
  g_return_if_fail (mw != NULL);

  gtk_widget_set_sensitive (GTK_WIDGET (mw->video_settings_frame),  settings.modifyable ? TRUE : FALSE);
  gtk_widget_set_sensitive (GTK_WIDGET (mw->video_settings_button),  settings.modifyable ? TRUE : FALSE);
  GTK_ADJUSTMENT (mw->adj_whiteness)->value = settings.whiteness;
  GTK_ADJUSTMENT (mw->adj_brightness)->value = settings.brightness;
  GTK_ADJUSTMENT (mw->adj_colour)->value = settings.colour;
  GTK_ADJUSTMENT (mw->adj_contrast)->value = settings.contrast;

  gtk_widget_queue_draw (GTK_WIDGET (mw->video_settings_frame));
}



void 
on_videoinput_device_closed_cb (Ekiga::VideoInputManager & /* manager */, Ekiga::VideoInputDevice & /*device*/, gpointer self)
{
  GmMainWindow *mw = NULL;
  g_return_if_fail (self != NULL);
  mw = gm_mw_get_mw (GTK_WIDGET (self));
  g_return_if_fail (mw != NULL);

  gm_main_window_update_sensitivity (GTK_WIDGET (self), TRUE, FALSE, FALSE);
  gm_main_window_update_logo_have_window (GTK_WIDGET (self));

  gtk_widget_set_sensitive (GTK_WIDGET (mw->video_settings_button),  FALSE);
}

void 
on_videoinput_device_added_cb (const Ekiga::VideoInputDevice & device, bool isDesired, gpointer self)
{
  GmMainWindow *mw = NULL;
  g_return_if_fail (self != NULL);
  mw = gm_mw_get_mw (GTK_WIDGET (self));
  g_return_if_fail (mw != NULL);
  gchar* message = NULL;

  /* Translators: This is a hotplug status */
  message = g_strdup_printf (_("Added video input device %s"),
			       device.GetString().c_str ());
  gm_main_window_flash_message (GTK_WIDGET (self), "%s", message);
  g_free (message);
  if (!isDesired && mw->calling_state == Standby && !mw->current_call) 
    gm_main_window_add_device_dialog_show (GTK_WIDGET (self), device, VideoInput);
}

void 
on_videoinput_device_removed_cb (const Ekiga::VideoInputDevice & device, bool, gpointer self)
{
  GmMainWindow *mw = NULL;
  g_return_if_fail (self != NULL);
  mw = gm_mw_get_mw (GTK_WIDGET (self));
  g_return_if_fail (mw != NULL);
  gchar* message = NULL;

  /* Translators: This is a hotplug status */
  message = g_strdup_printf (_("Removed video input device %s"),
			     device.GetString().c_str ());
  gm_main_window_flash_message (GTK_WIDGET (self), "%s", message);
  g_free (message);
}

void 
on_videoinput_device_error_cb (Ekiga::VideoInputManager & /* manager */, 
                               Ekiga::VideoInputDevice & device, 
                               Ekiga::VideoInputErrorCodes error_code, 
                               gpointer self)
{
  gchar *dialog_title = NULL;
  gchar *dialog_msg = NULL;
  gchar *tmp_msg = NULL;

  dialog_title =
  g_strdup_printf (_("Error while accessing video device %s"),
                   (const char *) device.name.c_str());

  tmp_msg = g_strdup (_("A moving logo will be transmitted during calls. Notice that you can always transmit a given image or the moving logo by choosing \"Picture\" as video plugin and \"Moving logo\" or \"Static picture\" as device."));
  switch (error_code) {

    case Ekiga::VI_ERROR_DEVICE:
      dialog_msg = g_strconcat (_("There was an error while opening the device. In case it is a pluggable device it may be sufficient to reconnect it. If not, or if it still is not accessible, please check your permissions and make sure that the appropriate driver is loaded."), "\n\n", tmp_msg, NULL);
      break;

    case Ekiga::VI_ERROR_FORMAT:
      dialog_msg = g_strconcat (_("Your video driver doesn't support the requested video format."), "\n\n", tmp_msg, NULL);
      break;

    case Ekiga::VI_ERROR_CHANNEL:
      dialog_msg = g_strconcat (_("Could not open the chosen channel."), "\n\n", tmp_msg, NULL);
      break;

    case Ekiga::VI_ERROR_COLOUR:
      dialog_msg = g_strconcat (_("Your driver doesn't seem to support any of the color formats supported by Ekiga.\n Please check your kernel driver documentation in order to determine which Palette is supported."), "\n\n", tmp_msg, NULL);
      break;

    case Ekiga::VI_ERROR_FPS:
      dialog_msg = g_strconcat (_("Error while setting the frame rate."), "\n\n", tmp_msg, NULL);
      break;

    case Ekiga::VI_ERROR_SCALE:
      dialog_msg = g_strconcat (_("Error while setting the frame size."), "\n\n", tmp_msg, NULL);
      break;

    case Ekiga::VI_ERROR_NONE:
    default:
      dialog_msg = g_strconcat (_("Unknown error."), "\n\n", tmp_msg, NULL);
      break;
  }

  gnomemeeting_warning_dialog_on_widget (GTK_WINDOW (GTK_WIDGET (self)),
                                         "show_device_warnings",
                                         dialog_title,
                                         "%s", dialog_msg);
  g_free (dialog_msg);
  g_free (dialog_title);
  g_free (tmp_msg);
}

void
on_audioinput_device_opened_cb (Ekiga::AudioInputManager & /* manager */,
                                Ekiga::AudioInputDevice & /* device */,
                                Ekiga::AudioInputSettings & settings,
                                gpointer self)
{
  GmMainWindow *mw = NULL;
  g_return_if_fail (self != NULL);
  mw = gm_mw_get_mw (GTK_WIDGET (self));
  g_return_if_fail (mw != NULL);

  gtk_widget_set_sensitive (GTK_WIDGET (mw->audio_input_volume_frame), settings.modifyable ? TRUE : FALSE);
  gtk_widget_set_sensitive (GTK_WIDGET (mw->audio_settings_button), settings.modifyable ? TRUE : FALSE);
  GTK_ADJUSTMENT (mw->adj_input_volume)->value = settings.volume;
  
  gtk_widget_queue_draw (GTK_WIDGET (mw->audio_input_volume_frame));
}



void 
on_audioinput_device_closed_cb (Ekiga::AudioInputManager & /* manager */, 
                                Ekiga::AudioInputDevice & /*device*/, 
                                gpointer self)
{
  GmMainWindow *mw = NULL;
  g_return_if_fail (self != NULL);
  mw = gm_mw_get_mw (GTK_WIDGET (self));
  g_return_if_fail (mw != NULL);

  gtk_widget_set_sensitive (GTK_WIDGET (mw->audio_settings_button), FALSE);
  gtk_widget_set_sensitive (GTK_WIDGET (mw->audio_input_volume_frame), FALSE);
}

void 
on_audioinput_device_added_cb (const Ekiga::AudioInputDevice & device, 
                               bool isDesired,
                               gpointer self)
{
  GmMainWindow *mw = NULL;
  g_return_if_fail (self != NULL);
  mw = gm_mw_get_mw (GTK_WIDGET (self));
  g_return_if_fail (mw != NULL);
  gchar* message = NULL;

  /* Translators: This is a hotplug status */
  message = g_strdup_printf (_("Added audio input device %s"),
			     device.GetString().c_str ());
  gm_main_window_flash_message (GTK_WIDGET (self), "%s", message);
  g_free (message);
  if (!isDesired  && mw->calling_state == Standby && !mw->current_call)
    gm_main_window_add_device_dialog_show (GTK_WIDGET (self), device,  AudioInput);
    
}

void 
on_audioinput_device_removed_cb (const Ekiga::AudioInputDevice & device, 
                                 bool,
                                 gpointer self)
{
  GmMainWindow *mw = NULL;
  g_return_if_fail (self != NULL);
  mw = gm_mw_get_mw (GTK_WIDGET (self));
  g_return_if_fail (mw != NULL);
  gchar* message = NULL;

  /* Translators: This is a hotplug status */
  message = g_strdup_printf (_("Removed audio input device %s"),
			     device.GetString().c_str ());
  gm_main_window_flash_message (GTK_WIDGET (self), "%s", message);
  g_free (message);
}

void 
on_audioinput_device_error_cb (Ekiga::AudioInputManager & /* manager */, 
                               Ekiga::AudioInputDevice & device, 
                               Ekiga::AudioInputErrorCodes error_code, 
                               gpointer self)
{
  gchar *dialog_title = NULL;
  gchar *dialog_msg = NULL;
  gchar *tmp_msg = NULL;

  dialog_title =
  g_strdup_printf (_("Error while opening audio input device %s"),
                   (const char *) device.name.c_str());

  /* Translators: This happens when there is an error with audio input:
   * Nothing ("silence") will be transmitted */
  tmp_msg = g_strdup (_("Only silence will be transmitted."));
  switch (error_code) {

    case Ekiga::AI_ERROR_DEVICE:
      dialog_msg = g_strconcat (tmp_msg, "\n\n", _("Unable to open the selected audio device for recording. In case it is a pluggable device it may be sufficient to reconnect it. If not, or if it still is not accessible, please check your audio setup, the permissions and that the device is not busy."), NULL);
      break;

    case Ekiga::AI_ERROR_READ:
      dialog_msg = g_strconcat (tmp_msg, "\n\n", _("The selected audio device was successfully opened but it is impossible to read data from this device. In case it is a pluggable device it may be sufficient to reconnect it. If not, or if it still is not accessible, please check your audio setup."), NULL);
      break;

    case Ekiga::AI_ERROR_NONE:
    default:
      dialog_msg = g_strconcat (tmp_msg, "\n\n", _("Unknown error."), NULL);
      break;
  }

  gnomemeeting_warning_dialog_on_widget (GTK_WINDOW (GTK_WIDGET (self)),
                                         "show_device_warnings",
                                         dialog_title,
                                         "%s", dialog_msg);
  g_free (dialog_msg);
  g_free (dialog_title);
  g_free (tmp_msg);

}

void
on_audiooutput_device_opened_cb (Ekiga::AudioOutputManager & /*manager*/,
                                 Ekiga::AudioOutputPS ps,
                                 Ekiga::AudioOutputDevice & /*device*/,
                                 Ekiga::AudioOutputSettings & settings,
                                 gpointer self)
{
  if (ps == Ekiga::secondary)
    return;

  GmMainWindow *mw = NULL;
  g_return_if_fail (self != NULL);
  mw = gm_mw_get_mw (GTK_WIDGET (self));
  g_return_if_fail (mw != NULL);

  gtk_widget_set_sensitive (GTK_WIDGET (mw->audio_settings_button), settings.modifyable ? TRUE : FALSE);
  gtk_widget_set_sensitive (GTK_WIDGET (mw->audio_output_volume_frame), settings.modifyable ? TRUE : FALSE);
  GTK_ADJUSTMENT (mw->adj_output_volume)->value = settings.volume;

  gtk_widget_queue_draw (GTK_WIDGET (mw->audio_output_volume_frame));
}



void 
on_audiooutput_device_closed_cb (Ekiga::AudioOutputManager & /*manager*/, 
                                 Ekiga::AudioOutputPS ps, 
                                 Ekiga::AudioOutputDevice & /*device*/, 
                                 gpointer self)
{
  if (ps == Ekiga::secondary)
    return;

  GmMainWindow *mw = NULL;
  g_return_if_fail (self != NULL);
  mw = gm_mw_get_mw (GTK_WIDGET (self));
  g_return_if_fail (mw != NULL);

  gtk_widget_set_sensitive (GTK_WIDGET (mw->audio_settings_button), FALSE);
  gtk_widget_set_sensitive (GTK_WIDGET (mw->audio_output_volume_frame), FALSE);
}

void 
on_audiooutput_device_added_cb (const Ekiga::AudioOutputDevice & device, 
                                bool isDesired,
                                gpointer self)
{
  GmMainWindow *mw = NULL;
  g_return_if_fail (self != NULL);
  mw = gm_mw_get_mw (GTK_WIDGET (self));
  g_return_if_fail (mw != NULL);
  gchar* message = NULL;

  message = g_strdup_printf (_("Added audio output device %s"), device.GetString().c_str ());
  gm_main_window_flash_message (GTK_WIDGET (self), "%s", message);
  g_free (message);
  if (!isDesired && mw->calling_state == Standby && !mw->current_call)
    gm_main_window_add_device_dialog_show (GTK_WIDGET (self), device, AudioOutput);
}

void 
on_audiooutput_device_removed_cb (const Ekiga::AudioOutputDevice & device, 
                                  bool,
                                  gpointer self)
{
  GmMainWindow *mw = NULL;
  g_return_if_fail (self != NULL);
  mw = gm_mw_get_mw (GTK_WIDGET (self));
  g_return_if_fail (mw != NULL);
  gchar* message = NULL;

  message = g_strdup_printf (_("Removed audio output device %s"),
			     device.GetString().c_str ());
  gm_main_window_flash_message (GTK_WIDGET (self), "%s", message);
  g_free (message);
}

void 
on_audiooutput_device_error_cb (Ekiga::AudioOutputManager & /*manager */, 
                                Ekiga::AudioOutputPS ps,
                                Ekiga::AudioOutputDevice & device, 
                                Ekiga::AudioOutputErrorCodes error_code, 
                                gpointer self)
{
  if (ps == Ekiga::secondary)
    return;

  gchar *dialog_title = NULL;
  gchar *dialog_msg = NULL;
  gchar *tmp_msg = NULL;

  dialog_title =
  g_strdup_printf (_("Error while opening audio output device %s"),
                   (const char *) device.name.c_str());

  tmp_msg = g_strdup (_("No incoming sound will be played."));
  switch (error_code) {

    case Ekiga::AO_ERROR_DEVICE:
      dialog_msg = g_strconcat (tmp_msg, "\n\n", _("Unable to open the selected audio device for playing. In case it is a pluggable device it may be sufficient to reconnect it. If not, or if it still is not accessible, please check your audio setup, the permissions and that the device is not busy."), NULL);
      break;

    case Ekiga::AO_ERROR_WRITE:
      dialog_msg = g_strconcat (tmp_msg, "\n\n", _("The selected audio device was successfully opened but it is impossible to write data from this device. In case it is a pluggable device it may be sufficient to reconnect it. If not, or if it still is not accessible, please check your audio setup."), NULL);
      break;

    case Ekiga::AO_ERROR_NONE:
    default:
      dialog_msg = g_strconcat (tmp_msg, "\n\n", _("Unknown error."), NULL);
      break;
  }

  gnomemeeting_warning_dialog_on_widget (GTK_WINDOW (GTK_WIDGET (self)),
                                         "show_device_warnings",
                                         dialog_title,
                                         "%s", dialog_msg);
  g_free (dialog_msg);
  g_free (dialog_title);
  g_free (tmp_msg);
}


/* Implementation */
static void
gm_mw_destroy (gpointer m)
{
  GmMainWindow *mw = GM_MAIN_WINDOW (m);

  g_return_if_fail (mw != NULL);

  gtk_widget_destroy (mw->audio_settings_window);
  gtk_widget_destroy (mw->video_settings_window);

  delete ((GmMainWindow *) mw);
}


static GmMainWindow *
gm_mw_get_mw (GtkWidget *main_window)
{
  g_return_val_if_fail (main_window != NULL, NULL);

  return GM_MAIN_WINDOW (g_object_get_data (G_OBJECT (main_window), 
					    "GMObject"));
}


static void
incoming_call_response_cb (GtkDialog *incoming_call_popup,
                           gint response,
                           gpointer data)
{
  Ekiga::Call *call = (Ekiga::Call *) data;

  gtk_widget_hide (GTK_WIDGET (incoming_call_popup));

  if (call) {

    switch (response) {
    case 2:
      call->answer ();
      break;

    default:
    case 0:
      call->hangup ();
      break;
    }
  }
}

static void
add_device_response_cb (GtkDialog *add_device_popup,
                           gint response,
                           gpointer data)
{
  deviceStruct *device_struct = (deviceStruct*) data;

  gtk_widget_hide (GTK_WIDGET (add_device_popup));

  if (response == 2) {

    switch (device_struct->deviceType)
    {
     case AudioInput:
       gm_conf_set_string (AUDIO_DEVICES_KEY "input_device", device_struct->name);
       break;
     case AudioOutput:
       gm_conf_set_string (AUDIO_DEVICES_KEY "output_device", device_struct->name);
       break;
     case VideoInput:
       gm_conf_set_string (VIDEO_DEVICES_KEY "input_device", device_struct->name);
       break;	                
     default:;
    }
  }
}

static void
place_call_cb (GtkWidget * /*widget*/,
               gpointer data)
{
  std::string uri;

  Ekiga::CallCore *call_core = NULL;
  GmMainWindow *mw = NULL;

  mw = gm_mw_get_mw (GTK_WIDGET (data));

  if (mw->calling_state == Standby && !mw->current_call) {

    size_t pos;

    gm_main_window_update_calling_state (GTK_WIDGET (data), Calling);
    call_core = dynamic_cast<Ekiga::CallCore*> (mw->core.get ("call-core"));
    uri = gm_main_window_get_call_url (GTK_WIDGET (data));
    if (call_core->dial (uri)) {

      pos = uri.find ("@");
      if (pos != std::string::npos) {

        std::string host = uri.substr (pos + 1);
        mw->accounts.remove (host);
        mw->accounts.push_front (host);
      }

    }
    else {
      gm_main_window_flash_message (GTK_WIDGET (data), _("Could not connect to remote host"));
      gm_main_window_update_calling_state (GTK_WIDGET (data), Standby);
    }
  }
  else if (mw->calling_state == Called && mw->current_call)
    mw->current_call->answer ();
}


static void
hangup_call_cb (GtkWidget * /*widget*/,
                gpointer data)
{
  GmMainWindow *mw = NULL;

  mw = gm_mw_get_mw (GTK_WIDGET (data));

  if (mw->current_call) {

    mw->current_call->hangup ();
    mw->current_call = NULL;
  }
}


static void 
toggle_call_cb (GtkWidget *widget,
                gpointer data)
{
  GmMainWindow *mw = NULL;

  mw = gm_mw_get_mw (GTK_WIDGET (data));

  if (gm_connect_button_get_connected (GM_CONNECT_BUTTON (mw->connect_button)))
    hangup_call_cb (widget, data);
  else if (!gm_connect_button_get_connected (GM_CONNECT_BUTTON (mw->connect_button))) {
    if (!mw->current_call)
      place_call_cb (widget, data);
    else
      mw->current_call->answer ();
  }
}


static void
on_status_icon_embedding_change (G_GNUC_UNUSED GObject obj,
				 G_GNUC_UNUSED GParamSpec param,
				 G_GNUC_UNUSED gpointer data)
{
  GtkWidget *main_window = NULL;
  GtkStatusIcon *status_icon = NULL;

  status_icon = GTK_STATUS_ICON (GnomeMeeting::Process ()->GetStatusicon ());
  main_window = GnomeMeeting::Process ()->GetMainWindow ();

  /* force the main window to show if no status icon for the user */
  if (!gtk_status_icon_is_embedded (GTK_STATUS_ICON (status_icon)))
    gtk_widget_show (main_window);
}

static GtkWidget *
gm_mw_init_uri_toolbar (GtkWidget *main_window)
{
  GmMainWindow *mw = NULL;

  GtkToolItem *item = NULL;

  GtkWidget *toolbar = NULL;
  GtkEntryCompletion *completion = NULL;
  
  g_return_val_if_fail (main_window != NULL, NULL);
  mw = gm_mw_get_mw (main_window);
  g_return_val_if_fail (mw != NULL, NULL);

  
  /* The main horizontal toolbar */
  toolbar = gtk_toolbar_new ();
  gtk_toolbar_set_style (GTK_TOOLBAR (toolbar), GTK_TOOLBAR_ICONS);
  gtk_toolbar_set_show_arrow (GTK_TOOLBAR (toolbar), FALSE);


  /* URL bar */
  /* Entry */
  item = gtk_tool_item_new ();
  mw->entry = gtk_entry_new ();
  mw->completion = gtk_list_store_new (1, G_TYPE_STRING);
  completion = gtk_entry_completion_new ();
  gtk_entry_completion_set_model (GTK_ENTRY_COMPLETION (completion), GTK_TREE_MODEL (mw->completion));
  gtk_entry_set_completion (GTK_ENTRY (mw->entry), completion);
  gtk_entry_completion_set_inline_completion (GTK_ENTRY_COMPLETION (completion), false);
  gtk_entry_completion_set_popup_completion (GTK_ENTRY_COMPLETION (completion), true);
  gtk_entry_completion_set_text_column (GTK_ENTRY_COMPLETION (completion), 0);

  gtk_container_add (GTK_CONTAINER (item), mw->entry);
  gtk_container_set_border_width (GTK_CONTAINER (item), 0);
  gtk_tool_item_set_expand (GTK_TOOL_ITEM (item), true);
  
  gtk_entry_set_text (GTK_ENTRY (mw->entry), "sip:");

  // activate Ctrl-L to get the entry focus
  gtk_widget_add_accelerator (mw->entry, "grab-focus",
			      mw->accel, GDK_L,
			      (GdkModifierType) GDK_CONTROL_MASK,
			      (GtkAccelFlags) 0);

  gtk_editable_set_position (GTK_EDITABLE (GTK_WIDGET (mw->entry)), -1);

  g_signal_connect (G_OBJECT (mw->entry), "changed", 
		    GTK_SIGNAL_FUNC (url_changed_cb), main_window);
  g_signal_connect (G_OBJECT (mw->entry), "activate", 
		    GTK_SIGNAL_FUNC (place_call_cb), main_window);

  gtk_toolbar_insert (GTK_TOOLBAR (toolbar), item, 0);

  /* The connect button */
  item = gtk_tool_item_new ();
  mw->connect_button = gm_connect_button_new (GM_STOCK_PHONE_PICK_UP_24,
					      GM_STOCK_PHONE_HANG_UP_24,
					      GTK_ICON_SIZE_LARGE_TOOLBAR);
  gtk_container_add (GTK_CONTAINER (item), mw->connect_button);
  gtk_container_set_border_width (GTK_CONTAINER (mw->connect_button), 0);
  gtk_tool_item_set_expand (GTK_TOOL_ITEM (item), FALSE);

  gtk_widget_set_tooltip_text (GTK_WIDGET (mw->connect_button),
			       _("Enter a URI on the left, and click this button to place a call"));
  
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar), item, -1);

  g_signal_connect (G_OBJECT (mw->connect_button), "clicked",
                    G_CALLBACK (toggle_call_cb), 
                    main_window);

  gtk_widget_show_all (GTK_WIDGET (toolbar));
  
  return toolbar;
}


static GtkWidget *
gm_mw_init_status_toolbar (GtkWidget *main_window)
{
  GmMainWindow *mw = NULL;

  GtkWidget *toolbar = NULL;
  GtkToolItem *item = NULL;

  g_return_val_if_fail (main_window != NULL, NULL);
  mw = gm_mw_get_mw (main_window);
  g_return_val_if_fail (mw != NULL, NULL);

  
  /* The main horizontal toolbar */
  toolbar = gtk_toolbar_new ();
  gtk_toolbar_set_style (GTK_TOOLBAR (toolbar), GTK_TOOLBAR_ICONS);
  gtk_toolbar_set_show_arrow (GTK_TOOLBAR (toolbar), FALSE);

  item = gtk_tool_item_new ();
  mw->status_option_menu = status_menu_new (mw->core); 
  status_menu_set_parent_window (STATUS_MENU (mw->status_option_menu), 
                                 GTK_WINDOW (main_window));
  gtk_container_add (GTK_CONTAINER (item), mw->status_option_menu);
  gtk_container_set_border_width (GTK_CONTAINER (item), 0);
  gtk_tool_item_set_expand (GTK_TOOL_ITEM (item), TRUE);
  
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar), item, 0);

  gtk_widget_show_all (toolbar);
  
  return toolbar;
}

	
static void
gm_mw_init_menu (GtkWidget *main_window)
{
  GmMainWindow *mw = NULL;

  Ekiga::ServiceCore *services = NULL;
  Ekiga::Trigger *local_cluster_trigger = NULL;
  GtkFrontend *gtk_frontend = NULL;
  
  GtkWidget *addressbook_window = NULL;
  GtkWidget *accounts_window = NULL;

  bool show_call_panel = false;
  
  PanelSection cps = DIALPAD;

  g_return_if_fail (main_window != NULL);
  mw = gm_mw_get_mw (main_window);

  services = GnomeMeeting::Process ()->GetServiceCore ();
  local_cluster_trigger = dynamic_cast<Ekiga::Trigger *>(services->get ("local-cluster"));
  gtk_frontend = dynamic_cast<GtkFrontend *>(services->get ("gtk-frontend"));
  addressbook_window = GTK_WIDGET (gtk_frontend->get_addressbook_window ()); 
  accounts_window = GnomeMeeting::Process ()->GetAccountsWindow ();

  mw->main_menu = gtk_menu_bar_new ();

  /* Default values */
  show_call_panel = gm_conf_get_bool (USER_INTERFACE_KEY "main_window/show_call_panel");
  cps = (PanelSection) gm_conf_get_int (USER_INTERFACE_KEY "main_window/panel_section"); 
  
  static MenuEntry gnomemeeting_menu [] =
    {
      GTK_MENU_NEW (_("_Chat")),

      GTK_MENU_ENTRY("connect", _("Ca_ll"), _("Place a new call"), 
		     GM_STOCK_PHONE_PICK_UP_16, 'o',
		     GTK_SIGNAL_FUNC (place_call_cb), main_window, TRUE),
      GTK_MENU_ENTRY("disconnect", _("_Hang up"),
		     _("Terminate the current call"), 
 		     GM_STOCK_PHONE_HANG_UP_16, GDK_Escape,
		     GTK_SIGNAL_FUNC (hangup_call_cb), main_window, FALSE),

      GTK_MENU_SEPARATOR,

      GTK_MENU_ENTRY("add_contact", _("A_dd Contact"), _("Add a contact to the roster"),
		     GTK_STOCK_ADD, 'n', 
		     GTK_SIGNAL_FUNC (pull_trigger_cb), local_cluster_trigger, true),

      GTK_MENU_THEME_ENTRY("address_book", _("Address _Book"),
			   _("Find contacts"),
 			   GM_ICON_ADDRESSBOOK, 'b',
			   GTK_SIGNAL_FUNC (show_widget_cb),
			   (gpointer) addressbook_window, TRUE),

      GTK_MENU_SEPARATOR,

      GTK_MENU_ENTRY("contact", _("_Contact"),
		     _("Act on selected contact"),
		     GTK_STOCK_EXECUTE, 0,
		     NULL, NULL, FALSE),

      GTK_MENU_SEPARATOR,

      GTK_MENU_ENTRY("hold_call", _("H_old Call"), _("Hold the current call"),
		     NULL, GDK_h, 
		     GTK_SIGNAL_FUNC (hold_current_call_cb), main_window,
		     FALSE),
      GTK_MENU_ENTRY("transfer_call", _("_Transfer Call"),
		     _("Transfer the current call"),
 		     NULL, GDK_t, 
		     GTK_SIGNAL_FUNC (transfer_current_call_cb), main_window, 
		     FALSE),

      GTK_MENU_SEPARATOR,

      GTK_MENU_ENTRY("suspend_audio", _("Suspend _Audio"),
		     _("Suspend or resume the audio transmission"),
		     NULL, GDK_m,
		     GTK_SIGNAL_FUNC (toggle_audio_stream_pause_cb),
		     main_window, FALSE),
      GTK_MENU_ENTRY("suspend_video", _("Suspend _Video"),
		     _("Suspend or resume the video transmission"),
		     NULL, GDK_p, 
		     GTK_SIGNAL_FUNC (toggle_video_stream_pause_cb),
		     main_window, FALSE),

      GTK_MENU_SEPARATOR,

      GTK_MENU_ENTRY("close", NULL, _("Close the Ekiga window"),
		     GTK_STOCK_CLOSE, 'W', 
		     GTK_SIGNAL_FUNC (window_closed_from_menu_cb),
		     (gpointer) main_window, TRUE),

      GTK_MENU_SEPARATOR,
      
      GTK_MENU_ENTRY("quit", NULL, _("Quit"),
		     GTK_STOCK_QUIT, 'Q', 
		     GTK_SIGNAL_FUNC (quit_callback), NULL, TRUE),

      GTK_MENU_NEW (_("_Edit")),

      GTK_MENU_ENTRY("configuration_assistant", _("_Configuration Assistant"),
		     _("Run the configuration assistant"),
		     NULL, 0, 
		     G_CALLBACK (show_assistant_window_cb),
		     NULL, TRUE),

      GTK_MENU_SEPARATOR,
      
      GTK_MENU_ENTRY("accounts", _("_Accounts"),
		     _("Edit your accounts"), 
		     NULL, 'E',
		     GTK_SIGNAL_FUNC (show_window_cb),
		     (gpointer) accounts_window, TRUE),

      GTK_MENU_ENTRY("preferences", NULL,
		     _("Change your preferences"), 
		     GTK_STOCK_PREFERENCES, 0,
		     G_CALLBACK (show_prefs_window_cb),
		     NULL, TRUE),

      GTK_MENU_NEW(_("_View")),

      GTK_MENU_RADIO_ENTRY("contacts", _("Con_tacts"), _("View the contacts list"),
			   NULL, 0,
			   GTK_SIGNAL_FUNC (radio_menu_changed_cb),
			   (gpointer) USER_INTERFACE_KEY "main_window/panel_section",
			   (cps == CONTACTS), TRUE),
      GTK_MENU_RADIO_ENTRY("dialpad", _("_Dialpad"), _("View the dialpad"),
			   NULL, 0,
			   GTK_SIGNAL_FUNC (radio_menu_changed_cb), 
			   (gpointer) USER_INTERFACE_KEY "main_window/panel_section",
			   (cps == DIALPAD), TRUE),
      GTK_MENU_RADIO_ENTRY("callhistory", _("_Call History"), _("View the call history"),
			   NULL, 0,
			   GTK_SIGNAL_FUNC (radio_menu_changed_cb),
			   (gpointer) USER_INTERFACE_KEY "main_window/panel_section",
			   (cps == CALL), TRUE),

      GTK_MENU_SEPARATOR,

      GTK_MENU_TOGGLE_ENTRY("callpanel", _("_Show Call Panel"), _("Show the call panel"),
                            NULL, 'J', 
                            GTK_SIGNAL_FUNC (toggle_menu_changed_cb),
                            (gpointer) USER_INTERFACE_KEY "main_window/show_call_panel", 
                            show_call_panel, TRUE),

      GTK_MENU_SEPARATOR,

      GTK_MENU_RADIO_ENTRY("local_video", _("_Local Video"),
			   _("Local video image"),
			   NULL, '1', 
			   GTK_SIGNAL_FUNC (display_changed_cb),
			   (gpointer) VIDEO_DISPLAY_KEY "video_view",
			   TRUE, FALSE),
      GTK_MENU_RADIO_ENTRY("remote_video", _("_Remote Video"),
			   _("Remote video image"),
			   NULL, '2', 
			   GTK_SIGNAL_FUNC (display_changed_cb), 
			   (gpointer) VIDEO_DISPLAY_KEY "video_view",
			   FALSE, FALSE),
      GTK_MENU_RADIO_ENTRY("both_incrusted", _("_Picture-in-Picture"),
			   _("Both video images"),
			   NULL, '3', 
			   GTK_SIGNAL_FUNC (display_changed_cb), 
			   (gpointer) VIDEO_DISPLAY_KEY "video_view",
			   FALSE, FALSE),
      GTK_MENU_RADIO_ENTRY("both_incrusted_window", _("Picture-in-Picture in Separate _Window"),
			   _("Both video images"),
			   NULL, '4', 
			   GTK_SIGNAL_FUNC (display_changed_cb), 
			   (gpointer) VIDEO_DISPLAY_KEY "video_view",
			   FALSE, FALSE),
      GTK_MENU_SEPARATOR,

      GTK_MENU_ENTRY("zoom_in", NULL, _("Zoom in"), 
		     GTK_STOCK_ZOOM_IN, '+', 
		     GTK_SIGNAL_FUNC (zoom_in_changed_cb),
		     (gpointer) VIDEO_DISPLAY_KEY "zoom", FALSE),
      GTK_MENU_ENTRY("zoom_out", NULL, _("Zoom out"), 
		     GTK_STOCK_ZOOM_OUT, '-', 
		     GTK_SIGNAL_FUNC (zoom_out_changed_cb),
		     (gpointer) VIDEO_DISPLAY_KEY "zoom", FALSE),
      GTK_MENU_ENTRY("normal_size", NULL, _("Normal size"), 
		     GTK_STOCK_ZOOM_100, '0',
		     GTK_SIGNAL_FUNC (zoom_normal_changed_cb),
		     (gpointer) VIDEO_DISPLAY_KEY "zoom", FALSE),
      GTK_MENU_ENTRY("fullscreen", _("_Fullscreen"), _("Switch to fullscreen"), 
		     GTK_STOCK_ZOOM_IN, GDK_F11, 
		     GTK_SIGNAL_FUNC (fullscreen_changed_cb),
		     (gpointer) main_window, FALSE),

      GTK_MENU_NEW(_("_Help")),

      GTK_MENU_ENTRY("help", NULL, 
                     _("Get help by reading the Ekiga manual"),
                     GTK_STOCK_HELP, GDK_F1, 
                     GTK_SIGNAL_FUNC (help_cb), NULL, TRUE),

      GTK_MENU_ENTRY("about", NULL,
		     _("View information about Ekiga"),
		     GTK_STOCK_ABOUT, 0, 
		     GTK_SIGNAL_FUNC (about_callback), (gpointer) main_window,
		     TRUE),
       
      GTK_MENU_END
    };


  gtk_build_menu (mw->main_menu, 
		  gnomemeeting_menu, 
		  mw->accel, 
		  mw->statusbar);

  gtk_widget_show_all (GTK_WIDGET (mw->main_menu));
}


static void 
gm_mw_init_contacts_list (GtkWidget *main_window)
{
  GmMainWindow *mw = NULL;

  GtkWidget *label = NULL;

  GtkFrontend *gtk_frontend = NULL;
  Ekiga::ServiceCore *services = NULL;
  GtkWidget* roster_view = NULL;

  g_return_if_fail (main_window != NULL);
  mw = gm_mw_get_mw (main_window);
  
  services = GnomeMeeting::Process ()->GetServiceCore ();
  g_return_if_fail (services != NULL);

  gtk_frontend = dynamic_cast<GtkFrontend *>(services->get ("gtk-frontend"));

  label = gtk_label_new (_("Contacts"));
  roster_view = GTK_WIDGET (gtk_frontend->get_roster_view ());
  gtk_notebook_append_page (GTK_NOTEBOOK (mw->main_notebook),
			    roster_view, label);
  g_signal_connect (G_OBJECT (roster_view), "presentity-selected",
		    G_CALLBACK (on_presentity_selected), main_window);
}


static void 
gm_mw_init_dialpad (GtkWidget *main_window)
{
  GmMainWindow *mw = NULL;

  GtkWidget *dialpad = NULL;
  GtkWidget *alignment = NULL;
  GtkWidget *label = NULL;

  g_return_if_fail (main_window != NULL);
  mw = gm_mw_get_mw (main_window);

  dialpad = ekiga_dialpad_new (mw->accel);
  g_signal_connect (dialpad, "button-clicked",
                    G_CALLBACK (dialpad_button_clicked_cb), main_window);

  alignment = gtk_alignment_new (0.5, 0.5, 0.2, 0.2);
  gtk_container_add (GTK_CONTAINER (alignment), dialpad);

  label = gtk_label_new (_("Dialpad"));
  gtk_notebook_append_page (GTK_NOTEBOOK (mw->main_notebook),
			    alignment, label);
}


static GtkWidget * 
gm_mw_video_settings_window_new (GtkWidget *main_window)
{
  GmMainWindow *mw = NULL;
  
  GtkWidget *hbox = NULL;
  GtkWidget *vbox = NULL;
  GtkWidget *image = NULL;
  GtkWidget *window = NULL;

  GtkWidget *hscale_brightness = NULL;
  GtkWidget *hscale_colour = NULL;
  GtkWidget *hscale_contrast = NULL;
  GtkWidget *hscale_whiteness = NULL;

  int brightness = 0, colour = 0, contrast = 0, whiteness = 0;

  g_return_val_if_fail (main_window != NULL, NULL);
  mw = gm_mw_get_mw (main_window);

  
  /* Build the window */
  window = gtk_dialog_new ();
  g_object_set_data_full (G_OBJECT (window), "window_name",
			  g_strdup ("video_settings_window"), g_free); 
  gtk_dialog_add_button (GTK_DIALOG (window), 
                         GTK_STOCK_CLOSE, 
                         GTK_RESPONSE_CANCEL);

  gtk_window_set_title (GTK_WINDOW (window), 
                        _("Video Settings"));

  /* Webcam Control Frame, we need it to disable controls */		
  mw->video_settings_frame = gtk_frame_new (NULL);
  gtk_frame_set_shadow_type (GTK_FRAME (mw->video_settings_frame), 
			     GTK_SHADOW_NONE);
  gtk_container_set_border_width (GTK_CONTAINER (mw->video_settings_frame), 5);
  
  /* Category */
  vbox = gtk_vbox_new (0, FALSE);
  gtk_container_add (GTK_CONTAINER (mw->video_settings_frame), vbox);
  
  /* Brightness */
  hbox = gtk_hbox_new (0, FALSE);
  image = gtk_image_new_from_icon_name (GM_ICON_BRIGHTNESS, GTK_ICON_SIZE_MENU);
  gtk_box_pack_start (GTK_BOX (hbox), image, FALSE, FALSE, 0);

  mw->adj_brightness = gtk_adjustment_new (brightness, 0.0, 
					   255.0, 1.0, 5.0, 1.0);
  hscale_brightness = gtk_hscale_new (GTK_ADJUSTMENT (mw->adj_brightness));
  gtk_range_set_update_policy (GTK_RANGE (hscale_brightness),
			       GTK_UPDATE_DELAYED);
  gtk_scale_set_draw_value (GTK_SCALE (hscale_brightness), FALSE);
  gtk_scale_set_value_pos (GTK_SCALE (hscale_brightness), GTK_POS_RIGHT);
  gtk_box_pack_start (GTK_BOX (hbox), hscale_brightness, TRUE, TRUE, 2);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 3);

  gtk_widget_set_tooltip_text (hscale_brightness, _("Adjust brightness"));

  g_signal_connect (G_OBJECT (mw->adj_brightness), "value-changed",
		    G_CALLBACK (video_settings_changed_cb), 
		    (gpointer) main_window);

  /* Whiteness */
  hbox = gtk_hbox_new (0, FALSE);
  image = gtk_image_new_from_icon_name (GM_ICON_WHITENESS, GTK_ICON_SIZE_MENU);
  gtk_box_pack_start (GTK_BOX (hbox), image, FALSE, FALSE, 0);

  mw->adj_whiteness = gtk_adjustment_new (whiteness, 0.0, 
					  255.0, 1.0, 5.0, 1.0);
  hscale_whiteness = gtk_hscale_new (GTK_ADJUSTMENT (mw->adj_whiteness));
  gtk_range_set_update_policy (GTK_RANGE (hscale_whiteness),
			       GTK_UPDATE_DELAYED);
  gtk_scale_set_draw_value (GTK_SCALE (hscale_whiteness), FALSE);
  gtk_scale_set_value_pos (GTK_SCALE (hscale_whiteness), GTK_POS_RIGHT);
  gtk_box_pack_start (GTK_BOX (hbox), hscale_whiteness, TRUE, TRUE, 2);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 3);

  gtk_widget_set_tooltip_text (hscale_whiteness, _("Adjust whiteness"));

  g_signal_connect (G_OBJECT (mw->adj_whiteness), "value-changed",
		    G_CALLBACK (video_settings_changed_cb), 
		    (gpointer) main_window);

  /* Colour */
  hbox = gtk_hbox_new (0, FALSE);
  image = gtk_image_new_from_icon_name (GM_ICON_COLOURNESS, GTK_ICON_SIZE_MENU);
  gtk_box_pack_start (GTK_BOX (hbox), image, FALSE, FALSE, 0);

  mw->adj_colour = gtk_adjustment_new (colour, 0.0, 
				       255.0, 1.0, 5.0, 1.0);
  hscale_colour = gtk_hscale_new (GTK_ADJUSTMENT (mw->adj_colour));
  gtk_range_set_update_policy (GTK_RANGE (hscale_colour),
			       GTK_UPDATE_DELAYED);
  gtk_scale_set_draw_value (GTK_SCALE (hscale_colour), FALSE);
  gtk_scale_set_value_pos (GTK_SCALE (hscale_colour), GTK_POS_RIGHT);
  gtk_box_pack_start (GTK_BOX (hbox), hscale_colour, TRUE, TRUE, 2);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 3);

  gtk_widget_set_tooltip_text (hscale_colour, _("Adjust color"));

  g_signal_connect (G_OBJECT (mw->adj_colour), "value-changed",
		    G_CALLBACK (video_settings_changed_cb), 
		    (gpointer) main_window);

  /* Contrast */
  hbox = gtk_hbox_new (0, FALSE);
  image = gtk_image_new_from_icon_name (GM_ICON_CONTRAST, GTK_ICON_SIZE_MENU);
  gtk_box_pack_start (GTK_BOX (hbox), image, FALSE, FALSE, 0);
  
  mw->adj_contrast = gtk_adjustment_new (contrast, 0.0, 
					 255.0, 1.0, 5.0, 1.0);
  hscale_contrast = gtk_hscale_new (GTK_ADJUSTMENT (mw->adj_contrast));
  gtk_range_set_update_policy (GTK_RANGE (hscale_contrast),
			       GTK_UPDATE_DELAYED);
  gtk_scale_set_draw_value (GTK_SCALE (hscale_contrast), FALSE);
  gtk_scale_set_value_pos (GTK_SCALE (hscale_contrast), GTK_POS_RIGHT);
  gtk_box_pack_start (GTK_BOX (hbox), hscale_contrast, TRUE, TRUE, 2);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 3);

  gtk_widget_set_tooltip_text (hscale_contrast, _("Adjust contrast"));

  g_signal_connect (G_OBJECT (mw->adj_contrast), "value-changed",
		    G_CALLBACK (video_settings_changed_cb), 
		    (gpointer) main_window);
  
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG (window)->vbox), 
                     mw->video_settings_frame);
  gtk_widget_show_all (mw->video_settings_frame);

  gtk_widget_set_sensitive (GTK_WIDGET (mw->video_settings_frame), FALSE);
  
  /* That's an usual GtkWindow, connect it to the signals */
  g_signal_connect_swapped (GTK_OBJECT (window), 
			    "response", 
			    G_CALLBACK (gnomemeeting_window_hide),
			    (gpointer) window);

  g_signal_connect (GTK_OBJECT (window), 
		    "delete-event", 
		    G_CALLBACK (delete_window_cb), NULL);

  return window;
}



static GtkWidget * 
gm_mw_audio_settings_window_new (GtkWidget *main_window)
{
  GmMainWindow *mw = NULL;
  
  GtkWidget *hscale_play = NULL; 
  GtkWidget *hscale_rec = NULL;
  GtkWidget *hbox = NULL;
  GtkWidget *vbox = NULL;
  GtkWidget *small_vbox = NULL;
  GtkWidget *window = NULL;
  

  /* Get the data from the GMObject */
  g_return_val_if_fail (main_window != NULL, NULL);
  mw = gm_mw_get_mw (main_window);
  

  /* Build the window */
  window = gtk_dialog_new ();
  g_object_set_data_full (G_OBJECT (window), "window_name",
			  g_strdup ("audio_settings_window"), g_free); 
  gtk_dialog_add_button (GTK_DIALOG (window), 
                         GTK_STOCK_CLOSE, 
                         GTK_RESPONSE_CANCEL);

  gtk_window_set_title (GTK_WINDOW (window), 
                        _("Audio Settings"));

  /* Audio control frame, we need it to disable controls */		
  mw->audio_output_volume_frame = gtk_frame_new (NULL);
  gtk_frame_set_shadow_type (GTK_FRAME (mw->audio_output_volume_frame), 
			     GTK_SHADOW_NONE);
  gtk_container_set_border_width (GTK_CONTAINER (mw->audio_output_volume_frame), 5);


  /* The vbox */
  vbox = gtk_vbox_new (0, FALSE);
  gtk_container_add (GTK_CONTAINER (mw->audio_output_volume_frame), vbox);

  /* Output volume */
  hbox = gtk_hbox_new (0, FALSE);
  gtk_box_pack_start (GTK_BOX (hbox), 
		      gtk_image_new_from_icon_name (GM_ICON_AUDIO_VOLUME_HIGH, 
						    GTK_ICON_SIZE_SMALL_TOOLBAR),
		      FALSE, FALSE, 0);
  
  small_vbox = gtk_vbox_new (0, FALSE);
  mw->adj_output_volume = gtk_adjustment_new (0, 0.0, 101.0, 1.0, 5.0, 1.0);
  hscale_play = gtk_hscale_new (GTK_ADJUSTMENT (mw->adj_output_volume));
  gtk_range_set_update_policy (GTK_RANGE (hscale_play),
			       GTK_UPDATE_DELAYED);
  gtk_scale_set_value_pos (GTK_SCALE (hscale_play), GTK_POS_RIGHT); 
  gtk_scale_set_draw_value (GTK_SCALE (hscale_play), FALSE);
  gtk_box_pack_start (GTK_BOX (small_vbox), hscale_play, TRUE, TRUE, 0);

  mw->output_signal = gtk_levelmeter_new ();
  gtk_box_pack_start (GTK_BOX (small_vbox), mw->output_signal, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (hbox), small_vbox, TRUE, TRUE, 2);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 3);

  gtk_container_add (GTK_CONTAINER (GTK_DIALOG (window)->vbox), 
                     mw->audio_output_volume_frame);
  gtk_widget_show_all (mw->audio_output_volume_frame);
  gtk_widget_set_sensitive (GTK_WIDGET (mw->audio_output_volume_frame),  FALSE);

  /* Audio control frame, we need it to disable controls */		
  mw->audio_input_volume_frame = gtk_frame_new (NULL);
  gtk_frame_set_shadow_type (GTK_FRAME (mw->audio_input_volume_frame), 
			     GTK_SHADOW_NONE);
  gtk_container_set_border_width (GTK_CONTAINER (mw->audio_input_volume_frame), 5);

  /* The vbox */
  vbox = gtk_vbox_new (0, FALSE);
  gtk_container_add (GTK_CONTAINER (mw->audio_input_volume_frame), vbox);

  /* Input volume */
  hbox = gtk_hbox_new (0, FALSE);
  gtk_box_pack_start (GTK_BOX (hbox),
		      gtk_image_new_from_icon_name (GM_ICON_MICROPHONE, 
						    GTK_ICON_SIZE_SMALL_TOOLBAR),
		      FALSE, FALSE, 0);

  small_vbox = gtk_vbox_new (0, FALSE);
  mw->adj_input_volume = gtk_adjustment_new (0, 0.0, 101.0, 1.0, 5.0, 1.0);
  hscale_rec = gtk_hscale_new (GTK_ADJUSTMENT (mw->adj_input_volume));
  gtk_range_set_update_policy (GTK_RANGE (hscale_rec),
			       GTK_UPDATE_DELAYED);
  gtk_scale_set_value_pos (GTK_SCALE (hscale_rec), GTK_POS_RIGHT); 
  gtk_scale_set_draw_value (GTK_SCALE (hscale_rec), FALSE);
  gtk_box_pack_start (GTK_BOX (small_vbox), hscale_rec, TRUE, TRUE, 0);

  mw->input_signal = gtk_levelmeter_new ();
  gtk_box_pack_start (GTK_BOX (small_vbox), mw->input_signal, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (hbox), small_vbox, TRUE, TRUE, 2);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 3);

  gtk_container_add (GTK_CONTAINER (GTK_DIALOG (window)->vbox), 
                     mw->audio_input_volume_frame);
  gtk_widget_show_all (mw->audio_input_volume_frame);
  gtk_widget_set_sensitive (GTK_WIDGET (mw->audio_input_volume_frame),  FALSE);

  g_signal_connect (G_OBJECT (mw->adj_output_volume), "value-changed",
		    G_CALLBACK (audio_volume_changed_cb), main_window);

  g_signal_connect (G_OBJECT (mw->adj_input_volume), "value-changed",
		    G_CALLBACK (audio_volume_changed_cb), main_window);



  
  /* That's an usual GtkWindow, connect it to the signals */
  g_signal_connect_swapped (GTK_OBJECT (window), 
			    "response", 
			    G_CALLBACK (gnomemeeting_window_hide),
			    (gpointer) window);

  g_signal_connect (GTK_OBJECT (window), 
		    "delete-event", 
		    G_CALLBACK (delete_window_cb), NULL);

  g_signal_connect (G_OBJECT (window), "show", 
                    GTK_SIGNAL_FUNC (audio_volume_window_shown_cb), main_window);

  g_signal_connect (G_OBJECT (window), "hide", 
                    GTK_SIGNAL_FUNC (audio_volume_window_hidden_cb), main_window);

  return window;
}


static void 
gm_mw_init_call (GtkWidget *main_window)
{
  GmMainWindow *mw = NULL;

  GtkWidget *frame = NULL;
  GtkWidget *event_box = NULL;
  GtkWidget *table = NULL;

  GtkWidget *toolbar = NULL;
  GtkToolItem *item = NULL;

  GtkWidget *image = NULL;
  GtkWidget *alignment = NULL;

  GdkColor white;
  gdk_color_parse ("white", &white);

  /* Get the data from the GMObject */
  mw = gm_mw_get_mw (main_window);

  /* The main table */
  frame = gtk_frame_new (NULL);
  event_box = gtk_event_box_new ();
  gtk_widget_modify_bg (event_box, GTK_STATE_PRELIGHT, &white);
  gtk_widget_modify_bg (event_box, GTK_STATE_NORMAL, &white);
  table = gtk_table_new (3, 4, FALSE);
  gtk_container_add (GTK_CONTAINER (event_box), table);
  gtk_container_add (GTK_CONTAINER (frame), event_box);

  /* The frame that contains the video */
  mw->video_frame = gtk_frame_new (NULL);
  gtk_frame_set_shadow_type (GTK_FRAME (mw->video_frame), 
                             GTK_SHADOW_NONE);
  
  mw->main_video_image = gtk_image_new ();
  gtk_container_set_border_width (GTK_CONTAINER (mw->video_frame), 0);
  gtk_container_add (GTK_CONTAINER (mw->video_frame), mw->main_video_image);
  gtk_table_attach (GTK_TABLE (table), GTK_WIDGET (mw->video_frame),       
                    0, 4, 0, 1,         
                    (GtkAttachOptions) GTK_EXPAND,      
                    (GtkAttachOptions) GTK_EXPAND,      
                    24, 24);

  /* The frame that contains information about the call */
  /* Text buffer */
  GtkTextBuffer *buffer = NULL;
  
  mw->info_text = gtk_text_view_new ();
  gtk_widget_modify_bg (mw->info_text, GTK_STATE_PRELIGHT, &white);
  gtk_widget_modify_bg (mw->info_text, GTK_STATE_NORMAL, &white);
  gtk_widget_modify_bg (mw->info_text, GTK_STATE_INSENSITIVE, &white);

  gtk_text_view_set_editable (GTK_TEXT_VIEW (mw->info_text), FALSE);
  gtk_widget_set_sensitive (GTK_WIDGET (mw->info_text), FALSE);
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (mw->info_text),
			       GTK_WRAP_WORD);

  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (mw->info_text));
  gtk_text_view_set_cursor_visible  (GTK_TEXT_VIEW (mw->info_text), FALSE);

  gtk_text_buffer_create_tag (buffer, "status",
			      "foreground", "black", 
                              "paragraph-background", "white",
                              "justification", GTK_JUSTIFY_CENTER,
                              "weight", PANGO_WEIGHT_BOLD,
                              "scale", 1.2,
                              NULL);
  gtk_text_buffer_create_tag (buffer, "codecs",
                              "justification", GTK_JUSTIFY_RIGHT,
                              "stretch", PANGO_STRETCH_CONDENSED,
			      "foreground", "darkgray", 
                              "paragraph-background", "white",
			      NULL);
  gtk_text_buffer_create_tag (buffer, "call-duration",
			      "foreground", "black", 
                              "paragraph-background", "white",
			      "justification", GTK_JUSTIFY_CENTER,
                              "weight", PANGO_WEIGHT_BOLD,
			      NULL);

  gm_main_window_set_status (main_window, _("Standby"));
  gm_main_window_set_call_duration (main_window, NULL);
  gm_main_window_set_call_info (main_window, NULL, NULL, NULL, NULL);

  alignment = gtk_alignment_new (0.0, 0.0, 1.0, 0.0);
  gtk_container_add (GTK_CONTAINER (alignment), mw->info_text);
  gtk_table_attach (GTK_TABLE (table), alignment,
                    0, 4, 1, 2,         
                    (GtkAttachOptions) (GTK_FILL | GTK_EXPAND),
                    (GtkAttachOptions) (GTK_FILL | GTK_EXPAND),
                    0, 0);
  
  /* The toolbar */
  toolbar = gtk_toolbar_new ();
  gtk_widget_modify_bg (toolbar, GTK_STATE_PRELIGHT, &white);
  gtk_widget_modify_bg (toolbar, GTK_STATE_NORMAL, &white);
  gtk_toolbar_set_style (GTK_TOOLBAR (toolbar), GTK_TOOLBAR_ICONS);
  gtk_toolbar_set_show_arrow (GTK_TOOLBAR (toolbar), FALSE);

  /* Audio Volume */
  item = gtk_tool_item_new ();
  mw->audio_settings_button = gtk_button_new ();
  gtk_button_set_relief (GTK_BUTTON (mw->audio_settings_button), GTK_RELIEF_NONE);
  image = gtk_image_new_from_icon_name (GM_ICON_AUDIO_VOLUME_HIGH,
                                        GTK_ICON_SIZE_MENU);
  gtk_container_add (GTK_CONTAINER (mw->audio_settings_button), image);
  gtk_container_add (GTK_CONTAINER (item), mw->audio_settings_button);
  gtk_tool_item_set_expand (GTK_TOOL_ITEM (item), FALSE);
  
  gtk_widget_show (mw->audio_settings_button);
  gtk_widget_set_sensitive (mw->audio_settings_button, FALSE);
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar), 
		      GTK_TOOL_ITEM (item), -1);
  gtk_tool_item_set_tooltip_text (GTK_TOOL_ITEM (item),
				  _("Change the volume of your soundcard"));
  g_signal_connect (G_OBJECT (mw->audio_settings_button), "clicked",
		    G_CALLBACK (show_window_cb),
		    (gpointer) mw->audio_settings_window);
  
  /* Video Settings */
  item = gtk_tool_item_new ();
  mw->video_settings_button = gtk_button_new ();
  gtk_button_set_relief (GTK_BUTTON (mw->video_settings_button), GTK_RELIEF_NONE);
  image = gtk_image_new_from_stock (GM_STOCK_COLOR_BRIGHTNESS_CONTRAST,
                                    GTK_ICON_SIZE_MENU);
  gtk_container_add (GTK_CONTAINER (mw->video_settings_button), image);
  gtk_container_add (GTK_CONTAINER (item), mw->video_settings_button);
  gtk_tool_item_set_expand (GTK_TOOL_ITEM (item), FALSE);
  
  gtk_widget_show (mw->video_settings_button);
  gtk_widget_set_sensitive (mw->video_settings_button, FALSE);
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar), 
		      GTK_TOOL_ITEM (item), -1);
  gtk_tool_item_set_tooltip_text (GTK_TOOL_ITEM (item),
				   _("Change the color settings of your video device"));

  g_signal_connect (G_OBJECT (mw->video_settings_button), "clicked",
		    G_CALLBACK (show_window_cb),
		    (gpointer) mw->video_settings_window);

  /* Video Preview Button */
  item = gtk_tool_item_new ();
  mw->preview_button = gtk_toggle_button_new ();
  gtk_button_set_relief (GTK_BUTTON (mw->preview_button), GTK_RELIEF_NONE);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (mw->preview_button),
                                gm_conf_get_bool (VIDEO_DEVICES_KEY "enable_preview"));
  image = gtk_image_new_from_icon_name (GM_ICON_CAMERA_VIDEO, 
                                        GTK_ICON_SIZE_MENU);
  gtk_container_add (GTK_CONTAINER (mw->preview_button), image);
  gtk_container_add (GTK_CONTAINER (item), mw->preview_button);
  gtk_tool_item_set_expand (GTK_TOOL_ITEM (item), FALSE);
  
  gtk_widget_show (mw->preview_button);
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar), 
		      GTK_TOOL_ITEM (item), -1);
  gtk_tool_item_set_tooltip_text (GTK_TOOL_ITEM (item),
				  _("Display images from your camera device"));

  g_signal_connect (G_OBJECT (mw->preview_button), "toggled",
		    G_CALLBACK (toolbar_toggle_button_changed_cb),
		    (gpointer) VIDEO_DEVICES_KEY "enable_preview");

  /* Call Pause */
  item = gtk_tool_item_new ();
  mw->hold_button = gtk_toggle_button_new ();
  image = gtk_image_new_from_icon_name (GM_ICON_MEDIA_PLAYBACK_PAUSE,
                                        GTK_ICON_SIZE_MENU);
  gtk_button_set_relief (GTK_BUTTON (mw->hold_button), GTK_RELIEF_NONE);
  gtk_container_add (GTK_CONTAINER (mw->hold_button), image);
  gtk_container_add (GTK_CONTAINER (item), mw->hold_button);
  gtk_tool_item_set_expand (GTK_TOOL_ITEM (item), FALSE);
  
  gtk_widget_show (mw->hold_button);
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar), 
		      GTK_TOOL_ITEM (item), -1);
  gtk_tool_item_set_tooltip_text (GTK_TOOL_ITEM (item),
				  _("Hold the current call"));
  gtk_widget_set_sensitive (GTK_WIDGET (mw->hold_button), FALSE);

  g_signal_connect (G_OBJECT (mw->hold_button), "clicked",
		    G_CALLBACK (hold_current_call_cb), main_window); 

  alignment = gtk_alignment_new (0.0, 0.0, 1.0, 0.0);
  gtk_container_add (GTK_CONTAINER (alignment), toolbar);
  gtk_table_attach (GTK_TABLE (table), alignment,
                    1, 3, 2, 3,         
                    (GtkAttachOptions) (GTK_FILL | GTK_EXPAND),
                    (GtkAttachOptions) (GTK_FILL | GTK_EXPAND),
                    0, 0);

  gtk_paned_pack2 (GTK_PANED (mw->hpaned), frame, true, false);
  gtk_widget_realize (mw->main_video_image);
}  

static void
gm_mw_init_history (GtkWidget *main_window)
{
  GmMainWindow *mw = NULL;
  Ekiga::ServiceCore *services = NULL;
  GtkWidget *label = NULL;
  GtkFrontend *gtk_frontend = NULL;

  g_return_if_fail (main_window != NULL);
  mw = gm_mw_get_mw (main_window);

  services = GnomeMeeting::Process ()->GetServiceCore ();
  g_return_if_fail (services != NULL);

  gtk_frontend = dynamic_cast<GtkFrontend *>(services->get ("gtk-frontend"));

  label = gtk_label_new (_("Call history"));
  gtk_notebook_append_page (GTK_NOTEBOOK (mw->main_notebook),
			    GTK_WIDGET (gtk_frontend->get_call_history_view ()),
			    label);
}

void
gm_mw_zooms_menu_update_sensitivity (GtkWidget *main_window,
                                     unsigned int zoom)
{
  GmMainWindow *mw = NULL;

  mw = gm_mw_get_mw (main_window);

  g_return_if_fail (mw != NULL);

  /* between 0.5 and 2.0 zoom */
  /* like above, also update the popup menus of the separate video windows */
  gtk_menu_set_sensitive (mw->main_menu, "zoom_in",
                          (zoom == 200) ? FALSE : TRUE);

  gtk_menu_set_sensitive (mw->main_menu, "zoom_out",
                          (zoom == 50) ? FALSE : TRUE);

  gtk_menu_set_sensitive (mw->main_menu, "normal_size",
                          (zoom == 100) ? FALSE : TRUE);
}


/* GTK callbacks */
static gint
gnomemeeting_tray_hack_cb (G_GNUC_UNUSED gpointer data)
{
  GtkWidget *main_window = NULL;
  GtkStatusIcon *statusicon = NULL;

  statusicon = GTK_STATUS_ICON (GnomeMeeting::Process ()->GetStatusicon ());
  main_window = GnomeMeeting::Process ()->GetMainWindow ();
  
  //gdk_threads_enter ();

  if (!gtk_status_icon_is_embedded (GTK_STATUS_ICON (statusicon))) 
    gtk_widget_show (main_window);
  
  //gdk_threads_leave ();

  return FALSE;
}


static void
on_presentity_selected (G_GNUC_UNUSED GtkWidget* view,
			Ekiga::Presentity* presentity,
			gpointer self)
{
  GmMainWindow *mw = gm_mw_get_mw (GTK_WIDGET (self));

  g_return_if_fail (mw != NULL);

  mw->presentity = presentity;

  gm_main_window_selected_presentity_build_menu (GTK_WIDGET (self));
}


static void
on_chat_unread_alert (G_GNUC_UNUSED GtkWidget* widget,
		      G_GNUC_UNUSED gpointer data)
{
  if (!gm_conf_get_bool (SOUND_EVENTS_KEY "enable_new_message_sound"))
    return;

  Ekiga::ServiceCore *core = GnomeMeeting::Process ()->GetServiceCore ();
  Ekiga::AudioOutputCore *audiooutput_core = dynamic_cast<Ekiga::AudioOutputCore *> (core->get ("audiooutput-core"));

  std::string file_name_string = gm_conf_get_string (SOUND_EVENTS_KEY "new_message_sound");

  if (!file_name_string.empty ())
    audiooutput_core->play_file(file_name_string);
}


static void 
panel_section_changed_nt (G_GNUC_UNUSED gpointer id, 
                          GmConfEntry *entry, 
                          gpointer data)
{
  gint section = 0;

  g_return_if_fail (data != NULL);
  
  if (gm_conf_entry_get_type (entry) == GM_CONF_INT) {

    //gdk_threads_enter ();
    section = gm_conf_entry_get_int (entry);
    gm_main_window_set_panel_section (GTK_WIDGET (data), 
                                      section);
    //gdk_threads_leave ();
  }
}


static void 
show_call_panel_changed_nt (G_GNUC_UNUSED gpointer id, 
                            GmConfEntry *entry, 
                            gpointer data)
{
  g_return_if_fail (data != NULL);

  if (gm_conf_entry_get_type (entry) == GM_CONF_BOOL) {

    //gdk_threads_enter ();
    if (gm_conf_entry_get_bool (entry)) 
      gm_main_window_show_call_panel (GTK_WIDGET (data));
    else 
      gm_main_window_hide_call_panel (GTK_WIDGET (data));

    //gdk_threads_leave ();
  }
}


static void 
pull_trigger_cb (GtkWidget * /*widget*/,
                 gpointer data)
{
  Ekiga::Trigger *trigger = (Ekiga::Trigger *) data;

  g_return_if_fail (trigger != NULL);

  trigger->pull ();
}


static void 
show_widget_cb (GtkWidget * /*widget*/,
                gpointer data)
{
  g_return_if_fail (data != NULL);

  gtk_widget_show_all (GTK_WIDGET (data));
}


static void 
hold_current_call_cb (G_GNUC_UNUSED GtkWidget *widget,
		      gpointer data)
{
  GmMainWindow *mw = NULL;

  mw = gm_mw_get_mw (GTK_WIDGET (data));

  if (mw->current_call) {
    mw->current_call->toggle_hold ();
  }
}


static void
toggle_audio_stream_pause_cb (GtkWidget * /*widget*/,
                              gpointer data)
{
  GmMainWindow *mw = NULL;

  mw = gm_mw_get_mw (GTK_WIDGET (data));

  if (mw->current_call)
    mw->current_call->toggle_stream_pause (Ekiga::Call::Audio);
}


static void
toggle_video_stream_pause_cb (GtkWidget * /*widget*/,
                              gpointer data)
{
  GmMainWindow *mw = NULL;

  mw = gm_mw_get_mw (GTK_WIDGET (data));

  if (mw->current_call)
    mw->current_call->toggle_stream_pause (Ekiga::Call::Video);
}



static void 
transfer_current_call_cb (G_GNUC_UNUSED GtkWidget *widget,
			  gpointer data)
{
  GtkWidget *main_window = NULL;
  
  g_return_if_fail (data != NULL);
  
  main_window = GnomeMeeting::Process ()->GetMainWindow ();

  gm_main_window_transfer_dialog_run (main_window, GTK_WIDGET (data), NULL);  
}


static gboolean 
video_window_expose_cb (GtkWidget *main_window,
                	G_GNUC_UNUSED GdkEventExpose *event,
            		G_GNUC_UNUSED gpointer data)
{
  GmMainWindow *mw = NULL;

  if (!main_window) 
    return FALSE;

  mw = gm_mw_get_mw (main_window);
  if (!mw)
    return FALSE;

  GtkWidget* videoWidget = mw->main_video_image;
  if (!GTK_WIDGET_REALIZED(videoWidget))
    return FALSE;

  if (!GDK_IS_WINDOW(mw->main_video_image->window))
    return FALSE;

  Ekiga::DisplayInfo display_info;
  display_info.x = mw->main_video_image->allocation.x;
  display_info.y = mw->main_video_image->allocation.y;
#ifdef WIN32  
  display_info.hwnd = ((HWND)GDK_WINDOW_HWND (mw->main_video_image->window));
#else 
  if (!mw->video_widget_gc) { 
    mw->video_widget_gc = gdk_gc_new(mw->main_video_image->window);
    if ( mw->video_widget_gc == NULL)
      return FALSE;

  }

  if (GDK_GC_XGC(mw->video_widget_gc) == NULL)
    return FALSE;

  display_info.gc = GDK_GC_XGC(mw->video_widget_gc);
  display_info.xdisplay = GDK_GC_XDISPLAY (mw->video_widget_gc);

  display_info.window = GDK_WINDOW_XWINDOW (mw->main_video_image->window);
  if (display_info.window == 0)  //FIXME: Should be None
    return FALSE;

  gdk_flush();
#endif
  display_info.widget_info_set = TRUE;

  Ekiga::VideoOutputCore *videooutput_core = dynamic_cast<Ekiga::VideoOutputCore *> (mw->core.get ("videooutput-core"));
  videooutput_core->set_display_info(display_info);

  return FALSE;
}

static void
video_window_shown_cb (GtkWidget *w,
		       gpointer data)
{
  GmMainWindow *mw = NULL;

  mw = gm_mw_get_mw (GTK_WIDGET (data));

  if (gm_conf_get_bool (VIDEO_DISPLAY_KEY "stay_on_top")
      && mw->current_call)
    gdk_window_set_always_on_top (GDK_WINDOW (w->window), TRUE);
}


static void 
audio_volume_changed_cb (GtkAdjustment * /*adjustment*/,
			 gpointer data)
{
  GmMainWindow *mw = NULL;
  g_return_if_fail (data != NULL);
  mw = gm_mw_get_mw (GTK_WIDGET (data));

  Ekiga::AudioInputCore *audioinput_core = dynamic_cast<Ekiga::AudioInputCore *> (mw->core.get ("audioinput-core"));
  Ekiga::AudioOutputCore *audiooutput_core = dynamic_cast<Ekiga::AudioOutputCore *> (mw->core.get ("audiooutput-core"));

  audiooutput_core->set_volume(Ekiga::primary, (unsigned)GTK_ADJUSTMENT (mw->adj_output_volume)->value);
  audioinput_core->set_volume((unsigned)GTK_ADJUSTMENT (mw->adj_input_volume)->value);
}

static void 
audio_volume_window_shown_cb (GtkWidget * /*widget*/,
	                      gpointer data)
{
  GmMainWindow *mw = NULL;
  g_return_if_fail (data != NULL);
  mw = gm_mw_get_mw (GTK_WIDGET (data));

  Ekiga::AudioInputCore *audioinput_core = dynamic_cast<Ekiga::AudioInputCore *> (mw->core.get ("audioinput-core"));
  Ekiga::AudioOutputCore *audiooutput_core = dynamic_cast<Ekiga::AudioOutputCore *> (mw->core.get ("audiooutput-core"));

  audioinput_core->set_average_collection(true);
  audiooutput_core->set_average_collection(true);
  mw->levelmeter_timeout_id = g_timeout_add_full (G_PRIORITY_DEFAULT_IDLE, 50, on_signal_level_refresh_cb, data, NULL);
}


static void 
audio_volume_window_hidden_cb (GtkWidget * /*widget*/,
                               gpointer data)
{
  GmMainWindow *mw = NULL;
  g_return_if_fail (data != NULL);
  mw = gm_mw_get_mw (GTK_WIDGET (data));

  Ekiga::AudioInputCore *audioinput_core = dynamic_cast<Ekiga::AudioInputCore *> (mw->core.get ("audioinput-core"));
  Ekiga::AudioOutputCore *audiooutput_core = dynamic_cast<Ekiga::AudioOutputCore *> (mw->core.get ("audiooutput-core"));

  g_source_remove (mw->levelmeter_timeout_id);
  audioinput_core->set_average_collection(false);
  audiooutput_core->set_average_collection(false);
}

static void 
video_settings_changed_cb (GtkAdjustment * /*adjustment*/,
			   gpointer data)
{ 
  GmMainWindow *mw = NULL;
  g_return_if_fail (data != NULL);
  mw = gm_mw_get_mw (GTK_WIDGET (data));
  g_return_if_fail (mw != NULL);

  Ekiga::VideoInputCore *videoinput_core = dynamic_cast<Ekiga::VideoInputCore *> (mw->core.get ("videoinput-core"));

  videoinput_core->set_whiteness ((unsigned) GTK_ADJUSTMENT (mw->adj_whiteness)->value);
  videoinput_core->set_brightness ((unsigned) GTK_ADJUSTMENT (mw->adj_brightness)->value);
  videoinput_core->set_colour ((unsigned) GTK_ADJUSTMENT (mw->adj_colour)->value);
  videoinput_core->set_contrast ((unsigned) GTK_ADJUSTMENT (mw->adj_contrast)->value);
}


static void 
panel_section_changed_cb (G_GNUC_UNUSED GtkNotebook *notebook,
                          G_GNUC_UNUSED GtkNotebookPage *page,
                          G_GNUC_UNUSED gint page_num,
                          gpointer data) 
{
  GmMainWindow *mw = NULL;

  gint current_page = 0;

  g_return_if_fail (data != NULL);
  mw = gm_mw_get_mw (GTK_WIDGET (data));

  current_page = 
    gtk_notebook_get_current_page (GTK_NOTEBOOK (mw->main_notebook));
  gm_conf_set_int (USER_INTERFACE_KEY "main_window/panel_section",
		   current_page);
}


static void
dialpad_button_clicked_cb (EkigaDialpad  * /* dialpad */,
			   const gchar *button_text,
			   GtkWidget   *main_window)
{
  GmMainWindow *mw = NULL;

  mw = gm_mw_get_mw (main_window);

  if (mw->current_call)
    mw->current_call->send_dtmf (button_text[0]);
  else
    gm_main_window_append_call_url (main_window, button_text);
}


static gint 
window_closed_cb (G_GNUC_UNUSED GtkWidget *widget,
		  G_GNUC_UNUSED GdkEvent *event,
		  gpointer data)
{
  GtkStatusIcon *statusicon = NULL;
  GtkWidget *main_window = NULL;

  GmMainWindow *mw = NULL;

  main_window = GnomeMeeting::Process ()->GetMainWindow ();
  statusicon = GTK_STATUS_ICON (GnomeMeeting::Process ()->GetStatusicon ());
  mw = gm_mw_get_mw (GTK_WIDGET (main_window));

  /* Hide in any case, but quit program when we don't have a status menu */
  gtk_widget_hide (GTK_WIDGET (data));
  if (!gtk_status_icon_is_embedded (GTK_STATUS_ICON (statusicon)))
    quit_callback (NULL, data);

  return (TRUE);
}


static void
window_closed_from_menu_cb (GtkWidget *widget,
                           gpointer data)
{
window_closed_cb (widget, NULL, data);
}


static void 
zoom_in_changed_cb (G_GNUC_UNUSED GtkWidget *widget,
		    gpointer data)
{
  GtkWidget *main_window = GnomeMeeting::Process ()->GetMainWindow ();
  g_return_if_fail (main_window != NULL);

  GmMainWindow *mw = gm_mw_get_mw (main_window);
  g_return_if_fail (mw != NULL);

  g_return_if_fail (data != NULL);

  Ekiga::DisplayInfo display_info;

  display_info.zoom = gm_conf_get_int ((char *) data);

  if (display_info.zoom < 200)
    display_info.zoom = display_info.zoom * 2;

  gm_conf_set_int ((char *) data, display_info.zoom);
  gm_mw_zooms_menu_update_sensitivity (main_window, display_info.zoom);
}


static void 
zoom_out_changed_cb (G_GNUC_UNUSED GtkWidget *widget,
		     gpointer data)
{
  GtkWidget *main_window = GnomeMeeting::Process ()->GetMainWindow ();
  g_return_if_fail (main_window != NULL);

  GmMainWindow *mw = gm_mw_get_mw (main_window);
  g_return_if_fail (mw != NULL);

  g_return_if_fail (data != NULL);

  Ekiga::DisplayInfo display_info;

  display_info.zoom = gm_conf_get_int ((char *) data);

  if (display_info.zoom  > 50)
    display_info.zoom  = (unsigned int) (display_info.zoom  / 2);

  gm_conf_set_int ((char *) data, display_info.zoom);
  gm_mw_zooms_menu_update_sensitivity (main_window, display_info.zoom);
}


static void 
zoom_normal_changed_cb (G_GNUC_UNUSED GtkWidget *widget,
			gpointer data)
{
  GtkWidget *main_window = GnomeMeeting::Process ()->GetMainWindow ();
  g_return_if_fail (main_window != NULL);

  GmMainWindow *mw = gm_mw_get_mw (main_window);
  g_return_if_fail (mw != NULL);

  g_return_if_fail (data != NULL);

  Ekiga::DisplayInfo display_info;

  display_info.zoom  = 100;

  gm_conf_set_int ((char *) data, display_info.zoom);
  gm_mw_zooms_menu_update_sensitivity (main_window, display_info.zoom);
}


void 
display_changed_cb (GtkWidget *widget,
		       gpointer data)
{
  GtkWidget *main_window = GnomeMeeting::Process ()->GetMainWindow ();
  g_return_if_fail (main_window != NULL);

  GmMainWindow *mw = gm_mw_get_mw (main_window);
  g_return_if_fail (mw != NULL);

  g_return_if_fail (data != NULL);

  GSList *group = NULL;
  int group_last_pos = 0;
  int active = 0;
  Ekiga::DisplayInfo display_info;

  group = gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM (widget));
  group_last_pos = g_slist_length (group) - 1; /* If length 1, last pos is 0 */

  /* Only do something when a new CHECK_MENU_ITEM becomes active,
     not when it becomes inactive */
  if (GTK_CHECK_MENU_ITEM (widget)->active) {

    while (group) {

      if (group->data == widget) 
	break;
      
      active++;
      group = g_slist_next (group);
    }

    gm_conf_set_int ((gchar *) data, group_last_pos - active);
  }
}


static void 
fullscreen_changed_cb (G_GNUC_UNUSED GtkWidget *widget,
		       G_GNUC_UNUSED gpointer data)
{
  GtkWidget* main_window = GnomeMeeting::Process()->GetMainWindow ();
  g_return_if_fail (main_window != NULL);
  gm_main_window_toggle_fullscreen (Ekiga::VO_FS_TOGGLE, main_window);
}


static void
url_changed_cb (GtkEditable  *e,
		gpointer data)
{
  GmMainWindow *mw = NULL;

  GtkTreeIter iter;
  const char *tip_text = NULL;
  gchar *entry = NULL;

  g_return_if_fail (data != NULL);
  mw = gm_mw_get_mw (GTK_WIDGET (data));

  tip_text = gtk_entry_get_text (GTK_ENTRY (e));

  if (g_strrstr (tip_text, "@") == NULL) {

    gtk_list_store_clear (mw->completion);

    for (std::list<std::string>::iterator it = mw->accounts.begin ();
         it != mw->accounts.end ();
         it++) {

      entry = g_strdup_printf ("%s@%s", tip_text, it->c_str ());
      gtk_list_store_append (mw->completion, &iter);
      gtk_list_store_set (mw->completion, &iter, 0, entry, -1);
      g_free (entry);
    }
  }

  gtk_widget_set_tooltip_text (GTK_WIDGET (e), tip_text);
}


static void 
toolbar_toggle_button_changed_cb (G_GNUC_UNUSED GtkWidget *widget,
				  gpointer data)
{
  bool shown = gm_conf_get_bool ((gchar *) data);

  gm_conf_set_bool ((gchar *) data, !shown);
}


static gboolean 
statusbar_clicked_cb (G_GNUC_UNUSED GtkWidget *widget,
		      G_GNUC_UNUSED GdkEventButton *event,
		      gpointer data)
{
  GmMainWindow *mw = NULL;

  g_return_val_if_fail (data != NULL, TRUE);

  mw = gm_mw_get_mw (GTK_WIDGET (data));

  g_return_val_if_fail (GTK_WIDGET (data), TRUE);

  gm_main_window_push_message (GTK_WIDGET (data), NULL);

  return FALSE;
}


static gboolean
main_window_focus_event_cb (GtkWidget *main_window,
			    G_GNUC_UNUSED GdkEventFocus *event,
			    G_GNUC_UNUSED gpointer user_data)
{
  if (gtk_window_get_urgency_hint (GTK_WINDOW (main_window)))
      gtk_window_set_urgency_hint (GTK_WINDOW (main_window), FALSE);

  return FALSE;
}


/* Public functions */
void 
gm_main_window_press_dialpad (GtkWidget *main_window,
			      const char c)
{
  guint key = 0;

  if (c == '*')
    key = GDK_KP_Multiply;
  else if (c == '#')
    key = GDK_numbersign;
  else
    key = GDK_KP_0 + atoi (&c);

  gtk_accel_groups_activate (G_OBJECT (main_window), key, (GdkModifierType) 0);
}


GtkWidget*
gm_main_window_get_video_widget (GtkWidget *main_window)
{
  GmMainWindow *mw = NULL;
  
  g_return_val_if_fail (main_window != NULL, NULL);
  mw = gm_mw_get_mw (main_window);
  g_return_val_if_fail (mw != NULL, NULL);

  return mw->main_video_image;
}


void 
gm_main_window_update_logo_have_window (GtkWidget *main_window)
{
  GmMainWindow *mw = NULL;

  g_return_if_fail (main_window != NULL);

  mw = gm_mw_get_mw (main_window);
  g_return_if_fail (mw != NULL);

  g_object_set (G_OBJECT (mw->main_video_image),
		"icon-name", GM_ICON_LOGO,
		"pixel-size", 72,
		NULL);
  
  gtk_widget_set_size_request (GTK_WIDGET (mw->main_video_image),
                               GM_QCIF_WIDTH, GM_QCIF_HEIGHT);

  GdkRectangle rect;
  rect.x = mw->main_video_image->allocation.x;
  rect.y = mw->main_video_image->allocation.y;
  rect.width = mw->main_video_image->allocation.width;
  rect.height = mw->main_video_image->allocation.height;

  gdk_window_invalidate_rect (GDK_WINDOW (main_window->window), &rect , TRUE);
}


void 
gm_main_window_set_call_hold (GtkWidget *main_window,
                              bool is_on_hold)
{
  GmMainWindow *mw = NULL;
  
  GtkWidget *child = NULL;
  
  
  g_return_if_fail (main_window != NULL);
  
  mw = gm_mw_get_mw (main_window);
  
  g_return_if_fail (mw != NULL);
  
  
  child = GTK_BIN (gtk_menu_get_widget (mw->main_menu, "hold_call"))->child;

  if (is_on_hold) {

    if (GTK_IS_LABEL (child))
      gtk_label_set_text_with_mnemonic (GTK_LABEL (child),
					_("_Retrieve Call"));

    /* Set the audio and video menu to unsensitive */
    gtk_menu_set_sensitive (mw->main_menu, "suspend_audio", FALSE);
    gtk_menu_set_sensitive (mw->main_menu, "suspend_video", FALSE);
    
    gm_main_window_set_channel_pause (main_window, TRUE, FALSE);
    gm_main_window_set_channel_pause (main_window, TRUE, TRUE);
  }
  else {

    if (GTK_IS_LABEL (child))
      gtk_label_set_text_with_mnemonic (GTK_LABEL (child),
					_("_Hold Call"));

    gtk_menu_set_sensitive (mw->main_menu, "suspend_audio", TRUE);
    gtk_menu_set_sensitive (mw->main_menu, "suspend_video", TRUE);

    gm_main_window_set_channel_pause (main_window, FALSE, FALSE);
    gm_main_window_set_channel_pause (main_window, FALSE, TRUE);
  }
  
  g_signal_handlers_block_by_func (G_OBJECT (mw->hold_button),
                                   (gpointer) hold_current_call_cb,
                                   main_window);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (mw->hold_button), 
                                is_on_hold);
  g_signal_handlers_unblock_by_func (G_OBJECT (mw->hold_button),
                                     (gpointer) hold_current_call_cb,
                                     main_window);
}


void 
gm_main_window_set_channel_pause (GtkWidget *main_window,
				  gboolean pause,
				  gboolean is_video)
{
  GmMainWindow *mw = NULL;
  
  GtkWidget *child = NULL;

  gchar *msg = NULL;
  
  g_return_if_fail (main_window != NULL);
  
  mw = gm_mw_get_mw (main_window);
  
  g_return_if_fail (mw != NULL);
  

  if (!pause && !is_video)
    msg = g_strdup (_("Suspend _Audio"));
  else if (!pause && is_video)
    msg = g_strdup (_("Suspend _Video"));
  else if (pause && !is_video)
    msg = g_strdup (_("Resume _Audio"));
  else if (pause && is_video)
    msg = g_strdup (_("Resume _Video"));

  
  if (is_video) {
    
    child =
      GTK_BIN (gtk_menu_get_widget (mw->main_menu, "suspend_video"))->child;
  }
  else {
    
    child =
      GTK_BIN (gtk_menu_get_widget (mw->main_menu, "suspend_audio"))->child;
  }
	

  if (GTK_IS_LABEL (child)) 
    gtk_label_set_text_with_mnemonic (GTK_LABEL (child),
				      msg);

  g_free (msg);
}


void
gm_main_window_update_calling_state (GtkWidget *main_window,
				     unsigned calling_state)
{
  GmMainWindow *mw = NULL;
  
  g_return_if_fail (main_window != NULL);

  mw = gm_mw_get_mw (main_window);

  g_return_if_fail (mw!= NULL);

  gm_main_window_selected_presentity_build_menu (main_window);
  switch (calling_state)
    {
    case Standby:
      
      /* Update the hold state */
      gm_main_window_set_call_hold (main_window, FALSE);

      /* Update the sensitivity, all channels are closed */
      gm_main_window_update_sensitivity (main_window, TRUE, FALSE, FALSE);
      gm_main_window_update_sensitivity (main_window, FALSE, FALSE, FALSE);
      
      /* Update the menus and toolbar items */
      gtk_menu_set_sensitive (mw->main_menu, "connect", TRUE);
      gtk_menu_set_sensitive (mw->main_menu, "disconnect", FALSE);
      gtk_menu_section_set_sensitive (mw->main_menu, "hold_call", FALSE);
      gtk_widget_set_sensitive (GTK_WIDGET (mw->hold_button), FALSE);
      gtk_widget_set_sensitive (GTK_WIDGET (mw->preview_button), TRUE);
      
      /* Update the connect button */
      gm_connect_button_set_connected (GM_CONNECT_BUTTON (mw->connect_button),
				       FALSE);
	
      /* Destroy the transfer call popup */
      if (mw->transfer_call_popup) 
	gtk_dialog_response (GTK_DIALOG (mw->transfer_call_popup),
			     GTK_RESPONSE_REJECT);

      break;


    case Calling:

      /* Update the menus and toolbar items */
      gtk_menu_set_sensitive (mw->main_menu, "connect", FALSE);
      gtk_menu_set_sensitive (mw->main_menu, "disconnect", TRUE);
      gtk_widget_set_sensitive (GTK_WIDGET (mw->preview_button), FALSE);

      /* Update the connect button */
      gm_connect_button_set_connected (GM_CONNECT_BUTTON (mw->connect_button),
				       TRUE);

      break;


    case Connected:

      /* Update the menus and toolbar items */
      gtk_menu_set_sensitive (mw->main_menu, "connect", FALSE);
      gtk_menu_set_sensitive (mw->main_menu, "disconnect", TRUE);
      gtk_menu_section_set_sensitive (mw->main_menu, "hold_call", TRUE);
      gtk_widget_set_sensitive (GTK_WIDGET (mw->hold_button), TRUE);
      gtk_widget_set_sensitive (GTK_WIDGET (mw->preview_button), FALSE);

      /* Update the connect button */
      gm_connect_button_set_connected (GM_CONNECT_BUTTON (mw->connect_button),
				       TRUE);

      break;


    case Called:

      /* Update the menus and toolbar items */
      gtk_menu_set_sensitive (mw->main_menu, "disconnect", TRUE);

      /* Update the connect button */
      gm_connect_button_set_connected (GM_CONNECT_BUTTON (mw->connect_button),
				       TRUE);

      break;

    default:
      break;
    }

  mw->calling_state = calling_state;
}


void
gm_main_window_update_sensitivity (GtkWidget *main_window,
				   bool is_video,
				   bool /*is_receiving*/,
				   bool is_transmitting)
{
  GmMainWindow *mw = NULL;
  
  mw = gm_mw_get_mw (main_window);

  g_return_if_fail (mw != NULL);

  if (is_transmitting) {

    if (!is_video) 
      gtk_menu_set_sensitive (mw->main_menu, "suspend_audio", TRUE);
    else 
      gtk_menu_set_sensitive (mw->main_menu, "suspend_video", TRUE);
  }	
  else {

    if (!is_video)
      gtk_menu_set_sensitive (mw->main_menu, "suspend_audio", FALSE);
    else
      gtk_menu_set_sensitive (mw->main_menu, "suspend_video", FALSE);

  }
}


void
gm_main_window_toggle_fullscreen (Ekiga::VideoOutputFSToggle toggle,
                                  GtkWidget   *main_window)
{
  GmMainWindow *mw = gm_mw_get_mw (main_window);
  g_return_if_fail (mw != NULL);

  Ekiga::VideoOutputMode videooutput_mode;

  switch (toggle) {
    case Ekiga::VO_FS_OFF:
      if (gm_conf_get_int (VIDEO_DISPLAY_KEY "video_view") == Ekiga::VO_MODE_FULLSCREEN) {

        videooutput_mode = (Ekiga::VideoOutputMode) gm_conf_get_int (VIDEO_DISPLAY_KEY "video_view_before_fullscreen");
        gm_conf_set_int (VIDEO_DISPLAY_KEY "video_view", videooutput_mode);
      }
      break;
    case Ekiga::VO_FS_ON:
      if (gm_conf_get_int (VIDEO_DISPLAY_KEY "video_view") != Ekiga::VO_MODE_FULLSCREEN) {

        videooutput_mode = (Ekiga::VideoOutputMode) gm_conf_get_int (VIDEO_DISPLAY_KEY "video_view");
        gm_conf_set_int (VIDEO_DISPLAY_KEY "video_view_before_fullscreen", videooutput_mode);
        gm_conf_set_int (VIDEO_DISPLAY_KEY "video_view", Ekiga::VO_MODE_FULLSCREEN);
      }
      break;

    case Ekiga::VO_FS_TOGGLE:
    default:
      if (gm_conf_get_int (VIDEO_DISPLAY_KEY "video_view") == Ekiga::VO_MODE_FULLSCREEN) {

        videooutput_mode = (Ekiga::VideoOutputMode) gm_conf_get_int (VIDEO_DISPLAY_KEY "video_view_before_fullscreen");
        gm_conf_set_int (VIDEO_DISPLAY_KEY "video_view", videooutput_mode);
      }
      else {

        videooutput_mode =  (Ekiga::VideoOutputMode) gm_conf_get_int (VIDEO_DISPLAY_KEY "video_view");
        gm_conf_set_int (VIDEO_DISPLAY_KEY "video_view_before_fullscreen", videooutput_mode);
        gm_conf_set_int (VIDEO_DISPLAY_KEY "video_view", Ekiga::VO_MODE_FULLSCREEN);
      }
      break;
  }
}


static void 
gm_main_window_show_call_panel (GtkWidget *self)
{
  GmMainWindow *mw = NULL;

  mw = gm_mw_get_mw (GTK_WIDGET (self));

  if (!GTK_WIDGET_VISIBLE (gtk_paned_get_child2 (GTK_PANED (mw->hpaned)))) {
    gtk_widget_show_all (gtk_paned_get_child2 (GTK_PANED (mw->hpaned)));
  }
}


static void 
gm_main_window_hide_call_panel (GtkWidget *self)
{
  GmMainWindow *mw = NULL;
  GtkRequisition req;
  int x, y = 0;

  mw = gm_mw_get_mw (GTK_WIDGET (self));

  if (GTK_WIDGET_VISIBLE (gtk_paned_get_child2 (GTK_PANED (mw->hpaned)))) {

    gtk_widget_size_request (gtk_paned_get_child2 (GTK_PANED (mw->hpaned)), &req);
    gtk_window_get_size (GTK_WINDOW (self), &x, &y);
    gtk_widget_hide (gtk_paned_get_child2 (GTK_PANED (mw->hpaned)));
    x = x - req.width;
    gtk_window_resize (GTK_WINDOW (self), x, y);
  }
}


void
gm_main_window_set_busy (GtkWidget *main_window,
			 bool busy)
{
  GmMainWindow *mw = NULL;
  
  GdkCursor *cursor = NULL;

  mw = gm_mw_get_mw (main_window);

  g_return_if_fail (mw != NULL);

  gtk_widget_set_sensitive (mw->main_toolbar, !busy);
  gtk_widget_set_sensitive (mw->main_menu, !busy);

  if (busy) {

    cursor = gdk_cursor_new (GDK_WATCH);
    gdk_window_set_cursor (GTK_WIDGET (main_window)->window, cursor);
    gdk_cursor_unref (cursor);
  }
  else
    gdk_window_set_cursor (GTK_WIDGET (main_window)->window, NULL);
}

void
gm_main_window_clear_signal_levels (GtkWidget *main_window)
{
  GmMainWindow *mw = NULL;

  g_return_if_fail (main_window != NULL);

  mw = gm_mw_get_mw (main_window);

  g_return_if_fail (mw != NULL);

  gtk_levelmeter_clear (GTK_LEVELMETER (mw->output_signal));
  gtk_levelmeter_clear (GTK_LEVELMETER (mw->input_signal));
}

void 
gm_main_window_selected_presentity_build_menu (GtkWidget *main_window)
{
  GmMainWindow *mw = NULL;
  GtkWidget* menu = NULL;

  g_return_if_fail (main_window != NULL);

  mw = gm_mw_get_mw (main_window);

  g_return_if_fail (mw != NULL);

  menu = gtk_menu_get_widget (mw->main_menu, "contact");
  if (mw->presentity != NULL) {

    MenuBuilderGtk builder;
    gtk_widget_set_sensitive (menu, TRUE);
    if (mw->presentity->populate_menu (builder)) {

      gtk_menu_item_set_submenu (GTK_MENU_ITEM (menu), builder.menu);
      gtk_widget_show_all (builder.menu);
    } 
    else {

      gtk_widget_set_sensitive (menu, FALSE);
      g_object_unref (builder.menu);
    }
  } 
  else {

    gtk_widget_set_sensitive (menu, FALSE);
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (menu), NULL);
  }
}


void gm_main_window_incoming_call_dialog_show (GtkWidget *main_window,
                                               Ekiga::Call & call)
{
  GmMainWindow *mw = NULL;
  
  GdkPixbuf *pixbuf = NULL;

  GtkWidget *label = NULL;
  GtkWidget *vbox = NULL;
  GtkWidget *b1 = NULL;
  GtkWidget *b2 = NULL;
  GtkWidget *incoming_call_popup = NULL;

  gchar *msg = NULL;

  // FIXME could the call become invalid ?
  const char *utf8_name = call.get_remote_party_name ().c_str ();
  const char *utf8_app = call.get_remote_application ().c_str ();
  const char *utf8_url = call.get_remote_uri ().c_str ();
  const char *utf8_local = call.get_local_party_name ().c_str ();

  g_return_if_fail (main_window);
  
  mw = gm_mw_get_mw (main_window);
  
  g_return_if_fail (mw != NULL);

  incoming_call_popup = gtk_dialog_new ();
  b2 = gtk_dialog_add_button (GTK_DIALOG (incoming_call_popup),
			      _("Reject"), 0);
  b1 = gtk_dialog_add_button (GTK_DIALOG (incoming_call_popup),
			      _("Accept"), 2);

  gtk_dialog_set_default_response (GTK_DIALOG (incoming_call_popup), 2);

  vbox = GTK_DIALOG (incoming_call_popup)->vbox;

  msg = g_strdup_printf ("%s <i>%s</i>", _("Incoming call from"), (const char*) utf8_name);
  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label), msg);
  gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 10);
  gtk_misc_set_alignment (GTK_MISC (label), 0.5, 0.0);
  gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);
  g_free (msg);

  pixbuf = gtk_widget_render_icon (GTK_WIDGET (incoming_call_popup),
				   GM_STOCK_PHONE_PICK_UP_16,
				   GTK_ICON_SIZE_MENU, NULL);
  gtk_window_set_icon (GTK_WINDOW (incoming_call_popup), pixbuf);
  g_object_unref (pixbuf);

  if (utf8_url) {
    
    label = gtk_label_new (NULL);
    msg = g_strdup_printf ("<b>%s</b> <span foreground=\"blue\"><u>%s</u></span>",
                           _("Remote URI:"), utf8_url);
    gtk_label_set_markup (GTK_LABEL (label), msg);
    gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);
    gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 2);
    gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.0);
    g_free (msg);
  }

  if (utf8_app) {

    label = gtk_label_new (NULL);
    msg = g_strdup_printf ("<b>%s</b> %s",
			   _("Remote Application:"), utf8_app);
    gtk_label_set_markup (GTK_LABEL (label), msg);
    gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);
    gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 2);
    gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.0);
    g_free (msg);
  }

  if (utf8_local) {
    
    label = gtk_label_new (NULL);
    msg =
      g_strdup_printf ("<b>%s</b> %s",
		       _("Account ID:"), utf8_local);
    gtk_label_set_markup (GTK_LABEL (label), msg);
    gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);
    gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 2);
    gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.0);
    g_free (msg);
  }

  msg = g_strdup_printf (_("Call from %s"), (const char*) utf8_name);
  gtk_window_set_title (GTK_WINDOW (incoming_call_popup), msg);
  g_free (msg);
  gtk_window_set_modal (GTK_WINDOW (incoming_call_popup), TRUE);
  gtk_window_set_keep_above (GTK_WINDOW (incoming_call_popup), TRUE);
  gtk_window_set_urgency_hint (GTK_WINDOW (main_window), TRUE);
  gtk_window_set_transient_for (GTK_WINDOW (incoming_call_popup),
				GTK_WINDOW (main_window));

  gtk_widget_show_all (incoming_call_popup);

  g_signal_connect (G_OBJECT (incoming_call_popup), "delete_event",
                    G_CALLBACK (gtk_widget_hide_on_delete), NULL);
  g_signal_connect (G_OBJECT (incoming_call_popup), "response",
                    GTK_SIGNAL_FUNC (incoming_call_response_cb), &call);

  call.cleared.connect (sigc::bind (sigc::ptr_fun (on_cleared_incoming_call_cb),
                                    (gpointer) incoming_call_popup));
  call.missed.connect (sigc::bind (sigc::ptr_fun (on_missed_incoming_call_cb), 
                                   (gpointer) incoming_call_popup));
}


#ifdef HAVE_NOTIFY
static void
notify_action_cb (NotifyNotification *notification,
                  gchar *action,
                  gpointer data)
{
  Ekiga::Call *call = (Ekiga::Call *) data;

  notify_notification_close (notification, NULL);

  if (call) {

    if (!strcmp (action, "accept"))
      call->answer ();
    else
      call->hangup ();
  }
}


static void
closed_cb (NotifyNotification* /*notify*/, 
           gpointer main_window)
{
  GmMainWindow *mw = NULL;

  g_return_if_fail (main_window != NULL);

  mw = gm_mw_get_mw (GTK_WIDGET (main_window));

  g_return_if_fail (mw != NULL);

  Ekiga::AudioOutputCore *audiooutput_core = dynamic_cast<Ekiga::AudioOutputCore *> (mw->core.get ("audiooutput-core"));
  if (audiooutput_core) 
    audiooutput_core->stop_play_event ("incoming_call_sound");
}


void gm_main_window_incoming_call_notify (GtkWidget *main_window,
                                          Ekiga::Call & call)
{
  NotifyNotification *notify = NULL;
  
  GtkStatusIcon *statusicon = NULL;

  gchar *uri = NULL;
  gchar *app = NULL;
  gchar *account = NULL;
  gchar *body = NULL;
  gchar *title = NULL;

  statusicon = GTK_STATUS_ICON (GnomeMeeting::Process ()->GetStatusicon ());

  // FIXME could the call become invalid ?
  const char *utf8_name = call.get_remote_party_name ().c_str ();
  const char *utf8_app = call.get_remote_application ().c_str ();
  const char *utf8_url = call.get_remote_uri ().c_str ();
  const char *utf8_local = call.get_local_party_name ().c_str ();

  title = g_strdup_printf ("%s %s", _("Incoming call from"), (const char*) utf8_name);

  if (utf8_url)
    uri = g_strdup_printf ("<b>%s</b> %s", _("Remote URI:"), utf8_url);
  if (utf8_app)
    app = g_strdup_printf ("<b>%s</b> %s", _("Remote Application:"), utf8_app);
  if (utf8_local)
    account = g_strdup_printf ("<b>%s</b> %s", _("Account ID:"), utf8_local);

  body = g_strdup_printf ("%s\n%s\n%s", uri, app, account);
  
  notify = notify_notification_new (title, body, GM_ICON_LOGO, NULL);
  notify_notification_add_action (notify, "accept", _("Accept"), notify_action_cb, &call, NULL);
  notify_notification_add_action (notify, "reject", _("Reject"), notify_action_cb, &call, NULL);
  notify_notification_set_timeout (notify, NOTIFY_EXPIRES_NEVER);
  notify_notification_set_urgency (notify, NOTIFY_URGENCY_CRITICAL);
  notify_notification_attach_to_status_icon (notify, statusicon);
  if (!notify_notification_show (notify, NULL))
    gm_main_window_incoming_call_dialog_show (main_window, call);
  else {
    call.cleared.connect (sigc::bind (sigc::ptr_fun (on_cleared_incoming_call_cb),
                                      (gpointer) notify));
    call.missed.connect (sigc::bind (sigc::ptr_fun (on_missed_incoming_call_cb), 
                                     (gpointer) notify));
  }

  g_signal_connect (notify, "closed", G_CALLBACK (closed_cb), main_window);

  g_free (uri);
  g_free (app);
  g_free (account);
  g_free (title);
  g_free (body);
}
#endif

void 
gm_main_window_set_panel_section (GtkWidget *main_window,
                                  int section)
{
  GmMainWindow *mw = NULL;
  
  GtkWidget *menu = NULL;
  
  g_return_if_fail (main_window != NULL);
  
  mw = gm_mw_get_mw (main_window);

  g_return_if_fail (mw != NULL);

  gtk_notebook_set_current_page (GTK_NOTEBOOK (mw->main_notebook), section);
  
  menu = gtk_menu_get_widget (mw->main_menu, "dialpad");
  
  gtk_radio_menu_select_with_widget (GTK_WIDGET (menu), section);
}


void 
gm_main_window_set_status (GtkWidget *main_window,
                           std::string /*short_status*/,
                           std::string /*long_status*/)
{
  GmMainWindow *mw = NULL;
  
  /*
  const char * status [] = 
    { 
      _("Online"), 
      _("Away"), 
      _("Do Not Disturb"), 
      _("Invisible"),
      NULL, 
      NULL 
    };
*/
  g_return_if_fail (main_window != NULL);

  mw = gm_mw_get_mw (main_window);
  g_return_if_fail (mw != NULL);
}


void 
gm_main_window_set_call_info (GtkWidget *main_window,
			      const char *tr_audio_codec,
			      G_GNUC_UNUSED const char *re_audio_codec,
			      const char *tr_video_codec,
			      G_GNUC_UNUSED const char *re_video_codec)
{
  GmMainWindow *mw = NULL;

  GtkTextIter iter;
  GtkTextIter *end_iter = NULL;
  GtkTextBuffer *buffer = NULL;
  
  gchar *info = NULL;
  
  g_return_if_fail (main_window != NULL);
  
  mw = gm_mw_get_mw (main_window);

  g_return_if_fail (mw != NULL);

  if (!tr_audio_codec && !tr_video_codec)
    info = g_strdup (" ");
  else
    info = g_strdup_printf ("%s - %s",
                            tr_audio_codec?tr_audio_codec:"", 
                            tr_video_codec?tr_video_codec:"");
  
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (mw->info_text));
  gtk_text_buffer_get_start_iter (buffer, &iter);
  gtk_text_iter_forward_lines (&iter, 2);
  end_iter = gtk_text_iter_copy (&iter);
  gtk_text_iter_forward_line (end_iter);
  gtk_text_buffer_delete (buffer, &iter, end_iter);
  gtk_text_iter_free (end_iter);
  gtk_text_buffer_insert_with_tags_by_name (buffer, &iter, info, 
                                            -1, "codecs", NULL);
  g_free (info);
}


void 
gm_main_window_set_status (GtkWidget *main_window,
			   const char *status)
{
  GmMainWindow *mw = NULL;

  GtkTextIter iter;
  GtkTextIter* end_iter = NULL;
  GtkTextBuffer *buffer = NULL;

  gchar *info = NULL;
  
  g_return_if_fail (main_window != NULL);
  
  mw = gm_mw_get_mw (main_window);

  g_return_if_fail (mw != NULL);
  
  info = g_strdup_printf ("%s\n", status);
  
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (mw->info_text));
  gtk_text_buffer_get_start_iter (buffer, &iter);
  end_iter = gtk_text_iter_copy (&iter);
  gtk_text_iter_forward_line (end_iter);
  gtk_text_buffer_delete (buffer, &iter, end_iter);
  gtk_text_iter_free (end_iter);
  gtk_text_buffer_insert_with_tags_by_name (buffer, &iter, info, 
                                            -1, "status", NULL);
  g_free (info);
}


void 
gm_main_window_set_call_duration (GtkWidget *main_window,
                                  const char *duration)
{
  GmMainWindow *mw = NULL;

  GtkTextIter iter;
  GtkTextIter* end_iter = NULL;
  GtkTextBuffer *buffer = NULL;

  gchar *info = NULL;

  g_return_if_fail (main_window != NULL);
  
  mw = gm_mw_get_mw (main_window);

  g_return_if_fail (mw != NULL);
  
  if (duration)
    info = g_strdup_printf (_("Call Duration: %s\n"), duration);
  else
    info = g_strdup ("\n");
  
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (mw->info_text));
  gtk_text_buffer_get_start_iter (buffer, &iter);
  gtk_text_iter_forward_line (&iter);
  end_iter = gtk_text_iter_copy (&iter);
  gtk_text_iter_forward_line (end_iter);
  gtk_text_buffer_delete (buffer, &iter, end_iter);
  gtk_text_iter_free (end_iter);
  gtk_text_buffer_insert_with_tags_by_name (buffer, &iter, info, 
                                            -1, "call-duration", NULL);

  g_free (info);
}


gboolean 
gm_main_window_transfer_dialog_run (GtkWidget *main_window,
				    GtkWidget *parent_window,
				    const char *u)
{
  GmMainWindow *mw = NULL;
  
  gint answer = 0;
  
  const char *forward_url = NULL;

  g_return_val_if_fail (main_window != NULL, FALSE);
  g_return_val_if_fail (parent_window != NULL, FALSE);
  
  mw = gm_mw_get_mw (main_window);

  g_return_val_if_fail (mw != NULL, FALSE);
  
  mw->transfer_call_popup = 
    gm_entry_dialog_new (_("Transfer call to:"),
			 _("Transfer"));
  
  gtk_window_set_transient_for (GTK_WINDOW (mw->transfer_call_popup),
				GTK_WINDOW (parent_window));
  
  gtk_dialog_set_default_response (GTK_DIALOG (mw->transfer_call_popup),
				   GTK_RESPONSE_ACCEPT);
  
  if (u && !strcmp (u, ""))
    gm_entry_dialog_set_text (GM_ENTRY_DIALOG (mw->transfer_call_popup), u);
  else
    gm_entry_dialog_set_text (GM_ENTRY_DIALOG (mw->transfer_call_popup), "sip:");

  gnomemeeting_threads_dialog_show (mw->transfer_call_popup);

  answer = gtk_dialog_run (GTK_DIALOG (mw->transfer_call_popup));
  switch (answer) {

  case GTK_RESPONSE_ACCEPT:

    forward_url = gm_entry_dialog_get_text (GM_ENTRY_DIALOG (mw->transfer_call_popup));
    if (strcmp (forward_url, "") && mw->current_call)
      mw->current_call->transfer (forward_url);
    break;

  default:
    break;
  }

  gtk_widget_destroy (mw->transfer_call_popup);
  mw->transfer_call_popup = NULL;

  return (answer == GTK_RESPONSE_ACCEPT);
}


void 
gm_main_window_add_device_dialog_show (GtkWidget *main_window,
                                       const Ekiga::Device & device,
                                       DeviceType deviceType)
{
  GmMainWindow *mw = NULL;
  
  GtkWidget *label = NULL;
  GtkWidget *vbox = NULL;
  GtkWidget *b1 = NULL;
  GtkWidget *b2 = NULL;
  GtkWidget *add_device_popup = NULL;

  g_return_if_fail (main_window);
  mw = gm_mw_get_mw (main_window);
  g_return_if_fail (mw != NULL);


  add_device_popup = gtk_dialog_new ();
  b2 = gtk_dialog_add_button (GTK_DIALOG (add_device_popup),
			      _("No"), 0);
  b1 = gtk_dialog_add_button (GTK_DIALOG (add_device_popup),
			      _("Yes"), 2);

  gtk_dialog_set_default_response (GTK_DIALOG (add_device_popup), 2);

  vbox = GTK_DIALOG (add_device_popup)->vbox;

  std::string msg;
  std::string title;

  switch (deviceType) {
    case AudioInput:
      msg = _("Detected new audio input device:");
      title = _("Audio Devices");
      break;
    case AudioOutput:
      msg = _("Detected new audio output device:");
      title = _("Audio Devices");
      break;
    case VideoInput:
      msg = _("Detected new video input device:");
      title = _("Video Devices");
      break;
    default:
      break;
  }
  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label), msg.c_str());
  gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 2);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.0);
  gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);
  
  msg  = "<b>" + device.GetString() + "</b>";
  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label), msg.c_str());
  gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 2);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.0);
  gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);

  msg  = _("Do you want to use it as default device?");
  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label), msg.c_str());
  gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 2);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.0);
  gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);

  gtk_window_set_title (GTK_WINDOW (add_device_popup), title.c_str ());
  gtk_window_set_modal (GTK_WINDOW (add_device_popup), TRUE);
  gtk_window_set_keep_above (GTK_WINDOW (add_device_popup), TRUE);
  gtk_window_set_urgency_hint (GTK_WINDOW (main_window), TRUE);
  gtk_window_set_transient_for (GTK_WINDOW (add_device_popup),
				GTK_WINDOW (main_window));

  gtk_widget_show_all (add_device_popup);


//  g_signal_connect (G_OBJECT (add_device_popup), "delete_event",
//                    G_CALLBACK (gtk_widget_hide_on_delete), NULL);
//  g_signal_connect (G_OBJECT (add_device_popup), "response",
//                    GTK_SIGNAL_FUNC (add_device_response_cb), &device);

  deviceStruct* device_struct = g_new(deviceStruct, 1);
  snprintf (device_struct->name, sizeof (device_struct->name), "%s", (device.GetString()).c_str());
  device_struct->deviceType = deviceType;

  g_signal_connect_data (G_OBJECT (add_device_popup), "delete_event",
                         G_CALLBACK (gtk_widget_hide_on_delete), 
                         (gpointer) device_struct,
                         (GClosureNotify) g_free,
                         (GConnectFlags) 0);

  g_signal_connect_data (G_OBJECT (add_device_popup), "response",
                         G_CALLBACK (add_device_response_cb), 
                         (gpointer) device_struct,
                         (GClosureNotify) g_free,
                         (GConnectFlags) 0);
}

GtkWidget *
gm_main_window_new (Ekiga::ServiceCore & core)
{
  GmMainWindow *mw = NULL;

  GtkWidget *window = NULL;
  
  GtkWidget *status_toolbar = NULL;

  GtkStatusIcon *status_icon = NULL;

  PanelSection section = DIALPAD;

  sigc::connection conn;

  Ekiga::ServiceCore *services = NULL;
  GtkFrontend *gtk_frontend = NULL;
  GtkWidget *chat_window = NULL;

  /* initialize the callback to play IM message sound */
  services = GnomeMeeting::Process ()->GetServiceCore ();
  gtk_frontend = dynamic_cast<GtkFrontend *>(services->get ("gtk-frontend"));
  chat_window = GTK_WIDGET (gtk_frontend->get_chat_window ());

  g_signal_connect (chat_window, "unread-alert",
		    G_CALLBACK (on_chat_unread_alert), NULL);

  /* The Top-level window */
  window = gm_window_new_with_key (USER_INTERFACE_KEY "main_window");
  gm_window_set_hide_on_delete (GM_WINDOW (window), FALSE);
  gtk_window_set_title (GTK_WINDOW (window), 
			_("Ekiga"));

  g_signal_connect (G_OBJECT (window), "focus-in-event",
		    GTK_SIGNAL_FUNC (main_window_focus_event_cb), NULL);


  /* The GMObject data */
  mw = new GmMainWindow (core);
  mw->presentity = NULL;
  mw->transfer_call_popup = NULL;
  mw->current_call = NULL;
  mw->timeout_id = -1;
  mw->levelmeter_timeout_id = -1;
  mw->audio_transmission_active = mw->audio_reception_active 
    = mw->video_transmission_active = mw->video_reception_active = false;
  g_object_set_data_full (G_OBJECT (window), "GMObject", 
			  mw, (GDestroyNotify) gm_mw_destroy);

#ifndef WIN32
  mw->video_widget_gc = NULL;
#endif

  /* Accelerators */
  mw->accel = gtk_accel_group_new ();
  gtk_window_add_accel_group (GTK_WINDOW (window), mw->accel);
  g_object_unref (mw->accel);

  mw->window_vbox = gtk_vbox_new (0, FALSE);
  gtk_container_add (GTK_CONTAINER (window), mw->window_vbox);
  gtk_widget_show_all (mw->window_vbox);

  /* The main menu */
  mw->statusbar = gm_statusbar_new ();
  gm_mw_init_menu (window); 
  gtk_box_pack_start (GTK_BOX (mw->window_vbox), mw->main_menu,
                      FALSE, FALSE, 0);

  /* The toolbars */
  mw->main_toolbar = gm_mw_init_uri_toolbar (window);
  gtk_box_pack_start (GTK_BOX (mw->window_vbox), mw->main_toolbar, false, false, 0); 

  /* The Audio & Video Settings windows */
  mw->audio_settings_window = gm_mw_audio_settings_window_new (window);
  mw->video_settings_window = gm_mw_video_settings_window_new (window);

  /* The 2 parts of the gui */
  mw->hpaned = gtk_hpaned_new ();
  gtk_box_pack_start (GTK_BOX (mw->window_vbox), mw->hpaned,
                      true, true, 0);

  mw->main_notebook = gtk_notebook_new ();

  gtk_notebook_popup_enable (GTK_NOTEBOOK (mw->main_notebook));
  gtk_notebook_set_show_tabs (GTK_NOTEBOOK (mw->main_notebook), true);
  gtk_notebook_set_scrollable (GTK_NOTEBOOK (mw->main_notebook), true);

  gm_mw_init_contacts_list (window);
  gm_mw_init_dialpad (window);
  gtk_paned_pack1 (GTK_PANED (mw->hpaned), mw->main_notebook, true, false);

  gm_mw_init_call (window);
  gm_mw_init_history (window);

  section = (PanelSection) 
    gm_conf_get_int (USER_INTERFACE_KEY "main_window/panel_section");
  gtk_widget_show (mw->hpaned);
  gtk_widget_show_all (GTK_WIDGET (gtk_paned_get_child1 (GTK_PANED (mw->hpaned))));
  if (gm_conf_get_bool (USER_INTERFACE_KEY "main_window/show_call_panel"))
    gtk_widget_show_all (GTK_WIDGET (gtk_paned_get_child2 (GTK_PANED (mw->hpaned))));
  gm_main_window_set_panel_section (window, section);

  /* Status toolbar */
  status_toolbar = gm_mw_init_status_toolbar (window);
  gtk_box_pack_start (GTK_BOX (mw->window_vbox), status_toolbar, false, false, 0);

  /* The statusbar with qualitymeter */
  gtk_statusbar_set_has_resize_grip (GTK_STATUSBAR (mw->statusbar), TRUE);
  GtkShadowType shadow_type;
  gtk_widget_style_get (mw->statusbar, "shadow-type", &shadow_type, NULL);

  GtkWidget *frame = gtk_frame_new (NULL);
  gtk_frame_set_shadow_type (GTK_FRAME (frame), shadow_type);
  gtk_box_pack_start (GTK_BOX (mw->statusbar), frame, FALSE, TRUE, 0);

  gtk_box_reorder_child (GTK_BOX (mw->statusbar), frame, 0);

  mw->qualitymeter = gm_powermeter_new ();
  gtk_container_add (GTK_CONTAINER (frame), mw->qualitymeter);

  mw->statusbar_ebox = gtk_event_box_new ();
  gtk_container_add (GTK_CONTAINER (mw->statusbar_ebox), mw->statusbar);

  gtk_box_pack_start (GTK_BOX (mw->window_vbox), mw->statusbar_ebox,
                      FALSE, FALSE, 0);
  gtk_widget_show_all (mw->statusbar_ebox);

  g_signal_connect (G_OBJECT (mw->statusbar_ebox), "button-press-event",
		    GTK_SIGNAL_FUNC (statusbar_clicked_cb), window);
 
  gtk_widget_realize (window);
  gm_main_window_update_logo_have_window (window);
  g_signal_connect_after (G_OBJECT (mw->main_notebook), "switch-page",
			  G_CALLBACK (panel_section_changed_cb), 
			  window);

  /* if the user tries to close the window : delete_event */
  g_signal_connect (G_OBJECT (window), "delete_event",
		    G_CALLBACK (window_closed_cb), 
		    (gpointer) window);
  g_signal_connect (G_OBJECT (window), "show", 
		    GTK_SIGNAL_FUNC (video_window_shown_cb), window);
  g_signal_connect (G_OBJECT (window), "expose-event", 
		    GTK_SIGNAL_FUNC (video_window_expose_cb), NULL);
  
  /* New Display Engine signals */
  Ekiga::VideoOutputCore *videooutput_core = dynamic_cast<Ekiga::VideoOutputCore *> (mw->core.get ("videooutput-core"));

  conn = videooutput_core->device_opened.connect (sigc::bind (sigc::ptr_fun (on_videooutput_device_opened_cb), (gpointer) window));
  mw->connections.push_back (conn);

  conn = videooutput_core->device_closed.connect (sigc::bind (sigc::ptr_fun (on_videooutput_device_closed_cb), (gpointer) window));
  mw->connections.push_back (conn);

  conn = videooutput_core->device_error.connect (sigc::bind (sigc::ptr_fun (on_videooutput_device_error_cb), (gpointer) window));
  mw->connections.push_back (conn);

  conn = videooutput_core->size_changed.connect (sigc::bind (sigc::ptr_fun (on_size_changed_cb), (gpointer) window));
  mw->connections.push_back (conn);

  conn = videooutput_core->fullscreen_mode_changed.connect (sigc::bind (sigc::ptr_fun (on_fullscreen_mode_changed_cb), (gpointer) window));
  mw->connections.push_back (conn);

  /* New VideoInput Engine signals */
  Ekiga::VideoInputCore *videoinput_core = dynamic_cast<Ekiga::VideoInputCore *> (mw->core.get ("videoinput-core"));

  conn = videoinput_core->device_opened.connect (sigc::bind (sigc::ptr_fun (on_videoinput_device_opened_cb), (gpointer) window));
  mw->connections.push_back (conn);

  conn = videoinput_core->device_closed.connect (sigc::bind (sigc::ptr_fun (on_videoinput_device_closed_cb), (gpointer) window));
  mw->connections.push_back (conn);

  conn = videoinput_core->device_added.connect (sigc::bind (sigc::ptr_fun (on_videoinput_device_added_cb), (gpointer) window));
  mw->connections.push_back (conn);

  conn = videoinput_core->device_removed.connect (sigc::bind (sigc::ptr_fun (on_videoinput_device_removed_cb), (gpointer) window));
  mw->connections.push_back (conn);

  conn = videoinput_core->device_error.connect (sigc::bind (sigc::ptr_fun (on_videoinput_device_error_cb), (gpointer) window));
  mw->connections.push_back (conn);

  /* New AudioInput Engine signals */
  Ekiga::AudioInputCore *audioinput_core = dynamic_cast<Ekiga::AudioInputCore *> (mw->core.get ("audioinput-core"));

  conn = audioinput_core->device_opened.connect (sigc::bind (sigc::ptr_fun (on_audioinput_device_opened_cb), (gpointer) window));
  mw->connections.push_back (conn);

  conn = audioinput_core->device_closed.connect (sigc::bind (sigc::ptr_fun (on_audioinput_device_closed_cb), (gpointer) window));
  mw->connections.push_back (conn);

  conn = audioinput_core->device_added.connect (sigc::bind (sigc::ptr_fun (on_audioinput_device_added_cb), (gpointer) window));
  mw->connections.push_back (conn);

  conn = audioinput_core->device_removed.connect (sigc::bind (sigc::ptr_fun (on_audioinput_device_removed_cb), (gpointer) window));
  mw->connections.push_back (conn);

  conn = audioinput_core->device_error.connect (sigc::bind (sigc::ptr_fun (on_audioinput_device_error_cb), (gpointer) window));
  mw->connections.push_back (conn);

  /* New AudioOutput Engine signals */
  Ekiga::AudioOutputCore *audiooutput_core = dynamic_cast<Ekiga::AudioOutputCore *> (mw->core.get ("audiooutput-core"));

  conn = audiooutput_core->device_opened.connect (sigc::bind (sigc::ptr_fun (on_audiooutput_device_opened_cb), (gpointer) window));
  mw->connections.push_back (conn);

  conn = audiooutput_core->device_closed.connect (sigc::bind (sigc::ptr_fun (on_audiooutput_device_closed_cb), (gpointer) window));
  mw->connections.push_back (conn);

  conn = audiooutput_core->device_added.connect (sigc::bind (sigc::ptr_fun (on_audiooutput_device_added_cb), (gpointer) window));
  mw->connections.push_back (conn);

  conn = audiooutput_core->device_removed.connect (sigc::bind (sigc::ptr_fun (on_audiooutput_device_removed_cb), (gpointer) window));
  mw->connections.push_back (conn);

  conn = audiooutput_core->device_error.connect (sigc::bind (sigc::ptr_fun (on_audiooutput_device_error_cb), (gpointer) window));
  mw->connections.push_back (conn);
    
  /* New Call Engine signals */
  Ekiga::CallCore *call_core = dynamic_cast<Ekiga::CallCore *> (mw->core.get ("call-core"));
  Ekiga::AccountCore *account_core = dynamic_cast<Ekiga::AccountCore *> (mw->core.get ("account-core"));

  /* Engine Signals callbacks */
  conn = account_core->registration_event.connect (sigc::bind (sigc::ptr_fun (on_registration_event), (gpointer) window));
  mw->connections.push_back (conn);

  conn = call_core->ready.connect (sigc::bind (sigc::ptr_fun (on_ready_cb), (gpointer) window));
  mw->connections.push_back (conn);

  conn = call_core->setup_call.connect (sigc::bind (sigc::ptr_fun (on_setup_call_cb), (gpointer) window));
  mw->connections.push_back (conn);

  conn = call_core->ringing_call.connect (sigc::bind (sigc::ptr_fun (on_ringing_call_cb), (gpointer) window));
  mw->connections.push_back (conn);
  
  conn = call_core->established_call.connect (sigc::bind (sigc::ptr_fun (on_established_call_cb), (gpointer) window));
  mw->connections.push_back (conn);
  
  conn = call_core->cleared_call.connect (sigc::bind (sigc::ptr_fun (on_cleared_call_cb), (gpointer) window));
  mw->connections.push_back (conn);
  
  conn = call_core->held_call.connect (sigc::bind (sigc::ptr_fun (on_held_call_cb), (gpointer) window));
  mw->connections.push_back (conn);
  
  conn = call_core->retrieved_call.connect (sigc::bind (sigc::ptr_fun (on_retrieved_call_cb), (gpointer) window));
  mw->connections.push_back (conn);
  
  conn = call_core->missed_call.connect (sigc::bind (sigc::ptr_fun (on_missed_call_cb), (gpointer) window));
  mw->connections.push_back (conn);
  
  conn = call_core->stream_opened.connect (sigc::bind (sigc::ptr_fun (on_stream_opened_cb), (gpointer) window));
  mw->connections.push_back (conn);
  
  conn = call_core->stream_closed.connect (sigc::bind (sigc::ptr_fun (on_stream_closed_cb), (gpointer) window));
  mw->connections.push_back (conn);

  conn = call_core->stream_paused.connect (sigc::bind (sigc::ptr_fun (on_stream_paused_cb), (gpointer) window));
  mw->connections.push_back (conn);

  conn = call_core->stream_resumed.connect (sigc::bind (sigc::ptr_fun (on_stream_resumed_cb), (gpointer) window));
  mw->connections.push_back (conn);

  conn = call_core->errors.add_handler (sigc::bind (sigc::ptr_fun (on_handle_errors), (gpointer) window));
  mw->connections.push_back (conn);

  /* Notifiers */
  gm_conf_notifier_add (USER_INTERFACE_KEY "main_window/panel_section",
			panel_section_changed_nt, window);

  gm_conf_notifier_add (USER_INTERFACE_KEY "main_window/show_call_panel",
			show_call_panel_changed_nt, window);

  /* Track status icon embed changes */
  status_icon = GTK_STATUS_ICON (GnomeMeeting::Process ()->GetStatusicon ());
  g_signal_connect (G_OBJECT (status_icon), "notify::embedded",
		    G_CALLBACK (on_status_icon_embedding_change), NULL);

  /* Until we are ready, nothing possible  */
  gm_main_window_set_busy (window, true);

  return window;
}


void 
gm_main_window_flash_message (GtkWidget *main_window, 
			      const char *msg, 
			      ...)
{
  GmMainWindow *mw = NULL;

  char buffer [1025];

  g_return_if_fail (main_window != NULL);

  mw = gm_mw_get_mw (main_window);

  va_list args;

  va_start (args, msg);

  if (msg == NULL)
    buffer[0] = 0;
  else 
    vsnprintf (buffer, 1024, msg, args);

  gm_statusbar_flash_message (GM_STATUSBAR (mw->statusbar), "%s", buffer);
  va_end (args);
}


void 
gm_main_window_push_message (GtkWidget *main_window, 
			     const char *msg, 
			     ...)
{
  GmMainWindow *mw = NULL;

  char buffer [1025];

  g_return_if_fail (main_window != NULL);

  mw = gm_mw_get_mw (main_window);

  va_list args;

  va_start (args, msg);

  if (msg == NULL)
    buffer[0] = 0;
  else 
    vsnprintf (buffer, 1024, msg, args);

  gm_statusbar_push_message (GM_STATUSBAR (mw->statusbar), "%s", buffer);
  va_end (args);
}


void 
gm_main_window_push_info_message (GtkWidget *main_window, 
				  const char *msg, 
				  ...)
{
  GmMainWindow *mw = NULL;
  char *buffer;
  
  g_return_if_fail (main_window != NULL);

  mw = gm_mw_get_mw (main_window);

  va_list args;

  va_start (args, msg);
  buffer = g_strdup_vprintf (msg, args);
  gm_statusbar_push_info_message (GM_STATUSBAR (mw->statusbar), "%s", buffer);
  g_free (buffer);
  va_end (args);
}


void 
gm_main_window_set_call_url (GtkWidget *main_window, 
			     const char *url)
{
  GmMainWindow *mw = NULL;

  g_return_if_fail (main_window != NULL && url != NULL);

  mw = gm_mw_get_mw (main_window);

  g_return_if_fail (mw != NULL);

  gtk_entry_set_text (GTK_ENTRY (mw->entry), url);
  gtk_editable_set_position (GTK_EDITABLE (mw->entry), -1);
  gtk_widget_grab_focus (GTK_WIDGET (mw->entry));
  gtk_editable_select_region (GTK_EDITABLE (mw->entry), -1, -1);
}


void 
gm_main_window_append_call_url (GtkWidget *main_window, 
				const char *url)
{
  GmMainWindow *mw = NULL;
  
  int pos = -1;

  g_return_if_fail (main_window != NULL && url != NULL);

  mw = gm_mw_get_mw (main_window);

  g_return_if_fail (mw != NULL && url != NULL);
 
  if (gtk_editable_get_selection_bounds (GTK_EDITABLE (mw->entry), NULL, NULL)) 
    gtk_editable_delete_selection (GTK_EDITABLE (mw->entry));

  pos = gtk_editable_get_position (GTK_EDITABLE (mw->entry));
  gtk_editable_insert_text (GTK_EDITABLE (mw->entry), url, strlen (url), &pos);
  gtk_editable_select_region (GTK_EDITABLE (mw->entry), -1, -1);
  gtk_editable_set_position (GTK_EDITABLE (mw->entry), pos);
}


const char *
gm_main_window_get_call_url (GtkWidget *main_window)
{
  GmMainWindow *mw = NULL;

  g_return_val_if_fail (main_window != NULL, NULL);

  mw = gm_mw_get_mw (main_window);

  g_return_val_if_fail (mw != NULL, NULL);
 
  return gtk_entry_get_text (GTK_ENTRY (mw->entry));
}


void 
gm_main_window_clear_stats (GtkWidget *main_window)
{
  GmMainWindow *mw = NULL;

  g_return_if_fail (main_window != NULL);

  mw = gm_mw_get_mw (main_window);

  g_return_if_fail (mw != NULL);

  gm_main_window_update_stats (main_window, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  if (mw->qualitymeter)
    gm_powermeter_set_level (GM_POWERMETER (mw->qualitymeter), 0.0);
}


void 
gm_main_window_update_stats (GtkWidget *main_window,
			     float lost,
			     float late,
			     float out_of_order,
			     int jitter,
			     G_GNUC_UNUSED float new_video_octets_received,
			     G_GNUC_UNUSED float new_video_octets_transmitted,
			     G_GNUC_UNUSED float new_audio_octets_received,
			     G_GNUC_UNUSED float new_audio_octets_transmitted,
			     unsigned int re_width,
			     unsigned int re_height,
			     unsigned int tr_width,
			     unsigned int tr_height)
{
  GmMainWindow *mw = NULL;
  
  gchar *stats_msg = NULL;
  gchar *stats_msg_tr = NULL;
  gchar *stats_msg_re = NULL;

  int jitter_quality = 0;
  gfloat quality_level = 0.0;

  
  g_return_if_fail (main_window != NULL);

  mw = gm_mw_get_mw (main_window);

  g_return_if_fail (mw != NULL);

  if ((tr_width > 0) && (tr_height > 0))
    /* Translators:
     * TX is a common abbreviation for "transmit" */
    stats_msg_tr = g_strdup_printf (_("TX: %dx%d "), tr_width, tr_height);

  if ((re_width > 0) && (re_height > 0)) 
    /* Translators:
     * RX is a common abbreviation for "receive" */
    stats_msg_re = g_strdup_printf (_("RX: %dx%d "), re_width, re_height);

  stats_msg = g_strdup_printf (_("Lost packets: %.1f %%\nLate packets: %.1f %%\nOut of order packets: %.1f %%\nJitter buffer: %d ms%s%s%s"), 
                                  lost, 
                                  late, 
                                  out_of_order, 
                                  jitter,
                                  (stats_msg_tr || stats_msg_re) ? "\nResolution: " : "", 
                                  (stats_msg_tr) ? stats_msg_tr : "", 
                                  (stats_msg_re) ? stats_msg_re : "");

  g_free(stats_msg_tr);
  g_free(stats_msg_re);


  if (mw->statusbar_ebox) {
    gtk_widget_set_tooltip_text (GTK_WIDGET (mw->statusbar_ebox), stats_msg);
  }
  g_free (stats_msg);

    /* "arithmetics" for the quality level */
    /* Thanks Snark for the math hints */
    if (jitter < 30)
      jitter_quality = 100;
    if (jitter >= 30 && jitter < 50)
      jitter_quality = 100 - (jitter - 30);
    if (jitter >= 50 && jitter < 100)
      jitter_quality = 80 - (jitter - 50) * 20 / 50;
    if (jitter >= 100 && jitter < 150)
      jitter_quality = 60 - (jitter - 100) * 20 / 50;
    if (jitter >= 150 && jitter < 200)
      jitter_quality = 40 - (jitter - 150) * 20 / 50;
    if (jitter >= 200 && jitter < 300)
      jitter_quality = 20 - (jitter - 200) * 20 / 100;
    if (jitter >= 300 || jitter_quality < 0)
      jitter_quality = 0;

    quality_level = (float) jitter_quality / 100;

    if ( (lost > 0.0) ||
         (late > 0.0) ||
         ((out_of_order > 0.0) && quality_level > 0.2) ) {
      quality_level = 0.2;
    }

    if ( (lost > 0.02) ||
         (late > 0.02) ||
         (out_of_order > 0.02) ) {
      quality_level = 0;
    }

    if (mw->qualitymeter)
      gm_powermeter_set_level (GM_POWERMETER (mw->qualitymeter),
  			       quality_level);
}


GdkPixbuf *
gm_main_window_get_current_picture (GtkWidget *main_window)
{
  GmMainWindow *mw = NULL;
  
  g_return_val_if_fail (main_window != NULL, NULL);

  mw = gm_mw_get_mw (main_window);

  g_return_val_if_fail (mw != NULL, NULL);

  return gtk_image_get_pixbuf (GTK_IMAGE (mw->main_video_image));
}


void 
gm_main_window_set_stay_on_top (GtkWidget *main_window,
				gboolean stay_on_top)
{
  GmMainWindow *mw = NULL;
  
  GdkWindow *gm_window = NULL;

  g_return_if_fail (main_window != NULL);
  mw = gm_mw_get_mw (main_window);
  g_return_if_fail (mw != NULL);
  
  gm_window = GDK_WINDOW (main_window->window);

  /* Update the stay-on-top attribute */
  gdk_window_set_always_on_top (GDK_WINDOW (gm_window), stay_on_top);
}

/* The main () */
int 
main (int argc, 
      char ** argv, 
      char ** envp)
{
  GOptionContext *context = NULL;

  GtkWidget *main_window = NULL;
  GtkWidget *assistant_window = NULL;

  GtkWidget *dialog = NULL;
  
  gchar *path = NULL;
  gchar *url = NULL;
  gchar *msg = NULL;
  gchar *title = NULL;

  int debug_level = 0;
  int debug_level_up = 0;
  int error = -1;
#ifdef HAVE_GNOME
  GnomeProgram *program;
#endif

  /* Globals */
#ifndef WIN32
  if (!XInitThreads ())
    exit (1);
#endif

  /* PWLIB initialization */
  PProcess::PreInitialise (argc, argv, envp);
  
  /* GTK+ initialization */
  g_type_init ();
  g_thread_init (NULL);
//  gdk_threads_init ();
#ifndef WIN32
  signal (SIGPIPE, SIG_IGN);
#endif

  /* initialize platform-specific code */
  gm_platform_init ();

  /* Configuration backend initialization */
  gm_conf_init ();

  /* Gettext initialization */
  path = g_build_filename (DATA_DIR, "locale", NULL);
  textdomain (GETTEXT_PACKAGE);
  bindtextdomain (GETTEXT_PACKAGE, path);
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
  g_free (path);

  /* Arguments initialization */
  GOptionEntry arguments [] =
    {
      {
	"debug", 'd', 0, G_OPTION_ARG_INT, &debug_level, 
       N_("Prints debug messages in the console (level between 1 and 5)"), 
       NULL
      },
      {
	"debug_user_plane", 'u', 0, G_OPTION_ARG_INT, &debug_level_up, 
       N_("Prints user plane debug messages in the console (level between 1 and 4)"), 
       NULL
      },
      {
	"call", 'c', 0, G_OPTION_ARG_STRING, &url,
	N_("Makes Ekiga call the given URI"),
	NULL
      },
      {
	NULL, 0, 0, (GOptionArg)0, NULL,
	NULL,
	NULL
      }
    };
  context = g_option_context_new (NULL);
  g_option_context_add_main_entries (context, arguments, PACKAGE_NAME);
  g_option_context_set_help_enabled (context, TRUE);
  
  /* GNOME Initialisation */
#ifdef HAVE_GNOME
  program = gnome_program_init (PACKAGE_NAME, VERSION,
			        LIBGNOMEUI_MODULE, argc, argv,
			        GNOME_PARAM_GOPTION_CONTEXT, context,
			        GNOME_PARAM_HUMAN_READABLE_NAME, "ekiga",
			        GNOME_PARAM_APP_DATADIR, DATA_DIR,
			        (void *) NULL);
#else
  g_option_context_add_group (context, gtk_get_option_group (TRUE));
  g_option_context_parse (context, &argc, &argv, NULL);
  g_option_context_free (context);
#endif

#ifdef HAVE_NOTIFY
  notify_init (PACKAGE_NAME);
#endif

#ifndef WIN32
  char* text_label =  g_strdup_printf ("%d", debug_level);
  setenv ("PTLIB_TRACE_CODECS", text_label, TRUE);
  g_free (text_label);
  text_label =  g_strdup_printf ("%d", debug_level_up);
  setenv ("PTLIB_TRACE_CODECS_USER_PLANE", text_label, TRUE);
  g_free (text_label);
#else
  char* text_label =  g_strdup_printf ("PTLIB_TRACE_CODECS=%d", debug_level);
  _putenv (text_label);
  g_free (text_label);
  text_label =  g_strdup_printf ("PTLIB_TRACE_CODECS_USER_PLANE=%d", debug_level_up);
  _putenv (text_label);
  g_free (text_label);
#endif

  /* Ekiga initialisation */
  static GnomeMeeting instance;

  if (debug_level != 0)
    PTrace::Initialise (PMAX (PMIN (5, debug_level), 0), NULL,
			PTrace::Timestamp | PTrace::Thread
			| PTrace::Blocks | PTrace::DateAndTime);
#ifdef EKIGA_REVISION
  PTRACE(1, "Ekiga SVN revision: " << EKIGA_REVISION);
#endif

#ifdef HAVE_DBUS
  if (!ekiga_dbus_claim_ownership ()) {
    ekiga_dbus_client_show ();
    if (url != NULL)
      ekiga_dbus_client_connect (url);
    exit (0);
  }
#endif

  /* Configuration database initialization */
#ifdef HAVE_GCONF
  if (!gnomemeeting_conf_check ()) 
    error = 3;
#endif

  /* Init gm_conf */
  gm_conf_watch ();

  GnomeMeeting::Process ()->InitEngine ();
  GnomeMeeting::Process ()->BuildGUI ();
  GnomeMeeting::Process ()->DetectInterfaces ();
  
  /* Add depreciated notifiers */
  gnomemeeting_conf_init ();

  /* Show the window if there is no error, exit with a popup if there
   * is a fatal error.
   */
  main_window = GnomeMeeting::Process ()->GetMainWindow ();
  GmMainWindow *mw = gm_mw_get_mw (main_window); //TODO no gm_mw_get_mw here
  Ekiga::CallCore *call_core = dynamic_cast<Ekiga::CallCore *> (mw->core.get ("call-core"));
  if (error == -1) {

    if (gm_conf_get_int (GENERAL_KEY "version") 
        < 1000 * MAJOR_VERSION + 10 * MINOR_VERSION + BUILD_NUMBER) {

      gnomemeeting_conf_upgrade ();
      // Only show the assistant window if version older than 2.00
      if (gm_conf_get_int (GENERAL_KEY "version") < 2000) {
        assistant_window = GnomeMeeting::Process ()->GetAssistantWindow ();
        gtk_widget_show_all (assistant_window);
      }
      const int schema_version = MAJOR_VERSION * 1000
                               + MINOR_VERSION * 10
                               + BUILD_NUMBER;

      /* Update the version number */
      gm_conf_set_int (GENERAL_KEY "version", schema_version);
    }
    else {

      /* Show the main window */
      if (!gm_conf_get_bool (USER_INTERFACE_KEY "start_hidden")) 
        gtk_widget_show (main_window);
      else
        g_timeout_add (15000, (GtkFunction) gnomemeeting_tray_hack_cb, NULL);
    }

    /* Call the given host if needed */
    if (url) 
      call_core->dial (url);
  }
  else {

    switch (error) {

    case 1:
      title = g_strdup (_("No usable audio plugin detected"));
      msg = g_strdup (_("Ekiga didn't find any usable audio plugin. Make sure that your installation is correct."));
      break;
    case 2:
      title = g_strdup (_("No usable audio codecs detected"));
      msg = g_strdup (_("Ekiga didn't find any usable audio codec. Make sure that your installation is correct."));
      break;
#ifdef HAVE_GCONF
    case 3:
      {
	gchar *key_name = g_strdup ("\"/apps/" PACKAGE_NAME "/general/gconf_test_age\"");
	title = g_strdup (_("Configuration database corruption"));
	msg = g_strdup_printf (_("Ekiga got an invalid value for the configuration key %s.\n\nIt probably means that your configuration schemas have not been correctly installed or that the permissions are not correct.\n\nPlease check the FAQ (http://www.ekiga.org/), the troubleshooting section of the GConf site (http://www.gnome.org/projects/gconf/) or the mailing list archives for more information (http://mail.gnome.org) about this problem."), key_name);
	g_free (key_name);
      }
      break;
#endif
    default:
      break;
    }

    dialog = gtk_message_dialog_new (GTK_WINDOW (main_window), 
                                     GTK_DIALOG_MODAL, 
                                     GTK_MESSAGE_ERROR,
                                     GTK_BUTTONS_OK, NULL);

    gtk_window_set_title (GTK_WINDOW (dialog), title);
    gtk_label_set_markup (GTK_LABEL (GTK_MESSAGE_DIALOG (dialog)->label), msg);
  
    g_signal_connect (GTK_OBJECT (dialog), "response",
                      G_CALLBACK (quit_callback),
                      GTK_OBJECT (dialog));
    g_signal_connect_swapped (GTK_OBJECT (dialog), "response",
                              G_CALLBACK (gtk_widget_destroy),
                              GTK_OBJECT (dialog));
  
    gtk_widget_show_all (dialog);

    g_free (title);
    g_free (msg);
  }

#ifdef HAVE_DBUS
  /* Create the dbus server instance */
  EkigaDBusComponent *dbus_component = ekiga_dbus_component_new (&mw->core);
#endif

  /* The GTK loop */
  //gdk_threads_enter ();
  gtk_main ();
  //gdk_threads_leave ();

#ifdef HAVE_DBUS
  g_object_unref (dbus_component);
#endif

  /* Exit Ekiga */
  GnomeMeeting::Process ()->Exit ();

  /* Save and shutdown the configuration */
  gm_conf_save ();
  gm_conf_shutdown ();

#ifdef HAVE_GNOME
  g_object_unref (program);
#endif

  /* deinitialize platform-specific code */
  gm_platform_shutdown ();

#ifdef HAVE_NOTIFY
  notify_uninit ();
#endif

  return 0;
}


#ifdef WIN32

typedef struct {
  int newmode;
} _startupinfo;

extern "C" void __getmainargs (int *argcp, char ***argvp, char ***envp, int glob, _startupinfo *sinfo);
int 
APIENTRY WinMain (HINSTANCE hInstance,
		  HINSTANCE hPrevInstance,
		  LPSTR     lpCmdLine,
		  int       nCmdShow)
{
  HANDLE ekr_mutex;
  int iresult;
  char **env;
  char **argv;
  int argc;
  _startupinfo info = {0};

  ekr_mutex = CreateMutex (NULL, FALSE, "EkigaIsRunning");
  if (GetLastError () == ERROR_ALREADY_EXISTS)
    MessageBox (NULL, "Ekiga is running already !", "Ekiga - 2nd instance", MB_ICONEXCLAMATION | MB_OK);
  else {

    /* use msvcrt.dll to parse command line */
    __getmainargs (&argc, &argv, &env, 0, &info);

    std::freopen("stdout.txt", "w", stdout);
    std::freopen("stderr.txt", "w", stderr);

    iresult = main (argc, argv, env);
  }
  CloseHandle (ekr_mutex);
  return iresult;
}
#endif

