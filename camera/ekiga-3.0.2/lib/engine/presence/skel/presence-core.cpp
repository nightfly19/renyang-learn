
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
 *                         presence-core.cpp  -  description
 *                         ------------------------------------------
 *   begin                : written in 2007 by Julien Puydt
 *   copyright            : (c) 2007 by Julien Puydt
 *                          (c) 2008 by Damien Sandras
 *   description          : implementation of the main
 *                          presentity managing object
 *
 */

#include "account-core.h"
#include "presence-core.h"
#include "personal-details.h"


Ekiga::PresenceCore::PresenceCore (Ekiga::ServiceCore& core)
{
  Ekiga::AccountCore *account_core = dynamic_cast <Ekiga::AccountCore *> (core.get ("account-core"));
  Ekiga::PersonalDetails *details = dynamic_cast <Ekiga::PersonalDetails *> (core.get ("personal-details"));

  if (details)
    details->updated.connect (sigc::bind (sigc::mem_fun (this, &Ekiga::PresenceCore::publish), details));
  if (account_core)
    account_core->registration_event.connect (sigc::bind (sigc::mem_fun (this, &Ekiga::PresenceCore::on_registration_event), details));
}

Ekiga::PresenceCore::~PresenceCore ()
{
}

void
Ekiga::PresenceCore::add_cluster (Cluster &cluster)
{
  clusters.insert (&cluster);
  cluster_added.emit (cluster);
  cluster.heap_added.connect (sigc::bind (sigc::mem_fun (this, &Ekiga::PresenceCore::on_heap_added), &cluster));
  cluster.heap_updated.connect (sigc::bind (sigc::mem_fun (this, &Ekiga::PresenceCore::on_heap_updated), &cluster));
  cluster.heap_removed.connect (sigc::bind (sigc::mem_fun (this, &Ekiga::PresenceCore::on_heap_removed), &cluster));
  cluster.presentity_added.connect (sigc::bind (sigc::mem_fun (this, &Ekiga::PresenceCore::on_presentity_added), &cluster));
  cluster.presentity_updated.connect (sigc::bind (sigc::mem_fun (this, &Ekiga::PresenceCore::on_presentity_updated), &cluster));
  cluster.presentity_removed.connect (sigc::bind (sigc::mem_fun (this, &Ekiga::PresenceCore::on_presentity_removed), &cluster));
  cluster.questions.add_handler (questions.make_slot ());
}

void
Ekiga::PresenceCore::visit_clusters (sigc::slot<bool, Cluster &> visitor)
{
  bool go_on = true;
  for (std::set<Cluster *>::iterator iter = clusters.begin ();
       iter != clusters.end () && go_on;
       iter++)
    go_on = visitor (*(*iter));
}

bool
Ekiga::PresenceCore::populate_menu (MenuBuilder &/*builder*/)
{
  // FIXME: to implement
  return false;
}

void Ekiga::PresenceCore::on_heap_added (Heap &heap,
					 Cluster *cluster)
{
  heap_added.emit (*cluster, heap);
}

void
Ekiga::PresenceCore::on_heap_updated (Heap &heap,
				      Cluster *cluster)
{
  heap_updated.emit (*cluster, heap);
}

void
Ekiga::PresenceCore::on_heap_removed (Heap &heap, Cluster *cluster)
{
  heap_removed.emit (*cluster, heap);
}

void
Ekiga::PresenceCore::on_presentity_added (Heap &heap,
					       Presentity &presentity,
					       Cluster *cluster)
{
  presentity_added.emit (*cluster, heap, presentity);
}

void
Ekiga::PresenceCore::on_presentity_updated (Heap &heap,
					    Presentity &presentity,
					    Cluster *cluster)
{
  presentity_updated (*cluster, heap, presentity);
}

void
Ekiga::PresenceCore::on_presentity_removed (Heap &heap,
					    Presentity &presentity,
					    Cluster *cluster)
{
  presentity_removed.emit (*cluster, heap, presentity);
}

void
Ekiga::PresenceCore::add_presentity_decorator (PresentityDecorator &decorator)
{
  presentity_decorators.push_back (&decorator);
}

