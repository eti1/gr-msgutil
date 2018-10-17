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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <gnuradio/io_signature.h>
#include <gnuradio/message.h>
#include "msgsrc_impl.h"

#define MSGSRC_MSG(fmt,...) fprintf(stderr, "msgsrc: " fmt, ##__VA_ARGS__)
//#define MSGSRC_DBG(a...) MSGSRC_MSG(a)
#define MSGSRC_DBG(a...)

namespace gr {
  namespace msgutil {

    msgsrc::sptr
    msgsrc::make(size_t itemsize, const std::string &tag_start, const std::string &tag_end, msg_queue::sptr msgq)
    {
      return gnuradio::get_initial_sptr
        (new msgsrc_impl(itemsize, tag_start, tag_end, msgq));
    }

    msgsrc_impl::msgsrc_impl(size_t itemsize, const std::string &tag_start, const std::string &tag_end, msg_queue::sptr msgq)
      : gr::sync_block("msgsrc",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(1, 1, itemsize)),
				d_itemsize(itemsize),
				d_tag_start(pmt::string_to_symbol(tag_start)),
				d_tag_end(pmt::string_to_symbol(tag_end)),
				d_msgq(msgq), d_msg_pos(0), d_eof(false)
    {
				MSGSRC_DBG("start tag: %s, end tag: %s\n",tag_start.c_str(), tag_end.c_str());
		}

    msgsrc_impl::~msgsrc_impl()  {  }

    int
    msgsrc_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      void *out = output_items[0];
			int oo = 0;
			size_t cnt;

			MSGSRC_DBG("i work!\n");

			if (d_eof)
				return -1;

			if (d_msg)
			{
				MSGSRC_DBG("got pending msg (%d)\n", d_msg->length());
				cnt = (d_msg->length()/d_itemsize) - d_msg_pos;
				if (cnt > noutput_items){
					cnt = noutput_items;
				}
				if(cnt)
				{
					if(d_msg_pos == 0)
						add_item_tag(0, nitems_written(0), d_tag_start, pmt::PMT_NIL);
					MSGSRC_DBG("copying %d samples\n", cnt);
					memcpy(out, d_msg->msg()+d_msg_pos*d_itemsize, cnt*d_itemsize);
					d_msg_pos += cnt;
					if(d_msg_pos*d_itemsize == d_msg->length()){
						MSGSRC_DBG("done copying message, reset\n");
						add_item_tag(0, nitems_written(0)+cnt-1, d_tag_end, pmt::PMT_NIL);
						d_msg.reset();
					}
					oo = cnt;
					/* FIXME ?*/
					return oo;
				}
			}
			else
			{
				if (d_msgq->empty_p() && oo)
					return oo;

				MSGSRC_DBG("wait message...\n");
				d_msg = d_msgq->delete_head();
				/* EOF on type 1 */
				if (d_msg->type() == 1){
					d_eof = true;
				}
				d_msg_pos = 0;
				MSGSRC_DBG("got message\n");
			}
      return oo;
    }

  } /* namespace msgutil */
} /* namespace gr */

