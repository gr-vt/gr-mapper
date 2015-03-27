#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright 2013 <+YOU OR YOUR COMPANY+>.
# 
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

from gnuradio import gr, gr_unittest
from gnuradio import blocks
import mapper
import numpy
from time import sleep

class qa_preamble (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

		
    def test_mapping (self):
        src_data = (0,0,0,1,0,0,1,0,1,1,1,1,1,1,0,0,1,0,0,1,1,0,0,1,)*16
        src = blocks.vector_source_b(src_data, False)
        mapped = mapper.mapper(mapper.PSK8, ([0,1,3,2,6,7,5,4]))
        demapped = mapper.demapper(mapper.PSK8, ([0,1,3,2,6,7,5,4]))

        dst = blocks.vector_sink_b(1)
        self.tb.connect(src, mapped, demapped, dst)
        self.tb.run()
        self.assertEqual(src_data, dst.data())
		
    def test_preamble (self):
        pream_len = 52
        pream = (mapper.preamble_generator(pream_len,511,1033)).get_preamble()
        
        rand_src    = blocks.vector_source_b(map(int, numpy.random.randint(0, 2, 1024)), True)
        head        = blocks.head(gr.sizeof_char*1, 1024)
        src_sink    = blocks.vector_sink_b(1)
        pream_inst  = mapper.preamble_insert_bb(pream_len*10, (pream))
        bit2symb    = mapper.mapper(mapper.BPSK, ([0,1]))
        pream_sync  = mapper.preamble_sync_cc(pream_len*10, (pream), mapper.BPSK, ([0,1]), .97, .90)
        symb2bit    = mapper.demapper(mapper.BPSK, ([0,1]))
        rec_sink    = blocks.vector_sink_b(1)

        self.tb.connect((rand_src, 0), (head, 0))
        self.tb.connect((head, 0), (pream_inst, 0))
        self.tb.connect((head, 0), (src_sink, 0))
        self.tb.connect((pream_inst, 0), (bit2symb, 0))
        self.tb.connect((bit2symb, 0), (pream_sync, 0))
        self.tb.connect((pream_sync, 0), (symb2bit, 0))
        self.tb.connect((symb2bit, 0), (rec_sink, 0))
        
        self.tb.start()
        sleep(1)
        self.tb.stop()
        
        data_space = pream_len*9
        sd = src_sink.data()
        rd = rec_sink.data()
        self.assertEqual(sd[0:data_space],rd[0:data_space])


if __name__ == '__main__':
    gr_unittest.run(qa_preamble, "qa_preamble.xml")
