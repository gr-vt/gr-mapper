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


#ifndef INCLUDED_MAPPER_PREAMBLE_INSERT_BB_H
#define INCLUDED_MAPPER_PREAMBLE_INSERT_BB_H

#include <mapper/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace mapper {

    class MAPPER_API preamble_insert_bb : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<preamble_insert_bb> sptr;

      static sptr make(int width, const std::vector<unsigned char> &preamble);

      virtual void rewind() = 0;
      virtual void set_width(int nw) = 0;
      virtual void set_preamble(const std::vector<unsigned char> &np) = 0;
    };

  } // namespace mapper
} // namespace gr

#endif /* INCLUDED_MAPPER_PREAMBLE_INSERT_BB_H */

