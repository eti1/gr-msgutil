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

#ifndef INCLUDED_MSGUTIL_MSGSRC_IMPL_H
#define INCLUDED_MSGUTIL_MSGSRC_IMPL_H

#include <msgutil/msgsrc.h>
#include <gnuradio/message.h>

namespace gr {
	namespace msgutil {

		class msgsrc_impl : public msgsrc
		{
		 private:
			size_t					d_itemsize;
			pmt::pmt_t			d_tag_start;
			pmt::pmt_t			d_tag_end;
			msg_queue::sptr	d_msgq;
			message::sptr		d_msg;
			size_t					d_msg_pos;
			bool						d_eof;

		 public:
			msgsrc_impl(size_t itemsize, const std::string &tag_start, const std::string &tag_end, msg_queue::sptr msgq);
			~msgsrc_impl();

			int work(int noutput_items,
				 gr_vector_const_void_star &input_items,
				 gr_vector_void_star &output_items);
		};
	} // namespace msgutil
} // namespace gr

#endif /* INCLUDED_MSGUTIL_MSGSRC_IMPL_H */

