
/* Ekiga -- A VoIP and Video-Conferencing application
 * Copyright (C) 2000-2007 Damien Sandras
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
 *                         opal-bank.h  -  description
 *                         ------------------------------------------
 *   begin                : written in 2008 by Damien Sandras
 *   copyright            : (c) 2008 by Damien Sandras
 *   description          : declaration of an OPAL bank 
 *
 */

#ifndef __OPAL_BANK_H__
#define __OPAL_BANK_H__

#include "bank-impl.h"
#include "opal-account.h"
#include "services.h"

namespace Opal
{
  /**
   * @addtogroup accounts
   * @internal
   * @{
   */
  class Bank: 
      public Ekiga::BankImpl<Account>,
      public Ekiga::Service
  {
public:
    Bank (Ekiga::ServiceCore &_core) : Ekiga::BankImpl<Opal::Account> (_core) {}

    virtual ~Bank () { }

    bool populate_menu (Ekiga::MenuBuilder & builder);

    const std::string get_name () const
    { return "opal-account-store"; }

    const std::string get_description () const
    { return "\tStores the opal accounts"; }

    void new_account (Account::Type t,
                      std::string username = "",
                      std::string password = "");

private:
    void on_new_account_form_submitted (Ekiga::Form & form, Account::Type t);

    void add (Account::Type t,
              std::string name, 
              std::string host,
              std::string user,
              std::string auth_user,
              std::string password,
              bool enabled,
              unsigned timeout);
  };

  /**
   * @}
   */
};

#endif
