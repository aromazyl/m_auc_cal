THIRD_PARTY=/data/env

GFLAGS_PATH=${THIRD_PARTY}/gflags/
BOOST_PATH=${THIRD_PARTY}/boost/
GLOG_PATH=${THIRD_PARTY}/glog/
HASH_PATH=${THIRD_PARTY}/murmurhash/

GFLAGS_INCLUDE=${GFLAGS_PATH}/include/
BOOST_INCLUDE=${BOOST_PATH}/include/
GLOG_INCLUDE=${GLOG_PATH}/include/
HASH_INCLUDE=${HASH_PATH}/include/

GFLAGS_LIB=${GFLAGS_PATH}/lib/
BOOST_LIB=${BOOST_PATH}/lib/
GLOG_LIB=${GLOG_PATH}/lib/
HASH_LIB=${HASH_PATH}/lib/

INCLUDE=-I ${BOOST_INCLUDE} -I ${GLOG_INCLUDE} -I ${GFLAGS_INCLUDE} -I ${HASH_INCLUDE} -L${BOOST_LIB} -L${GLOG_LIB} -L${GFLAGS_LIB} -L ${HASH_LIB} -lgflags -lglog -pthread -lboost_system -lboost_thread -lrt -DGFLAGS_NAMESPACE=google -DBUILD_SHARED_LIBS=ON  -lSMHasherSupport 

AucCalMain: cal_auc.cc mpi_base.cc main.cc
	mpic++ -g -o AucCalMain main.cc cal_auc.cc mpi_base.cc ${INCLUDE}

clean:
	rm -rf *.o AucCalMain
