#!/bin/bash

RATE=20000000
NCHAN=1
RTD_DT=0.5

PREFIX="merry-"
SUFFIX="-${NCHAN}ch-$((RATE/1000000))MHz"

RANGE=1 # 1 = +-1 Volt, 0 = +-5 Volts

OUTDIR="/daq"
FNAME=`date +${PREFIX}%Y%m%d-%H%M%S${SUFFIX}.data`

if comedi_config --read-buffer 65536 /dev/comedi0 && comedi_test -s0 /dev/comedi0 >& /dev/null ; then 
    echo "Writing data to $FNAME."
    /daq/acq_c/acq_c -n ${NCHAN} -F ${RATE} -o ${OUTDIR}/${FNAME} -d ${1:-$RTD_DT} -m /tmp/rtd/latest_acquisition.data -r $RANGE
fi


