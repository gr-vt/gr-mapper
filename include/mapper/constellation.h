#ifndef CONSTELLATION_H
#define CONSTELLATION_H

#include <mapper/api.h>
#include <gnuradio/gr_complex.h>
#include <boost/variant.hpp>
#include <stdint.h>
#include <vector>
#include <map>

namespace gr {
  namespace mapper {

    const int default_greymap_bpsk[2] = {0,1};
    const int default_greymap_qpsk[4] = {0,1,3,2};
//    const int default_greymap_psk8[8] = {7,6,2,3,1,0,4,5};
    const int default_greymap_psk8[8] = {0,1,3,2,7,6,4,5};
    const int default_greymap_pam4[4] = {0,1,3,2};
    const int default_greymap_qam16[16] = {2,6,14,10,3,7,15,11,1,5,13,9,0,4,12,8};
    const int default_greymap_qam64[64] = {0,32,8,40,3,35,11,43,48,16,56,24,51,19,59,27,12,44,4,36,15,47,7,39,60,28,52,20,63,31,55,23,2,34,10,42,1,33,9,41,50,18,58,26,49,17,57,25,14,46,6,38,13,45,5,37,62,30,54,22,61,29,53,21};
    

    typedef enum {
        BPSK=0,
        P2BPSK=1,
        QPSK=2,
        PSK8=3,
        PAM4=4,
        QAM16=5,
        QAM64=6,
        } modtype_t;

    class MAPPER_API constellation {
        public: 
          constellation(modtype_t modtype, std::vector<int> symbol_values, gr_complex scalar = gr_complex(1,0));
          void map(const uint8_t *in, gr_complex* out, int nsymbols, uint16_t mapidx=0);
          void map(const float *in, gr_complex* out, int nsymbols, uint16_t mapidx=0);
          void demap_hard(const gr_complex* in, uint8_t *out, int nsymbols);
          void demap_soft(const gr_complex* in, float *out, int nsymbols);
          int bps() const ;
          int nmaps() const ;

        private:
          std::vector<std::vector<gr_complex> > d_wmaps;
          std::vector<std::vector<gr_complex> > d_mappings;
          gr_complex d_scalar;
          modtype_t d_modtype;
          uint16_t d_bps;
          uint16_t d_mapidx_max;
          const std::vector<int> d_symbol_values;
          std::vector<int> d_symbol_values_rev;

          // we only base class this so we can hide the templating later...
          class demap_base {
            public:
                demap_base() {}
                virtual void _demap_hard(const gr_complex* in, uint8_t *out, int nsymbols) = 0;
                virtual void _demap_soft(const gr_complex* in, float *out, int nsymbols) = 0;
          };

          template <modtype_t T> class demap_impl : public demap_base {
            private:
                const constellation *c;
            public:
                demap_impl(const constellation *_c);
                void _demap_hard(const gr_complex* in, uint8_t *out, int nsymbols);
                void _demap_soft(const gr_complex* in, float *out, int nsymbols);
            };

          demap_base* d_demapper;

        };
          
  }
}


#endif
