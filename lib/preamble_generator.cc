#include <mapper/preamble_generator.h>
#include <stdio.h>

namespace gr {
  namespace mapper {

    preamble_generator::preamble_generator() :
      d_bit_length(0),
      d_seed(0),
      d_mask(0),
      d_reg(0)
    {
        d_preamble.push_back(0);
    }

    preamble_generator::preamble_generator(int N, int seed, int mask) :
      d_bit_length(N),
      d_seed(seed),
      d_mask(mask),
      d_reg(seed)
    {
      for(int idx = 0; idx < d_bit_length; idx++){ d_preamble.push_back(0); }
      make_preamble();
    }
    
    preamble_generator*
    preamble_generator::clone(void) const
    {
      return new preamble_generator(*this);
    }

    preamble_generator::~preamble_generator()
    {}

    void preamble_generator::make_preamble(){
      d_reg = d_seed;
      for(int idx = 0; idx < d_bit_length; idx++){
        d_preamble[idx] = d_reg&1;
        d_reg = (d_reg>>1)^(d_preamble[idx]*d_mask);
      }
    }

    std::vector<int> preamble_generator::get_preamble(){
      return d_preamble;
    }

  } /* namespace mapper */
} /* namespace gr */

