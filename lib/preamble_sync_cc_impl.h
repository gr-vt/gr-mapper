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

#ifndef INCLUDED_MAPPER_PREAMBLE_SYNC_CC_IMPL_H
#define INCLUDED_MAPPER_PREAMBLE_SYNC_CC_IMPL_H

#include <mapper/preamble_sync_cc.h>

namespace gr {
  namespace mapper {

    class preamble_sync_cc_impl : public preamble_sync_cc
    {
     private:
      int d_last;
      int d_state;
      int d_width;
      std::vector<uint8_t> d_preamble;
      std::vector<gr_complex> d_preamble_map;
      modtype_t d_modtype;
      std::vector<int> d_symbol_map;
      float d_gtas;
      float d_gtls;
      int d_interp;
      float d_offset;

      constellation d_const;
      uint16_t d_mapidx;

      std::vector<float> d_preamble_phase;

      pream_sync_correlator d_corr;

      void get_preamble_phase(void);
      void unwrap(float *phase_array, int N);

      bool d_early_term;

     public:
      preamble_sync_cc_impl(int width, const std::vector<unsigned char> &preamble, modtype_t modtype, const std::vector<int> &symbol_map,
                            float garble_thresh_acquire_sync, float garble_thresh_loose_sync);
      ~preamble_sync_cc_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace mapper
} // namespace gr

#endif /* INCLUDED_MAPPER_PREAMBLE_SYNC_CC_IMPL_H */

