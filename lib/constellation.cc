#include <mapper/constellation.h>
#include <stdexcept>
#include <cmath>
#include <boost/format.hpp>
#include <stdio.h>

const gr_complex sym_bpsk[] = {gr_complex(1,0), gr_complex(-1,0)};
const gr_complex sym_bpsk_off[] = {gr_complex(0,1), gr_complex(0,-1)};
const gr_complex sym_qpsk[] = {gr_complex(1,0), gr_complex(0,1), gr_complex(-1,0), gr_complex(0,-1)};
const gr_complex sym_8psk[] = {gr_complex(1,0), gr_complex(M_SQRT1_2, M_SQRT1_2), gr_complex(0,1), gr_complex(-M_SQRT1_2, M_SQRT1_2), gr_complex(-1,0), gr_complex(-M_SQRT1_2, -M_SQRT1_2), gr_complex(0,-1), gr_complex(M_SQRT1_2, -M_SQRT1_2)};
const gr_complex sym_pam4[] = {gr_complex(-3,0), gr_complex(-1,0), gr_complex(3,0), gr_complex(1,0)};
const gr_complex sym_qam16[]= {gr_complex(-3,3), gr_complex(-1,3), gr_complex(1,3), gr_complex(3,3),
        gr_complex(-3,1), gr_complex(-1,1), gr_complex(1,1), gr_complex(3,1),
        gr_complex(-3,-1), gr_complex(-1,-1), gr_complex(1,-1), gr_complex(3,-1),
        gr_complex(-3,-3), gr_complex(-1,-3), gr_complex(1,-3), gr_complex(3,-3)};
const gr_complex sym_qam64[]= {gr_complex(-7,7), gr_complex(-5,7), gr_complex(-3,7), gr_complex(-1,7), gr_complex(1,7), gr_complex(3,7), gr_complex(5,7), gr_complex(7,7),
        gr_complex(-7,5), gr_complex(-5,5), gr_complex(-3,5), gr_complex(-1,5), gr_complex(1,5), gr_complex(3,5), gr_complex(5,5), gr_complex(7,5),
        gr_complex(-7,3), gr_complex(-5,3), gr_complex(-3,3), gr_complex(-1,3), gr_complex(1,3), gr_complex(3,3), gr_complex(5,3), gr_complex(7,3),
        gr_complex(-7,1), gr_complex(-5,1), gr_complex(-3,1), gr_complex(-1,1), gr_complex(1,1), gr_complex(3,1), gr_complex(5,1), gr_complex(7,1),
        gr_complex(-7,-1), gr_complex(-5,-1), gr_complex(-3,-1), gr_complex(-1,-1), gr_complex(1,-1), gr_complex(3,-1), gr_complex(5,-1), gr_complex(7,-1),
        gr_complex(-7,-3), gr_complex(-5,-3), gr_complex(-3,-3), gr_complex(-1,-3), gr_complex(1,-3), gr_complex(3,-3), gr_complex(5,-3), gr_complex(7,-3),
        gr_complex(-7,-5), gr_complex(-5,-5), gr_complex(-3,-5), gr_complex(-1,-5), gr_complex(1,-5), gr_complex(3,-5), gr_complex(5,-5), gr_complex(7,-5),
        gr_complex(-7,-7), gr_complex(-5,-7), gr_complex(-3,-7), gr_complex(-1,-7), gr_complex(1,-7), gr_complex(3,-7), gr_complex(5,-7), gr_complex(7,-7)};

#define SQUARED_DISTANCE(x,y)  (  ((x.real()-y.real())*(x.real()-y.real())) + ((x.imag()-y.imag())*(x.imag()-y.imag())) )

namespace gr {
        namespace mapper {

