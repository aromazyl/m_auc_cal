#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <cassert>
#include <cmath>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <glog/logging.h>
#include "cal_auc.h"
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
AucCalculation::AucCalculation(boost::shared_ptr<mpi::MpiBase> mpiBase) : mpiPtr_ (mpiBase) {
    conf = AucCalculationConf::GetSingletonPtr();
    LOG(INFO) << "loading model data, file path:" << conf->GetModelFilePath();
    LoadModelData(conf->GetModelFilePath());
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

void AucCalculation::LoadModelData(const std::string& modelPath) {
    std::vector<std::string> subfiles;
    opendirectory(modelPath, "", &subfiles);
    boost::atomic<int> counter(0);
    for (size_t i = 0; i < subfiles.size(); ++i) {
        // this->pool_.Submit(boost::bind(&AucCalculation::LoadModelDataFile, this, subfiles[i], &counter));
        this->LoadModelDataFile(subfiles[i], &counter);
    }
    while (counter < subfiles.size()) {
        usleep(1000);
    }
    LOG(INFO) << "loading model data sucess";
}

void AucCalculation::LoadModelDataFile(const std::string& modelfile, boost::atomic<int>* counter) {
    std::ifstream fs(modelfile.c_str());
    assert(fs.is_open());
    std::string line;
    std::vector<std::string> terms;
    LOG(INFO) << "model file:" << modelfile;
    while (std::getline(fs, line)) {
        terms.clear();
        boost::split(terms, line, boost::is_any_of("\t :"));
        {
            AutoLock(&(this->modelLock_));
            this->model_[Hash(terms[0])]
                = atof(terms[1].c_str());
        }
    }
    *counter += 1;
}

bool AucCalculation::Run(const std::string& filepath) {
    std::vector<std::string> subfiles;
    opendirectory(filepath, "", &subfiles);
    boost::atomic<int> counter(0);
    LOG(INFO) << "run prediction";
    for (size_t i = 0; i < subfiles.size(); ++i) {
        this->pool_.Submit(boost::bind(&AucCalculation::Prediction, this, subfiles[i], &counter));
        // this->Prediction(subfiles[i], &counter);
    }
    while (counter < subfiles.size()) {
        usleep(1000);
    }
    LOG(INFO) << "merge prediction result";
    MergeData();
    LOG(INFO) << "dump result";
    if (conf->GetMyRank() == 0) {
        this->DumpResult(CalculateAuc());
    }
    return true;
}

void AucCalculation::DumpResult(float score) {
    std::cout << "auc score is " << score << std::endl;
}

void AucCalculation::Prediction(const std::string& filePath, boost::atomic<int>* counter) {
    std::ifstream fs(filePath.c_str());
    assert(fs.is_open());
    std::string line;
    std::vector<std::string> line_part;
    while (std::getline(fs, line)) {
        line_part.clear();
        boost::split(line_part, line, boost::is_any_of("\t :"));
        float score = 0;
        for (int i = 3; i < line_part.size(); ++i)   
            if (this->model_.count(Hash(line_part[i])))
                score += this->model_.at(Hash(line_part[i]));
        score += model_.at(0);
        score = 1.0 / (1.0 + exp(-score));
        const int& binNum = ctrClickInfo_->num;
        ClickInfo& info = this->ctrClickInfo_->info[int(binNum * score)]; 
        if (line_part[2][0] == '0') {
            info.nonClick += 1;
        } else {
            info.Click += 1;
        }
    }
    *counter += 1;
}

/*
float AucCalculation::CalculateScore(Instance* instance) const {
    float ret = 0.0;
    for (int i = 0; i < instance->fnum; ++i) {
        const uint64_t& fidx = instance->feature[i].idx;
        if (model_.count(fidx)) {
            ret += model_.at(fidx);
        }
    }

    return 1.0 / (1.0 + exp(- ret + model_.at(0)));
}
*/

void AucCalculation::MergeData() {
    assert(mpiPtr_);
    const int64_t clickInfoOffset = (sizeof(numClickInfos) + sizeof(ClickInfo) * conf->GetBinNum());
    long long int offset = 0;
    mpiPtr_->WorkerMasterCommunication(
            ctrClickInfo_,
            (sizeof(numClickInfos) + sizeof(ClickInfo) * ctrClickInfo_->num),
            (void**)(&ctrClickInfoBuffer_),
            // clickInfoOffset * conf->GetTotalRank()
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

