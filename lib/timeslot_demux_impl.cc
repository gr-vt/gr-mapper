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
#include "timeslot_demux_impl.h"
#include <stdio.h>

namespace gr {
  namespace mapper {

    timeslot_demux::sptr
    timeslot_demux::make(int nusers, bool syncd, const std::vector<uint8_t> &ts_id, int ts_len)
    {
      return gnuradio::get_initial_sptr
        (new timeslot_demux_impl(nusers, syncd, ts_id, ts_len));
    }

    /*
     * The private constructor
     */
    timeslot_demux_impl::timeslot_demux_impl(int nusers, bool syncd, const std::vector<uint8_t> &ts_id, int ts_len)
      : gr::block("timeslot_demux",
              gr::io_signature::make(1, 1, sizeof(uint8_t)),
              gr::io_signature::make(1, 1, sizeof(uint8_t))),
        D_Nusers(nusers),
        D_synch(syncd),
        D_ts_key(ts_id),
        D_length(ts_len),
        D_purge((nusers-1)*ts_len),
        D_offset(0),
        D_search(ts_id.size(),255),
        D_state(0),
        D_fill(0)
    {
      //printf("Number of users = %d\n",D_Nusers);
      //printf("User length = %d\n",D_length);
      //printf("User key = %u",D_ts_key[0]);
      //for(int i = 1; i < D_ts_key.size(); i++) printf(", %u",D_ts_key[i]);
      //printf("\n");
      //printf("Purge Length = %d\n",D_purge);
    }

    /*
     * Our virtual destructor.
     */
    timeslot_demux_impl::~timeslot_demux_impl()
    {
    }

    void
    timeslot_demux_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        ninput_items_required[0] = noutput_items*D_Nusers;
    }

    int
    timeslot_demux_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        //printf("Made it here.C\n");
        const uint8_t *in = (const uint8_t *) input_items[0];
        uint8_t *out = (uint8_t *) output_items[0];

        int ii(0), oo(0);