void
Ekiga::PresenceCore::remove_presentity_decorator (PresentityDecorator& decorator)
{
  presentity_decorators.remove (&decorator);
}

bool
Ekiga::PresenceCore::populate_presentity_menu (Presentity& presentity,
					       const std::string uri,
					       MenuBuilder &builder)
{
  bool populated = false;

  for (std::list<PresentityDecorator *>::const_iterator iter
	 = presentity_decorators.begin ();
       iter != presentity_decorators.end ();
       iter++) {

    populated = (*iter)->populate_menu (presentity, uri, builder) || populated;
  }

  return populated;
}

void
Ekiga::PresenceCore::add_presence_fetcher (PresenceFetcher &fetcher)
{
  presence_fetchers.push_back (&fetcher);
  fetcher.presence_received.connect (sigc::mem_fun (this,
						    &Ekiga::PresenceCore::on_presence_received));
  fetcher.status_received.connect (sigc::mem_fun (this,
						  &Ekiga::PresenceCore::on_status_received));
  for (std::map<std::string, uri_info>::const_iterator iter
	 = uri_infos.begin ();
       iter != uri_infos.end ();
       ++iter)
    fetcher.fetch (iter->first);
}

void
Ekiga::PresenceCore::remove_presence_fetcher (PresenceFetcher& fetcher)
{
  presence_fetchers.remove (&fetcher);
  // FIXME: unconnect the signals!
}

void
Ekiga::PresenceCore::fetch_presence (const std::string uri)
{
  uri_infos[uri].count++;

  if (uri_infos[uri].count == 1) {

    for (std::list<PresenceFetcher*>::iterator iter
	   = presence_fetchers.begin ();
	 iter != presence_fetchers.end ();
	 iter++)
      (*iter)->fetch (uri);
  }

  presence_received.emit (uri, uri_infos[uri].presence);
  status_received.emit (uri, uri_infos[uri].status);
}

void Ekiga::PresenceCore::unfetch_presence (const std::string uri)
{
  uri_infos[uri].count--;

  if (uri_infos[uri].count <= 0) {

    uri_infos.erase (uri_infos.find (uri));

    for (std::list<PresenceFetcher*>::iterator iter
	   = presence_fetchers.begin ();
	 iter != presence_fetchers.end ();
	 iter++)
      (*iter)->unfetch (uri);
  }
}

void
Ekiga::PresenceCore::on_presence_received (const std::string uri,
					   const std::string presence)
{
  uri_infos[uri].presence = presence;
  presence_received.emit (uri, presence);
}

void
Ekiga::PresenceCore::on_status_received (const std::string uri,
					 const std::string status)
{
  uri_infos[uri].status = status;
  status_received.emit (uri, status);
}

void
Ekiga::PresenceCore::add_presence_publisher (PresencePublisher &publisher)
{
  presence_publishers.push_back (&publisher);
}

void
Ekiga::PresenceCore::remove_presence_publisher (PresencePublisher& publisher)
{
  presence_publishers.remove (&publisher);
}

void Ekiga::PresenceCore::publish (const PersonalDetails* details) 
{
  for (std::list<PresencePublisher*>::iterator iter
	 = presence_publishers.begin ();
       iter != presence_publishers.end ();
       iter++)
    (*iter)->publish (*details);
}

bool
Ekiga::PresenceCore::is_supported_uri (const std::string uri) const
{
  bool result = false;

  for (std::set<sigc::slot<bool, std::string> >::const_iterator iter
	 = uri_testers.begin ();
       iter != uri_testers.end () && result == false;
       iter++)
    result = (*iter) (uri);

  return result;
}

void
Ekiga::PresenceCore::add_supported_uri (sigc::slot<bool,std::string> tester)
{
  uri_testers.insert (tester);
}

void
Ekiga::PresenceCore::on_registration_event (const Ekiga::Account& /*account*/,
					    Ekiga::AccountCore::RegistrationState state,
					    std::string /*info*/,
					    Ekiga::PersonalDetails *details)
{
  if (state == Ekiga::AccountCore::Registered)
    publish (details);
}
