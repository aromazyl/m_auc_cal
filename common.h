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

/*
struct Instance {
    char uid[20];
    char cid[20];
    int islastInstance;
    float label;
    float weight;
    int fnum;
    Feature feature[0];
};
*/

/*
std::string DumpInstance(const Instance& instance) {
    std::string ret;
    char s[100];
    sprintf("uid: [%s], cid: [%s],"
            "feature number: [%d],"
            "islastInstance: [%d],"
            "label: [%f]", 
            *(&(instance.uid)),
            instance.cid,
            instance.fnum,
            instance.islastInstance,
            instance.label);
    ret += s;
    for (int i = 0; i < instance.fnum; ++i) {
        ret += DumpFeature(instance.feature[i]);
        ret += "\t";
    }
    return ret;
}
*/

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

/*
class InstanceParser {
    public:
        static bool Parse(char* line, Instance* instance, char* kvBuffer) {
            instance->fnum = 0;
            sscanf(line, "%s %s %f:%f %s\n",
                    instance->uid,
                    instance->cid,
                    &(instance->label),
                    &(instance->weight),
                    kvBuffer);

            int last = 0;
            for (int offset = 0; (kvBuffer[offset] != '\0') && (kvBuffer[offset] != '\n'); ++offset) {
                if (kvBuffer[offset] == ' ') {
                    instance->feature[instance->fnum].idx = strtol(kvBuffer + last, NULL, 10);
                    instance->fnum += 1;
                    last = offset;
                } else if (last != offset && kvBuffer[offset] == '\0') {
                    instance->feature[instance->fnum].idx = strtol(kvBuffer + last, NULL, 10);
                    instance->fnum += 1;
                }
            }
            return true;
        }
};
*/

}}