        if(D_synch){
          //printf("Working in synched mode.\n");
          while((ii<ninput_items[0])&&(oo<noutput_items)){
            switch(D_state)
            {
              case 0:{//starting up
                //printf("Doing initial TS load.\n");
                //printf("State 0.\n");
                D_search[ii%D_ts_key.size()] = in[ii];
                ii++;
                D_fill++;
                if(ii >= D_ts_key.size()){
                  D_state = 1;
                  //printf("D_ts_key = %u",D_ts_key[0]);
                  //for(int i =1; i<D_ts_key.size(); i++) printf(", %u",D_ts_key[i]);
                  //printf("\t D_search = %u",D_search[0]);
                  //for(int i =1; i<D_search.size(); i++) printf(", %u",D_search[i]);
                  //printf("\t");
                }
                break;
              }
              case 1:{//start up completed -> search
                //printf("Doing comparisons on the TS.\n");
                int count = 0;
                for(int i=0; i < D_ts_key.size(); i++){
                  count += (D_ts_key[i]==D_search[i]);
                }
                if(count == D_ts_key.size()){//user data found
                  D_offset = 0;
                  D_state = 2;
                  //printf("Going to 2\n");
                  //printf("User ID GOOD\n");
                }
                else{//not sure if wrong user or reloading user id
                  if(D_fill==D_ts_key.size()){//user id is loaded, wrong user
                    D_offset = D_ts_key.size();
                    D_state = 3;
                    //printf("Going to 3\n");
                    //printf("User ID FAIL\n");
                  }
                  else{//user id is still being loaded
                    for(int i=0; i < D_search.size(); i++){
                      D_search[i] = D_search[i+1];
                    }
                    D_search[D_search.size()-1] = in[ii];
                    ii++;
                    D_fill++;
                    //printf("User ID LOAD\n");
                  }
                }
                break;
              }
              case 2:{//search complete -> move
                //ii is first to oo
                //printf("TS identified, copying.\n");
                int max_copy = D_length - D_ts_key.size() - D_offset;
                max_copy = ((noutput_items-oo) < max_copy) ? (noutput_items-oo) : max_copy;
                max_copy = ((ninput_items[0]-ii) < max_copy) ? (ninput_items[0]-ii) : max_copy;
                D_offset = (D_offset+max_copy)%(D_length-D_ts_key.size());
                memcpy( &out[oo], &in[ii], sizeof(uint8_t)*max_copy );
                oo += max_copy;
                ii += max_copy;
                if(D_offset==0){//remove other users now
                  if(D_Nusers == 1){
                    for(int i=0; i<D_search.size(); i++) D_search[i] = 255;
                    D_state = 1;
                    D_fill = 0;
                  }else{
                    D_state = 3;
                  }
                }
                break;
              }
              case 3:{
                //printf("TS not identified, purging.\n");
                int max_purge = 0;
                int limit = 0;
                if(D_purge == 0){
                  max_purge = D_length - D_offset;
                  limit = D_length;
                }
                else{
                  max_purge = D_purge - D_offset;
                  limit = D_purge;
                }
                //printf("first purge val = %d",max_purge);
                max_purge = ((ninput_items[0]-ii) < max_purge) ? (ninput_items[0]-ii) : max_purge;
                if(max_purge <= 0) max_purge = 0;
                D_offset = (D_offset+max_purge)%(limit);
                //printf("\tNew offset = %d\n",D_offset);
                ii += max_purge;
                if(D_offset==0){//reset D_search and search
                  for(int i=0; i<D_search.size(); i++) D_search[i] = 255;
                  D_state = 1;
                  D_fill = 0;
                }
                break;
              }
              default:{//something bad has happened....
                return -1;
              }
            }
          }
        }else{
          while((ii<ninput_items[0])&&(oo<noutput_items)){
            switch(D_state)
            {
              case 0:{//starting up
                D_search[ii%D_ts_key.size()] = in[ii];
                ii++;
                if(ii >= D_ts_key.size()) D_state = 1;
                break;
              }
              case 1:{//start up completed -> search
                int count = 0;
                for(int i=0; i < D_ts_key.size(); i++){
                  count += (D_ts_key[i]==D_search[i]);
                }
                if(count == D_ts_key.size()){
                  D_state = 2;
                  D_offset = 0;
                }
                else{
                  for(int i=0; i < D_search.size(); i++){
                    D_search[i] = D_search[i+1];
                  }
                  D_search[D_search.size()-1] = in[ii];
                  ii++;
                }
                break;
              }
              case 2:{//search complete -> move
                //ii is first to oo
                int max_copy = D_length - D_ts_key.size() - D_offset;
                max_copy = ((noutput_items-oo) < max_copy) ? (noutput_items-oo) : max_copy;
                max_copy = ((ninput_items[0]-ii) < max_copy) ? (ninput_items[0]-ii) : max_copy;
                D_offset = (D_offset+max_copy)%(D_length-D_ts_key.size());
                memcpy( &out[oo], &in[ii], sizeof(uint8_t)*max_copy );
                oo += max_copy;
                ii += max_copy;
                if(D_offset==0){//remove other users now
                  D_state = 3;
                }
                break;
              }
              case 3:{
                int max_purge = 0;
                int limit = 0;
                if(D_purge == 0){
                  max_purge = D_length - D_offset;
                  limit = D_length;
                }
                else{
                  max_purge = D_purge - D_offset;
                  limit = D_purge;
                }
                max_purge = ((ninput_items[0]-ii) < max_purge) ? (ninput_items[0]-ii) : max_purge;
                if(max_purge <= 0) max_purge = 0;
                D_offset = (D_offset+max_purge)%(limit);
                ii += max_purge;
                if(D_offset==0){//reset d_search and search
                  for(int i=0; i<D_search.size(); i++) D_search[i] = 255;
                  D_state = 1;
                }
                break;
              }
              default:{
                for(int i=0; i< D_search.size(); i++) D_search[i] = 255;
                D_state = 1;
              }
            }
          }
        }

        consume_each (ii);

        // Tell runtime system how many output items we produced.
        return oo;
    }

  } /* namespace mapper */
} /* namespace gr */