                constellation::constellation(modtype_t modtype, std::vector<int> symbol_values, gr_complex scalar) :
                        d_modtype(modtype),
                        d_scalar(scalar),
                        d_symbol_values(symbol_values)
                {
                        switch(d_modtype){
                                case BPSK:
                                        d_wmaps.push_back( std::vector<gr_complex>( &sym_bpsk[0], &sym_bpsk[2] ) );
                                        d_demapper = new demap_impl<BPSK>(this);
                                        break;
                                case P2BPSK:
                                        d_wmaps.push_back( std::vector<gr_complex>( &sym_bpsk[0], &sym_bpsk[2] ) );
                                        d_wmaps.push_back( std::vector<gr_complex>( &sym_bpsk_off[0], &sym_bpsk_off[2] ) );
                                        d_demapper = new demap_impl<P2BPSK>(this);
                                        break;
                                case QPSK:
                                        d_wmaps.push_back( std::vector<gr_complex>( &sym_qpsk[0], &sym_qpsk[4] ) );
                                        // rotate to pi/4 qpsk
                                        for(int i=0; i <4; i++){ d_wmaps[0][i] *= gr_complex(M_SQRT1_2, M_SQRT1_2); }
                                        d_demapper = new demap_impl<QPSK>(this);
                                        break;
                                case PSK8:
                                        d_wmaps.push_back( std::vector<gr_complex>( &sym_8psk[0], &sym_8psk[8] ) );
                                        d_demapper = new demap_impl<PSK8>(this);
                                        break;
                                case PAM4:
                                        d_wmaps.push_back( std::vector<gr_complex>( &sym_pam4[0], &sym_pam4[4] ) );
                                        d_demapper = new demap_impl<PAM4>(this);
                                        break;
                                case QAM16:
                                        d_wmaps.push_back( std::vector<gr_complex>( &sym_qam16[0], &sym_qam16[16] ) );
                                        d_demapper = new demap_impl<QAM16>(this);
                                        break;
                                case QAM64:
                                        d_wmaps.push_back( std::vector<gr_complex>( &sym_qam64[0], &sym_qam64[64] ) );
                                        d_demapper = new demap_impl<QAM64>(this);
                                        break;
                                default:
                                        throw std::runtime_error("bad mod type durring init");
                        }

                        d_bps = log2(d_wmaps[0].size());
                        d_mapidx_max = d_wmaps.size();


                        // normalize constellation power to 1
                        for(int j=0; j< d_wmaps.size(); j++){
                            float avgp = 0;
                            for(int i=0; i<d_wmaps[j].size(); i++){ avgp += std::sqrt((d_wmaps[j][i] * std::conj(d_wmaps[j][i])).real()); }
                            float scale = d_wmaps[j].size() / avgp;
                            for(int i=0; i<d_wmaps[j].size(); i++){ d_wmaps[j][i] = d_wmaps[j][i] * scale; }
                        }
        
        // * do a bit of input validation

        // make sure greymap size matches
        if(symbol_values.size() != d_wmaps[0].size()){
            throw std::runtime_error("incorrect number of symbols for given constellation");
            }

        d_symbol_values_rev.resize(symbol_values.size());
        // make sure greymap values are in valid range (we don't check that all symbols are represented for you though)
        for(int i=0; i<symbol_values.size(); i++){
            if((symbol_values[i] < 0) || (symbol_values[i] >= d_wmaps[0].size())){
                throw std::runtime_error((boost::format("bad symbol value recieved (%d) values must be in range [0,%d] for given constellation")%(symbol_values[i])%(d_wmaps[0].size()-1)).str());
            }

            // set up reverse mapping
            d_symbol_values_rev[ symbol_values[i] ] = i;
        }

        // 

        // perform white map to grey map translation ...
        for(int i=0; i<d_wmaps.size(); i++){
            std::vector<gr_complex> tm(d_wmaps[i].size());
            //printf("map indx = %d\n", i);
            for(int j=0; j<d_wmaps[i].size(); j++){
                int idx = symbol_values[j];
                gr_complex sym = d_wmaps[i][idx];
                tm[j] = sym;
                //printf(" %d --> (%f,%f) \n", j, sym.real(), sym.imag() );
            }
            d_mappings.push_back(tm);
        }

        // set up mappers

    }

