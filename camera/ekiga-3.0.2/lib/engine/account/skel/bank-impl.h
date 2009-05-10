
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
 *                         bank.h  -  description
 *                         ------------------------------------------
 *   begin                : written in 2008 by Damien Sandras
 *   copyright            : (c) 2008 by Damien Sandras
 *   description          : declaration of a partial implementation
 *                          of a Bank
 *
 */

#ifndef __BANK_IMPL_H__
#define __BANK_IMPL_H__

#include "lister.h"
#include "account-core.h"
#include "account.h"
#include "bank.h"
#include "gmconf.h"

namespace Ekiga
{
  class AccountCore;

/**
 * @addtogroup accounts
 * @{
 */

  /** Generic implementation for the Ekiga::BankImpl abstract class.
   *
   * This class is there to make it easy to implement a new type of account
   * bank: it will take care of implementing the external api, you
   * just have to decide when to add and remove accounts.
   *
   * Any deleted BankImpl is automatically removed from the AccountCore. 
   * The implementor should not have to take care about that.
   *
   * You can remove a Account from an Ekiga::BankImpl in two ways:
   *  - either by calling the remove_account method,
   *  - or by emission of the account's removed signal.
   *
   * Notice that this class won't take care of removing the Account from a
   * backend -- only from the Ekiga::BankImpl.
   * If you want the Account <b>deleted</b> from the backend, then you
   * probably should have an organization like:
   *  - the account has a 'deleted' signal ;
   *  - the bank listens to this signal ;
   *  - when the signal is received, then do a remove_account followed by
   *    calling the appropriate api function to delete the account in your
   *    backend.
   */
  template<class T = Account>
  class BankImpl:
    public Bank,
    public sigc::trackable,
    protected Lister<T>
  {

  public:

    typedef typename Lister<T>::iterator iterator;
    typedef typename Lister<T>::const_iterator const_iterator;

    /** The constructor
     */
    BankImpl (ServiceCore &core);

    /** The destructor.
     */
    virtual ~BankImpl ();

    /** Visit all accounts of the bank and trigger the given callback.
     * @param The callback (the return value means "go on" and allows
     *  stopping the visit)
     */
    void visit_accounts (sigc::slot<bool, Account &> visitor);

    /** Find the account with the given address of record in the Bank
     * @param aor is the address of record of the Account or the host to search 
     *        for
     * @return The Account corresponding to the find result
     *         The returned account should not be freed.
     */
    Ekiga::Account *find_account (const std::string & aor);

    /** This function be called when a new account has to be added to the Bank.
     */
    void new_account ();
  
    /** Returns an iterator to the first Account of the collection
     */
    iterator begin ();

    /** Returns an iterator to the last Account of the collection
     */
    iterator end ();

    /** Returns a const iterator to the first Account of the collection
     */
    const_iterator begin () const;

    /** Returns a const iterator to the last Account of the collection
     */
    const_iterator end () const;

  protected:

    /** Adds a account to the Ekiga::BankImpl.
     * @param: The account to be added.
     * @return: The Ekiga::BankImpl 'account_added' signal is emitted when the account
     * has been added. The Ekiga::BankImpl 'account_updated' signal will be emitted
     * when the account has been updated and the Ekiga::BankImpl 'account_removed' signal
     * will be emitted when the account has been removed from the Ekiga::BankImpl.
     */
    void add_account (T &account);

    /** Removes a account from the Ekiga::BankImpl.
     * @param: The account to be removed.
     * @return: The Ekiga::BankImpl 'account_removed' signal is emitted when the account
     * has been removed.
     */
    void remove_account (T &account);

    /** Save the bank to the GmConf key.
     */
    void save () const;

  protected:
    ServiceCore & core;
    AccountCore *account_core;

  private:
    void on_registration_event (Ekiga::AccountCore::RegistrationState, std::string info, Ekiga::Account *account);
    void on_mwi_event (std::string info, Ekiga::Account *account);
  };

/**
 * @}
 */

};


/* here begins the code from the template functions */

