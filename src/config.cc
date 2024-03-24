#include "config.h"
#include <napi.h>

using namespace Napi;

namespace
{
    char *new_cstr(const char *str)
    {
        auto size = strlen(str) + 1;
        auto *copy = new char[size];
        strncpy(copy, str, size);
        return copy;
    }
} // namespace

uint64_t GetInt64(const Napi::Value &initialValue, const char *key, uint64_t defaultValue)
{
    if (!initialValue.IsObject())
    {
        return defaultValue;
    }

    Napi::Object object = initialValue.As<Napi::Object>();
    Napi::Value value = object.Get(key);

    if (value.IsUndefined() || value.IsEmpty())
    {
        // return default value if undefined
        return defaultValue;
    }

    if (!value.IsBigInt())
    {
        // throw error if wrong type
        TypeError::New(value.Env(), "Wrong argument").ThrowAsJavaScriptException();
    }

    // return value
    bool lossless;
    return value.As<BigInt>().Int64Value(&lossless);
}

bool GetBool(const Napi::Value &initialValue, const char *key, bool defaultValue)
{
    if (!initialValue.IsObject())
    {
        return defaultValue;
    }

    // iterate over keys for deep query
    Napi::Object object = initialValue.As<Napi::Object>();
    Napi::Value value = object.Get(key);

    // interrupted before the last key
    if (value.IsUndefined() || value.IsEmpty())
    {
        // return default value if undefined
        return defaultValue;
    }

    if (!value.IsBoolean())
    {
        // throw error if wrong type
        TypeError::New(value.Env(), "Wrong argument").ThrowAsJavaScriptException();
    }

    // return value
    return value.ToBoolean().Value();
}

const char *GetString(const Napi::Value &initialValue, const char *key, const char *defaultValue)
{
    if (!initialValue.IsObject())
    {
        return new_cstr(defaultValue);
    }

    // iterate over keys for deep query
    Napi::Object object = initialValue.As<Napi::Object>();
    Napi::Value value = object.Get(key);

    // interrupted before the last key
    if (value.IsUndefined() || value.IsEmpty())
    {
        // return default value if undefined
        return new_cstr(defaultValue);
    }

    if (!value.IsString())
    {
        // throw error if wrong type
        TypeError::New(value.Env(), "Wrong argument").ThrowAsJavaScriptException();
    }

    // return value
    return new_cstr(value.ToString().Utf8Value().c_str());
}

cm_processor_config GetProcessorConfig(const Value &object, cm_processor_config defaultValue)
{
    // create object
    cm_processor_config config = {};
    for (size_t i = 0; i < CM_MACHINE_X_REG_COUNT; ++i)
    {
        config.x[i] = defaultValue.x[i];
    }
    for (size_t i = 0; i < CM_MACHINE_F_REG_COUNT; ++i)
    {
        config.f[i] = defaultValue.f[i];
    }
    config.pc = GetInt64(object, "pc", defaultValue.pc);
    config.fcsr = GetInt64(object, "fcsr", defaultValue.fcsr);
    config.mvendorid = GetInt64(object, "mvendorid", defaultValue.mvendorid);
    config.marchid = GetInt64(object, "marchid", defaultValue.marchid);
    config.mimpid = GetInt64(object, "mimpid", defaultValue.mimpid);
    config.mcycle = GetInt64(object, "mcycle", defaultValue.mcycle);
    config.icycleinstret = GetInt64(object, "icycleinstret", defaultValue.icycleinstret);
    config.mstatus = GetInt64(object, "mstatus", defaultValue.mstatus);
    config.mtvec = GetInt64(object, "mtvec", defaultValue.mtvec);
    config.mscratch = GetInt64(object, "mscratch", defaultValue.mscratch);
    config.mepc = GetInt64(object, "mepc", defaultValue.mepc);
    config.mcause = GetInt64(object, "mcause", defaultValue.mcause);
    config.mtval = GetInt64(object, "mtval", defaultValue.mtval);
    config.misa = GetInt64(object, "misa", defaultValue.misa);
    config.mie = GetInt64(object, "mie", defaultValue.mie);
    config.mip = GetInt64(object, "mip", defaultValue.mip);
    config.medeleg = GetInt64(object, "medeleg", defaultValue.medeleg);
    config.mideleg = GetInt64(object, "mideleg", defaultValue.mideleg);
    config.mcounteren = GetInt64(object, "mcounteren", defaultValue.mcounteren);
    config.menvcfg = GetInt64(object, "menvcfg", defaultValue.menvcfg);
    config.stvec = GetInt64(object, "stvec", defaultValue.stvec);
    config.sscratch = GetInt64(object, "sscratch", defaultValue.sscratch);
    config.sepc = GetInt64(object, "sepc", defaultValue.sepc);
    config.scause = GetInt64(object, "scause", defaultValue.scause);
    config.stval = GetInt64(object, "stval", defaultValue.stval);
    config.satp = GetInt64(object, "satp", defaultValue.satp);
    config.scounteren = GetInt64(object, "scounteren", defaultValue.scounteren);
    config.senvcfg = GetInt64(object, "senvcfg", defaultValue.senvcfg);
    config.ilrsc = GetInt64(object, "ilrsc", defaultValue.ilrsc);
    config.iflags = GetInt64(object, "iflags", defaultValue.iflags);
    return config;
}

