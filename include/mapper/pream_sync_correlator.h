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

#ifndef INCLUDED_MAPPER_PREAMBLE_SYNC_CORRELATOR_H
#define INCLUDED_MAPPER_PREAMBLE_SYNC_CORRELATOR_H

#include <mapper/api.h>
#include <gnuradio/gr_complex.h>
#include <cmath>
#include <stdio.h>
#include <vector>


namespace gr {
  namespace mapper {

    class MAPPER_API pream_sync_correlator {
      public:
        pream_sync_correlator();
        pream_sync_correlator(int N, gr_complex *symbols);
        ~pream_sync_correlator();
        void next_symbol(gr_complex next_sym);
        float xcorr_value(void);
        float acquire_value(void);
        void reset(void);
        void rewind(void);
        pream_sync_correlator* clone() const;
        
      private:
        int d_preamble_symbol_count;
        std::vector<gr_complex> d_preamble_symbols;
        std::vector<float> d_preamble_phase_change;
        std::vector<gr_complex> d_in_stream;
        void unwrap(float *phase_array, int N);
    };
  }
}

#endif /* INCLUDED_MAPPER_PREAMBLE_SYNC_CORRELATOR_H */
