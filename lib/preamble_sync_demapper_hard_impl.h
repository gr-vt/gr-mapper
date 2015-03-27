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

#ifndef INCLUDED_MAPPER_PREAMBLE_SYNC_DECODE_HARD_IMPL_H
#define INCLUDED_MAPPER_PREAMBLE_SYNC_DECODE_HARD_IMPL_H

#include <mapper/preamble_sync_demapper_hard.h>

namespace gr {
  namespace mapper {

    class preamble_sync_demapper_hard_impl : public preamble_sync_demapper_hard
    {
     private:
      std::vector<uint8_t> d_preamble;
      std::vector<gr_complex> d_preamble_map;
      std::vector<int> d_symbol_map;

      int d_acq_bt;
      int d_loo_bt;
      int d_bps;
      int d_state;
      int d_width;
      int d_offset;
      int d_mapidx;
 //     int d_loose_count;
      bool d_passthrough;

      modtype_t d_modtype;

      constellation d_const;

      int d_processed;


     public:
      preamble_sync_demapper_hard_impl(int width, const std::vector<unsigned char> &preamble, modtype_t modtype,
                                const std::vector<int> &symbol_map, int acquire_bit_thresh, int loose_bit_thresh,
                                bool passthrough);
      ~preamble_sync_demapper_hard_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace mapper
} // namespace gr

#endif /* INCLUDED_MAPPER_PREAMBLE_SYNC_DECODE_HARD_IMPL_H */

