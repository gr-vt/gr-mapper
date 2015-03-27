#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: Sync Test
# Generated: Mon Apr  7 19:18:04 2014
##################################################

from PyQt4 import Qt
from cmath import exp, pi; from random import randint;
from gnuradio import blocks
from gnuradio import channels
from gnuradio import digital
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio import qtgui
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser
import PyQt4.Qwt5 as Qwt
import mapper
import numpy
import sip
import sys

class sync_test(gr.top_block, Qt.QWidget):

    def __init__(self):
        gr.top_block.__init__(self, "Sync Test")
        Qt.QWidget.__init__(self)
        self.setWindowTitle("Sync Test")
        try:
             self.setWindowIcon(Qt.QIcon.fromTheme('gnuradio-grc'))
        except:
             pass
        self.top_scroll_layout = Qt.QVBoxLayout()
        self.setLayout(self.top_scroll_layout)
        self.top_scroll = Qt.QScrollArea()
        self.top_scroll.setFrameStyle(Qt.QFrame.NoFrame)
        self.top_scroll_layout.addWidget(self.top_scroll)
        self.top_scroll.setWidgetResizable(True)
        self.top_widget = Qt.QWidget()
        self.top_scroll.setWidget(self.top_widget)
        self.top_layout = Qt.QVBoxLayout(self.top_widget)
        self.top_grid_layout = Qt.QGridLayout()
        self.top_layout.addLayout(self.top_grid_layout)

        self.settings = Qt.QSettings("GNU Radio", "sync_test")
        self.restoreGeometry(self.settings.value("geometry").toByteArray())


        ##################################################
        # Variables
        ##################################################
        self.pream_len = pream_len = 84
        self.samp_rate = samp_rate = 32000
        self.pream = pream = (mapper.preamble_generator(pream_len,511,1033)).get_preamble()
        self.SNR = SNR = 40
        self.Rotation = Rotation = 0
        self.Offset = Offset = 0

        ##################################################
        # Blocks
        ##################################################
        self._SNR_tool_bar = Qt.QToolBar(self)
        self._SNR_tool_bar.addWidget(Qt.QLabel("SNR"+": "))
        self._SNR_line_edit = Qt.QLineEdit(str(self.SNR))
        self._SNR_tool_bar.addWidget(self._SNR_line_edit)
        self._SNR_line_edit.returnPressed.connect(
        	lambda: self.set_SNR(eng_notation.str_to_num(self._SNR_line_edit.text().toAscii())))
        self.top_layout.addWidget(self._SNR_tool_bar)
        self._Rotation_layout = Qt.QVBoxLayout()
        self._Rotation_label = Qt.QLabel("Rotation")
        self._Rotation_slider = Qwt.QwtSlider(None, Qt.Qt.Horizontal, Qwt.QwtSlider.BottomScale, Qwt.QwtSlider.BgSlot)
        self._Rotation_slider.setRange(0, 2*pi, pi/100)
        self._Rotation_slider.setValue(self.Rotation)
        self._Rotation_slider.setMinimumWidth(200)
        self._Rotation_slider.valueChanged.connect(self.set_Rotation)
        self._Rotation_label.setAlignment(Qt.Qt.AlignBottom | Qt.Qt.AlignHCenter)
        self._Rotation_layout.addWidget(self._Rotation_label)
        self._Rotation_layout.addWidget(self._Rotation_slider)
        self.top_layout.addLayout(self._Rotation_layout)
        self._Offset_layout = Qt.QVBoxLayout()
        self._Offset_tool_bar = Qt.QToolBar(self)
        self._Offset_layout.addWidget(self._Offset_tool_bar)
        self._Offset_tool_bar.addWidget(Qt.QLabel("Offset"+": "))
        self._Offset_counter = Qwt.QwtCounter()
        self._Offset_counter.setRange(-100, 100, 1)
        self._Offset_counter.setNumButtons(2)
        self._Offset_counter.setValue(self.Offset)
        self._Offset_tool_bar.addWidget(self._Offset_counter)
        self._Offset_counter.valueChanged.connect(self.set_Offset)
        self._Offset_slider = Qwt.QwtSlider(None, Qt.Qt.Horizontal, Qwt.QwtSlider.BottomScale, Qwt.QwtSlider.BgSlot)
        self._Offset_slider.setRange(-100, 100, 1)
        self._Offset_slider.setValue(self.Offset)
        self._Offset_slider.setMinimumWidth(200)
        self._Offset_slider.valueChanged.connect(self.set_Offset)
        self._Offset_layout.addWidget(self._Offset_slider)
        self.top_layout.addLayout(self._Offset_layout)
        self.qtgui_const_sink_x_0_1 = qtgui.const_sink_c(
        	840-84, #size
        	"QT GUI Plot", #name
        	1 #number of inputs
        )
        self.qtgui_const_sink_x_0_1.set_update_time(0.10)
        self.qtgui_const_sink_x_0_1.set_y_axis(-2, 2)
        self.qtgui_const_sink_x_0_1.set_x_axis(-2, 2)
        self._qtgui_const_sink_x_0_1_win = sip.wrapinstance(self.qtgui_const_sink_x_0_1.pyqwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._qtgui_const_sink_x_0_1_win)
        self.qtgui_const_sink_x_0 = qtgui.const_sink_c(
        	840-84, #size
        	"QT GUI Plot", #name
        	1 #number of inputs
        )
        self.qtgui_const_sink_x_0.set_update_time(0.10)
        self.qtgui_const_sink_x_0.set_y_axis(-2, 2)
        self.qtgui_const_sink_x_0.set_x_axis(-2, 2)
        self._qtgui_const_sink_x_0_win = sip.wrapinstance(self.qtgui_const_sink_x_0.pyqwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._qtgui_const_sink_x_0_win)
        self.mapper_preamble_sync_cc_0 = mapper.preamble_sync_cc(pream_len*10, (pream), mapper.PSK8, ([0,1,2,3,4,5,6,7]), .97, .90)
        self.mapper_preamble_insert_bb_0 = mapper.preamble_insert_bb(pream_len*10, (pream))
        self.mapper_mapper_0 = mapper.mapper(mapper.PSK8, ([0,1,2,3,4,5,6,7]))
        self.mapper_demapper_0 = mapper.demapper(mapper.PSK8, ([0,1,2,3,4,5,6,7]))
        self.digital_costas_loop_cc_0 = digital.costas_loop_cc(1.5*pi/100, 8)
        self.channels_channel_model_0 = channels.channel_model(
        	noise_voltage=10**(-SNR/20.0),
        	frequency_offset=Offset,
        	epsilon=1.0,
        	taps=(exp( (0 + 1j*Rotation) ), ),
        	noise_seed=0,
        	block_tags=False
        )
        self.blocks_vector_sink_x_0_0 = blocks.vector_sink_b(1)
        self.blocks_unpack_k_bits_bb_0 = blocks.unpack_k_bits_bb(3)
        self.blocks_throttle_0 = blocks.throttle(gr.sizeof_char*1, samp_rate)
        self.analog_random_source_x_0 = blocks.vector_source_b(map(int, numpy.random.randint(0, 7, 10000)), True)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.mapper_preamble_insert_bb_0, 0), (self.mapper_mapper_0, 0))
        self.connect((self.mapper_preamble_sync_cc_0, 0), (self.mapper_demapper_0, 0))
        self.connect((self.mapper_mapper_0, 0), (self.channels_channel_model_0, 0))
        self.connect((self.channels_channel_model_0, 0), (self.digital_costas_loop_cc_0, 0))
        self.connect((self.digital_costas_loop_cc_0, 0), (self.mapper_preamble_sync_cc_0, 0))
        self.connect((self.mapper_preamble_sync_cc_0, 0), (self.qtgui_const_sink_x_0_1, 0))
        self.connect((self.analog_random_source_x_0, 0), (self.blocks_unpack_k_bits_bb_0, 0))
        self.connect((self.blocks_unpack_k_bits_bb_0, 0), (self.blocks_throttle_0, 0))
        self.connect((self.blocks_throttle_0, 0), (self.mapper_preamble_insert_bb_0, 0))
        self.connect((self.mapper_demapper_0, 0), (self.blocks_vector_sink_x_0_0, 0))
        self.connect((self.channels_channel_model_0, 0), (self.qtgui_const_sink_x_0, 0))


