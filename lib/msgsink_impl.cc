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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <gnuradio/io_signature.h>
#include <gnuradio/message.h>
#include "msgsink_impl.h"


#define MSGSINK_MSG(fmt,...) fprintf(stderr, "msgsink: " fmt, ##__VA_ARGS__)
#define MSGSINK_ERR(a...) MSGSINK_MSG(a)
//#define MSGSINK_DBG(a...) MSGSINK_MSG(a)
#define MSGSINK_DBG(a...)

namespace gr {
	namespace msgutil {
		msgsink::sptr
		msgsink::make(size_t itemsize, const std::string &tag_start, const std::string &tag_end, size_t bufsize, msg_queue::sptr msgq)
		{
			return gnuradio::get_initial_sptr
				(new msgsink_impl(itemsize, tag_start, tag_end, bufsize, msgq));
		}

		msgsink_impl::msgsink_impl(size_t itemsize, const std::string &tag_start, const std::string &tag_end, size_t bufsize, msg_queue::sptr msgq)
			: gr::sync_block("msgsink",
							gr::io_signature::make(1, 1, itemsize),
							gr::io_signature::make(0,0,0)),
				d_itemsize(itemsize), d_bufsize(bufsize),
				d_tag_start(pmt::mp(tag_start)), d_tag_end(pmt::mp(tag_end)),
				d_state(ST_WAIT), d_bufpos(0), d_msgq(msgq)
		{
			d_buf = (char*)malloc(itemsize*bufsize);
			MSGSINK_DBG("start tag: %s, end tag: %s\n",tag_start.c_str(), tag_end.c_str());
		}

		msgsink_impl::~msgsink_impl() {
			free(d_buf);
		}

		int
		msgsink_impl::add_to_buf(const char*in, int cnt)
		{
			MSGSINK_DBG("add %d to sink\n", cnt);
			if (cnt+d_bufpos > d_bufsize){
				MSGSINK_ERR("dropping samples bicause buffer is too small\n");
				cnt = d_bufsize - d_bufpos;
			}
			memcpy(d_buf+d_bufpos*d_itemsize, in, cnt*d_itemsize);
			d_bufpos += cnt;
		}

		int
		msgsink_impl::work(int noutput_items,
				gr_vector_const_void_star &input_items,
				gr_vector_void_star &output_items)
		{
			const char *in = (const char*)input_items[0];
			std::vector<tag_t> start_tags;
			std::vector<tag_t> end_tags;
			size_t i,	prev, spl_cnt;
			int ii, of;
			message::sptr msg;
			std::vector<tag_t>::iterator sit;
			std::vector<tag_t>::iterator eit;

			MSGSINK_DBG("work (%d - %d)\n", nitems_read(0), nitems_read(0)+noutput_items);
			get_tags_in_range(start_tags, 0, nitems_read(0), nitems_read(0)+noutput_items, d_tag_start);
			get_tags_in_range(end_tags, 0, nitems_read(0), nitems_read(0)+noutput_items, d_tag_end);

			sit = start_tags.begin();
			eit = end_tags.begin();

			MSGSINK_DBG("%d tags start\n", start_tags.size());

			for(ii=0; ii < noutput_items;)
			{
				if(d_state == ST_WAIT)
				{
					while (1){
						if (sit == start_tags.end()){
						/* No start tag in this window. Nothing to do*/
							return noutput_items;
						}
						of = sit->offset - nitems_read(0);
						sit++;
						/* ignore nested start tags */
						if (of >= ii)
							break;
					}
					/* Found start of new packet */
					MSGSINK_DBG("start packet at %d (%d)\n", of, of+nitems_read(0));
					d_state = ST_RX;
					ii = of;
				}
				if (d_state == ST_RX)
				{
					of = 0;
					while (1){
						if (eit == end_tags.end()){
							/* No End tag in this window. Bufferize all window and wait for more */
							add_to_buf(&in[ii], noutput_items - ii);
							return noutput_items;
						}
						/* Found end of current packet */
						of = eit->offset - nitems_read(0);
						eit++;
						/* Ignore trailing end tags */
						if (of > ii)
							break;
					}
					MSGSINK_DBG("end packet at of %d (%d)\n", of, of+nitems_read(0));
					if (of < ii)
						throw std::runtime_error("2: of < ii");
					of++;
					/* Add end of packet to buffer */
					add_to_buf(&in[ii], of - ii);
					/* Send the message */
					if (d_bufpos){
						MSGSINK_DBG("send message (%d)\n", d_bufpos);
						msg = message::make(0, d_itemsize, d_bufpos, d_bufpos*d_itemsize);
						memcpy(msg->msg(), d_buf, d_bufpos*d_itemsize);
						d_msgq->insert_tail(msg);

						/* Reset message */
						msg.reset();
						d_bufpos = 0;
						d_state = ST_WAIT;
					}
					else{
						MSGSINK_DBG("skip empty message\n");
					}
					ii = of;
				}
			}
			return noutput_items;
	 }
	} /* namespace msgutil */
} /* namespace gr */
