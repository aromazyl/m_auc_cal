#include <cstdio>
#include <glog/logging.h>
#include <assert.h>
#include "stringprintf.hpp"
#include "mpi_base.h"

namespace news_dl { namespace mpi {
    MpiBase::MpiBase(int argc, char* argv[]) {
        this->Init(argc, argv);
        LR::AucCalculationConf* conf = LR::AucCalculationConf::GetSingletonPtr();
    }

    void MpiBase::Init(int argc, char* argv[]) {
        LR::AucCalculationConf* conf = LR::AucCalculationConf::GetSingletonPtr();
        int ret = MPI_Init(&argc, &argv);
        if (0 != ret) {
            fprintf(stderr, "MPI_Init failed\n");
            exit(1);
        }
        ret = MPI_Comm_rank(MPI_COMM_WORLD, &myRank_);
        if (0 != ret) {
            fprintf(stderr, "MPI_Comm_size failed\n");
            exit(1);
        }
        printf("My mpi rank is %d\n", myRank_);

        ret = MPI_Comm_size(MPI_COMM_WORLD, &totalRank_);
        if (0 != ret) {
            fprintf(stderr, "MPI_Comm_size failed\n");
            exit(1);
        }
        conf->SetMyRank(myRank_)->SetTotalRank(totalRank_);
        printf("init successfull \n");
    }

    bool MpiBase::Destroy() {
        MPI_Finalize();
        return true;
    }

    int MpiBase::WorkerMasterCommunication(void* ctrBufferForSend,
                                           int buffersize,
                                           void** ctrBufferForRecved,
                                           int* bufferSizeRecved) {
        LR::AucCalculationConf* conf = LR::AucCalculationConf::GetSingletonPtr();
        CHECK(ctrBufferForSend) << "ctr buffer for send is empty";
        assert(ctrBufferForRecved);
        assert(*ctrBufferForRecved);
        std::cout << base_string::StringPrintf(
            "MPI_Gather info,"
            "ctrBufferForSend:%p"
            "buffersize:%lld",
            ctrBufferForSend,
            buffersize);

        int ret = MPI_Gather(ctrBufferForSend,
                             buffersize,
                             MPI_CHAR,
                             *ctrBufferForRecved,
                             buffersize * totalRank_,
                             MPI_CHAR,
                             conf->GetMasterIdx(),
                             MPI_COMM_WORLD);
        if (0 != ret) {
          LOG(ERROR) << base_string::StringPrintf(
              "MPI_Gather failed,"
              "ctrBufferForSend:%p"
              "buffersize:%lld",
              ctrBufferForSend,
              buffersize);
          exit(1);
        }
        // *ctrBufferForRecved = ctrRecvBuffer_;
        *bufferSizeRecved = totalRank_ * buffersize;
        return 0;
    }
}}
