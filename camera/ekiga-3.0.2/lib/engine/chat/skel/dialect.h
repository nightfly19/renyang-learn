
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
 *                         dialect.h  -  description
 *                         ------------------------------------------
 *   begin                : written in 2007 by Julien Puydt
 *   copyright            : (c) 2007 by Julien Puydt
 *   description          : declaration of the interface of a chat
 *                          implementation backend
 *
 */

#ifndef __DIALECT_H__
#define __DIALECT_H__

#include "chat-simple.h"
#include "chat-multiple.h"

namespace Ekiga
{

  class Dialect
  {
  public:

    /** The destructor.
     */
    virtual ~Dialect () {}

    /** Triggers a callback for all simple chats of the Dialect.
     * @param: The callback (the return value means "go on" and allows
     * stopping the visit)
     */
    virtual void visit_simple_chats (sigc::slot<bool, SimpleChat&> visitor) = 0;

    /** Triggers a callback for all multiple chats of the Dialect.
     * @param: The callback (the return value means "go on" and allows
     * stopping the visit)
     */
    virtual void visit_multiple_chats (sigc::slot<bool, MultipleChat&> visitor) = 0;


    /** Feed possible actions on this Dialect to the given MenuBuilder
     * @param A MenuBuilder object to populate.
     */
    virtual bool populate_menu (MenuBuilder &) = 0;

    /** This signal is emitted when an Ekiga::SimpleChat has been added to
     * the dialect.
     */
    sigc::signal<void, SimpleChat&> simple_chat_added;

    /** This signal is emitted when an Ekiga::MultipleChat has been added to
     * the dialect.
     */
    sigc::signal<void, MultipleChat&> multiple_chat_added;

    /** This chain allows the Dialect to present forms to the user.
     */
    ChainOfResponsibility<FormRequest*> questions;
  };

};

#endif