template<typename T>
Ekiga::BankImpl<T>::BankImpl (Ekiga::ServiceCore & _core) : core (_core)
{
  /* Account Core */
  account_core = dynamic_cast<Ekiga::AccountCore*> (core.get ("account-core"));

  /* this is signal forwarding */
  Lister<T>::object_added.connect (account_added.make_slot ());
  Lister<T>::object_removed.connect (account_removed.make_slot ());
  Lister<T>::object_updated.connect (account_updated.make_slot ());

  GSList *accounts = gm_conf_get_string_list ("/apps/ekiga/protocols/accounts_list");
  GSList *accounts_iter = accounts;

  while (accounts_iter) {

    T *account = new T (core, (char *) accounts_iter->data);
    add_account (*account);

    accounts_iter = g_slist_next (accounts_iter);
  }

  g_slist_foreach (accounts, (GFunc) g_free, NULL);
  g_slist_free (accounts);
}


template<typename T>
Ekiga::BankImpl<T>::~BankImpl ()
{
  account_core->remove_bank (*this);
}


template<typename T>
void
Ekiga::BankImpl<T>::save () const
{
  GSList *accounts = NULL;

  for (const_iterator it = begin ();
       it != end ();
       it++) {

    std::string acct_str = (*it).as_string ();
    if (!acct_str.empty ())
      accounts = g_slist_append (accounts, g_strdup (acct_str.c_str ()));
  }

  gm_conf_set_string_list ("/apps/ekiga/protocols/accounts_list", accounts);

  g_slist_foreach (accounts, (GFunc) g_free, NULL);
  g_slist_free (accounts);
}


template<typename T>
void
Ekiga::BankImpl<T>::visit_accounts (sigc::slot<bool, Account &> visitor)
{
  Lister<T>::visit_objects (visitor);
}


template<typename T>
Ekiga::Account *Ekiga::BankImpl<T>::find_account (const std::string & aor)
{
  for (typename Ekiga::BankImpl<T>::iterator it = Lister<T>::begin ();
       it != Lister<T>::end ();
       it++) {

    if (aor.find ("@") != std::string::npos) {
      if (it->get_aor () == aor) {
        return (&(*it));
      }
    }
    else {
      if (it->get_host () == aor) {
        return (&(*it));
      }
    }
  }

  return NULL;
}


template<typename T>
typename Ekiga::BankImpl<T>::iterator
Ekiga::BankImpl<T>::begin ()
{
  return Lister<T>::begin ();
}


template<typename T>
typename Ekiga::BankImpl<T>::iterator
Ekiga::BankImpl<T>::end ()
{
  return Lister<T>::end ();
}


template<typename T>
typename Ekiga::BankImpl<T>::const_iterator
Ekiga::BankImpl<T>::begin () const
{
  return Lister<T>::begin ();
}


template<typename T>
typename Ekiga::BankImpl<T>::const_iterator
Ekiga::BankImpl<T>::end () const
{
  return Lister<T>::end ();
}


template<typename T>
void
Ekiga::BankImpl<T>::add_account (T &account)
{
  add_object (account);

  account.questions.add_handler (questions.make_slot ());
  account.trigger_saving.connect (sigc::mem_fun (this, &Ekiga::BankImpl<T>::save));
  account.registration_event.connect (sigc::bind (sigc::mem_fun (this, &Ekiga::BankImpl<T>::on_registration_event), &account));
  account.mwi_event.connect (sigc::bind (sigc::mem_fun (this, &Ekiga::BankImpl<T>::on_mwi_event), &account));
}


template<typename T>
void
Ekiga::BankImpl<T>::remove_account (T &account)
{
  remove_object (account);
}


template<typename T>
void
Ekiga::BankImpl<T>::on_registration_event (Ekiga::AccountCore::RegistrationState state, std::string info, Ekiga::Account *account)
{
  registration_event.emit (account, state, info);
}


template<typename T>
void
Ekiga::BankImpl<T>::on_mwi_event (std::string info, Ekiga::Account *account)
{
  mwi_event.emit (account, info);
}
#endif
