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
#include "demapper_impl.h"

namespace gr {
  namespace mapper {

    demapper::sptr
    demapper::make(modtype_t modtype, std::vector<int> symbol_values)
    {
      return gnuradio::get_initial_sptr
        (new demapper_impl(modtype, symbol_values));
    }

    /*
     * The private constructor
     */
    demapper_impl::demapper_impl(modtype_t modtype, std::vector<int> symbol_values)
      : gr::sync_interpolator("demapper",
              gr::io_signature::make(1,1, sizeof(gr_complex)),
              gr::io_signature::make(1,1, sizeof(uint8_t)), 1),
        d_const(modtype, symbol_values),
        d_mapidx(0)
    {
        set_interpolation(d_const.bps());
    }

    /*
     * Our virtual destructor.
     */
    demapper_impl::~demapper_impl()
    {
    }

    int
    demapper_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
        const gr_complex *in = (const gr_complex *) input_items[0];
        uint8_t *out = (uint8_t *) output_items[0];
        d_const.demap_hard(in, out, noutput_items/d_const.bps());
        return noutput_items;
    }

  } /* namespace mapper */
} /* namespace gr */

