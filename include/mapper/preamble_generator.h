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

#ifndef INCLUDED_MAPPER_PREAMBLE_GENERATOR_H
#define INCLUDED_MAPPER_PREAMBLE_GENERATOR_H

#include <mapper/api.h>
#include <stdio.h>
#include <vector>


namespace gr {
  namespace mapper {

    class MAPPER_API preamble_generator {
      public:
        preamble_generator();
        preamble_generator(int N, int seed, int mask);
        ~preamble_generator();
        preamble_generator* clone() const;
        std::vector<int> get_preamble();
        
      private:
        int d_bit_length;
        int d_reg;
        int d_seed;
        int d_mask;
        std::vector<int> d_preamble;
        void make_preamble();
    };
  }
}

#endif /* INCLUDED_MAPPER_PREAMBLE_GENERATOR_H */
