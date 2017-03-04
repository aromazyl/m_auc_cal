#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <string>

namespace news_dl { namespace LR {
struct Feature {
    int64_t idx;
};

inline std::string DumpFeature(const Feature& feature) {
    char s[20];
    sprintf(s, "feature idx:[%lld]", feature.idx);
    return s;
}

struct ClickInfo {
    // int index;
    int nonClick;
    int Click;
};

inline std::string DumpClickInfo(const ClickInfo* clickInfo) {
    std::string ret;
    char s[100];
    sprintf(s, "clickInfo: nonClick[%d], Click[%d]",
            clickInfo->nonClick, clickInfo->Click);
    ret += s;
    return ret;
}

struct numClickInfos {
    int num;
    ClickInfo info[0];
};

class ModelParser {
    public:
        static bool Parse(char* line, int* key, float* value) {
            if (sscanf(line, "%d %f\n", key, value) == 2)
                return true;
            else
                return false;
        }
};

}}
