THIRD_PARTY=/data/env

GFLAGS_PATH=${THIRD_PARTY}/gflags/
GLOG_PATH=${THIRD_PARTY}/glog/

GFLAGS_INCLUDE=${GFLAGS_PATH}/include/
GLOG_INCLUDE=${GLOG_PATH}/include/

GFLAGS_LIB=${GFLAGS_PATH}/lib/
GLOG_LIB=${GLOG_PATH}/lib/

INCLUDE=-I ${GLOG_INCLUDE} -I ${GFLAGS_INCLUDE} -L${GLOG_LIB} -L${GFLAGS_LIB} -lgflags -lglog -pthread -DGFLAGS_NAMESPACE=google -DBUILD_SHARED_LIBS=ON

AucCalMain: cal_auc.cc mpi_base.cc main.cc
	mpic++ -g -o AucCalMain main.cc cal_auc.cc mpi_base.cc ${INCLUDE} -std=c++11

clean:
	rm -rf *.o AucCalMain
