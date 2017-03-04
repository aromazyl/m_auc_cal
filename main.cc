#include <memory>
#include <glog/logging.h>
#include <gflags/gflags.h>
#include "conf.h"
#include "cal_auc.h"
#include "mpi_base.h"

DEFINE_string(data_path, "", "test data directory");
DEFINE_int32(bin_number, 1000, "split bin numbers");
DEFINE_int32(master_index, 0, "master index");

using namespace news_dl;
using namespace LR;
using namespace mpi;

int main(int argc, char* argv[]) {
    google::ParseCommandLineFlags(&argc, &argv, true);
    google::InitGoogleLogging(argv[0]);
    AucCalculationConf* conf = AucCalculationConf::GetSingletonPtr()
                ->SetBinNum(FLAGS_bin_number)
                ->SetMasterIdx(FLAGS_master_index)
                ->SetScoreFilePath(FLAGS_data_path);
                // ->SetReadingBlockSize(FLAGS_block_size);

    std::shared_ptr<MpiBase> mpibase;
    std::cout << "mpi init" << std::endl;
    mpibase.reset(new MpiBase(argc, argv));
    std::cout << "auc calculation init" << std::endl;
    std::shared_ptr<AucCalculation> aucCalculation;
    aucCalculation.reset(new AucCalculation(mpibase));
    std::cout << "run" << std::endl;
    aucCalculation->Run(conf->GetScoreFilePath());

    return 0;
}
