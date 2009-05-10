
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
 *                         contact-core.h  -  description
 *                         ------------------------------------------
 *   begin                : written in 2007 by Julien Puydt
 *   copyright            : (c) 2007 by Julien Puydt
 *   description          : interface of the main contact managing object
 *
 */

#ifndef __CONTACT_CORE_H__
#define __CONTACT_CORE_H__

#include "services.h"
#include "source.h"

/* declaration of a few helper classes */
namespace Ekiga
{

/**
 * @defgroup contacts Address Book
 * @{
 */

  class ContactDecorator
  {
  public:

    virtual ~ContactDecorator ()
    {}

    virtual bool populate_menu (Contact &/*contact*/,
				const std::string /*uri*/,
				MenuBuilder &/*builder*/) = 0;
  };

  /** Core object for address book support.
   *
   * Notice that you give sources to this object as references, so they won't
   * be freed here : it's up to you to free them somehow.
   */
  class ContactCore: public Service
  {
  public:

    /** The constructor.
     */
    ContactCore ()
    {}

    /** The destructor.
     */
    ~ContactCore ();


    /*** Service Implementation ***/

    /** Returns the name of the service.
     * @return The service name.
     */
    const std::string get_name () const
    { return "contact-core"; }


    /** Returns the description of the service.
     * @return The service description.
     */
    const std::string get_description () const
    { return "\tContact managing object"; }


    /*** Public API ***/

    /** Adds a source to the ContactCore service.
     * @param The source to be added.
     */
    void add_source (Source &source);


    /** Triggers a callback for all Ekiga::Source sources of the
     * ContactCore service.
     * @param The callback (the return value means "go on" and allows
     *  stopping the visit)
     */
    void visit_sources (sigc::slot<bool, Source &> visitor);


    /** This signal is emitted when a Ekiga::Source has been
     * added to the ContactCore Service.
     */
    sigc::signal<void, Source &> source_added;

    /** This signal is emitted when a book has been added to one of
     * the sources
     */
    sigc::signal<void, Source &, Book &> book_added;

    /** This signal is emitted when a book has been removed from one of
     * the sources
     */
    sigc::signal<void, Source &, Book &> book_removed;

    /** This signal is emitted when a book has been updated in one of
     * the sources
     */
    sigc::signal<void, Source &, Book &> book_updated;

    /** This signal is emitted when a contact has been added to one of
     * the book of one of the sources
     */
    sigc::signal<void, Source &, Book &, Contact &> contact_added;

    /** This signal is emitted when a contact has been removed from one of
     * the book of one of the sources
     */
    sigc::signal<void, Source &, Book &, Contact &> contact_removed;

    /** This signal is emitted when a contact has been updated in one of
     * the book of one of the sources
     */
    sigc::signal<void, Source &, Book &, Contact &> contact_updated;

  private:

    std::set<Source *> sources;

    /* those functions are signal relayers */
    void on_book_added (Book &book, Source *source);
    void on_book_removed (Book &book, Source *source);
    void on_book_updated (Book &book, Source *source);
    void on_contact_added (Book &book, Contact &contact, Source *source);
    void on_contact_removed (Book &book, Contact &contact, Source *source);
    void on_contact_updated (Book &book, Contact &contact, Source *source);

    /*** Contact Helpers ***/

  public:

    void add_contact_decorator (ContactDecorator &decorator);

    /** Create the menu for a given Contact and its actions.
     * @param The Ekiga::Contact for which the actions could be made available.
     * @param The uri for which actions could be made available.
     * @param A MenuBuilder object to populate.
     */
    bool populate_contact_menu (Contact &contact,
				const std::string uri,
                                MenuBuilder &builder);

  private:

    std::list<ContactDecorator *> contact_decorators;


    /*** Misc ***/

  public:

    /** Create the menu for the ContactCore and its actions.
     * @param A MenuBuilder object to populate.
     */
    bool populate_menu (MenuBuilder &builder);

    /** This signal is emitted when the ContactCore Service has been
     * updated.
     */
    sigc::signal<void> updated;


    /** This chain allows the ContactCore to present forms to the user
     */
    ChainOfResponsibility<FormRequest*> questions;
  };

/**
 * @}
 */

};
#endif
