
/*
 * Ekiga -- A VoIP and Video-Conferencing application
 * Copyright (C) 2000-2008 Damien Sandras

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
 *                         codec-description.cpp  -  description
 *                         ------------------------------------------
 *   begin                : written in January 2008 by Damien Sandras 
 *   copyright            : (c) 2008 by Damien Sandras
 *   description          : declaration of the interface of a codec description.
 *
 */

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>

#include "codec-description.h"


using namespace Ekiga;

CodecDescription::CodecDescription ()
: rate (0), active (false), audio (false)
{
}


CodecDescription::CodecDescription (std::string _name,
                                    unsigned _rate,
                                    bool _audio,
                                    std::string _protocols,
                                    bool _active)
: name (_name), rate (_rate), active (_active), audio (_audio)
{
  char *pch = NULL;

  pch = strtok ((char *) _protocols.c_str (), ",");
  while (pch != NULL) {

    std::string protocol = pch;
    protocol = protocol.substr (protocol.find_first_not_of (" "));
    protocols.push_back (protocol);
    pch = strtok (NULL, ",");
  }

  protocols.unique ();
  protocols.sort ();
}


CodecDescription::CodecDescription (std::string codec)
{
  int i = 0;
  char *pch = NULL;

  std::string tmp [5];

  pch = strtok ((char *) codec.c_str (), "*");
  while (pch != NULL) {

    tmp [i] = pch;
    pch = strtok (NULL, "*");

    i++;
  }

  if (i < 4)
    return;

  pch = strtok ((char *) tmp [3].c_str (), " ");
  while (pch != NULL) {

    protocols.push_back (pch);
    pch = strtok (NULL, " ");
  }

  name = tmp [0];
  rate = atoi (tmp [1].c_str ());
  audio = atoi (tmp [2].c_str ());
  active = atoi (tmp [4].c_str ());
}


std::string CodecDescription::str ()
{
  std::stringstream val;
  std::stringstream proto;

  val << name << "*" << rate << "*" << audio << "*";
  protocols.sort ();
  for (std::list<std::string>::iterator iter = protocols.begin ();
       iter != protocols.end ();
       iter++) {

    if (iter != protocols.begin ())
      proto << " ";

    proto << *iter;
  }
  val << proto.str () << "*" << (active ? "1" : "0");

  return val.str ();
}


bool CodecDescription::operator== (const CodecDescription & c) const
{
  CodecDescription d = c;
  CodecDescription e = (*this);

  return (e.str () == d.str ());
}


bool CodecDescription::operator!= (const CodecDescription & c) const
{
  return (!((*this) == c));
}


CodecList::CodecList (GSList *codecs_config)
{
  GSList *codecs_config_it = NULL;
  
  codecs_config_it = (GSList *) codecs_config;
  while (codecs_config_it) {


    Ekiga::CodecDescription d = Ekiga::CodecDescription ((char *) codecs_config_it->data);
    if (!d.name.empty ())
      (*this).push_back (d);

    codecs_config_it = g_slist_next (codecs_config_it);
  }
}


void CodecList::append (CodecList & list)
{
  (*this).insert ((*this).begin (), list.begin (), list.end ());
}


CodecList CodecList::get_audio_list ()
{
  CodecList list;

  for (CodecList::iterator it = (*this).begin ();
       it != (*this).end ();
       it++) {

    if ((*it).audio)
      list.push_back (*it);
  }

  return list;
}


CodecList CodecList::get_video_list ()
{
  CodecList list;

  for (CodecList::iterator it = (*this).begin ();
       it != (*this).end ();
       it++) {

    if (!(*it).audio)
      list.push_back (*it);
  }

  return list;
}


GSList *CodecList::gslist ()
{
  GSList *list = NULL;

  for (CodecList::iterator it = (*this).begin ();
       it != (*this).end ();
       it++) {

    list = g_slist_append (list, g_strdup ((*it).str ().c_str ()));
  }

  return list;
}


bool CodecList::operator== (const CodecList & c) const
{
  CodecList::const_iterator it2 = c.begin ();

  if ((*this).size () != c.size ())
    return false;

  for (CodecList::const_iterator it = (*this).begin ();
       it != (*this).end ();
       it++) {

    if ((*it) != (*it2)) 
      return false;
    
    it2++;
  }

  return true;
}


bool CodecList::operator!= (const CodecList & c) const
{
  return (!(*this == c));
}


std::ostream& operator<< (std::ostream & os, const CodecList & c)
{
  std::stringstream str;
  for (CodecList::const_iterator it = c.begin ();
       it != c.end ();
       it++) {

    if (it != c.begin ())
      str << " ; ";

    str << (*it).name;
  }

  return os << str.str ();
}
