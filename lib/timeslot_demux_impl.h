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

#ifndef INCLUDED_MAPPER_TIMESLOT_DEMUX_IMPL_H
#define INCLUDED_MAPPER_TIMESLOT_DEMUX_IMPL_H

#include <mapper/timeslot_demux.h>

namespace gr {
  namespace mapper {

    class timeslot_demux_impl : public timeslot_demux
    {
     private:
      std::vector<uint8_t> D_ts_key;
      std::vector<uint8_t> D_search;
      bool D_synch;
      int D_Nusers;
      int D_offset;
      int D_purge;
      int D_length;
      int D_state;
      int D_fill;

     public:
      timeslot_demux_impl(int nusers, bool syncd, const std::vector<uint8_t> &ts_id, int ts_len);
      ~timeslot_demux_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace mapper
} // namespace gr

#endif /* INCLUDED_MAPPER_TIMESLOT_DEMUX_IMPL_H */

