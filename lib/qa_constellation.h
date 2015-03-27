/* -*- c++ -*- */
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

#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>
#include <mapper/constellation.h>

namespace gr {
  namespace mapper {

    class qa_constellation : public CppUnit::TestCase
    {
    public:
      CPPUNIT_TEST_SUITE(qa_constellation);
      CPPUNIT_TEST(t0);
      CPPUNIT_TEST(t1);
      CPPUNIT_TEST(t2);
      CPPUNIT_TEST_SUITE_END();

    private:
      void t0();
      void t1();
      void t2();

      void map_demap(constellation &c, std::vector<uint8_t> &data_bits, std::vector<uint8_t> &hard_out, std::vector<float> &soft_out, std::vector<uint8_t> &soft_sign_out);

    };

  } /* namespace mapper */
} /* namespace gr */

