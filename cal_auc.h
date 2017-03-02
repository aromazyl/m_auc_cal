#pragma once

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/tr1/unordered_map.hpp>
#include <boost/atomic.hpp>
#include <MurmurHash2.h>
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
    static inline uint64_t Hash(const std::string& str) {
      return static_cast<uint64_t>(MurmurHash64A(
            str.c_str(),
            str.length(),
            SEED));
    }
  public:
    AucCalculation(boost::shared_ptr<mpi::MpiBase> mpiBase);
    ~AucCalculation();

    bool Run(const std::string& filepath);

    void LoadModelData(const std::string& modelPath);

  private:
    void Prediction(const std::string& filename, boost::atomic<int>* counter);
    float CalculateAuc();

    void LoadModelDataFile(const std::string& modelfile, boost::atomic<int>* counter);
    // float CalculateScore(Instance* instance) const;

    void MergeData();
    void DumpResult(float score);


  private:
    // boost::shared_ptr<DataProvider> modelData_;
    boost::shared_ptr<mpi::MpiBase> mpiPtr_;

    std::tr1::unordered_map<uint64_t, float> model_;
    pthread_mutex_t modelLock_;

    numClickInfos* ctrClickInfo_;
    numClickInfos* ctrClickInfoBuffer_;

    // ThreadPool pool_;

    LR::AucCalculationConf* conf;
};
}}
