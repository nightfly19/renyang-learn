
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
 *                         opal-call.cpp  -  description
 *                         ------------------------------------------
 *   begin                : written in 2007 by Damien Sandras
 *   copyright            : (c) 2007 by Damien Sandras
 *   description          : declaration of the interface of a call handled by
 *                          Opal.
 *
 */


#include <cctype>
#include <algorithm>

#include <opal/buildopts.h>
#include <ptbuildopts.h>

#include <ptlib.h>

#include <opal/manager.h>
#include <opal/rtpconn.h>
#include <opal/pcss.h>
#include <sip/sippdu.h>

#include "config.h"

#include "opal-call.h"
#include "call.h"

using namespace Opal;

Opal::Call::Call (OpalManager & _manager, Ekiga::ServiceCore & _core)
  : OpalCall (_manager), Ekiga::Call (), core (_core),
    runtime (*dynamic_cast<Ekiga::Runtime*>(core.get ("runtime"))),
    call_setup(false),outgoing(true),jitter(0)
{
  re_a_bytes = tr_a_bytes = re_v_bytes = tr_v_bytes = 0.0;
  last_v_tick = last_a_tick = PTime ();
  total_a =
    total_v =
    lost_a =
    too_late_a =
    out_of_order_a =
    lost_v =
    too_late_v =
    out_of_order_v = 0;
  lost_packets = late_packets = out_of_order_packets = 0.0;
  re_a_bw = tr_a_bw = re_v_bw = tr_v_bw = 0.0;

  NoAnswerTimer.SetNotifier (PCREATE_NOTIFIER (OnNoAnswerTimeout));
}


Opal::Call::~Call ()
{
}


void
Opal::Call::hangup ()
{
  PThread::Create (PCREATE_NOTIFIER (OnHangup), NULL,
                   PThread::AutoDeleteThread, PThread::NormalPriority,
                   "Opal::Call Answer Thread");
}


void
Opal::Call::answer ()
{
  PThread::Create (PCREATE_NOTIFIER (OnAnswer), NULL,
                   PThread::AutoDeleteThread, PThread::NormalPriority,
                   "Opal::Call Answer Thread");
}


void
Opal::Call::transfer (std::string uri)
{
  PSafePtr<OpalConnection> connection = NULL;

  int i = 0;

  do {

    connection = GetConnection (i);
    i++;
  }  while (PIsDescendant(&(*connection), OpalPCSSConnection));

  if (connection)
    connection->TransferConnection (uri);
}


void
Opal::Call::toggle_hold ()
{
  PSafePtr<OpalConnection> connection = NULL;

  bool on_hold = false;
  int i = 0;

  do {

    connection = GetConnection (i);
    i++;
  }  while (PIsDescendant(&(*connection), OpalPCSSConnection));

  if (connection) {

    on_hold = connection->IsConnectionOnHold ();
    if (!on_hold)
      connection->HoldConnection ();
    else
      connection->RetrieveConnection ();
  }
}


void
Opal::Call::toggle_stream_pause (StreamType type)
{
  OpalMediaStreamPtr stream = NULL;
  PSafePtr<OpalConnection> connection = NULL;
  PString codec_name;
  std::string stream_name;

  bool paused = false;

  int i = 0;

  do {

    connection = GetConnection (i);
    i++;
  }  while (PIsDescendant(&(*connection), OpalPCSSConnection));

  if (connection != NULL) {

    stream = connection->GetMediaStream ((type == Audio) ? OpalMediaType::Audio () : OpalMediaType::Video (), false);
    if (stream != NULL) {

      stream_name = std::string ((const char *) stream->GetMediaFormat ().GetEncodingName ());
      std::transform (stream_name.begin (), stream_name.end (), stream_name.begin (), (int (*) (int)) toupper);
      paused = stream->IsPaused ();
      stream->SetPaused (!paused);

      if (paused)
        runtime.run_in_main (sigc::bind (stream_resumed, stream_name, type));
      else
        runtime.run_in_main (sigc::bind (stream_paused, stream_name, type));
    }
  }
}


void
Opal::Call::send_dtmf (const char dtmf)
{
  PSafePtr<OpalConnection> connection = NULL;

  int i = 0;

  do {

    connection = GetConnection (i);
    i++;
  }  while (PIsDescendant(&(*connection), OpalPCSSConnection));

  if (connection != NULL)
    connection->SendUserInputTone (dtmf, 180);
}


void Opal::Call::set_no_answer_forward (unsigned delay, const std::string & uri)
{
  forward_uri = uri;

  NoAnswerTimer.SetInterval (0, PMIN (delay, 60));
}


void Opal::Call::set_reject_delay (unsigned delay)
{
  NoAnswerTimer.SetInterval (0, PMIN (delay, 60));
}


