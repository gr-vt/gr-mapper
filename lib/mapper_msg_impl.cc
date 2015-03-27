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
#include "mapper_msg_impl.h"

namespace gr {
  namespace mapper {

    mapper_msg::sptr
    mapper_msg::make(modtype_t modtype, std::vector<int> symbol_values)
    {
      return gnuradio::get_initial_sptr
        (new mapper_msg_impl(modtype, symbol_values));
    }

    /*
     * The private constructor
     */
    mapper_msg_impl::mapper_msg_impl(modtype_t modtype, std::vector<int> symbol_values)
      : gr::sync_block("mapper_msg",
              gr::io_signature::make(0,0,0),
              gr::io_signature::make(0,0,0)),
        d_modtype(modtype)
    {
        // set up ports
        message_port_register_in(pmt::mp("pdus"));
        message_port_register_out(pmt::mp("cpdus"));
        set_msg_handler(pmt::mp("pdus"), boost::bind( &mapper_msg_impl::handle_pdu, this, _1) );

        // set up mappers
        d_const[modtype] = boost::shared_ptr<constellation>(new constellation(QPSK, std::vector<int>(&default_greymap_qpsk[0], &default_greymap_qpsk[4])));

        // set up default mapper
        d_const[modtype] = boost::shared_ptr<constellation>(new constellation(modtype, symbol_values));
    }

    void mapper_msg_impl::handle_pdu(pmt::pmt_t pdu){
        pmt::pmt_t meta = pmt::car(pdu);
        pmt::pmt_t bit_data = pmt::cdr(pdu);

        // start with default modulation
        modtype_t modtype = d_modtype;

        // check for dynamic modtype in dictionary
        if(!pmt::eqv(meta, pmt::PMT_NIL)){
            pmt::pmt_t dynamic_modtype = pmt::dict_ref(meta, pmt::mp("modulation"), pmt::PMT_NIL);
            if(!pmt::eqv(dynamic_modtype, pmt::PMT_NIL)){
                int mt = pmt::to_long(dynamic_modtype);
                modtype = (modtype_t) mt;
            }
        }

        // check/compute parameters
        int bps = d_const[modtype]->bps();
        int len_bits = pmt::length(bit_data);
        int len_symbols = len_bits / bps;
        size_t offset = 0;
        while( len_bits % bps != 0){
            std::vector<uint8_t> nv = pmt::u8vector_elements(bit_data);
            nv.push_back(0);
            len_bits++;
            bit_data = pmt::init_u8vector(len_bits, nv);
            //throw std::runtime_error("bits do not evenly devide into symbols!");
            }

        // perform the mapping into a new c32 vector
        pmt::pmt_t cpdu_vector(pmt::make_c32vector( len_symbols, gr_complex(0,0) ) );
        d_const[modtype]->map( 
                    pmt::u8vector_elements(bit_data, offset), 
                    pmt::c32vector_writable_elements(cpdu_vector,offset), 
                    len_symbols );
    
        // send out the cpdu
        message_port_pub(pmt::mp("cpdus"), pmt::cons(meta, cpdu_vector));
    }

    /*
     * Our virtual destructor.
     */
    mapper_msg_impl::~mapper_msg_impl()
    {
    }

    int
    mapper_msg_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
        throw std::runtime_error("this is not a stream block");
        return noutput_items;
    }

  } /* namespace mapper */
} /* namespace gr */

