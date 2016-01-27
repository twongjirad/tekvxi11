#test commit
export HODODAQ_HOME=/home/twongjirad/working/HodoDAQ
export TEKVXI11_DIR=/home/twongjirad/working/tekvxi11/

export HODODAQ_INCDIR=${HODODAQ_HOME}/include
export HODODAQ_LIBDIR=${HODODAQ_HOME}/lib
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${TEKVXI11_DIR}:${TEKVXI11_DIR}/vxi11_1.10
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${TEKVXI11_DIR}/hododaq:${HODODAQ_LIBDIR}:${HODODAQ_HOME}
