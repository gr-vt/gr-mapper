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
#include "preamble_sync_cc_impl.h"
#include <stdio.h>
#include <stdexcept>
#include <cmath>

namespace gr {
  namespace mapper {

    preamble_sync_cc::sptr
    preamble_sync_cc::make(int width, const std::vector<unsigned char> &preamble, modtype_t modtype, const std::vector<int> &symbol_map,
                           float garble_thresh_acquire_sync, float garble_thresh_loose_sync)
    {
      return gnuradio::get_initial_sptr
        (new preamble_sync_cc_impl(width, preamble, modtype, symbol_map, garble_thresh_acquire_sync, garble_thresh_loose_sync));
    }

    preamble_sync_cc_impl::preamble_sync_cc_impl(int width, const std::vector<unsigned char> &preamble, modtype_t modtype, const std::vector<int> &symbol_map,
                                                 float garble_thresh_acquire_sync, float garble_thresh_loose_sync)
      : gr::block("preamble_sync_cc",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(gr_complex))),
        d_const(modtype, symbol_map),
        d_preamble(preamble),
        d_state(0),
        d_last(0),
        d_modtype(modtype),
        d_symbol_map(symbol_map),
        d_gtas(garble_thresh_acquire_sync),
        d_gtls(garble_thresh_loose_sync),
        d_mapidx(0),
        d_offset(0.0)
    {
      //printf("d_gtas = %f, d_gtls = %f\n",d_gtas,d_gtls);
      d_interp = d_const.bps();
      assert(width%d_interp == 0);
      d_width = width;
      get_preamble_phase();
      d_corr = pream_sync_correlator(int(preamble.size()/d_interp),&d_preamble_map[0]);
      //printf("width = %04d, preamble_len = %04d\nsearch_space = %04d, ",width,(int)preamble.size(),(int)preamble.size()/d_interp);
      //printf("data_space = %04d, payload_space = %04d\n",width/d_interp-(int)preamble.size()/d_interp,width/d_interp);
      d_early_term = false;
    }

    preamble_sync_cc_impl::~preamble_sync_cc_impl()
    {
    }

