/* -*- c++ -*- */
/* 
 * Copyright 2018 <+YOU OR YOUR COMPANY+>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.	If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */


#ifndef INCLUDED_MSGUTIL_MSGSINK_H
#define INCLUDED_MSGUTIL_MSGSINK_H

#include <msgutil/api.h>
#include <gnuradio/sync_block.h>
#include <gnuradio/msg_queue.h>

namespace gr {
	namespace msgutil {

		/*!
		 * \brief <+description of block+>
		 * \ingroup msgutil
		 *
		 */
		class MSGUTIL_API msgsink : virtual public gr::sync_block
		{
		 public:
			typedef boost::shared_ptr<msgsink> sptr;

			/*!
			 * \brief Return a shared_ptr to a new instance of msgutil::msgsink.
			 *
			 * To avoid accidental use of raw pointers, msgutil::msgsink's
			 * constructor is in a private implementation
			 * class. msgutil::msgsink::make is the public interface for
			 * creating new instances.
			 */
			static sptr make(size_t itemsize, const std::string &tag_start, const std::string &tag_end, size_t bufsize, msg_queue::sptr msgq);
		};

	} // namespace msgutil
} // namespace gr

#endif /* INCLUDED_MSGUTIL_MSGSINK_H */

