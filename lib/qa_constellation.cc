/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

/*
 * This class gathers together all the test cases for the gr-filter
 * directory into a single test suite.  As you create new test cases,
 * add them here.
 */

#include <gnuradio/attributes.h>
#include <cppunit/TestAssert.h>
#include "qa_constellation.h"

namespace gr {
  namespace mapper {

    
    void 
    qa_constellation::map_demap(constellation &c, std::vector<uint8_t> &data_bits, std::vector<uint8_t> &hard_out, std::vector<float> &soft_out, std::vector<uint8_t> &soft_sign_out)
    {
        // generate random bits
        int nbits = data_bits.size();
        printf("gen bits.\n");
        for(int i=0; i<nbits; i++){
            data_bits[i] = random()%2;
        }
        
        // map bits
        printf("map syms.\n");
        int nsym = nbits/c.bps();
        std::vector<gr_complex> syms(nbits/c.bps());
        c.map( &data_bits[0], &syms[0], nsym);

        // add slight offsets (Avoid div by zero distance?)
        for(int i=0; i<nsym; i++){
            printf("sym[%f,%f]\n", syms[i].real(), syms[i].imag());
            syms[i] += gr_complex(0.01, 0.01);
            }

        // demap hard
        printf("demap hard.\n");
        hard_out.resize(nsym*c.bps());
        c.demap_hard(&syms[0], &hard_out[0], nsym); 
        
        // demap soft
        printf("demap soft.\n");
        soft_out.resize(nsym*c.bps());
        soft_sign_out.resize(nsym*c.bps());
        c.demap_soft(&syms[0], &soft_out[0], nsym);       
        for(int i=0; i<nbits; i++){
            soft_sign_out[i] = (soft_out[i]>0)?1:0;
        }
    }

    void
    qa_constellation::t0()
    {
        printf("TEST BPSK\n");

        // Test QPSK
        int nbits = 6;

        std::vector<int> gm(2);
        gm[0] = 0; gm[1] = 1;;
        //gm[0] = 1; gm[1] = 0;;
        CPPUNIT_ASSERT_EQUAL(gm.size(), 2UL);
        constellation c1(BPSK, gm);

        // output vectors
        std::vector<uint8_t> data_bits(nbits);
        std::vector<uint8_t> out_hard;
        std::vector<float> out_soft;
        std::vector<uint8_t> out_soft_sign;
        map_demap(c1, data_bits, out_hard, out_soft, out_soft_sign);

        // compare hard bits      
        printf("compare hard decisions...\n");
        for(int i=0; i<data_bits.size(); i++){
            printf("cmp[%d -> %d]\n", data_bits[i], out_hard[i]);
            CPPUNIT_ASSERT_EQUAL(data_bits[i], out_hard[i]);
            }

        // compare soft/sign bits 
        printf("compare soft decision signs...\n");
        for(int i=0; i<data_bits.size(); i++){
            printf("cmp[%d -> %d]\n", data_bits[i], out_soft_sign[i]);
            CPPUNIT_ASSERT_EQUAL(data_bits[i], out_soft_sign[i]);
            }
    }


    void
    qa_constellation::t1()
    {
        printf("TEST QPSK\n");

        // Test QPSK
        int nbits = 2*4;
    
        std::vector<int> gm(4);
        gm[0] = 0; gm[1] = 1; gm[2] = 3; gm[3] = 2;
        //gm[0] = 2; gm[1] = 0; gm[2] =3; gm[3] = 1;
        CPPUNIT_ASSERT_EQUAL(gm.size(), 4UL);
        constellation c1(QPSK, gm);

        // output vectors
        std::vector<uint8_t> data_bits(nbits);
        std::vector<uint8_t> out_hard;
        std::vector<float> out_soft;
        std::vector<uint8_t> out_soft_sign;
        map_demap(c1, data_bits, out_hard, out_soft, out_soft_sign);
    
        // compare hard bits      
        printf("compare hard decisions...\n");     
        for(int i=0; i<data_bits.size(); i++){
            printf("cmp[%d -> %d]\n", data_bits[i], out_hard[i]);
            CPPUNIT_ASSERT_EQUAL(data_bits[i], out_hard[i]);   
            }
        
        // compare soft/sign bits 
        printf("compare soft decision signs...\n");     
        for(int i=0; i<data_bits.size(); i++){
            printf("cmp[%d -> %d]\n", data_bits[i], out_soft_sign[i]);
            CPPUNIT_ASSERT_EQUAL(data_bits[i], out_soft_sign[i]);   
            }
    }

    void
    qa_constellation::t2()
    {
        printf("TEST 8PSK\n");

        // Test 8PSK
        int nbits = 3*10;

        std::vector<int> gm(8);
        for(int i=0; i<8; i++){
            gm[i] = default_greymap_psk8[i];
            }
        
        CPPUNIT_ASSERT_EQUAL(gm.size(), 8UL);
        constellation c1(PSK8, gm);

        // output vectors
        std::vector<uint8_t> data_bits(nbits);
        std::vector<uint8_t> out_hard;
        std::vector<float> out_soft;
        std::vector<uint8_t> out_soft_sign;
        map_demap(c1, data_bits, out_hard, out_soft, out_soft_sign);

        // compare hard bits      
        printf("compare hard decisions...\n");
        for(int i=0; i<data_bits.size(); i++){
            printf("cmp[%d -> %d]\n", data_bits[i], out_hard[i]);
            CPPUNIT_ASSERT_EQUAL(data_bits[i], out_hard[i]);
            }

        // compare soft/sign bits 
        printf("compare soft decision signs...\n");
        for(int i=0; i<data_bits.size(); i++){
            printf("cmp[%d -> %d]\n", data_bits[i], out_soft_sign[i]);
            CPPUNIT_ASSERT_EQUAL(data_bits[i], out_soft_sign[i]);
            }
    }


  } /* namespace constellation */
} /* namespace gr */

        
