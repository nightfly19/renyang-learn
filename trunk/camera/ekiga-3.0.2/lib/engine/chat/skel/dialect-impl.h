
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
 *                         dialect.h  -  description
 *                         ------------------------------------------
 *   begin                : written in 2008 by Julien Puydt
 *   copyright            : (c) 2008 by Julien Puydt
 *   description          : basic implementation of a real chat backend
 *
 */

#ifndef __DIALECT_IMPL_H__
#define __DIALECT_IMPL_H__

#include "dialect.h"
#include "lister.h"

namespace Ekiga
{
  template<typename SimpleChatType = SimpleChat,
	   typename MultipleChatType = MultipleChat>
  class DialectImpl:
    public Dialect,
    public sigc::trackable
  {
  public:

    /** The constructor.
     */
    DialectImpl ();

    /** The destructor.
     */
    ~DialectImpl ();

    /** Triggers a callback for all simple chats of the Dialect.
     * @param: The callback (the return value means "go on" and allows
     * stopping the visit)
     */
    void visit_simple_chats (sigc::slot<bool, SimpleChat&> visitor);

    /** Triggers a callback for all multiple chats of the Dialect.
     * @param: The callback (the return value means "go on" and allows
     * stopping the visit)
     */
    void visit_multiple_chats (sigc::slot<bool, MultipleChat&> visitor);

  protected:

    /* More STL-like ways to access the chats within this Ekiga::DialectImpl
     */
    typedef typename Lister<SimpleChatType>::iterator simple_iterator;
    typedef typename Lister<SimpleChatType>::const_iterator simple_const_iterator;
    typedef typename Lister<MultipleChatType>::iterator multiple_iterator;
    typedef typename Lister<MultipleChatType>::const_iterator multiple_const_iterator;

    simple_iterator simple_begin ();
    simple_iterator simple_end ();
    simple_const_iterator simple_begin () const;
    simple_const_iterator simple_end () const;

    multiple_iterator multiple_begin ();
    multiple_iterator multiple_end ();
    multiple_const_iterator multiple_begin () const;
    multiple_const_iterator multiple_end () const;

    /** Adds a SimpleChat to the Ekiga::Dialect.
     * @param The SimpleChat to be added.
     * @return: The Ekiga::Dialect 'simple_chat_added' signal is emitted.
     */
    void add_simple_chat (SimpleChatType* chat);

    /** Removes a SimpleChat from the Ekiga::Dialect.
     * @param The SimpleChat to be removed.
     */
    void remove_simple_chat (SimpleChatType* chat);

    /** Adds a MultipleChat to the Ekiga::Dialect.
     * @param The MultipleChat to be added.
     * @return: The Ekiga::Dialect 'multiple_chat_added' signal is emitted.
     */
    void add_multiple_chat (MultipleChatType* chat);

    /** Removes a MultipleChat from the Ekiga::Dialect.
     * @param The MultipleChat to be removed.
     */
    void remove_multiple_chat (MultipleChatType* chat);

  private:

    Lister<SimpleChatType> simple_chats;
    Lister<MultipleChatType> multiple_chats;
  };
};

template<typename SimpleChatType, typename MultipleChatType>
Ekiga::DialectImpl<SimpleChatType, MultipleChatType>::DialectImpl ()
{
}

template<typename SimpleChatType, typename MultipleChatType>
Ekiga::DialectImpl<SimpleChatType, MultipleChatType>::~DialectImpl ()
{
}

template<typename SimpleChatType, typename MultipleChatType>
void
Ekiga::DialectImpl<SimpleChatType, MultipleChatType>::visit_simple_chats (sigc::slot<bool, SimpleChat&> visitor)
{
  simple_chats.visit_objects (visitor);
}

template<typename SimpleChatType, typename MultipleChatType>
void
Ekiga::DialectImpl<SimpleChatType, MultipleChatType>::visit_multiple_chats (sigc::slot<bool, MultipleChat&> visitor)
{
  multiple_chats.visit_objects (visitor);
}

template<typename SimpleChatType, typename MultipleChatType>
typename Ekiga::DialectImpl<SimpleChatType, MultipleChatType>::simple_iterator
Ekiga::DialectImpl<SimpleChatType, MultipleChatType>::simple_begin ()
{
  return simple_iterator (simple_chats.begin ());
}

template<typename SimpleChatType, typename MultipleChatType>
typename Ekiga::DialectImpl<SimpleChatType, MultipleChatType>::simple_iterator
Ekiga::DialectImpl<SimpleChatType, MultipleChatType>::simple_end ()
{
  return simple_iterator (simple_chats.end ());
}

template<typename SimpleChatType, typename MultipleChatType>
typename Ekiga::DialectImpl<SimpleChatType, MultipleChatType>::simple_const_iterator
Ekiga::DialectImpl<SimpleChatType, MultipleChatType>::simple_begin () const
{
  return simple_const_iterator (simple_chats.begin ());
}

template<typename SimpleChatType, typename MultipleChatType>
typename Ekiga::DialectImpl<SimpleChatType, MultipleChatType>::simple_const_iterator
Ekiga::DialectImpl<SimpleChatType, MultipleChatType>::simple_end () const
{
  return simple_const_iterator (simple_chats.end ());
}

template<typename SimpleChatType, typename MultipleChatType>
typename Ekiga::DialectImpl<SimpleChatType, MultipleChatType>::multiple_iterator
Ekiga::DialectImpl<SimpleChatType, MultipleChatType>::multiple_begin ()
{
  return multiple_iterator (multiple_chats.begin ());
}

template<typename SimpleChatType, typename MultipleChatType>
typename Ekiga::DialectImpl<SimpleChatType, MultipleChatType>::multiple_iterator
Ekiga::DialectImpl<SimpleChatType, MultipleChatType>::multiple_end ()
{
  return multiple_iterator (multiple_chats.end ());
}

template<typename SimpleChatType, typename MultipleChatType>
typename Ekiga::DialectImpl<SimpleChatType, MultipleChatType>::multiple_const_iterator
Ekiga::DialectImpl<SimpleChatType, MultipleChatType>::multiple_begin () const
{
  return multiple_const_iterator (multiple_chats.begin ());
}

template<typename SimpleChatType, typename MultipleChatType>
typename Ekiga::DialectImpl<SimpleChatType, MultipleChatType>::multiple_const_iterator
Ekiga::DialectImpl<SimpleChatType, MultipleChatType>::multiple_end () const
{
  return multiple_const_iterator (multiple_chats.end ());
}

template<typename SimpleChatType, typename MultipleChatType>
void
Ekiga::DialectImpl<SimpleChatType, MultipleChatType>::add_simple_chat (SimpleChatType* chat)
{
  simple_chats.add_object (*chat);
  simple_chat_added.emit (*chat);
}

template<typename SimpleChatType, typename MultipleChatType>
void
Ekiga::DialectImpl<SimpleChatType, MultipleChatType>::remove_simple_chat (SimpleChatType* chat)
{
  /* FIXME */
}

template<typename SimpleChatType, typename MultipleChatType>
void
Ekiga::DialectImpl<SimpleChatType, MultipleChatType>::add_multiple_chat (MultipleChatType* chat)
{
  multiple_chats.add_object (*chat);
  multiple_chat_added.emit (*chat);
}

template<typename SimpleChatType, typename MultipleChatType>
void
Ekiga::DialectImpl<SimpleChatType, MultipleChatType>::remove_multiple_chat (MultipleChatType* chat)
{
  /* FIXME */
}

#endif
