
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
 *                         config.cpp  -  description
 *                         --------------------------
 *   begin                : Wed Feb 14 2001
 *   copyright            : (C) 2000-2006 by Damien Sandras 
 *   description          : This file contains most of config stuff.
 *                          All notifiers are here.
 *                          Callbacks that updates the config cache 
 *                          are in their file, except some generic one that
 *                          are in this file.
 *   Additional code      : Miguel Rodríguez Pérez  <miguelrp@gmail.com>
 *
 */


#include "config.h"

#include "conf.h"

#include "manager.h"

#include "ekiga.h"
#include "preferences.h"
#include "assistant.h"
#include "accounts.h"
#include "main.h"
#include "misc.h"

#include "gmdialog.h"
#include "gmstockicons.h"
#include "gmmenuaddon.h"
#include "gmconfwidgets.h"



/* Declarations */
static void manager_changed_nt (gpointer id,
				GmConfEntry *entry,
				gpointer data);

static void sound_events_list_changed_nt (gpointer id,
					  GmConfEntry *entry,
					  gpointer data);

static void stay_on_top_changed_nt (gpointer id,
				    GmConfEntry *entry,
                                    gpointer data);

static void network_settings_changed_nt (gpointer id,
					 GmConfEntry *entry,
                                         gpointer data);


/* DESCRIPTION  :  This notifier is called when the config database data
 *                 associated with the audio or video manager changes.
 * BEHAVIOR     :  Updates the devices list for the new manager.
 * PRE          :  /
 */
static void
manager_changed_nt (G_GNUC_UNUSED gpointer id,
		    GmConfEntry *entry,
		    G_GNUC_UNUSED gpointer data)
{
  if (gm_conf_entry_get_type (entry) == GM_CONF_STRING) {
    //gdk_threads_enter ();
//     GnomeMeeting::Process ()->DetectDevices (); //FIXME
    //gdk_threads_leave ();
  }
}


/* DESCRIPTION  :  This callback is called when something changes in the sound
 *                 events list.
 * BEHAVIOR     :  It updates the events list widget.
 * PRE          :  A pointer to the prefs window GMObject.
 */
static void
sound_events_list_changed_nt (G_GNUC_UNUSED gpointer id,
			      GmConfEntry *entry,
			      G_GNUC_UNUSED gpointer data)
{
  GtkWidget *prefs_window;

  if (gm_conf_entry_get_type (entry) == GM_CONF_STRING
      || gm_conf_entry_get_type (entry) == GM_CONF_BOOL) {
   
    prefs_window = GnomeMeeting::Process ()->GetPrefsWindow (false);
    if (prefs_window) {
      //gdk_threads_enter ();
      gm_prefs_window_sound_events_list_build (prefs_window);
      //gdk_threads_leave ();
    }
  }
}


/* DESCRIPTION  :  This callback is called when the "stay_on_top" 
 *                 config value changes.
 * BEHAVIOR     :  Changes the hint for the video windows.
 * PRE          :  /
 */
static void 
stay_on_top_changed_nt (G_GNUC_UNUSED gpointer id,
                        GmConfEntry *entry, 
                        gpointer data)
{
  bool val = false;
    
  g_return_if_fail (data != NULL);

  if (gm_conf_entry_get_type (entry) == GM_CONF_BOOL) {

    //gdk_threads_enter ();

    val = gm_conf_entry_get_bool (entry);

    gm_main_window_set_stay_on_top (GTK_WIDGET (data), val);

    //gdk_threads_leave ();
  }
}


/* DESCRIPTION    : This is called when any setting related to the druid 
 *                  network speed selecion changes.
 * BEHAVIOR       : Just writes an entry in the config database registering 
 *                  that fact.
 * PRE            : None
 */
static void 
network_settings_changed_nt (G_GNUC_UNUSED gpointer id,
                             GmConfEntry *, 
                             gpointer)
{
  //gdk_threads_enter ();
  gm_conf_set_int (GENERAL_KEY "kind_of_net", NET_CUSTOM);
  //gdk_threads_leave ();
}


/* The functions */
gboolean 
gnomemeeting_conf_check ()
{
  int conf_test = -1;
  
  /* Check the config is ok */
  conf_test = gm_conf_get_int (GENERAL_KEY "gconf_test_age");
  
  if (conf_test != SCHEMA_AGE) 
    return FALSE;

  return TRUE;
}


