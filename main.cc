#include <boost/shared_ptr.hpp>
#include <glog/logging.h>
#include <gflags/gflags.h>
#include "conf.h"
#include "cal_auc.h"
#include "mpi_base.h"

DEFINE_string(model_path, "", "train model directory");
DEFINE_string(data_path, "", "test data directory");
DEFINE_int32(bin_number, 1000, "split bin numbers");
DEFINE_int32(master_index, 0, "master index");
// DEFINE_int64(block_size, 1000000, "single block size, byte");

using namespace news_dl;
using namespace LR;
using namespace mpi;

int main(int argc, char* argv[]) {
    google::ParseCommandLineFlags(&argc, &argv, true);
    google::InitGoogleLogging(argv[0]);
    AucCalculationConf* conf = AucCalculationConf::GetSingletonPtr()
                ->SetBinNum(FLAGS_bin_number)
                ->SetMasterIdx(FLAGS_master_index)
                ->SetScoreFilePath(FLAGS_data_path)
                ->SetModelFilePath(FLAGS_model_path);
                // ->SetReadingBlockSize(FLAGS_block_size);

    boost::shared_ptr<MpiBase> mpibase;
    LOG(INFO) << "mpi init";
    mpibase.reset(new MpiBase(argc, argv));
    LOG(INFO) << "auc calculation init";
    boost::shared_ptr<AucCalculation> aucCalculation;
    aucCalculation.reset(new AucCalculation(mpibase));
    LOG(INFO) << "run";
    aucCalculation->Run(conf->GetScoreFilePath());

    return 0;
}
