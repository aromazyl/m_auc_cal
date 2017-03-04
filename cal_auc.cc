#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <cassert>
#include <cmath>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fstream>
#include <glog/logging.h>
#include <atomic>
#include <memory>
#include <functional>
#include "cal_auc.h"
#include "stringprintf.hpp"
namespace {
inline void opendirectory(const std::string& filename, const std::string& fatherpath, std::vector<std::string>* filepath) {
    assert(filepath);
    DIR* dir;
    struct dirent* ent;
    struct stat file_info;
    if ((filename == ".") || (filename == "..")) return;
    if ((dir = opendir((fatherpath + filename).c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            lstat((fatherpath + filename + "/" + ent->d_name).c_str(), &file_info);
            if (S_ISDIR(file_info.st_mode) || (S_ISLNK(file_info.st_mode)))
                opendirectory(ent->d_name, fatherpath + filename + "/", filepath);
            else
                filepath->push_back(fatherpath + filename + "/" + ent->d_name);
        }
        closedir(dir);
    } else {
        fprintf(stderr, "%s open failure.\n", filename.c_str());
    }
}
}
namespace news_dl { namespace LR {
AucCalculation::AucCalculation(std::shared_ptr<mpi::MpiBase> mpiBase) : mpiPtr_ (mpiBase) {
    conf = AucCalculationConf::GetSingletonPtr();
    LOG(INFO) << "loading model data, file path:" << conf->GetModelFilePath();
    ctrClickInfo_ = static_cast<numClickInfos*>(malloc(sizeof(numClickInfos) + sizeof(ClickInfo) * conf->GetBinNum()));
    assert(ctrClickInfo_);
    ctrClickInfo_->num = conf->GetBinNum();
    ctrClickInfoBuffer_ = static_cast<numClickInfos*>(malloc((sizeof(numClickInfos) + sizeof(ClickInfo) * conf->GetBinNum()) * conf->GetTotalRank()));
    pthread_mutex_init(&(this->modelLock_), NULL);
}

AucCalculation::~AucCalculation() {
    if (ctrClickInfo_) free(ctrClickInfo_);
    if (ctrClickInfoBuffer_) free(ctrClickInfoBuffer_);
    pthread_mutex_destroy(&(this->modelLock_));
}

void AucCalculation::LoadPredictionData(FILE* file_pointer, std::atomic<int>* counter) {
  int label;
  float score;
  char buf[1024];
  while (fgets(buf, sizeof(buf), file_pointer)) {
    sscanf(buf, "%d\t%f", &label, &score);
    const int binNum = conf->GetBinNum();
    int rank;
    if (score == 1) rank = binNum - 1;
    else rank = score * binNum;
    mu_.lock();
    if (label == 1) {
      ctrClickInfo_->info[rank].Click += 1;
    } else {
      ctrClickInfo_->info[rank].nonClick += 1;
    }
    mu_.unlock();
  }
  fclose(file_pointer);
  ++(*counter);
}

bool AucCalculation::Run(const std::string& filepath) {
    std::vector<std::string> subfiles;
    opendirectory(filepath, "", &subfiles);
    std::atomic<int> counter(0);
    std::cout << "run prediction" << std::endl;
    for (size_t i = 0; i < subfiles.size(); ++i) {
      std::cout << base_string::StringPrintf("subfiles[%d]=%s\n", i, subfiles[i].c_str());
      FILE* f = fopen(subfiles[i].c_str(), "r");

      // this->pool_.Submit(std::bind(&AucCalculation::LoadPredictionData, this, f, &counter));
      this->LoadPredictionData(f, &counter);
      // this->Prediction(subfiles[i], &counter);
    }
    while (counter < subfiles.size()) {
        usleep(1000);
    }
    std::cout << "merge prediction result" << std::endl;
    MergeData();
    std::cout << "dump result";
    if (conf->GetMyRank() == 0) {
        this->DumpResult(CalculateAuc());
    }
    return true;
}

void AucCalculation::DumpResult(float score) {
    std::cout << "auc score is " << score << std::endl;
}

void AucCalculation::MergeData() {
    assert(mpiPtr_);
    const int64_t clickInfoOffset = (sizeof(numClickInfos) + sizeof(ClickInfo) * conf->GetBinNum());
    int offset = 0;
    mpiPtr_->WorkerMasterCommunication(
            ctrClickInfo_,
            (int)(sizeof(numClickInfos) + sizeof(ClickInfo) * ctrClickInfo_->num),
            (void**)(&ctrClickInfoBuffer_),
            &offset
            );

    if (conf->GetMyRank() == 0) {
        for (int i = 1; i < conf->GetTotalRank(); ++i) {
            for (int index = 0; index < conf->GetBinNum(); ++i) {
                ClickInfo& info = ctrClickInfoBuffer_->info[index];
                const ClickInfo& rinfo = ((numClickInfos*)((char*)(ctrClickInfoBuffer_) + clickInfoOffset * i))->info[index];
                info.nonClick += rinfo.nonClick;
                info.Click += rinfo.Click;
            }
        }
    }
}

float AucCalculation::CalculateAuc() {
    float clickSum = 0.0;
    float nclickSum = 0.0;
    float aucResult = 0.0;
    for (int i = 0; i < conf->GetBinNum(); ++i) {
        const ClickInfo& info = ctrClickInfoBuffer_->info[i];
        clickSum += info.Click;
        nclickSum += info.nonClick;
        aucResult += 1.0 * nclickSum * info.Click + 1.0 * clickSum * info.nonClick / 2.0;
    }
    return aucResult / (clickSum * nclickSum);
}

}}

