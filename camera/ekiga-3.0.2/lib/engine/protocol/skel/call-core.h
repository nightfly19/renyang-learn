
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
 *                         call-core.h  -  description
 *                         ------------------------------------------
 *   begin                : written in 2007 by Damien Sandras 
 *   copyright            : (c) 2007 by Damien Sandras
 *   description          : declaration of the interface of a call core.
 *                          A call core manages CallManagers.
 *
 */

#ifndef __CALL_CORE_H__
#define __CALL_CORE_H__

#include "form-request.h"
#include "chain-of-responsibility.h"
#include "services.h"
#include "call.h"

#include <sigc++/sigc++.h>
#include <set>
#include <map>
#include <iostream>


namespace Ekiga
{

/**
 * @defgroup calls Calls and protocols
 * @{
 */

  class CallManager;

  class CallCore
    : public Service
    {

  public:
      typedef std::set<CallManager *>::iterator iterator;
      typedef std::set<CallManager *>::const_iterator const_iterator;

      /** The constructor
       */
      CallCore () { nr_ready = 0; }

      /** The destructor
       */
      ~CallCore () { }


      /*** Service Implementation ***/

      /** Returns the name of the service.
       * @return The service name.
       */
      const std::string get_name () const
        { return "call-core"; }


      /** Returns the description of the service.
       * @return The service description.
       */
      const std::string get_description () const
        { return "\tCall Core managing Call Manager objects"; }


      /** Adds a call handled by the CallCore serice.
       * @param call is the call to be added.
       * @param manager is the CallManager handling it.
       */
      void add_call (Call *call, CallManager *manager);

      /** Adds a CallManager to the CallCore service.
       * @param The manager to be added.
       */
      void add_manager (CallManager &manager);

      /** Return iterator to beginning
       * @return iterator to beginning
       */
      iterator begin ();
      const_iterator begin () const;

      /** Return iterator to end
       * @return iterator to end 
       */
      iterator end ();
      const_iterator end () const;

      /** This signal is emitted when a Ekiga::CallManager has been
       * added to the CallCore Service.
       */
      sigc::signal<void, CallManager &> manager_added;


      /*** Call Management ***/                 

      /** Create a call based on the remote uri given as parameter
       * @param: an uri to call
       * @return: true if a Ekiga::Call could be created
       */
      bool dial (const std::string uri); 

      /*** Call Related Signals ***/
      
      /** See call.h for the API
       */
      sigc::signal<void, CallManager &, Call &> ringing_call;
      sigc::signal<void, CallManager &, Call &> setup_call;
      sigc::signal<void, CallManager &, Call &> missed_call;
      sigc::signal<void, CallManager &, Call &, std::string> cleared_call;
      sigc::signal<void, CallManager &, Call &> established_call;
      sigc::signal<void, CallManager &, Call &> held_call;
      sigc::signal<void, CallManager &, Call &> retrieved_call;
      sigc::signal<void, CallManager &, Call &, std::string, Call::StreamType, bool> stream_opened;
      sigc::signal<void, CallManager &, Call &, std::string, Call::StreamType, bool> stream_closed;
      sigc::signal<void, CallManager &, Call &, std::string, Call::StreamType> stream_paused;
      sigc::signal<void, CallManager &, Call &, std::string, Call::StreamType> stream_resumed;

      /*** Misc ***/
      sigc::signal<void, CallManager &> manager_ready;
      sigc::signal<void> ready;

      /** This chain allows the CallCore to report errors to the user
       */
      ChainOfResponsibility<std::string> errors;

  private:
      void on_new_call (Call *call, CallManager *manager);
      void on_ringing_call (Call *call, CallManager *manager);
      void on_setup_call (Call *call, CallManager *manager);
      void on_missed_call (Call *call, CallManager *manager);
      void on_cleared_call (std::string, Call *call, CallManager *manager);
      void on_established_call (Call *call, CallManager *manager);
      void on_held_call (Call *call, CallManager *manager);
      void on_retrieved_call (Call *call, CallManager *manager);
      void on_stream_opened (std::string name, Call::StreamType type, bool is_transmitting, Call *call, CallManager *manager);
      void on_stream_closed (std::string name, Call::StreamType type, bool is_transmitting, Call *call, CallManager *manager);
      void on_stream_paused (std::string name, Call::StreamType type, Call *call, CallManager *manager);
      void on_stream_resumed (std::string name, Call::StreamType type, Call *call, CallManager *manager);

      void on_im_failed (std::string, std::string, CallManager *manager);
      void on_im_sent (std::string, std::string, CallManager *manager);
      void on_im_received (std::string, std::string, std::string, CallManager *manager);
      void on_new_chat (std::string, std::string, CallManager *manager);

      void on_manager_ready (CallManager *manager);
      
      std::set<CallManager *> managers;
      unsigned nr_ready;
    };

/**
 * @}
 */

};


#endif