const std::string
Opal::Call::get_id () const
{
  return GetToken ();
}


const std::string
Opal::Call::get_local_party_name () const
{
  return local_party_name;
}


const std::string
Opal::Call::get_remote_party_name () const
{
  return remote_party_name;
}


const std::string
Opal::Call::get_remote_application () const
{
  return remote_application;
}


const std::string
Opal::Call::get_remote_uri () const
{
  return remote_uri;
}


const std::string
Opal::Call::get_duration () const
{
  std::stringstream duration;
  
  if (start_time.IsValid () && IsEstablished ()) {

    PTimeInterval t = PTime () - start_time;

    duration << setfill ('0') << setw (2) << t.GetHours () << ":";
    duration << setfill ('0') << setw (2) << (t.GetMinutes () % 60) << ":";
    duration << setfill ('0') << setw (2) << (t.GetSeconds () % 60);
  }

  return duration.str ();
}


time_t 
Opal::Call::get_start_time () const
{
  return start_time.GetTimeInSeconds ();
}


bool 
Opal::Call::is_outgoing () const
{
  return outgoing; 
}


void
Opal::Call::parse_info (OpalConnection & connection)
{
  char special_chars [] = "([;=";
  int i = 0;
  std::string::size_type idx;
  std::string l_party_name;
  std::string r_party_name;
  std::string app;

  if (!PIsDescendant(&connection, OpalPCSSConnection)) {

    outgoing = connection.IsOriginating ();

    if (outgoing)
      remote_uri = (const char *) connection.GetCall().GetPartyB ();
    else
      remote_uri = (const char *) connection.GetRemotePartyCallbackURL ();

    l_party_name = (const char *) connection.GetCalledPartyURL ();
    r_party_name = (const char *) connection.GetRemotePartyName ();
    app = (const char *) connection.GetRemoteProductInfo ().AsString ();
    start_time = connection.GetConnectionStartTime ();
    if (!start_time.IsValid ())
      start_time = PTime ();

    if (!l_party_name.empty ())
      local_party_name = (const char *) SIPURL (l_party_name).GetUserName ();
    if (!r_party_name.empty ())
      remote_party_name = r_party_name;
    if (!app.empty ())
      remote_application = app;

    while (i < 3) {

      idx = remote_party_name.find_first_of (special_chars [i]);
      if (idx != std::string::npos)
        remote_party_name = remote_party_name.substr (0, idx);

      idx = remote_application.find_first_of (special_chars [i]);
      if (idx != std::string::npos)
        remote_application = remote_application.substr (0, idx);

      idx = remote_uri.find_first_of (special_chars [i]);
      if (idx != std::string::npos)
        remote_uri = remote_uri.substr (0, idx);
      i++;
    }
  }
}


PBoolean
Opal::Call::OnEstablished (OpalConnection & connection)
{
  RTP_Session *session = NULL; 
  OpalMediaStreamPtr stream;

  NoAnswerTimer.Stop (false);

  if (!PIsDescendant(&connection, OpalPCSSConnection)) {

    parse_info (connection);
    runtime.emit_signal_in_main (established);
  }

  if (PIsDescendant(&connection, OpalRTPConnection)) {

    stream = connection.GetMediaStream (OpalMediaType::Audio (), false);
    if (stream != NULL) {

      session = PDownCast (OpalRTPConnection, &connection)->GetSession (stream->GetSessionID ());
      if (session) {
      
        session->SetIgnorePayloadTypeChanges (TRUE);
        session->SetRxStatisticsInterval(50);
        session->SetTxStatisticsInterval(50);
      }
    }

    stream = connection.GetMediaStream (OpalMediaType::Video (), false);
    if (stream != NULL) { 

      session = PDownCast (OpalRTPConnection, &connection)->GetSession (stream->GetSessionID ());
      if (session) {

        session->SetIgnorePayloadTypeChanges (TRUE);
        session->SetRxStatisticsInterval(50);
        session->SetTxStatisticsInterval(50);
      }
    }
  }
  
  return OpalCall::OnEstablished (connection);
}


void 
Opal::Call::OnReleased (OpalConnection & connection)
{
  parse_info (connection);

  OpalCall::OnReleased (connection);
}


