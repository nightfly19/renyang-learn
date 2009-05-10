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
 *                         videooutput.cpp  -  description
 *                         ------------------------------
 *   begin                : Sat Feb 17 2001
 *   copyright            : (C) 2000-2007 by Damien Sandras
 *   description          : PVideoOutputDevice class to permit to display via
 *                          GMVideoDisplay class
 *
 */


#include "config.h"

#define P_FORCE_STATIC_PLUGIN 

#include "videooutput.h"

#include "ekiga.h"
#include "misc.h"
#include "main.h"

int PVideoOutputDevice_EKIGA::devices_nbr = 0;


PMutex PVideoOutputDevice_EKIGA::videoDisplay_mutex;
/* Plugin definition */
class PVideoOutputDevice_EKIGA_PluginServiceDescriptor 
: public PDevicePluginServiceDescriptor
{
  public:
    virtual PObject *CreateInstance (int) const 
      {
	return new PVideoOutputDevice_EKIGA (*(GnomeMeeting::Process ()->GetServiceCore ())); 
      }
    
    
    virtual PStringArray GetDeviceNames(int) const 
      { 
	return PStringList("EKIGA"); 
      }
    
    virtual bool ValidateDeviceName (const PString & deviceName, 
				     int) const 
      { 
	return deviceName.Find("EKIGA") == 0; 
      }
} PVideoOutputDevice_EKIGA_descriptor;

PCREATE_PLUGIN(EKIGA, PVideoOutputDevice, &PVideoOutputDevice_EKIGA_descriptor);


/* The Methods */
PVideoOutputDevice_EKIGA::PVideoOutputDevice_EKIGA (Ekiga::ServiceCore & _core)
: core (_core), videooutput_core (*(dynamic_cast<Ekiga::VideoOutputCore *> (_core.get ("videooutput-core"))))
{ 
 PWaitAndSignal m(videoDisplay_mutex);

  is_active = FALSE;
  
  /* Used to distinguish between input and output device. */
  device_id = 0; 
}


PVideoOutputDevice_EKIGA::~PVideoOutputDevice_EKIGA()
{
  PWaitAndSignal m(videoDisplay_mutex);

  if (is_active) {
    devices_nbr--;
    if (devices_nbr==0)
      videooutput_core.stop();
    is_active = false;
  }
}


bool 
PVideoOutputDevice_EKIGA::Open (const PString &name,
				G_GNUC_UNUSED bool unused)
{ 
  if (name == "EKIGAIN") 
    device_id = 1; 

  return TRUE; 

}

PStringArray PVideoOutputDevice_EKIGA::GetDeviceNames() const
{
  PStringArray devlist;
  devlist.AppendString(GetDeviceName());

  return devlist;
}


bool PVideoOutputDevice_EKIGA::IsOpen ()
{
  return TRUE;
}


bool PVideoOutputDevice_EKIGA::SetFrameData (unsigned x,
					   unsigned y,
					   unsigned width,
					   unsigned height,
					   const BYTE * data,
					   bool endFrame)
{
 PWaitAndSignal m(videoDisplay_mutex);

  if (x > 0 || y > 0)
    return FALSE;

  if (width < 160 || width > 2048) 
    return FALSE;
  
  if (height <120 || height > 2048) 
    return FALSE;

  if (!endFrame)
    return FALSE;

  if (!is_active) {
    if (devices_nbr == 0) {
      videooutput_core.start();
    }
    is_active = TRUE;
    devices_nbr++;
  }

  videooutput_core.set_frame_data((const char*) data, width, height, (device_id == LOCAL), devices_nbr);

  return TRUE;
}

bool PVideoOutputDevice_EKIGA::SetColourFormat (const PString & colour_format)
{
  if (colour_format == "YUV420P") {
    return PVideoOutputDevice::SetColourFormat (colour_format);
  }

  return FALSE;  
}
