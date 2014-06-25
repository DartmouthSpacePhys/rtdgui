#!/bin/bash

RATE=20000000
NCHAN=1
RTD_DT=0.5

RANGE=1  # 1 = +-1 Volt, 0 = +-5 Volts

/daq/acq_c/acq_c -n ${NCHAN} -F ${RATE} -d ${1:-$RTD_DT} -m /tmp/rtd/latest_acquisition.data -r $RANGE
