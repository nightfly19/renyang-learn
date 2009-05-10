
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

#ifndef __BANK_H__
#define __BANK_H__

#include "lister.h"
#include "account-core.h"
#include "account.h"
#include "gmconf.h"

namespace Ekiga
{
  class AccountCore;

/**
 * @addtogroup accounts
 * @{
 */

  class Bank 
  {
  public:


    virtual ~Bank () { }


    /** Visit all accounts of the bank and trigger the given callback.
     * @param The callback (the return value means "go on" and allows
     *  stopping the visit)
     */
    virtual void visit_accounts (sigc::slot<bool, Account &> visitor) = 0;


    /** Find the account with the given address of record in the Bank
     * @param aor is the address of record of the Account
     * @return The Account corresponding to the find result
     */
    virtual Ekiga::Account *find_account (const std::string & aor) = 0;


    /** Create the menu for that Bank and its actions.
     * This function is purely virtual and should be implemented by
     * the descendant of the Ekiga::Bank.
     * @param A MenuBuilder object to populate.
     */
    virtual bool populate_menu (MenuBuilder &) = 0;
      

    /** This signal is emitted when a account has been added.
     */
    sigc::signal<void, Account &> account_added;

    /** This signal is emitted when a account has been removed.
     */
    sigc::signal<void, Account &> account_removed;

    /** This signal is emitted when a account has been updated.
     */
    sigc::signal<void, Account &> account_updated;

    /** This signal is emitted when there is a new registration event
     * @param: account is the account 
     *         state is the state
     *         info contains information about the registration status
     */
    sigc::signal<void, const Ekiga::Account *, Ekiga::AccountCore::RegistrationState, std::string> registration_event;

    /** This signal is emitted when there is a new message waiting event
     * @param: account is the account 
     *         info contains information about the indication 
     */
    sigc::signal<void, const Ekiga::Account *, std::string> mwi_event;

    /** This chain allows the BankImpl to present forms to the user
     */
    ChainOfResponsibility<FormRequest*> questions;
  };

/**
 * @}
 */

};
#endif
