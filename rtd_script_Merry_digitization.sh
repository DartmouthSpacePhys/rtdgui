#! /bin/bash

/daq/rtdgui/rtdgui /daq/rtdgui/hf2_config.input&
/daq/prtd/prtd_1ch -c /daq/prtd/hf2_config.input
