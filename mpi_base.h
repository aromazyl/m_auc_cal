#pragma once
#pragma pack(8)
#include <boost/thread/mutex.hpp>
#include <vector>
#include "common.h"
#include "mpi.h"
#include "conf.h"
#define int64 long long

namespace news_dl { namespace mpi {

enum MsgTag {
    worker_send_pctr_to_master = 1,
};

class MpiBase {
    public:
        MpiBase(int argc, char* argv[]);
        ~MpiBase() { Destroy(); }

    public:
        void Init(int argc, char* argv[]);
        bool Destroy();

        int GetMyRank() { return myRank_; }
        int GetTotalRank() { return totalRank_; }

    public:
        int WorkerMasterCommunication(
                void* ctrBufferForSend,
                int64 bufferSize,
                void** ctrBufferForRecved,
                int64* bufferSizeRecved);

    private:
        int myRank_;
        int totalRank_;

        // MPI_Request mpiWorkerSendReqs_;
        // MPI_Request mpiMasterRecvReqs_;

        boost::mutex mutSendRecv_;
};

}}