    int constellation::bps() const { return d_bps; }
    int constellation::nmaps() const { return d_wmaps.size(); }

    void constellation::map(const float *in, gr_complex* out, int nsymbols, uint16_t mapidx)
    {
        int nbits = nsymbols*bps();
        std::vector<uint8_t> bt(nbits);
        for(int i=0; i<nbits; i++){ bt[i] = in[i]; }
        map(&bt[0], out, nsymbols, mapidx);
    }


    void constellation::map(const uint8_t *in, gr_complex* out, int nsymbols, uint16_t mapidx)
    {   
        for(int i=0; i<nsymbols; i++){
            uint16_t idx(0);
            for(int j=0; j<d_bps; j++){
                // sanity check - hopefully this does not slow things down too much
                if(__builtin_expect( in[i*d_bps + j] > 0x01, false)){
                    throw std::runtime_error((boost::format("ERROR: constellation mapper expects values of 0x00 and 0x01 in *ONLY* - received 0x%02x at bit offset %d!")% ((int)(in[i*d_bps+j]))%(d_bps*i)).str());
                }
                idx |= (in[i*d_bps + j] << (d_bps - j-1));
            }
            out[i] = d_mappings[mapidx][idx];
//            out[i] = d_mappings[mapidx][default_greymap_psk8[idx]];
            mapidx = (mapidx + 1)%d_mapidx_max;
        }
    }
    
    void constellation::demap_hard(const gr_complex* in, uint8_t *out, int nsymbols)
    { 
        d_demapper->_demap_hard(in,out,nsymbols);
    }
    
    void constellation::demap_soft(const gr_complex* in, float *out, int nsymbols)
    { 
        d_demapper->_demap_soft(in,out,nsymbols);
    }
    template <modtype_t T> constellation::demap_impl<T>::demap_impl(const constellation* _c) : c(_c) {}

    // provide the general case demapper
    template <modtype_t T> void constellation::demap_impl<T>::_demap_hard(const gr_complex* in, uint8_t *out, int nsymbols)
    {
        // iterate over symbols
        for(int i=0; i<nsymbols; i++){

            // choose the nearest point
            float mindist = SQUARED_DISTANCE(in[i], c->d_mappings[0][0]);
            int min_sidx = 0;
            for(int j=1; j<c->d_mappings[0].size(); j++){
                float new_dist = SQUARED_DISTANCE(in[i], c->d_mappings[0][j]);
                min_sidx = (new_dist < mindist) ? j : min_sidx;
                mindist = std::min(mindist, new_dist);
                }

            int sval = c->d_symbol_values_rev[min_sidx];

            // map bits to output
            for(int j=0; j<c->d_bps; j++){
                out[i*c->d_bps + j] = (min_sidx >> (c->d_bps-j-1)) & 0x01;
                }

            }
    }

    // implement specialized fast qpsk demapper
    template <> void constellation::demap_impl<QPSK>::_demap_hard(const gr_complex* in, uint8_t *out, int nsymbols)
    {
        for(int i=0; i<nsymbols; i++){
            //uint8_t idx = c->d_symbol_values[
            uint8_t idx = c->d_symbol_values_rev[
                            ((in[i].real() > 0.0) ?
                                ((in[i].imag() > 0.0) ? 0 : 3)://((in[i].imag() > 0.0) ? 0 : 2)
                                ((in[i].imag() > 0.0) ? 1 : 2))//((in[i].imag() > 0.0) ? 1 : 3))
                                    ];
            out[2*i+1] = idx&0x01;
            out[2*i] = (idx>>1)&0x01;
        }
    }
   