void
gnomemeeting_conf_init ()
{
  GtkWidget *main_window = NULL;
  
  main_window = GnomeMeeting::Process ()->GetMainWindow ();


  /* There are in general 2 notifiers to attach to each widget :
   * - the notifier that will update the widget itself to the new key,
   *   that one is attached when creating the widget.
   * - the notifier to take an appropriate action, that one is in this file.
   *   
   * Notice that there can be more than 2 notifiers for a key, some actions
   * like updating the ILS server are for example required for
   * several different key changes, they are thus in a separate notifier when
   * they can be reused at several places. If not, a same notifier can contain
   * several actions.
   */

  /* Notifiers to AUDIO_DEVICES_KEY */
  gm_conf_notifier_add (AUDIO_DEVICES_KEY "plugin", 
			manager_changed_nt, NULL);

  /* Notifiers to VIDEO_DEVICES_KEY */
  gm_conf_notifier_add (VIDEO_DEVICES_KEY "plugin", 
			manager_changed_nt, NULL);


  
  /* Notifiers for the VIDEO_DISPLAY_KEY keys */
  gm_conf_notifier_add (VIDEO_DISPLAY_KEY "stay_on_top", 
			stay_on_top_changed_nt, main_window);
  
  
  /* Notifiers for SOUND_EVENTS_KEY keys */
  gm_conf_notifier_add (SOUND_EVENTS_KEY "enable_incoming_call_sound", 
			sound_events_list_changed_nt, NULL);
  
  gm_conf_notifier_add (SOUND_EVENTS_KEY "incoming_call_sound",
			sound_events_list_changed_nt, NULL);

  gm_conf_notifier_add (SOUND_EVENTS_KEY "enable_ring_tone_sound", 
			sound_events_list_changed_nt, NULL);
  
  gm_conf_notifier_add (SOUND_EVENTS_KEY "ring_tone_sound", 
			sound_events_list_changed_nt, NULL);
  
  gm_conf_notifier_add (SOUND_EVENTS_KEY "enable_busy_tone_sound", 
			sound_events_list_changed_nt, NULL);
  
  gm_conf_notifier_add (SOUND_EVENTS_KEY "busy_tone_sound",
			sound_events_list_changed_nt, NULL);
  
  gm_conf_notifier_add (SOUND_EVENTS_KEY "enable_new_voicemail_sound", 
			sound_events_list_changed_nt, NULL);
  
  gm_conf_notifier_add (SOUND_EVENTS_KEY "new_voicemail_sound",
			sound_events_list_changed_nt, NULL);

  gm_conf_notifier_add (SOUND_EVENTS_KEY "enable_new_message_sound",
			sound_events_list_changed_nt, NULL);

  gm_conf_notifier_add (SOUND_EVENTS_KEY "new_message_sound",
			sound_events_list_changed_nt, NULL);

  
  /* Notifiers for the VIDEO_CODECS_KEY keys */
  gm_conf_notifier_add (VIDEO_CODECS_KEY "enable_video",
			network_settings_changed_nt, NULL);	     

  gm_conf_notifier_add (VIDEO_CODECS_KEY "maximum_video_tx_bitrate", 
			network_settings_changed_nt, NULL);


  gm_conf_notifier_add (VIDEO_CODECS_KEY "temporal_spatial_tradeoff", 
			network_settings_changed_nt, NULL);
}


