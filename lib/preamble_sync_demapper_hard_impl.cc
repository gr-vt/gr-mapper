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
#include "preamble_sync_demapper_hard_impl.h"
#include <stdio.h>
#include <stdexcept>
#include <cmath>


namespace gr {
  namespace mapper {

    preamble_sync_demapper_hard::sptr
    preamble_sync_demapper_hard::make(int width, const std::vector<unsigned char> &preamble, modtype_t modtype,
                               const std::vector<int> &symbol_map, int acquire_bit_thresh, int loose_bit_thresh,
                               bool passthrough)
    {
      return gnuradio::get_initial_sptr
        (new preamble_sync_demapper_hard_impl(width, preamble, modtype, symbol_map, acquire_bit_thresh, loose_bit_thresh, passthrough));
    }

    /*
     * The private constructor
     */
    preamble_sync_demapper_hard_impl::preamble_sync_demapper_hard_impl(int width, const std::vector<unsigned char> &preamble, modtype_t modtype,
                                                         const std::vector<int> &symbol_map, int acquire_bit_thresh, int loose_bit_thresh,
                                                         bool passthrough)
      : gr::block("preamble_sync_demapper_hard",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(uint8_t))),//gr::io_signature::make(1, 1, sizeof(float))),
        d_const(modtype, symbol_map),
        d_preamble(preamble),
        d_state(0),
        d_modtype(modtype),
        d_symbol_map(symbol_map),
        d_acq_bt(acquire_bit_thresh),
        d_loo_bt(loose_bit_thresh),
        d_mapidx(0),
        d_offset(0),
        d_passthrough(passthrough),
        d_processed(0)
//        d_loose_count(0)
    {
      //printf("Made block.\n");
      d_bps = d_const.bps();
      assert(width%d_bps == 0);
      d_width = width;
    }

    /*
     * Our virtual destructor.
     */
    preamble_sync_demapper_hard_impl::~preamble_sync_demapper_hard_impl()
    {
    }

    void
    preamble_sync_demapper_hard_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
        int payload_sym_count = d_width/d_bps;
        int data_sym_count = payload_sym_count - d_preamble.size()/d_bps;
        int max_out;
        if(d_passthrough){
          max_out = (int) ceil((float)noutput_items / float(payload_sym_count * d_bps));
        }
        else{
          max_out = (int) ceil((float)noutput_items / float(data_sym_count * d_bps));
        }
        ninput_items_required[0] = max_out * payload_sym_count;//Needs at least one full payload to output anything
        //printf("Forecasted\n");
    }

    int
    preamble_sync_demapper_hard_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        const gr_complex *in = (const gr_complex *) input_items[0];
        //float *out = (float *) output_items[0];
        uint8_t *out = (uint8_t *) output_items[0];

        int oo(0), ii(0);
        int consumed(0);
        int produced(0);
        int pream_len = (int) d_preamble.size();
        int preamble_space = pream_len/d_bps;
        int payload_space = d_width/d_bps;
        int data_space = payload_space - preamble_space;
        int M = (int) pow(2.0,(double)d_bps);
        int N = d_const.nmaps();
        int MN = M*N;

        std::vector<uint8_t> comparer(d_preamble.size(),255);
        std::vector< std::vector<uint8_t> > comparing(MN,comparer);
        //printf("Working.\n");
        int n_symbols;
        if(d_passthrough){
          n_symbols = payload_space;
        }
        else{
          n_symbols = data_space;
        }

