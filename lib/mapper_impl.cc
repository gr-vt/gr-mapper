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
#include "mapper_impl.h"

namespace gr {
  namespace mapper {

    mapper::sptr
    mapper::make(modtype_t modtype, std::vector<int> symbol_values)
    {
      return gnuradio::get_initial_sptr
        (new mapper_impl(modtype, symbol_values));
    }

    /*
     * The private constructor
     */
    mapper_impl::mapper_impl(modtype_t modtype, std::vector<int> symbol_values)
      : gr::sync_decimator("mapper",
              gr::io_signature::make(1,1, sizeof(uint8_t)),
              gr::io_signature::make(1,1, sizeof(gr_complex)), 1),
        d_const(modtype, symbol_values),
        d_mapidx(0)   
    {
        set_decimation(d_const.bps());
    }

    /*
     * Our virtual destructor.
     */
    mapper_impl::~mapper_impl()
    {
    }

    int
    mapper_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
        const uint8_t *in = (const uint8_t *) input_items[0];
        gr_complex *out = (gr_complex *) output_items[0];
        d_const.map(in, out, noutput_items, d_mapidx);
        d_mapidx = (d_mapidx+noutput_items)%d_const.nmaps();
        return noutput_items;
    }

  } /* namespace mapper */
} /* namespace gr */

