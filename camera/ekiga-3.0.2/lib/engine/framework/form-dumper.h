
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
 *                         form-dumper.h  -  description
 *                         ------------------------------------------
 *   begin                : written in 2007 by Julien Puydt
 *   copyright            : (c) 2007 by Julien Puydt
 *   description          : declaration of an object which prints forms
 *
 */

#ifndef __FORM_DUMPER_H__
#define __FORM_DUMPER_H__

#include <iostream>
#include <string>

#include "form.h"

namespace Ekiga
{

/**
 * @addtogroup forms
 * @{
 */

  class FormDumper: public FormVisitor
  {
  public:

    FormDumper (std::ostream &out);

    void dump (const Form &form);

    void title (const std::string );

    void instructions (const std::string );

    void error (const std::string );

    void hidden (const std::string name,
		 const std::string value);

    void boolean (const std::string name,
		  const std::string description,
		  bool value);

    void text (const std::string name,
	       const std::string description,
	       const std::string value);

    void private_text (const std::string name,
		       const std::string description,
		       const std::string value);

    void multi_text (const std::string name,
		     const std::string description,
		     const std::string value);

    void single_choice (const std::string name,
			const std::string description,
			const std::string value,
			const std::map<std::string, std::string> choices);

    void multiple_choice (const std::string name,
			  const std::string description,
			  const std::set<std::string> values,
			  const std::map<std::string, std::string> choices);

    void editable_set (const std::string name,
		       const std::string description,
		       const std::set<std::string> values);
  private:

    std::ostream &out;
  };

/**
 * @}
 */

};

#endif