cm_ram_config GetRamConfig(const Value &object, cm_ram_config defaultValue)
{
    // create object
    cm_ram_config config = {};

    // get values from object or use default value
    config.length = GetInt64(object, "length", defaultValue.length);
    config.image_filename = GetString(object, "image_filename", defaultValue.image_filename);

    return config;
}

cm_dtb_config GetDtbConfig(const Napi::Value &object, cm_dtb_config defaultValue)
{
    // create object
    cm_dtb_config config = {};

    // copy default values and return
    config.bootargs = GetString(object, "bootargs", defaultValue.bootargs);
    config.init = GetString(object, "init", defaultValue.init);
    config.entrypoint = GetString(object, "entrypoint", defaultValue.entrypoint);
    config.image_filename = GetString(object, "image_filename", defaultValue.image_filename);

    return config;
}

cm_memory_range_config GetMemoryRangeConfig(const Napi::Value &object, cm_memory_range_config defaultValue)
{
    cm_memory_range_config config = {};
    config.start = GetInt64(object, "start", defaultValue.start);
    config.length = GetInt64(object, "length", defaultValue.length);
    config.shared = GetBool(object, "shared", defaultValue.shared);
    config.image_filename = GetString(object, "image_filename", defaultValue.image_filename);
    return config;
}

cm_memory_range_config_array GetFlashDrive(const Napi::Object &object, cm_memory_range_config_array defaultValue)
{
    cm_memory_range_config_array config = {};

    if (!object.Has("flash_drive"))
    {
        config.count = defaultValue.count;
        config.entry = new cm_memory_range_config[config.count];
        for (size_t i = 0; i < config.count; ++i)
        {
            config.entry[i] = defaultValue.entry[i];
            config.entry[i].image_filename = new_cstr(defaultValue.entry[i].image_filename);
        }
        return defaultValue;
    }

    Value value = object.Get("flash_drive");
    if (!value.IsArray())
    {
        TypeError::New(object.Env(), "Wrong argument 8").ThrowAsJavaScriptException();
    }

    Array drives = value.As<Array>();
    config.count = drives.Length();
    config.entry = new cm_memory_range_config[config.count];
    for (size_t i = 0; i < config.count; ++i)
    {
        Napi::Value value = drives.Get(i);
        config.entry[i] = GetMemoryRangeConfig(value, {});
    }

    return config;
}

cm_tlb_config GetTlbConfig(const Napi::Value &object, cm_tlb_config defaultValue)
{
    cm_tlb_config config = {};
    config.image_filename = GetString(object, "image_filename", defaultValue.image_filename);
    return config;
}

cm_clint_config GetClintConfig(const Napi::Value &object, cm_clint_config defaultValue)
{
    cm_clint_config config = {};
    config.mtimecmp = GetInt64(object, "mtimecmp", defaultValue.mtimecmp);
    return config;
}

cm_htif_config GetHtifConfig(const Napi::Value &object, cm_htif_config defaultValue)
{
    cm_htif_config config = {};
    config.console_getchar = GetBool(object, "console_getchar", defaultValue.console_getchar);
    config.fromhost = GetInt64(object, "fromhost", defaultValue.fromhost);
    config.tohost = GetInt64(object, "tohost", defaultValue.tohost);
    config.yield_automatic = GetBool(object, "yield_automatic", defaultValue.yield_automatic);
    config.yield_manual = GetBool(object, "yield_manual", defaultValue.yield_manual);
    return config;
}

