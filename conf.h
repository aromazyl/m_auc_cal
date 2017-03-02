#pragma once

#include <cstdlib>
#include <cstring>
#include <string>
#include "singleton.hpp"

namespace news_dl { namespace LR {

class AucCalculationConf : public Singleton<AucCalculationConf> {
    public:
        AucCalculationConf() {
            binNum_ = 1000;
            masterId_ = 0;
        }

    public:
        AucCalculationConf* SetMyRank(int rank)                                 { this->myRank_ = rank; return this; }
        AucCalculationConf* SetTotalRank(int rank)                              { this->totalRank_ = rank; return this; }
        AucCalculationConf* SetBinNum(unsigned binNum)                          { this->binNum_ = binNum; return this; }
        AucCalculationConf* SetMasterIdx(unsigned index)                        { this->masterId_ = index; return this; }
        AucCalculationConf* SetReadingBlockSize(int64_t blockSize)              { this->blockSize_ = blockSize; return this; }
        AucCalculationConf* SetDataFilePath(const std::string& filename)        { this->dataFilePath_ = filename; return this; }
        AucCalculationConf* SetModelFilePath(const std::string& filename)       { this->modelFilePath_ = filename; return this; }
        AucCalculationConf* SetScoreFilePath(const std::string& filePath)       { this->scoreFilePath_ = filePath; return this; }

    public:
        int GetMyRank() const                                                   { return myRank_; }
        int GetTotalRank() const                                                { return totalRank_; }
        unsigned GetBinNum()  const                                             { return binNum_; }
        unsigned GetMasterIdx() const                                           { return masterId_; }
        int64_t  GetReadingBlockSize() const                                    { return blockSize_; }
        const std::string& GetScoreFilePath() const                             { return scoreFilePath_; }
        const std::string& GetModelFilePath() const                             { return modelFilePath_; }
        const std::string& GetDataFilePath() const                              { return dataFilePath_; }

    private:
        int myRank_;
        int totalRank_;
        unsigned binNum_;
        unsigned masterId_;
        int64_t blockSize_;
        std::string scoreFilePath_;
        std::string modelFilePath_;
        std::string dataFilePath_;
};

}
}