void
Opal::Call::OnCleared ()
{
  std::string reason;

  NoAnswerTimer.Stop (false);

  // TODO find a better way than that
  while (!call_setup)
    PThread::Current ()->Sleep (100);

  if (!IsEstablished ()
      && !is_outgoing ()
      && GetCallEndReason () != OpalConnection::EndedByAnswerDenied) {

    runtime.emit_signal_in_main (missed);
  }
  else {

    switch (GetCallEndReason ()) {

    case OpalConnection::EndedByLocalUser :
      reason = _("Local user cleared the call");
      break;
    case OpalConnection::EndedByNoAccept :
      reason = _("Local user rejected the call");
      break;
    case OpalConnection::EndedByAnswerDenied :
      reason = _("Local user rejected the call");
      break;
    case OpalConnection::EndedByRemoteUser :
      reason = _("Remote user cleared the call");
      break;
    case OpalConnection::EndedByRefusal :
      reason = _("Remote user rejected the call");
      break;
    case OpalConnection::EndedByCallerAbort :
      reason = _("Remote user has stopped calling");
      break;
    case OpalConnection::EndedByTransportFail :
      reason = _("Abnormal call termination");
      break;
    case OpalConnection::EndedByConnectFail :
      reason = _("Could not connect to remote host");
      break;
    case OpalConnection::EndedByGatekeeper :
      reason = _("The Gatekeeper cleared the call");
      break;
    case OpalConnection::EndedByNoUser :
      reason = _("User not found");
      break;
    case OpalConnection::EndedByNoBandwidth :
      reason = _("Insufficient bandwidth");
      break;
    case OpalConnection::EndedByCapabilityExchange :
      reason = _("No common codec");
      break;
    case OpalConnection::EndedByCallForwarded :
      reason = _("Call forwarded");
      break;
    case OpalConnection::EndedBySecurityDenial :
      reason = _("Security check failed");
      break;
    case OpalConnection::EndedByLocalBusy :
      reason = _("Local user is busy");
      break;
    case OpalConnection::EndedByLocalCongestion :
      reason = _("Congested link to remote party");
      break;
    case OpalConnection::EndedByRemoteBusy :
      reason = _("Remote user is busy");
      break;
    case OpalConnection::EndedByRemoteCongestion :
      reason = _("Congested link to remote party");
      break;
    case OpalConnection::EndedByHostOffline :
      reason = _("Remote host is offline");
      break;
    case OpalConnection::EndedByTemporaryFailure :
    case OpalConnection::EndedByUnreachable :
    case OpalConnection::EndedByNoEndPoint :
    case OpalConnection::EndedByNoAnswer :
      reason = _("User is not available");
      break;
    case OpalConnection::EndedByQ931Cause:
    case OpalConnection::EndedByDurationLimit:
    case OpalConnection::EndedByInvalidConferenceID:
    case OpalConnection::EndedByNoDialTone:
    case OpalConnection::EndedByNoRingBackTone:
    case OpalConnection::EndedByOutOfService:
    case OpalConnection::EndedByAcceptingCallWaiting:
    case OpalConnection::EndedWithQ931Code:
    case OpalConnection::NumCallEndReasons:
    default :
      reason = _("Call completed");
    }

    runtime.emit_signal_in_main (cleared, reason);
  }
}


OpalConnection::AnswerCallResponse
Opal::Call::OnAnswerCall (OpalConnection & connection,
			  const PString & caller)
{
  remote_party_name = (const char *) caller;

  parse_info (connection);

  return OpalCall::OnAnswerCall (connection, caller);
}


PBoolean
Opal::Call::OnSetUp (OpalConnection & connection)
{
  parse_info (connection);

  runtime.emit_signal_in_main (setup);
  call_setup = true;

  cleared.connect (sigc::mem_fun (this, &Opal::Call::on_cleared_call));
  missed.connect (sigc::mem_fun (this, &Opal::Call::on_missed_call));

  return OpalCall::OnSetUp (connection);
}


PBoolean
Opal::Call::OnAlerting (OpalConnection & connection)
{
  if (!PIsDescendant(&connection, OpalPCSSConnection))
    runtime.emit_signal_in_main (ringing);

  return OpalCall::OnAlerting (connection);
}


void 
Opal::Call::OnHold (OpalConnection & /*connection*/, 
                    bool /*from_remote*/, 
                    bool on_hold)
{
  if (on_hold)
    runtime.emit_signal_in_main (held);
  else
    runtime.emit_signal_in_main (retrieved);
}


void
Opal::Call::OnOpenMediaStream (OpalMediaStream & stream)
{
  StreamType type = (stream.GetMediaFormat().GetMediaType() == OpalMediaType::Audio ()) ? Audio : Video;
  bool is_transmitting = false;
  std::string stream_name;

  stream_name = std::string ((const char *) stream.GetMediaFormat ().GetEncodingName ());
  std::transform (stream_name.begin (), stream_name.end (), stream_name.begin (), (int (*) (int)) toupper);
  is_transmitting = !stream.IsSource ();

  runtime.run_in_main (sigc::bind (stream_opened, stream_name, type, is_transmitting));
}


