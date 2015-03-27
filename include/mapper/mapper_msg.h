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


#ifndef INCLUDED_MAPPER_MAPPER_MSG_H
#define INCLUDED_MAPPER_MAPPER_MSG_H

#include <mapper/api.h>
#include <mapper/constellation.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace mapper {

    /*!
     * \brief <+description of block+>
     * \ingroup mapper
     *
     */
    class MAPPER_API mapper_msg : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<mapper_msg> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of mapper::mapper_msg.
       *
       * To avoid accidental use of raw pointers, mapper::mapper_msg's
       * constructor is in a private implementation
       * class. mapper::mapper_msg::make is the public interface for
       * creating new instances.
       */
      static sptr make(modtype_t modtype, std::vector<int> symbol_values);
    };

  } // namespace mapper
} // namespace gr

#endif /* INCLUDED_MAPPER_MAPPER_MSG_H */

