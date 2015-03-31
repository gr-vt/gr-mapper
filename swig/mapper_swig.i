/* -*- c++ -*- */

#define MAPPER_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "mapper_swig_doc.i"

%{
#include "mapper/mapper.h"
#include "mapper/demapper.h"
#include "mapper/demapper_soft.h"
#include "mapper/mapper_msg.h"
#include "mapper/demapper_msg.h"
#include "mapper/preamble_insert_bb.h"
#include "mapper/preamble_sync_cc.h"
#include "mapper/preamble_generator.h"
#include "mapper/preamble_sync_demapper.h"
#include "mapper/preamble_sync_demapper_hard.h"
#include "mapper/preamble_strip.h"
#include "mapper/timeslot_demux.h"
%}


%include "mapper/preamble_generator.h"
%include "mapper/constellation.h"
%include "mapper/mapper.h"
GR_SWIG_BLOCK_MAGIC2(mapper, mapper);
%include "mapper/demapper.h"
GR_SWIG_BLOCK_MAGIC2(mapper, demapper);
%include "mapper/demapper_soft.h"
GR_SWIG_BLOCK_MAGIC2(mapper, demapper_soft);
%include "mapper/mapper_msg.h"
GR_SWIG_BLOCK_MAGIC2(mapper, mapper_msg);
%include "mapper/demapper_msg.h"
GR_SWIG_BLOCK_MAGIC2(mapper, demapper_msg);
%include "mapper/preamble_insert_bb.h"
GR_SWIG_BLOCK_MAGIC2(mapper, preamble_insert_bb);
%include "mapper/preamble_sync_cc.h"
GR_SWIG_BLOCK_MAGIC2(mapper, preamble_sync_cc);
%include "mapper/preamble_sync_demapper.h"
GR_SWIG_BLOCK_MAGIC2(mapper, preamble_sync_demapper);
%include "mapper/preamble_sync_demapper_hard.h"
GR_SWIG_BLOCK_MAGIC2(mapper, preamble_sync_demapper_hard);
%include "mapper/preamble_strip.h"
GR_SWIG_BLOCK_MAGIC2(mapper, preamble_strip);
%include "mapper/timeslot_demux.h"
GR_SWIG_BLOCK_MAGIC2(mapper, timeslot_demux);
