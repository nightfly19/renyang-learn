
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
 *                         sipendpoint.h  -  description
 *                         -----------------------------
 *   begin                : Wed 24 Nov 2004
 *   copyright            : (C) 2000-2006 by Damien Sandras
 *   description          : This file contains the SIP Endpoint class.
 *
 */


#ifndef _SIP_ENDPOINT_H_
#define _SIP_ENDPOINT_H_


#include "config.h"

#include "common.h"

#include "manager.h"
#include "presence-core.h"
#include "call-manager.h"
#include "call-protocol-manager.h"
#include "account-core.h"
#include "opal-account.h"
#include "sip-dialect.h"


PDICTIONARY (msgDict, PString, PString);


namespace Opal {

  namespace Sip {

    class EndPoint : public SIPEndPoint, 
                                public Ekiga::CallProtocolManager,
                                public Ekiga::PresenceFetcher,
                                public Ekiga::PresencePublisher,
                                public Ekiga::PresentityDecorator,
                                public Ekiga::AccountSubscriberImpl<Opal::Account>,
                                public Ekiga::ContactDecorator
    {
      PCLASSINFO(EndPoint, SIPEndPoint);

  public:

      EndPoint (CallManager &ep, Ekiga::ServiceCore & core, unsigned listen_port);

      ~EndPoint ();

      /* ContactDecorator and PresentityDecorator */
      bool populate_menu (Ekiga::Contact &contact,
			  const std::string uri,
                          Ekiga::MenuBuilder &builder);

      bool populate_menu (Ekiga::Presentity& presentity,
			  const std::string uri,
                          Ekiga::MenuBuilder & builder);

      bool menu_builder_add_actions (const std::string & fullname,
                                     const std::string& uri,
                                     Ekiga::MenuBuilder & builder);


      /* PresenceFetcher */
      void fetch (const std::string uri);
      void unfetch (const std::string uri);


      /* PresencePublisher */
      void publish (const Ekiga::PersonalDetails & details);


      /* Chat subsystem */
      bool send_message (const std::string & uri, 
                         const std::string & message);


      /* CallProtocolManager */
      bool dial (const std::string & uri); 

      const std::string & get_protocol_name () const;

      void set_dtmf_mode (unsigned mode);
      unsigned get_dtmf_mode () const;

      bool set_listen_port (unsigned port);
      const Ekiga::CallProtocolManager::Interface & get_listen_interface () const;


      /* SIP EndPoint */
      void set_nat_binding_delay (unsigned delay);
      unsigned get_nat_binding_delay ();

      void set_outbound_proxy (const std::string & uri);
      const std::string & get_outbound_proxy () const;

      void set_forward_uri (const std::string & uri);
      const std::string & get_forward_uri () const;

      
      /* AccountSubscriber */
      bool subscribe (const Opal::Account & account);
      bool unsubscribe (const Opal::Account & account);


      /* OPAL Methods */
      void ShutDown ();

      void Register (const Opal::Account & account);

      void OnRegistered (const PString & aor,
                         bool wasRegistering);

      void OnRegistrationFailed (const PString & aor,
                                 SIP_PDU::StatusCodes reason,
                                 bool wasRegistering);

      void OnMWIReceived (const PString & party, 
                          OpalManager::MessageWaitingType type, 
                          const PString & info);

      bool OnIncomingConnection (OpalConnection &connection,
                                 unsigned options,
                                 OpalConnection::StringOptions * stroptions);

      virtual void OnPresenceInfoReceived (const PString & user,
                                           const PString & basic,
                                           const PString & note);

      virtual bool OnReceivedMESSAGE (OpalTransport & transport,
                                      SIP_PDU & pdu);

      void OnMessageFailed (const SIPURL & messageUrl,
                            SIP_PDU::StatusCodes reason);

      SIPURL GetRegisteredPartyName (const SIPURL & host);


      /* Callbacks */
  private:
      void on_dial (std::string uri);
      void on_message (std::string uri,
		       std::string name);
      void on_transfer (std::string uri);

      PMutex msgDataMutex;
      msgDict msgData;

      CallManager & manager;

      std::list<std::string> to_subscribe_uris;  // List of uris to subscribe
      std::list<std::string> subscribed_uris;    // List of subscribed uris
      std::list<std::string> domains;            // List of registered domains
      std::list<std::string> aors;               // List of registered aor
      Ekiga::ServiceCore & core;
      Ekiga::PresenceCore & presence_core;
      Ekiga::Runtime & runtime;
      Ekiga::AccountCore & account_core;

      Ekiga::CallProtocolManager::Interface listen_iface;

      std::string protocol_name;
      std::string uri_prefix;
      std::string forward_uri;
      std::string outbound_proxy;

      unsigned listen_port;

      SIP::Dialect *dialect;

      std::map<std::string, std::pair <std::string, std::string> > uri_presences;
    };
  };
};
#endif