    // implement specialized fast bpsk demapper
    template <> void constellation::demap_impl<BPSK>::_demap_hard(const gr_complex* in, uint8_t *out, int nsymbols)
    {
        for(int i=0; i<nsymbols; i++){
            out[i] = in[i].real() > 0 ? c->d_symbol_values_rev[0] : c->d_symbol_values_rev[1];
        }
    }

    
/*
    // provide the general case demapper (Direct implementation)
    template<modtype_t T> void constellation::demap_impl<T>::_demap_soft(const gr_complex* in, float *out, int nsymbols)
    {
        float sigma = 0.3; // kludged
        float sigsq = pow(0.3,2);
        for(int i=0; i<nsymbols; i++){
            std::vector<float> distances(c->d_mappings[0].size());

            // compute distances
            for(int k=0; k<distances.size(); k++){
                distances[k] = SQUARED_DISTANCE( in[i], c->d_mappings[0][k] );
                }

            // compute the LLR for each bit
            for(int j=0; j<c->d_bps; j++){
                float LLR = 0;
                float p0=0;
                float p1=0;

                // compute the contribution of each point to the LLR
                for(int k=0; k<distances.size(); k++){
                    int bval = (c->d_symbol_values[k] >> j) & 0x01;
                    float prob = std::exp(distances[k]/sigsq);
                    if(bval==1){
                        p1 += prob;
                    } else {
                        p0 += prob;
                    }
                }
                LLR = log(p1/p0);
                //printf("LLR = %f\n",LLR);
                // set output
                out[i*c->d_bps + j] = LLR;
            }
        }
    }
*/
    // provide the general case demapper (efficient approximation)
    template<modtype_t T> void constellation::demap_impl<T>::_demap_soft(const gr_complex* in, float *out, int nsymbols)
    {
        float sigma = 0.3; // kludged
        float sigsq = pow(0.3,2);
        for(int i=0; i<nsymbols; i++){
            std::vector<float> distances(c->d_mappings[0].size());
            
            // compute distances
            //printf("in = (%f,%f)\n", in[i].real(), in[i].imag());
            for(int k=0; k<distances.size(); k++){
                distances[k] = SQUARED_DISTANCE( in[i], c->d_mappings[0][k] );
                //printf("dist[%d] (%f,%f) = %f\n", k, c->d_mappings[0][k].real(), c->d_mappings[0][k].imag(), distances[k]);
                }

            // compute the LLR for each bit
            for(int j=0; j<c->d_bps; j++){
                //printf("LLR bit %d\n", j);

                float LLR = 0;

                // find min distance for each bit case
                int minidx[2] = {-1, -1};
                for(int k=0; k<distances.size(); k++){

                    int sval = k;
                    //`int sval = c->d_symbol_values_rev[k];
                    int bval = ( sval >> (c->d_bps-j-1)) & 0x01;
                    //printf("k=%d, sval=%d, bval=%d\n", k, sval, bval);

                    //printf("(bit=%d) k=%d, bval=%d\n", j, k, bval);
                    if(minidx[bval] == -1){
                        //printf("initial set...\n");
                        minidx[bval] = k;
                    } else {
                        //int a = minidx[bval];
                        minidx[bval] = (distances[k] < distances[minidx[bval]]) ? k : minidx[bval];
                        //printf("compare %f[%d] < %f[%d] --> %d...\n", distances[k], k, distances[a], a, minidx[bval]);
                    }
                }
                
                float p1=distances[minidx[1]];
                float p0=distances[minidx[0]];
                //printf("p0 = %f, p1 = %f\n", p0,p1);
               
                // approximate llr from  ln(e^(max(p1))/e^(max(p0)))
                LLR = (p1 - p0);
                if(std::isnan(LLR) || (LLR > 100))
                    LLR = 100.0f;
                if(LLR < -100)
                    LLR = -100;

                //printf("LLR[%d] = %f, p0 = %f, p1 = %f\n", i*c->d_bps + j, LLR, p0, p1);

                // set output
                //out[i*c->d_bps + j] = LLR;
                out[i*c->d_bps + j] = -LLR;
            }
        //throw std::runtime_error("done");
        }
    }
//    // implement specialized efficient demappers if desired
//    template<modtype_t T> void constellation::demap_impl<QPSK>::_demap_soft(const gr_complex* in, float *out, int nsymbols)
//    {
//        printf("qpsk soft demap.\n");       
//    }

  }
}
