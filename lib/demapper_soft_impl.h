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

#ifndef INCLUDED_MAPPER_DEMAPPER_SOFT_IMPL_H
#define INCLUDED_MAPPER_DEMAPPER_SOFT_IMPL_H

#include <mapper/demapper_soft.h>

namespace gr {
  namespace mapper {

    class demapper_soft_impl : public demapper_soft
    {
     private:
      constellation d_const;
      int d_mapidx;

     public:
      demapper_soft_impl(modtype_t modtype, std::vector<int> symbol_values);
      ~demapper_soft_impl();

      // Where all the action really happens
      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } // namespace mapper
} // namespace gr

#endif /* INCLUDED_MAPPER_DEMAPPER_SOFT_IMPL_H */

