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


#include <mapper/api.h>
#include <mapper/pream_sync_correlator.h>
#include <gnuradio/gr_complex.h>
#include <cmath>
#include <stdio.h>

//#define MULTI_COMP(x,y)  gr_complex(  ((x.real()*y.real())-(x.imag()-y.imag())) , ((x.real()*y.imag())+(x.imag()*y.real())) )

namespace gr {
  namespace mapper {

    pream_sync_correlator::pream_sync_correlator() :
      d_preamble_symbol_count(0)
    {
        d_preamble_symbols.push_back(gr_complex(0,0));
        d_preamble_phase_change.push_back(0.0);
        d_in_stream.push_back(gr_complex(0,0));
    }

    pream_sync_correlator::pream_sync_correlator(int N, gr_complex *symbols) :
      d_preamble_symbol_count(N)
    {
      for(int idx = 0; idx < N; idx++){
        d_preamble_symbols.push_back(symbols[idx]);
        d_in_stream.push_back(gr_complex(0,0));
      }
      d_in_stream.push_back(gr_complex(0,0));
      float phase[N];
      for(int idx = 1; idx < N; idx++){
        phase[idx] = arg(d_preamble_symbols[idx]);
      }
      unwrap(phase,N);
      d_preamble_phase_change.push_back(0.0);
      for(int idx = 1; idx < N; idx++){
        d_preamble_phase_change.push_back(phase[idx] - phase[idx-1]);
      }
    }
    
    pream_sync_correlator*
    pream_sync_correlator::clone(void) const
    {
      /*pream_sync_correlator new_clone = pream_sync_correlator(d_preamble_symbol_count,&d_preamble_symbols[0]);
      for(idx = d_preamble_symbol_count-1; idx >= 0; idx--){
        new_clone.next_symbol(d_in_stream[idx]);
      }*/
      return new pream_sync_correlator(*this);
    }

    pream_sync_correlator::~pream_sync_correlator()
    {}
      
    void pream_sync_correlator::next_symbol(gr_complex next_sym){
/*      for(int idx = d_preamble_symbol_count; idx > 0; idx--){
        d_in_stream[idx] = d_in_stream[idx-1];
      }
      d_in_stream[0] = next_sym;*/
      for(int idx = 0; idx < d_preamble_symbol_count; idx++){
        d_in_stream[idx] = d_in_stream[idx+1];
      }
      d_in_stream[d_preamble_symbol_count] = next_sym;
    }

    float pream_sync_correlator::xcorr_value(void){//cross correlation
      gr_complex out(0,0);
/*      for(int idx = 0; idx < d_preamble_symbol_count; idx++){
        out += d_in_stream[d_preamble_symbol_count - 1 - idx] * conj(d_preamble_symbols[idx]);
      }
      return (float)(abs(out)/(float(d_preamble_symbol_count)));*/
      for(int idx = 0; idx < d_preamble_symbol_count; idx++){
        gr_complex temp = (d_in_stream[idx + 1] * conj(d_preamble_symbols[idx]));
//        if(idx == 0) { printf("(%2.4f,%2.4f) ",temp.real(),temp.imag()); }
//        else { printf("+ (%2.4f,%2.4f) ",temp.real(),temp.imag()); }
        out+= temp;
      }
      double holder = sqrt(double(out.real() * out.real()) + double(out.imag() * out.imag()))/d_preamble_symbol_count;
//      printf("= (%2.4f,%2.4f) => (%2.4f)\n",out.real(),out.imag(), holder);
      return float(holder);
    }

    float pream_sync_correlator::acquire_value(void){
      float phase[d_preamble_symbol_count];
      for(int idx=0; idx < d_preamble_symbol_count; idx++){
        phase[idx] = arg(d_in_stream[d_preamble_symbol_count - 1 - idx]);
//          printf("\t1: sa[%02d] = (%02.4f,%02.4f), phase[%02d] = %02.4f\n",idx,small_array[idx].real(),small_array[idx].imag(),idx,phase[idx]);
      }
      unwrap(phase, d_preamble_symbol_count);
      float phase_change[d_preamble_symbol_count];
      float phase_diff = 0.0;//[d_preamble_symbol_count-1];
      phase_change[0] = 0.0;
//      phase_diff[idx] = 0.0;
//          printf("\t\t2: phase_portion[%02d,%02d] = %02.4f, \t\t\t\t\t\t\t\t\t\t\t\t",idx,0,phase_portion[0]);
//          printf("\t3: phase_change[%02d] = %02.4f\n",0,phase_change[0]);
      for(int ind = 1; ind < d_preamble_symbol_count; ind++){
//          printf("\t\t2: phase_portion[%02d] = %02.4f,\tphase_portion[%02d] = %02.4f",ind,phase_portion[ind],ind-1,phase_portion[ind-1]);
        phase_change[ind] = phase[ind] - phase[ind-1];
//          printf("\t3: phase_change[%02d] = %02.4f\n",ind,phase_change[ind]);
        phase_diff += fabs(d_preamble_phase_change[ind] - phase_change[ind]);
      }
//        printf("\t\t\t\t4: val = %02.4f, idx = %02d\n",phase_diff[idx],idx);
//      float min = phase_diff[0];
//      int minidx = 0;
//      for(int idx = 0; idx < N-d_preamble_symbol_count; idx++){
//        if(min > phase_diff[idx]){
//          min = phase_diff[idx];
//          minidx = idx;
//        }
//      }
//      printf("\tminval = %02.4f, idx = %02d\n",min,minidx);
//      printf("\n\n");
      return phase_diff;
    }
    
    void pream_sync_correlator::unwrap(float *phase_array, int N) {
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

    void pream_sync_correlator::reset(void){
      for(int idx = 0; idx < d_preamble_symbol_count; idx++){
        d_in_stream[idx] = (gr_complex(0,0));
      }
    }

    void pream_sync_correlator::rewind(void){//THIS CAN ONLY BE USED FOR 1 REWIND!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
/*      for(int idx = 0; idx < d_preamble_symbol_count; idx ++){
        d_in_stream[idx] = d_in_stream[idx+1];
      }*/
      for(int idx = d_preamble_symbol_count; idx > 0; idx--){
        d_in_stream[idx] = d_in_stream[idx-1];
      }
    }
  }
}

