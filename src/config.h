#ifndef CNODE_MACHINE_CONFIG_H
#define CNODE_MACHINE_CONFIG_H

#include <napi.h>
#include "machine-c-api.h"

class MemoryRangeConfig : public Napi::Object
{
public:
    static MemoryRangeConfig New(napi_env env, cm_memory_range_config &config);
    MemoryRangeConfig(napi_env env, napi_value value);
    void Unwrap(cm_memory_range_config &config);
};

cm_machine_config GetMachineConfig(const Napi::Object &value);

#endif