void 
gnomemeeting_conf_upgrade ()
{
  gchar *conf_url = NULL;

  int version = 0;

  version = gm_conf_get_int (GENERAL_KEY "version");
  
  /* Install the sip:, h323: and callto: GNOME URL Handlers */
  conf_url = gm_conf_get_string ("/desktop/gnome/url-handlers/callto/command");
					       
  if (!conf_url
      || !strcmp (conf_url, "gnomemeeting -c \"%s\"")) {

    
    gm_conf_set_string ("/desktop/gnome/url-handlers/callto/command", 
			"ekiga -c \"%s\"");

    gm_conf_set_bool ("/desktop/gnome/url-handlers/callto/needs_terminal", 
		      false);
    
    gm_conf_set_bool ("/desktop/gnome/url-handlers/callto/enabled", true);
  }
  g_free (conf_url);

  conf_url = gm_conf_get_string ("/desktop/gnome/url-handlers/h323/command");
  if (!conf_url 
      || !strcmp (conf_url, "gnomemeeting -c \"%s\"")) {

    gm_conf_set_string ("/desktop/gnome/url-handlers/h323/command", 
                        "ekiga -c \"%s\"");

    gm_conf_set_bool ("/desktop/gnome/url-handlers/h323/needs_terminal", false);

    gm_conf_set_bool ("/desktop/gnome/url-handlers/h323/enabled", true);
  }
  g_free (conf_url);

  conf_url = gm_conf_get_string ("/desktop/gnome/url-handlers/sip/command");
  if (!conf_url 
      || !strcmp (conf_url, "gnomemeeting -c \"%s\"")) {

    gm_conf_set_string ("/desktop/gnome/url-handlers/sip/command", 
                        "ekiga -c \"%s\"");

    gm_conf_set_bool ("/desktop/gnome/url-handlers/sip/needs_terminal", false);

    gm_conf_set_bool ("/desktop/gnome/url-handlers/sip/enabled", true);
  }
  g_free (conf_url);

  /* Upgrade IP detector IP address */
  conf_url = gm_conf_get_string (NAT_KEY "public_ip_detector");
  if (conf_url && !strcmp (conf_url, "http://213.193.144.104/ip/"))
    gm_conf_set_string (NAT_KEY "public_ip_detector", 
			"http://ekiga.net/ip/");
  g_free (conf_url);

  /* New full name key */
  conf_url = gm_conf_get_string (PERSONAL_DATA_KEY "full_name");
  if (!conf_url || (conf_url && !strcmp (conf_url, ""))) {

    gchar *fullname = NULL;
    gchar *firstname = gm_conf_get_string (PERSONAL_DATA_KEY "firstname");
    gchar *lastname = gm_conf_get_string (PERSONAL_DATA_KEY "lastname");

    if (firstname && lastname && strcmp (firstname, "") && strcmp (lastname, "")) {
      fullname = g_strdup_printf ("%s %s", firstname, lastname);
      gm_conf_set_string (PERSONAL_DATA_KEY "firstname", "");
      gm_conf_set_string (PERSONAL_DATA_KEY "lastname", "");
      gm_conf_set_string (PERSONAL_DATA_KEY "full_name", fullname);
      g_free (fullname);
    }
    g_free (firstname);
    g_free (lastname);
  }
  g_free (conf_url);

  /* diamondcard is now set at sip.diamondcard.us */
  GSList *accounts = gm_conf_get_string_list ("/apps/ekiga/protocols/accounts_list");
  GSList *accounts_iter = accounts;
  while (accounts_iter) {

    PString acct = (gchar *) accounts_iter->data;
    acct.Replace ("eugw.ast.diamondcard.us", "sip.diamondcard.us", TRUE);
    g_free (accounts_iter->data);
    accounts_iter->data = g_strdup ((const char *) acct);
    accounts_iter = g_slist_next (accounts_iter);
  }
  gm_conf_set_string_list ("/apps/ekiga/protocols/accounts_list", accounts);
  g_slist_foreach (accounts, (GFunc) g_free, NULL);
  g_slist_free (accounts);

  /* Audio devices */
  gchar *plugin = NULL;
  gchar *device = NULL;
  gchar *new_device = NULL;
  plugin = gm_conf_get_string (AUDIO_DEVICES_KEY "plugin");
  if (plugin && strcmp (plugin, "")) {
    device = gm_conf_get_string (AUDIO_DEVICES_KEY "input_device");
    new_device = g_strdup_printf ("%s (PTLIB/%s)", device, plugin);
    gm_conf_set_string (AUDIO_DEVICES_KEY "plugin", "");
    gm_conf_set_string (AUDIO_DEVICES_KEY "input_device", new_device);
    g_free (device);
    g_free (new_device);

    device = gm_conf_get_string (AUDIO_DEVICES_KEY "output_device");
    new_device = g_strdup_printf ("%s (PTLIB/%s)", device, plugin);
    gm_conf_set_string (AUDIO_DEVICES_KEY "plugin", "");
    gm_conf_set_string (AUDIO_DEVICES_KEY "output_device", new_device);
    g_free (device);
    g_free (new_device);

    device = gm_conf_get_string (SOUND_EVENTS_KEY "output_device");
    new_device = g_strdup_printf ("%s (PTLIB/%s)", device, plugin);
    gm_conf_set_string (SOUND_EVENTS_KEY "plugin", "");
    gm_conf_set_string (SOUND_EVENTS_KEY "output_device", new_device);
    g_free (device);
    g_free (new_device);
  }
  g_free (plugin);

  /* Video devices */
  plugin = gm_conf_get_string (VIDEO_DEVICES_KEY "plugin");
  if (plugin && strcmp (plugin, "")) {
    device = gm_conf_get_string (VIDEO_DEVICES_KEY "input_device");
    new_device = g_strdup_printf ("%s (PTLIB/%s)", device, plugin);
    gm_conf_set_string (VIDEO_DEVICES_KEY "plugin", "");
    gm_conf_set_string (VIDEO_DEVICES_KEY "input_device", new_device);
    g_free (device);
    g_free (new_device);
  }
  g_free (plugin);
}
