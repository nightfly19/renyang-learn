
/*
 * Ekiga -- A VoIP and Video-Conferencing application
 * Copyright (C) 2000-2007 Damien Sandras

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
 *                         contact-core.cpp  -  description
 *                         ------------------------------------------
 *   begin                : written in 2007 by Julien Puydt
 *   copyright            : (c) 2007 by Julien Puydt
 *   description          : implementation of the main contact managing object
 *
 */

#include <iostream>

#include "config.h"

#include "contact-core.h"

static void
on_search ()
{
  std::cout << "Search not implemented yet" << std::endl;
}

Ekiga::ContactCore::~ContactCore ()
{
}

bool
Ekiga::ContactCore::populate_menu (MenuBuilder &builder)
{
  bool populated = false;

  builder.add_action ("search", _("_Find"), sigc::ptr_fun (on_search));
  populated = true;

  for (std::set<Source *>::const_iterator iter = sources.begin ();
       iter != sources.end ();
       iter++) {

    if (populated)
      builder.add_separator ();
    populated = (*iter)->populate_menu (builder);
  }

  return populated;
}

void
Ekiga::ContactCore::add_source (Source &source)
{
  sources.insert (&source);
  source_added.emit (source);
  source.book_added.connect (sigc::bind (sigc::mem_fun (this, &Ekiga::ContactCore::on_book_added), &source));
  source.book_removed.connect (sigc::bind (sigc::mem_fun (this, &Ekiga::ContactCore::on_book_removed), &source));
  source.book_updated.connect (sigc::bind (sigc::mem_fun (this, &Ekiga::ContactCore::on_book_updated), &source));
  source.contact_added.connect (sigc::bind (sigc::mem_fun (this, &Ekiga::ContactCore::on_contact_added), &source));
  source.contact_removed.connect (sigc::bind (sigc::mem_fun (this, &Ekiga::ContactCore::on_contact_removed), &source));
  source.contact_updated.connect (sigc::bind (sigc::mem_fun (this, &Ekiga::ContactCore::on_contact_updated), &source));
  source.questions.add_handler (questions.make_slot ());
}

void
Ekiga::ContactCore::visit_sources (sigc::slot<bool, Source &> visitor)
{
  bool go_on = true;

  for (std::set<Source *>::iterator iter = sources.begin ();
       iter != sources.end () && go_on;
       iter++)
    go_on = visitor (*(*iter));
}


void
Ekiga::ContactCore::on_book_added (Book &book,
				   Source *source)
{
  book_added.emit (*source, book);
}


void
Ekiga::ContactCore::on_book_removed (Book &book,
				     Source *source)
{
  book_removed.emit (*source, book);
}


void
Ekiga::ContactCore::on_book_updated (Book &book,
				     Source *source)
{
  book_updated.emit (*source, book);
}


void
Ekiga::ContactCore::on_contact_added (Book &book,
				      Contact &contact,
				      Source *source)
{
  contact_added.emit (*source, book, contact);
}


void
Ekiga::ContactCore::on_contact_removed (Book &book,
					Contact &contact,
					Source *source)
{
  contact_removed.emit (*source, book, contact);
}


void
Ekiga::ContactCore::on_contact_updated (Book &book,
					Contact &contact,
					Source *source)
{
  contact_updated.emit (*source, book, contact);
}


void
Ekiga::ContactCore::add_contact_decorator (ContactDecorator &decorator)
{
  contact_decorators.push_back (&decorator);
}


bool
Ekiga::ContactCore::populate_contact_menu (Contact &contact,
					   const std::string uri,
					   MenuBuilder &builder)
{
  bool populated = false;

  for (std::list<ContactDecorator *>::const_iterator iter
	 = contact_decorators.begin ();
       iter != contact_decorators.end ();
       iter++) {

    populated = (*iter)->populate_menu (contact, uri, builder) || populated;
  }

  return populated;
}
