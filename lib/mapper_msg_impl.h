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

#ifndef INCLUDED_MAPPER_MAPPER_MSG_IMPL_H
#define INCLUDED_MAPPER_MAPPER_MSG_IMPL_H

#include <mapper/mapper_msg.h>

namespace gr {
  namespace mapper {

    class mapper_msg_impl : public mapper_msg
    {
     private:
      void handle_pdu(pmt::pmt_t pdu);
      modtype_t d_modtype;
      std::map<modtype_t, boost::shared_ptr<constellation> > d_const;

     public:
      mapper_msg_impl(modtype_t modtype, std::vector<int> symbol_values);
      ~mapper_msg_impl();

      // Where all the action really happens
      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } // namespace mapper
} // namespace gr

#endif /* INCLUDED_MAPPER_MAPPER_MSG_IMPL_H */