void
Opal::Call::OnClosedMediaStream (OpalMediaStream & stream)
{
  StreamType type = (stream.GetMediaFormat().GetMediaType() == OpalMediaType::Audio ()) ? Audio : Video;
  bool is_transmitting = false;
  std::string stream_name;

  stream_name = std::string ((const char *) stream.GetMediaFormat ().GetEncodingName ());
  std::transform (stream_name.begin (), stream_name.end (), stream_name.begin (), (int (*) (int)) toupper);
  is_transmitting = !stream.IsSource ();

  runtime.run_in_main (sigc::bind (stream_closed, stream_name, type, is_transmitting));
}


void
Opal::Call::OnRTPStatistics (const OpalConnection & /* connection */,
			     const RTP_Session & session)
{
  PWaitAndSignal m(stats_mutex); // The stats are computed from two different threads

  if (session.IsAudio ()) {

    PTimeInterval t = PTime () - last_a_tick;
    if (t.GetMilliSeconds () < 500)
      return;

    unsigned elapsed_seconds = max ((unsigned long) t.GetMilliSeconds (), (unsigned long) 1);
    double octets_received = session.GetOctetsReceived ();
    double octets_sent = session.GetOctetsSent ();

    re_a_bw = max ((octets_received - re_a_bytes) / elapsed_seconds, 0.0);
    tr_a_bw = max ((octets_sent - tr_a_bytes) / elapsed_seconds, 0.0);

    re_a_bytes = octets_received;
    tr_a_bytes = octets_sent;
    last_a_tick = PTime ();

    total_a = session.GetPacketsReceived ();
    lost_a = session.GetPacketsLost ();
    too_late_a = session.GetPacketsTooLate ();
    out_of_order_a = session.GetPacketsOutOfOrder ();

    jitter = session.GetJitterBufferSize () / max ((unsigned) session.GetJitterTimeUnits (), (unsigned) 8);
  }
  else {

    PTimeInterval t = PTime () - last_v_tick;
    if (t.GetMilliSeconds () < 500)
      return;

    unsigned elapsed_seconds = max ((unsigned long) t.GetMilliSeconds (), (unsigned long) 1);
    double octets_received = session.GetOctetsReceived ();
    double octets_sent = session.GetOctetsSent ();

    re_v_bw = max ((octets_received - re_v_bytes) / elapsed_seconds, 0.0);
    tr_v_bw = max ((octets_sent - tr_v_bytes) / elapsed_seconds, 0.0);

    re_v_bytes = octets_received;
    tr_v_bytes = octets_sent;
    last_v_tick = PTime ();

    total_v = session.GetPacketsReceived ();
    lost_v = session.GetPacketsLost ();
    too_late_v = session.GetPacketsTooLate ();
    out_of_order_v = session.GetPacketsOutOfOrder ();
  }

  lost_packets = (lost_a + lost_v) / max ((unsigned long)(total_a + total_v), (unsigned long) 1);
  late_packets = (too_late_a + too_late_v) / max ((unsigned long)(total_a + total_v), (unsigned long) 1);
  out_of_order_packets = (out_of_order_a + out_of_order_v) / max ((unsigned long)(total_a + total_v), (unsigned long) 1);
}


void
Opal::Call::OnAnswer (PThread &, INT /*param*/)
{
  PSafePtr<OpalConnection> connection = NULL;
  int i = 0;

  if (!is_outgoing () && !IsEstablished ()) {

    do {

      connection = GetConnection (i);
      i++;
    }  while (!PIsDescendant(&(*connection), OpalPCSSConnection));

    if (PIsDescendant(&(*connection), OpalPCSSConnection)) {
      PDownCast (OpalPCSSConnection, &(*connection))->AcceptIncoming ();
    }
  }
}


void
Opal::Call::OnHangup (PThread &, INT /*param*/)
{
  if (!is_outgoing () && !IsEstablished ())
    Clear (OpalConnection::EndedByAnswerDenied);
  else
    Clear ();
}


void
Opal::Call::OnNoAnswerTimeout (PTimer &,
                               INT) 
{
  PSafePtr<OpalConnection> connection = NULL;
  int i = 0;

  if (!is_outgoing ()) {

    if (!forward_uri.empty ()) {

      do {
        connection = GetConnection (i);
        i++;
      }  while (PIsDescendant(&(*connection), OpalPCSSConnection));

      if (!PIsDescendant(&(*connection), OpalPCSSConnection)) 
        connection->ForwardCall (forward_uri);
    }
    else
      Clear (OpalConnection::EndedByNoAnswer);
  }
}


void
Opal::Call::on_cleared_call (std::string /*reason*/)
{
  OpalCall::OnCleared ();
}


void
Opal::Call::on_missed_call ()
{
  OpalCall::OnCleared ();
}