//        while((oo<noutput_items) && (ii < ninput_items[0]) && (ninput_items[0] >= payload_space)) {
        while(1) {
          if(!(ninput_items[0] >= payload_space)){
//            printf("ninput break.\tninput = %d\tpayload = %d\n",ninput_items[0], payload_space);
            break;
          }
          if(!(oo < noutput_items - n_symbols*d_bps)){
//            printf("oo break.\too = %d\ttotal_out = %d\n",oo,noutput_items - n_symbols*d_bps);
            break;
          }
          if(!(ii < ninput_items[0] - data_space)){
            //printf("ii break.\tii = %d\tninput = %d\tstate = %d\n",ii,ninput_items[0],d_state);
            break;
          }
          switch(d_state) {
            case 0: {
              //printf("State 0:\n");
              for(int idx=0; idx < MN; idx++){
                for(int ind=d_bps; ind < pream_len; ind++){
                  comparing[idx][ind-d_bps] = comparing[idx][ind];
                }
              }
              //printf("Compare has shifted.\t Pream_len = %d\n",pream_len);
              for(int idx=0; idx < MN; idx++){
                uint8_t symbol_bits[d_bps];
                gr_complex symbol = in[ii] * exp(gr_complex(0,2*M_PI*idx/MN));
                d_const.demap_hard(&symbol, &symbol_bits[0], 1);
                for(int ind = d_bps; ind > 0; ind--){
                  //printf("comparing[%d][%d] = symbol_bits[%d] = %d\n",idx,pream_len-ind,d_bps-ind,symbol_bits[d_bps-ind]);
                  comparing[idx][pream_len-ind] = symbol_bits[d_bps-ind];
                }
              }
              //printf("Compare has updated.\n");
              int ham_dist[MN];
              for(int idx=0; idx < MN; idx++){ ham_dist[idx] = 0; }
              for(int idx=0; idx < MN; idx++){
                //printf("idx = %d of MN = %d\t",idx,MN);
                for(int ind=0; ind < pream_len; ind++){
                  ham_dist[idx] += d_preamble[ind]^comparing[idx][ind];
                }
                //printf("dist = %d\n",ham_dist[idx]);
              }
              bool acq_met = false;
              bool loo_met = false;
              for(int idx=0; idx < MN; idx++){
                //printf("ii = %d,\tham_dist[%d] = %d\n",ii,idx,ham_dist[idx]);
                if(ham_dist[idx] <= d_acq_bt){
                  d_offset = idx;
                  acq_met = true;
 //                 d_loose_count = 0;
                  d_state = 2;
                  //printf("Acquired @ ii = %04d\t%d\td_offset = %d\tdist = %d\tidx = %d\n",ii,d_processed+ii,d_offset,ham_dist[idx],idx);
                  //printf("%d\n",d_processed+ii);
                }
                else if(ham_dist[idx] <= d_loo_bt){
                  if(!acq_met){
                    d_offset = idx;
                    loo_met = true;
                    d_state = 1;
                    //printf("Loose @ ii = %d\td_offset = %d\n",ii,d_offset);
                    //printf("Loose @ ii = %04d\td_offset = %d\tdist = %d\tidx = %d",ii,d_offset,ham_dist[idx],idx);
                  }
                }
              }
              if(!acq_met && !loo_met) { ii++; }
            } break;
            case 1: {
              //loose threshhold reached
              //printf("\n");
              //printf("Loose @ ii = %d\td_offset = %d\n",ii,d_offset);
              //printf("State 1(LOO):\td_offset = %d\tin[%d] = (%2.4f,%2.4f)\n",d_offset,ii,in[ii].real(),in[ii].imag());
              //printf("Loose Thresh Met. Proceeding. loose_count = %d\t",d_loose_count);
/*              if(d_loose_count >=100){
                ii += payload_space;
                d_loose_count = 0;
              }
              else{
                d_loose_count++;
              }*/
              d_state = 2;
            } break;
            case 2: {
              //acquire threshhold reached
              //printf("State 2(ACQ):\td_offset = %d\tin[%d] = (%2.4f,%2.4f)\n",d_offset,ii,in[ii].real(),in[ii].imag());
              //printf("Acquiring 1 Payload.\n");
              int start_pointer;
              if(d_passthrough){
                start_pointer = ii+1-preamble_space;
              }
              else{
                start_pointer = ii+1;
              }

              std::vector<gr_complex> in_rotated(n_symbols);

              for(int idx = 0; idx < n_symbols; idx++){
                in_rotated[idx] = in[start_pointer+idx] * exp(gr_complex(0,2*M_PI*d_offset/MN));
              }
              //float soft_out[n_symbols*d_bps];
              //d_const.demap_soft(&in_rotated[0], &soft_out[0], n_symbols);
              uint8_t hard_out[n_symbols*d_bps];
              d_const.demap_hard(&in_rotated[0], &hard_out[0], n_symbols);
              //memcpy(&out[oo], &soft_out[0], sizeof(float)*n_symbols*d_bps);
              memcpy(&out[oo], &hard_out[0], sizeof(uint8_t)*n_symbols*d_bps);
              /*else{
                for(int idx = 0; idx < data_space; idx++){
                  in_rotated[idx] = in[ii+1+idx] * exp(gr_complex(0,2*M_PI*d_offset/MN));
                }
                float soft_out[data_space*d_bps];
                d_const.demap_soft(&in_rotated[0], &soft_out[0], data_space);
                memcpy(&out[oo], &soft_out[0], sizeof(float)*data_space*d_bps);
              }*/
              consumed = ii+1+data_space;
              produced += n_symbols*d_bps;
              oo += n_symbols*d_bps;
              ii += data_space+1;
              comparing.assign(MN,comparer);
              d_state = 0;
            } break;
          }
        }
        if((ii >= ninput_items[0] - data_space) && (oo == 0) && (ninput_items[0] >= 2*payload_space)){
          //printf("No Sync Found. Dumping single payload.\n");
          consumed = payload_space;
          produced = 0;
        }
        //d_const.demap_soft(in,out,noutput_items/d_bps);
        // Do <+signal processing+>
        // Tell runtime system how many input items we consumed on
        // each input stream.
        //printf("Produced = %d,\tConsumed = %d\n",produced,consumed);
        //if(consumed >= 3*payload_space){ printf("nin = %d,\tnout = %d\n",ninput_items[0],noutput_items); }
        consume_each (consumed);
        d_processed += consumed;

        // Tell runtime system how many output items we produced.
        return produced;
    }

  } /* namespace mapper */
} /* namespace gr */

