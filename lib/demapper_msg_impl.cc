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
#include "demapper_msg_impl.h"

namespace gr {
  namespace mapper {

    demapper_msg::sptr
    demapper_msg::make(modtype_t modtype, std::vector<int> symbol_values)
    {
      return gnuradio::get_initial_sptr
        (new demapper_msg_impl(modtype, symbol_values));
    }

    /*
     * The private constructor
     */
    demapper_msg_impl::demapper_msg_impl(modtype_t modtype, std::vector<int> symbol_values)
      : gr::sync_block("demapper_msg",
              gr::io_signature::make(0,0,0),
              gr::io_signature::make(0,0,0)),
        d_modtype(modtype)
    {
        // set up ports
        message_port_register_in(pmt::mp("cpdus"));
        message_port_register_out(pmt::mp("fpdus"));
        set_msg_handler(pmt::mp("cpdus"), boost::bind( &demapper_msg_impl::handle_pdu, this, _1) );

        // set up demappers
        d_const[modtype] = boost::shared_ptr<constellation>(new constellation(QPSK, std::vector<int>(&default_greymap_qpsk[0], &default_greymap_qpsk[4])));

        // set up default demapper
        d_const[modtype] = boost::shared_ptr<constellation>(new constellation(modtype, symbol_values));
    }

    void demapper_msg_impl::handle_pdu(pmt::pmt_t pdu){
        pmt::pmt_t meta = pmt::car(pdu);
        pmt::pmt_t sym_data = pmt::cdr(pdu);

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
        int len_syms = pmt::length(sym_data);
        int len_bits = len_syms * bps;
        size_t offset = 0;

        // perform the mapping into a new f32 vector
        pmt::pmt_t fpdu_vector(pmt::make_f32vector( len_bits,  0.0f ) );
        d_const[modtype]->demap_soft( 
                    pmt::c32vector_elements(sym_data, offset), 
                    pmt::f32vector_writable_elements(fpdu_vector,offset), 
                    len_syms );
    
        // send out the cpdu
        message_port_pub(pmt::mp("fpdus"), pmt::cons(meta, fpdu_vector));
    }

    /*
     * Our virtual destructor.
     */
    demapper_msg_impl::~demapper_msg_impl()
    {
    }

    int
    demapper_msg_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
        throw std::runtime_error("this is not a stream block");
        return noutput_items;
    }

  } /* namespace mapper */
} /* namespace gr */

