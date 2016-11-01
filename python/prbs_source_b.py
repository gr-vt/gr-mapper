#!/usr/bin/env python

import numpy
from gnuradio import gr
import prbs_base

class prbs_source_b(gr.sync_block):
    def __init__(self, which_mode="PRBS31", reset_len=100000):
        gr.sync_block.__init__(self,
            name="prbs_source_b",
            in_sig=[],
            out_sig=[numpy.int8])
        self.base = prbs_base.prbs_base(which_mode, reset_len)

    def work(self, input_items, output_items):
        out = output_items[0]
        nout = len(out)
        gen = self.base.gen_n(nout)
        out[:] = gen[:]
        return nout

