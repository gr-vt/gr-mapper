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

#ifndef INCLUDED_MAPPER_PREAMBLE_INSERT_BB_IMPL_H
#define INCLUDED_MAPPER_PREAMBLE_INSERT_BB_IMPL_H

#include <mapper/preamble_insert_bb.h>

namespace gr {
  namespace mapper {

    class preamble_insert_bb_impl : public preamble_insert_bb
    {
     private:
      std::vector<unsigned char> d_data;
      int d_width;
      bool d_update;
      int d_new_width;
      int d_offset;

     public:
      preamble_insert_bb_impl(int width, const std::vector<unsigned char> &preamble);
      ~preamble_insert_bb_impl();

      void rewind() { d_offset = 0; }
      void set_width(int nw) { d_new_width = nw; d_update = true; }
      void set_preamble(const std::vector<unsigned char> &np) { d_data = np; rewind(); }
      void do_update() { d_width = d_new_width; }

      // Where all the action really happens
      int general_work(int noutput_items,
                        gr_vector_int &ninput_items,
	                      gr_vector_const_void_star &input_items,
	                      gr_vector_void_star &output_items);
    };

  } // namespace mapper
} // namespace gr

#endif /* INCLUDED_MAPPER_PREAMBLE_INSERT_BB_IMPL_H */

