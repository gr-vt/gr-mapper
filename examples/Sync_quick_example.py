#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: Top Block
# Generated: Mon Apr  7 14:53:04 2014
##################################################

from cmath import exp, pi
from random import randint
from math import floor
from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser
import mapper
import numpy
from time import *


tb = gr.top_block()

##################################################
# Variables
##################################################
pream_len = pream_len = 52
samp_rate = samp_rate = 32000
pream = pream = (mapper.preamble_generator(pream_len,511,1033)).get_preamble()

##################################################
# Blocks
##################################################
rand_src    = blocks.vector_source_b(map(int, numpy.random.randint(0, 2, 1024)), True)
#head        = blocks.head(gr.sizeof_char*1, 1024)
src_sink    = blocks.vector_sink_b(1)
pream_inst  = mapper.preamble_insert_bb(pream_len*10, (pream))
pre_sink    = blocks.vector_sink_b(1)
bit2symb    = mapper.mapper(mapper.BPSK, ([0,1]))
sym_sink    = blocks.vector_sink_c(1)
pream_sync  = mapper.preamble_sync_cc(pream_len*10, (pream), mapper.BPSK, ([0,1]), .97, .90)
snc_sink    = blocks.vector_sink_c(1)
symb2bit    = mapper.demapper(mapper.BPSK, ([0,1]))
rec_sink    = blocks.vector_sink_b(1)

##################################################
# Connections
##################################################
#tb.connect((rand_src, 0), (head, 0))
tb.connect((rand_src, 0), (pream_inst, 0))
tb.connect((rand_src, 0), (src_sink, 0))
#tb.connect((head, 0), (pream_inst, 0))
#tb.connect((head, 0), (src_sink, 0))
tb.connect((pream_inst, 0), (bit2symb, 0))
tb.connect((pream_inst, 0), (pre_sink, 0))
tb.connect((bit2symb, 0), (pream_sync, 0))
tb.connect((bit2symb, 0), (sym_sink, 0))
tb.connect((pream_sync, 0), (symb2bit, 0))
tb.connect((pream_sync, 0), (snc_sink, 0))
tb.connect((symb2bit, 0), (rec_sink, 0))


tb.start()
sleep(1)
tb.stop()
    
print "preamble", pream
sd = src_sink.data()
print "source data", len(sd)
pd = pre_sink.data()
print "bit stream", len(pd)
sy = sym_sink.data()
print "symbols", len(sy)
sn = snc_sink.data()
print "sync symbols", len(sn)
rd = rec_sink.data()
print "recoved", len(rd)

first_data = []
check_data = []
data_space = pream_len*9
poss = len(rd)/data_space
#print poss

for x in range(0,poss):
  first_data.append(sd[x*data_space:(x+1)*data_space])
  check_data.append(rd[x*data_space:(x+1)*data_space])

#for x in range(0,poss):
#  print first_data[x] == check_data[x]

print first_data == check_data


