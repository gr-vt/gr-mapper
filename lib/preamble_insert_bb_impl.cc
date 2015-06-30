/* -*- c++ -*- */
/* 
 * Copyright 2015 Free Software Foundation, Inc
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

#include <gnuradio/io_signature.h>
#include "preamble_insert_bb_impl.h"
#include <algorithm>
#include <stdio.h>
#include <stdexcept>

namespace gr {
  namespace mapper {

    preamble_insert_bb::sptr
    preamble_insert_bb::make(int width, const std::vector<unsigned char> &preamble)
    {
      return gnuradio::get_initial_sptr
        (new preamble_insert_bb_impl(width, preamble));
    }

    preamble_insert_bb_impl::preamble_insert_bb_impl(int width,
                                        const std::vector<unsigned char> &preamble)
      : gr::block("preamble_insert_bb",
              gr::io_signature::make(1, 1, sizeof(char)),
              gr::io_signature::make(1, 1, sizeof(char))),
        d_data(preamble),
        d_offset(0)
    {
      assert(d_offset < width);
      assert(d_offset >= 0);
      assert((size_t)width > preamble.size());
      set_width(width);
    }

    preamble_insert_bb_impl::~preamble_insert_bb_impl()
    {}

    int
    preamble_insert_bb_impl::general_work(int noutput_items,
                              gr_vector_int &ninput_items,
			                        gr_vector_const_void_star &input_items,
			                        gr_vector_void_star &output_items)
    {
      const char *in = (const char *) input_items[0];
      uint8_t *out = (uint8_t *) output_items[0];

      int ii(0), oo(0);

      while((oo < noutput_items) && (ii < ninput_items[0])) {
        //printf("d_offset = %d, oo = %d, noutput_items = %d, ii = %d, ninput_itmes = %d \n", d_offset, oo, noutput_items, ii, ninput_items[0]);
        if(d_offset >= ((int)d_data.size())) {
          //printf("Copying data through \n");
          //printf("options (%d, %d, %d) \n", noutput_items - oo, ninput_items[0] - ii, d_width - d_offset);
          //printf("error if negative (%d) \n", d_width - d_offset);
          //if((d_width-d_offset)<0){
          //  printf("d_width = %d, d_offset = %d \n", d_width, d_offset);
          //}
          int max_copy = std::min(std::min(noutput_items - oo, ninput_items[0] - ii),
                                  d_width - d_offset);
          //printf("max_copy = %d \n", max_copy);
          memcpy( &out[oo], &in[ii], sizeof(char)*max_copy );
          //printf("memcpy \n");
          ii += max_copy;
          oo += max_copy;
          d_offset = (d_offset + max_copy) % d_width;
        }
        else {
          //printf("Copying preamble through \n");
          if(d_update) { 
            do_update();
            //printf("Updating \n");
          }
          //printf("options (%d, %d) \n", noutput_items - oo, ((int)d_data.size()) - d_offset);
          //printf("error if negative (%d) \n", ((int)d_data.size()) - d_offset);
          //if((((int)d_data.size())-d_offset)<0){
          //  printf("preamble size = %d, d_offset = %d \n", ((int)d_data.size()), d_offset);
          //}
          int max_copy = std::min(noutput_items - oo, ((int)d_data.size()) - d_offset);
          //printf("max_copy = %d \n", max_copy);
          memcpy( &out[oo], &d_data[d_offset], sizeof(char)*max_copy );
          //printf("memcpy \n");
          oo += max_copy;
          d_offset = (d_offset + max_copy) % d_width;
        }
      }
      consume_each(ii);

      return oo;
    }

  } /* namespace mapper */
} /* namespace gr */