    void
    preamble_sync_cc_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
        //int count = int(noutput_items/(d_width - d_preamble.size()));
        //ninput_items_required[0] = count*d_width;
        float payload_sym_count = (float)d_width/(float(d_interp));
        float payload_sym_count_out = payload_sym_count - float(d_preamble.size())/(float(d_interp));
        int max_out = (int) ceil((float)noutput_items/payload_sym_count_out);
        ninput_items_required[0] = max_out * int(payload_sym_count);
        //printf("noutput_items = %d, ninput_items_required = %d\n",noutput_items,ninput_items_required[0]);
    }

    int
    preamble_sync_cc_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        //printf("nin = %d,\tnout = %d\n",ninput_items[0],noutput_items);
        const gr_complex *in = (const gr_complex *) input_items[0];
        gr_complex *out = (gr_complex *) output_items[0];

        int oo(0), ii(0);
        int consumed = 0;
        int produced = 0;
        int search_space = (int) d_preamble.size()/d_interp;
        int data_space = d_width/d_interp - search_space;
        int payload_space = d_width/d_interp;
        float phase[search_space];
        if(d_early_term) {
          d_corr.reset();
          d_state = 0;
          d_early_term = false;
          /*printf("EARLY TERMINATION OCCURED, REACQUIRE %2.4f\n",d_corr.xcorr_value());*/
        }

        while((oo < noutput_items) && (ii < ninput_items[0]) && (ninput_items[0] >= payload_space)) {
          if((d_state == 0) && ((ninput_items[0] - ii) < payload_space)) { //not enough space for a full preamble, wait
//            printf("::not enough search.\t");
//            printf("remaining input = %d\t",(ninput_items[0]-ii-1));
//            printf("needed = %d\n",search_space+data_space);
//            printf("ii = %d\t",ii);
//            printf("search = %d\n",search_space);
            break; 
          }
          if(((d_state == 1) || (d_state == 2)) &&
              (((noutput_items-oo) < data_space)
              || ((ninput_items[0] - ii - 1) < data_space))) { //not enough for a full payload, wait
//            printf("::not enough data.\t");
//            printf("remaining input = %03d\t",(ninput_items[0]-ii-1));
//            printf("remaining output = %03d\n",(noutput_items-oo));
            d_early_term = true;
//            printf("noutput = %d\t",noutput_items);
//            printf("oo = %d\t",oo);
//            printf("search = %d\t",search_space);
//            printf("data = %d\n",data_space);
            break;
          }
          if((d_state == 3) && (ii < (search_space - 1))) { //not sure what is triggering this
//            printf("Fluke occured, last state = %d\n",d_last);
            d_early_term = true;
            d_last = 3;
            d_state = 0;
          }
          //printf("d_state = %d, ii = %d\n",d_state,ii);
          switch(d_state) {
            case 0: //searching with at least enough room for a preamble
            {
              d_corr.next_symbol(in[ii]);
              float c_val = d_corr.xcorr_value();
              if(c_val > d_gtas){//preamble should be within 0-3 symbols
//                printf("acq ");
                d_last = 0;
                d_state = 1;
              }              
              else if(c_val > d_gtls){//looks like preamble is around here
//                printf("loose ");
                d_last = 0;
                d_state = 2;
              }
              else{//keep looking
                ii++;
              }
              break;
            }
            case 1: //acquire find
            {
//              printf("ii = %04d\n",ii);
              pream_sync_correlator *corr_clone = d_corr.clone();
              std::vector<float> xcorr_vals;
              xcorr_vals.push_back(corr_clone->xcorr_value());
              for(int idx = 1; idx < 4; idx++){
                corr_clone->next_symbol(in[ii+idx]);
                xcorr_vals.push_back(corr_clone->xcorr_value());
              }
              int maxidx = 0;
              float max = xcorr_vals[0];
//              printf("Cval[%04d] = %2.4f,",ii,max);
              for(int idx = 1; idx < 4; idx++){
//                printf("\tCval[%04d] = %2.4f,",ii+idx,xcorr_vals[idx]);
                if(max < xcorr_vals[idx]){ max = xcorr_vals[idx]; maxidx = idx; }
              }
              ii += maxidx;
//              printf(" ii = %04d\n",ii);
              d_last = 1;
//              printf("ii = %04d, N_in = %04d, remain = %04d\n",ii,ninput_items[0],ninput_items[0]-(ii+1));
              d_state = 3;
              break;
            }
            case 2: //loose find
            {
              //d_corr.reset();
              //printf("ii = %04d\n",ii);
              pream_sync_correlator *corr_clone = d_corr.clone();
              std::vector<float> xcorr_vals;
              xcorr_vals.push_back(corr_clone->xcorr_value());
              for(int idx = 1; idx < search_space; idx++){
                corr_clone->next_symbol(in[ii+idx]);
                xcorr_vals.push_back(corr_clone->xcorr_value());
              }
              int index = 0;
              int maxidx = 0;
              float max = 0.0;
              while(d_gtas > xcorr_vals[index]){
                if(max < xcorr_vals[index]){ max = xcorr_vals[index]; maxidx = index; }
                index++;
                if(index == search_space) break;
              }
              if(index < search_space){
                ii += index;
//                printf("\t\tpath chosen = (%d,%d)\n",2,1);
                d_last = 2;
                d_state = 1;
                break;
              }
              else{// this should hopefully never happen at high snr (not sure about low snr)
                if(maxidx == 0){ d_corr.rewind(); }
                ii += maxidx-1;
//                printf("\t\tpath chosen = (%d,%d)\n",2,2);
                d_last = 2;
                d_state = 1;
                break;
              }
            }
            case 3: //found
            {
              //printf("State 2.\n");
              //printf("Outputing now.\n");
///////////////////////////////////////////No leftovers allowed!!!!
              ////////////correct phase offset
              //int max_copy = std::min(data_space,(noutput_items - oo)/d_interp);
              if(ii == 0) {  //this shouldn't trigger, but it is...
                printf("FLUKE?");
              }
              std::vector<float> offset_vec;
              offset_vec.push_back(0.0);
              int max = std::max(ii-search_space+1,0);
              int ss = 0;
              if(ii-search_space+1 < 0){ ss = ii+1; }
              else { ss = search_space; }
              int off_ss = search_space - ss;
              for(int idx=max; idx < max+ss; idx++){//there's at least d_width/d_interp more within 'in'
                offset_vec.push_back(arg(in[idx]) - arg(d_preamble_map[idx-max+off_ss]));
                //printf("offset[%d] = %f\t",idx,offset_vec[idx]);
              }
              //printf("\n");
              unwrap(&offset_vec[0],ss);
              float offset = 0.0;
              for(int idx=0; idx < ss; idx++){
                offset += offset_vec[idx];
              }
              //printf("Offset = %f, search_space = %d, mean = %f\n",offset,search_space,offset/((float)search_space));
              offset = offset/float(ss);
              d_offset = offset;
/*              if(offset > -0.35){
                printf("ii = %04d ",ii);
                printf("offset = %2.4f\n", offset);
              }*/
              //correct only what's consumed
              std::vector<gr_complex> payload;
              for(int idx=ii+1; idx < ii+1+data_space; idx++){//correct data only
                payload.push_back( in[idx] *  exp(gr_complex(0,-d_offset)));//
              }
              //DECODE HERE
              //d_const.demap_hard(&payload[0], &out[oo], data_space);
/*              for(int idx=0; idx<(int)payload.size(); idx++){
                out[idx+oo] = payload[idx];
              }
              consumed = ii+d_width;
              //produced += data_space*d_interp;
              produced += data_space;

              ii += consumed;
              oo += produced;
              d_state = 0;*/
              consumed = ii+data_space+1;
              memcpy(&out[oo], &payload[0], sizeof(gr_complex)*data_space);
              //memcpy(&out[oo], &in[ii+1], sizeof(gr_complex)*data_space);
              oo += data_space;
              produced = oo;
              //printf("\t\tconsumed = %d,\tproduced = %d\n",consumed,produced);
              for(int idx = ii+data_space-search_space+1; idx < ii+data_space+1; idx++){
                d_corr.next_symbol(in[idx]);
              }
              ii += data_space+1;
              d_last = 3;
              d_state = 0;
              break;
            }
          }
        }
        

        // Tell runtime system how many input items we consumed on
        // each input stream.
        consume_each (consumed);

        // Tell runtime system how many output items we produced.
        //printf("consumed = %d,\tproduced = %d\n",consumed,produced);
        return produced;
    }

    void preamble_sync_cc_impl::get_preamble_phase() {
      //gr_complex *desired;
      uint8_t *preamble = (uint8_t *) &d_preamble[0];
      std::vector<gr_complex> preamble_map(int(d_preamble.size()/d_interp));
      //printf("Preamble Sync - mapping.\n");
      //for(int ind = 0; ind<d_preamble.size(); ind++){ printf("in[%d] = %d\n",ind,preamble[ind]); }
      //printf("count = %d,\t",int(d_preamble.size() / d_interp));
      //printf("idx = %d \n",d_mapidx);
      d_const.map(&preamble[0], &preamble_map[0], int(d_preamble.size() / d_interp), d_mapidx);
      for(int idx = 0; idx < d_preamble.size()/d_interp; idx++){
        d_preamble_map.push_back(preamble_map[idx]);
      }
      //for(int ind = 0; ind<d_preamble.size()/d_interp; ind++){ printf("d_preamble_map[%d] = (%f, %f)\n",ind,d_preamble_map[ind].real(),d_preamble_map[ind].imag()); }
      d_mapidx = (d_mapidx + d_preamble.size()/d_interp)%d_const.nmaps();
      float phase[int(d_preamble.size()/d_interp)];
      for(int idx = 0; idx < d_preamble.size()/d_interp; idx++){
        phase[idx] = arg(d_preamble_map[idx]);
      }
      unwrap(phase,(int)d_preamble.size()/d_interp);
      float phase_change[d_preamble.size()/d_interp];
      phase_change[0] = 0.0;
      for(int ind = 1; ind < d_preamble.size()/d_interp; ind++) {
        phase_change[ind] = phase[ind] - phase[ind-1];
      }
      for(int ind = 0; ind < d_preamble.size()/d_interp; ind++) d_preamble_phase.push_back(phase_change[ind]);
//      d_preamble_phase = phase_change;
      //for(int ind = 0; ind<d_preamble.size()/d_interp; ind++){ printf("d_preamble_phase[%d] = %f\n",ind,d_preamble_phase[ind]); }
    }
    
    void preamble_sync_cc_impl::unwrap(float *phase_array, int N) {
      float dp[N];
      float dps[N];
      float dp_corr[N];
      float cumsum[N];
      float cutoff = M_PI;
      int j;

      for(j=0; j<N-1; j++){
        dp[j] = phase_array[j+1] - phase_array[j];
        dps[j] = (dp[j]+cutoff) - floor((dp[j]+cutoff)/(2*cutoff))
                  * (2*cutoff) - cutoff;
        if((dps[j] == -cutoff) && (dp[j] > 0)) { dps[j] = cutoff; }
        dp_corr[j] = dps[j] - dp[j];
        if(fabs(dp[j]) < cutoff) { dp_corr[j] = 0; }
      }
      cumsum[0] = dp_corr[0];
      for(j=1; j< N ; j++){
        cumsum[j] = cumsum[j-1] + dp_corr[j];
        phase_array[j] += cumsum[j-1];
      }
    }
    

  } /* namespace mapper */
} /* namespace gr */