# QT sink close method reimplementation
    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "sync_test")
        self.settings.setValue("geometry", self.saveGeometry())
        event.accept()

    def get_pream_len(self):
        return self.pream_len

    def set_pream_len(self, pream_len):
        self.pream_len = pream_len
        self.set_pream((mapper.preamble_generator(self.pream_len,511,1033)).get_preamble())
        self.mapper_preamble_insert_bb_0.mapper.set_width(self.pream_len*10)

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.blocks_throttle_0.set_sample_rate(self.samp_rate)

    def get_pream(self):
        return self.pream

    def set_pream(self, pream):
        self.pream = pream
        self.mapper_preamble_insert_bb_0.mapper.set_preamble((self.pream))

    def get_SNR(self):
        return self.SNR

    def set_SNR(self, SNR):
        self.SNR = SNR
        self._SNR_line_edit.setText(eng_notation.num_to_str(self.SNR))
        self.channels_channel_model_0.set_noise_voltage(10**(-self.SNR/20.0))

    def get_Rotation(self):
        return self.Rotation

    def set_Rotation(self, Rotation):
        self.Rotation = Rotation
        self._Rotation_slider.setValue(self.Rotation)
        self.channels_channel_model_0.set_taps((exp( (0 + 1j*self.Rotation) ), ))

    def get_Offset(self):
        return self.Offset

    def set_Offset(self, Offset):
        self.Offset = Offset
        self.channels_channel_model_0.set_frequency_offset(self.Offset)
        self._Offset_counter.setValue(self.Offset)
        self._Offset_slider.setValue(self.Offset)

if __name__ == '__main__':
    import ctypes
    import os
    if os.name == 'posix':
        try:
            x11 = ctypes.cdll.LoadLibrary('libX11.so')
            x11.XInitThreads()
        except:
            print "Warning: failed to XInitThreads()"
    parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
    (options, args) = parser.parse_args()
    qapp = Qt.QApplication(sys.argv)
    tb = sync_test()
    tb.start()
    tb.show()
    def quitting():
        tb.stop()
        tb.wait()
    qapp.connect(qapp, Qt.SIGNAL("aboutToQuit()"), quitting)
    qapp.exec_()
    tb = None #to clean up Qt widgets

