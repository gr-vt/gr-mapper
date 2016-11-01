#!/usr/bin/env python
import numpy as np
import operator,copy

class prbs_base:
    modes = {
        "PRBS7":[0,6,7],
        "PRBS15":[0,14,15],
        "PRBS23":[0,18,23],
        "PRBS31":[0,28,31]
        }

    def __init__(self, which_mode = "PRBS31", reset_len=100000):
        # https://en.wikipedia.org/wiki/Pseudorandom_binary_sequence
        self.gen_poly = self.modes[which_mode]
        self.reg  = np.ones([max(self.gen_poly)+1], dtype='uint8')
        self.reg[0::4] = 0  # pick a seed that likely doesnt start with long strings of 0s or 1s for sanity sake
        self.init_reg = copy.copy(self.reg)
        self.idx = 0
        self.reset_len = reset_len
        self.pregen()

    def pregen(self):
        self.pre = self.gen_n(self.reset_len, lookup=False)

    def gen_n_nocheck(self, n=1000):
        o = np.zeros([n],dtype='uint8')
        for i in range(0, n):
            nv = reduce(operator.xor, map(lambda x: self.reg[x], self.gen_poly[1:]))
            self.reg[1:] = self.reg[0:-1]
            self.reg[0] = nv
            o[i] = nv
        return o

    def gen_n_nocheck2(self, n=1000):
        return self.pre[self.idx:self.idx+n]

    def gen_n(self, n=1000, lookup=True):
        o = np.array([],dtype='int8')
        while n > 0:
            nout = min(n,self.reset_len-self.idx)
            if lookup:
                o = np.concatenate([o, self.gen_n_nocheck2(nout)])
            else:
                o = np.concatenate([o, self.gen_n_nocheck(nout)])
            self.idx += nout
            n -= nout
            if self.idx == self.reset_len:
                self.reg[:] = self.init_reg[:]
                self.idx = 0
        return o


if __name__ == "__main__":
    m = prbs_base()
    v = m.gen_n(1000)
    print v
        



