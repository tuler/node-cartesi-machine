#include <napi.h>
#include "machine.h"

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    return Machine::Init(env, exports);
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