cm_rollup_config GetRollupConfig(const Napi::Value &object, cm_rollup_config defaultValue)
{
    cm_rollup_config config = {};

    config.has_value = GetBool(object, "has_value", defaultValue.has_value);
    if (object.IsObject())
    {
        Napi::Object obj = object.As<Napi::Object>();
        config.rx_buffer = GetMemoryRangeConfig(obj.Get("rx_buffer"), defaultValue.rx_buffer);
        config.tx_buffer = GetMemoryRangeConfig(obj.Get("tx_buffer"), defaultValue.tx_buffer);
        config.input_metadata = GetMemoryRangeConfig(obj.Get("input_metadata"), defaultValue.input_metadata);
        config.voucher_hashes = GetMemoryRangeConfig(obj.Get("voucher_hashes"), defaultValue.voucher_hashes);
        config.notice_hashes = GetMemoryRangeConfig(obj.Get("notice_hashes"), defaultValue.notice_hashes);
    }

    return config;
}

cm_uarch_processor_config GetUarchProcessorConfig(const Napi::Object &object, cm_uarch_processor_config defaultValue)
{
    cm_uarch_processor_config config = {};
    config.cycle = GetInt64(object, "cycle", defaultValue.cycle);
    config.halt_flag = GetBool(object, "halt_flag", defaultValue.halt_flag);
    config.pc = GetInt64(object, "pc", defaultValue.pc);
    for (size_t i = 0; i < CM_MACHINE_UARCH_X_REG_COUNT; ++i)
    {
        config.x[i] = defaultValue.x[i];
    }
    return config;
}

cm_uarch_ram_config GetUarchRamConfig(const Napi::Object &object, cm_uarch_ram_config defaultValue)
{
    cm_uarch_ram_config config = {};
    config.image_filename = GetString(object, "image_filename", defaultValue.image_filename);
    return config;
}

cm_uarch_config GetUarchConfig(const Napi::Object &object, cm_uarch_config defaultValue)
{
    cm_uarch_config config = {};
    config.processor = GetUarchProcessorConfig(object, defaultValue.processor);
    config.ram = GetUarchRamConfig(object, defaultValue.ram);
    return config;
}

cm_machine_config GetMachineConfig(const Napi::Object &object)
{
    // create object with default values
    const cm_machine_config *defaultConfig = cm_new_default_machine_config();

    // create object, use values from object or deep copy default values
    cm_machine_config config = {};
    config.processor = GetProcessorConfig(object.Get("processor"), defaultConfig->processor);
    config.ram = GetRamConfig(object.Get("ram"), defaultConfig->ram);
    config.dtb = GetDtbConfig(object.Get("dtb"), defaultConfig->dtb);
    config.flash_drive = GetFlashDrive(object, defaultConfig->flash_drive);

    config.tlb = GetTlbConfig(object.Get("tlb"), defaultConfig->tlb);
    config.clint = GetClintConfig(object.Get("clint"), defaultConfig->clint);
    config.htif = GetHtifConfig(object.Get("htif"), defaultConfig->htif);
    config.rollup = GetRollupConfig(object.Get("rollup"), defaultConfig->rollup);
    config.uarch = GetUarchConfig(object, defaultConfig->uarch);

    // free memory
    cm_delete_machine_config(defaultConfig);

    return config;
}

MemoryRangeConfig::MemoryRangeConfig(napi_env env, napi_value value) : Napi::Object(env, value){};

MemoryRangeConfig MemoryRangeConfig::New(napi_env env, cm_memory_range_config &config)
{
    Napi::Object object = Napi::Object::New(env);
    object.Set("start", Napi::BigInt::New(env, config.start));
    object.Set("length", Napi::BigInt::New(env, config.length));
    object.Set("shared", Napi::Boolean::New(env, config.shared));
    object.Set("image_filename", Napi::String::New(env, config.image_filename));
    return MemoryRangeConfig(env, object);
}

void MemoryRangeConfig::Unwrap(cm_memory_range_config &config)
{
    config.start = GetInt64(*this, "start", config.start);
    config.length = GetInt64(*this, "length", config.length);
    config.shared = GetBool(*this, "shared", config.shared);
    config.image_filename = GetString(*this, "image_filename", config.image_filename);
    return;
}
