#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <atomic>
#include <mutex>
#include "common.h"
#include "conf.h"
#include "mpi_base.h"
#include "simple_threadpool.hpp"
#ifndef SEED
#define SEED 2017
#endif
// #include "data_provider.h"

namespace news_dl { namespace LR {
class AucCalculation {
  public:
    AucCalculation(std::shared_ptr<mpi::MpiBase> mpiBase);
    ~AucCalculation();

    bool Run(const std::string& filepath);

  private:
    float CalculateAuc();

    void MergeData();
    void DumpResult(float score);
    void LoadPredictionData(FILE*, std::atomic<int>*);


  private:
    std::shared_ptr<mpi::MpiBase> mpiPtr_;
    pthread_mutex_t modelLock_;

    numClickInfos* ctrClickInfo_;
    numClickInfos* ctrClickInfoBuffer_;

    ThreadPool pool_;
    std::mutex mu_;

    LR::AucCalculationConf* conf;
};
}}
