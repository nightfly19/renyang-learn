
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
 *                         gtklevelmeter.h  -  description
 *                         -------------------------------
 *   begin                : Sat Dec 23 2003
 *   copyright            : (C) 2003 by Stefan Brüns <lurch@gmx.li>
 *   description          : This file contains a GTK VU Meter.
 *
 */


#ifndef __GTK_LEVELMETER_H__
#define __GTK_LEVELMETER_H__

#include <gdk/gdk.h>
#include <gtk/gtkwidget.h>

#include <stdlib.h>


G_BEGIN_DECLS

#define GTK_LEVELMETER(obj) GTK_CHECK_CAST (obj, gtk_levelmeter_get_type (), GtkLevelMeter)
#define GTK_LEVELMETER_CLASS(klass) GTK_CHECK_CLASS_CAST (klass, gtk_levelmeter_get_type (), GtkLevelMeterClass)
#define GTK_IS_LEVELMETER(obj) GTK_CHECK_TYPE (obj, gtk_levelmeter_get_type ())


typedef struct _GtkLevelMeter GtkLevelMeter;
typedef struct _GtkLevelMeterClass GtkLevelMeterClass;


typedef enum
{
  GTK_METER_LEFT_TO_RIGHT,
  GTK_METER_BOTTOM_TO_TOP
} GtkLevelMeterOrientation;


struct _GtkLevelMeter
{
  GtkWidget widget;

  /* Orientation of the level meter */
  GtkLevelMeterOrientation orientation;

  /* show a peak indicator */
  gboolean showPeak;

  /* show a continous or a segmented (LED like) display */
  gboolean isSegmented;

  /* The ranges of different color of the display */
  GArray* colorEntries;

  /* The pixmap for double buffering */
  GdkPixmap* offscreen_image;

  /* The pixmap with the highlighted bar */
  GdkPixmap* offscreen_image_hl;

  /* The pixmap with the dark bar */
  GdkPixmap* offscreen_image_dark;

  /* The levels */
  gfloat level, peak;
};


struct _GtkLevelMeterClass
{
  GtkWidgetClass parent_class;
};


struct _GtkLevelMeterColorEntry
{
  GdkColor color;
  gfloat stopvalue;
  GdkColor darkcolor;
};


typedef struct _GtkLevelMeterColorEntry GtkLevelMeterColorEntry;

/* DESCRIPTION  :  /
 * BEHAVIOR     :  Creates a new VU meter
 * PRE          :  /
 */
GtkWidget *gtk_levelmeter_new (void);

/* DESCRIPTION  :  /
 * BEHAVIOR     :  Get the GType 
 * PRE          :  /
 */
GType gtk_levelmeter_get_type (void);

/* DESCRIPTION  :  /
 * BEHAVIOR     :  Set new values for level.
 * PRE          :  Level should be between 0.0 and 1.0,
 *                 lower/higher values are clamped.
 */
void gtk_levelmeter_set_level (GtkLevelMeter *, 
			       gfloat);


/* DESCRIPTION  :  /
 * BEHAVIOR     :  Clear the GtkLevelMeter.
 * PRE          :  /
 */
void gtk_levelmeter_clear (GtkLevelMeter *);


/* DESCRIPTION  :  /
 * BEHAVIOR     :  Set new colors for the different ranges of the meter
 * PRE          :  Each array entry has to be a GtkLevelMeterColorEntry,
 *                 the number of entries is not limited, each range starts
 *                 at the stopvalue of the previous entry (or 0.0 for the
 *                 first), color allocation is done by the widget. A copy
 *                 of the array is stored, so the array given as an argument
 *                 can be deleted after the function call.
 */
void gtk_levelmeter_set_colors (GtkLevelMeter *,
				GArray *);

G_END_DECLS

#endif /* __GTK_LEVELMETER_H__ */
