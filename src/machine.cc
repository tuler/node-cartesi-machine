#include <napi.h>
#include "config.h"
#include "machine.h"
#include "proof.h"

#define THROW_IF_FALSE(info, condition, message)                                \
    if (!(condition))                                                           \
    {                                                                           \
        Napi::TypeError::New(info.Env(), message).ThrowAsJavaScriptException(); \
        return Napi::Value();                                                   \
    }

#define THROW_IF_FALSE_VOID(info, condition, message)                           \
    if (!(condition))                                                           \
    {                                                                           \
        Napi::TypeError::New(info.Env(), message).ThrowAsJavaScriptException(); \
        return;                                                                 \
    }

#define THROW_IF_WRONG_TYPE(info, condition) \
    THROW_IF_FALSE(info, condition, "Wrong argument");

#define THROW_IF_WRONG_TYPE_VOID(info, condition) \
    THROW_IF_FALSE_VOID(info, condition, "Wrong argument");

#define THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, min, max) \
    THROW_IF_FALSE(info, info.Length() >= min && info.Length() <= max, "Wrong number of arguments");

#define THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, min, max) \
    THROW_IF_FALSE_VOID(info, info.Length() >= min && info.Length() <= max, "Wrong number of arguments");

#define THROW_IF_CM_ERROR(env, error_code, err_msg)            \
    if (error_code != CM_ERROR_OK)                             \
    {                                                          \
        Napi::String str;                                      \
        if (err_msg != nullptr)                                \
        {                                                      \
            str = Napi::String::New(env, err_msg);             \
            cm_delete_cstring(err_msg);                        \
        }                                                      \
        else                                                   \
        {                                                      \
            str = Napi::String::New(env, "Unknown error");     \
        }                                                      \
        const Napi::Error error = Napi::Error::New(env, str);  \
        error.Set("code", Napi::Number::New(env, error_code)); \
        error.ThrowAsJavaScriptException();                    \
        return Napi::Value();                                  \
    }

#define THROW_IF_CM_ERROR_VOID(env, error_code, err_msg)       \
    if (error_code != CM_ERROR_OK)                             \
    {                                                          \
        Napi::String str;                                      \
        if (err_msg != nullptr)                                \
        {                                                      \
            str = Napi::String::New(env, err_msg);             \
            cm_delete_cstring(err_msg);                        \
        }                                                      \
        else                                                   \
        {                                                      \
            str = Napi::String::New(env, "Unknown error");     \
        }                                                      \
        const Napi::Error error = Napi::Error::New(env, str);  \
        error.Set("code", Napi::Number::New(env, error_code)); \
        error.ThrowAsJavaScriptException();                    \
        return;                                                \
    }

Napi::Object Machine::Init(Napi::Env env, Napi::Object exports)
{
    Napi::Function func = DefineClass(
        env,
        "CartesiMachine", {
                              InstanceMethod<&Machine::GetRootHash>("get_root_hash"),
                              InstanceMethod<&Machine::Store>("store"),
                              InstanceMethod<&Machine::Run>("run"),
                              InstanceMethod<&Machine::RunUarch>("run_uarch"),
                              InstanceMethod<&Machine::ReadMemory>("read_memory"),
                              InstanceMethod<&Machine::WriteMemory>("write_memory"),
                              InstanceMethod<&Machine::ReadWord>("read_word"),
                              InstanceMethod<&Machine::ReadVirtualMemory>("read_virtual_memory"),
                              InstanceMethod<&Machine::WriteVirtualMemory>("write_virtual_memory"),
                              InstanceMethod<&Machine::GetProof>("get_proof"),
                              InstanceMethod<&Machine::Snapshot>("snapshot"),
                              InstanceMethod<&Machine::Rollback>("rollback"),
                              InstanceMethod<&Machine::VerifyMerkleTree>("verify_merkle_tree"),
                              InstanceMethod<&Machine::VerifyDirtyPageMaps>("verify_dirty_page_maps"),
                              InstanceMethod<&Machine::ReplaceMemoryRange>("replace_memory_range"),

                              InstanceMethod<&Machine::ReadPc>("read_pc"),
                              InstanceMethod<&Machine::ReadFcsr>("read_fcsr"),
                              InstanceMethod<&Machine::ReadMcycle>("read_mcycle"),
                              InstanceMethod<&Machine::ReadIcycleinstret>("read_icycleinstret"),
                              InstanceMethod<&Machine::ReadMstatus>("read_mstatus"),
                              InstanceMethod<&Machine::ReadMenvcfg>("read_menvcfg"),
                              InstanceMethod<&Machine::ReadMtvec>("read_mtvec"),
                              InstanceMethod<&Machine::ReadMscratch>("read_mscratch"),
                              InstanceMethod<&Machine::ReadMepc>("read_mepc"),
                              InstanceMethod<&Machine::ReadMcause>("read_mcause"),
                              InstanceMethod<&Machine::ReadMtval>("read_mtval"),
                              InstanceMethod<&Machine::ReadMisa>("read_misa"),
                              InstanceMethod<&Machine::ReadMie>("read_mie"),
                              InstanceMethod<&Machine::ReadMip>("read_mip"),
                              InstanceMethod<&Machine::ReadMedeleg>("read_medeleg"),
                              InstanceMethod<&Machine::ReadMideleg>("read_mideleg"),
                              InstanceMethod<&Machine::ReadMcounteren>("read_mcounteren"),
                              InstanceMethod<&Machine::ReadStvec>("read_stvec"),
                              InstanceMethod<&Machine::ReadSscratch>("read_sscratch"),
                              InstanceMethod<&Machine::ReadSepc>("read_sepc"),
                              InstanceMethod<&Machine::ReadScause>("read_scause"),
                              InstanceMethod<&Machine::ReadStval>("read_stval"),
                              InstanceMethod<&Machine::ReadSatp>("read_satp"),
                              InstanceMethod<&Machine::ReadScounteren>("read_scounteren"),
                              InstanceMethod<&Machine::ReadSenvcfg>("read_senvcfg"),
                              InstanceMethod<&Machine::ReadIlrsc>("read_ilrsc"),
                              InstanceMethod<&Machine::ReadIflags>("read_iflags"),
                              InstanceMethod<&Machine::ReadHtifTohost>("read_htif_tohost"),
                              InstanceMethod<&Machine::ReadHtifFromhost>("read_htif_fromhost"),
                              InstanceMethod<&Machine::ReadHtifIhalt>("read_htif_ihalt"),
                              InstanceMethod<&Machine::ReadHtifIconsole>("read_htif_iconsole"),
                              InstanceMethod<&Machine::ReadHtifIyield>("read_htif_iyield"),
                              InstanceMethod<&Machine::ReadClintMtimecmp>("read_clint_mtimecmp"),
                              InstanceMethod<&Machine::ReadUarchPc>("read_uarch_pc"),
                              InstanceMethod<&Machine::ReadUarchCycle>("read_uarch_cycle"),
                              InstanceMethod<&Machine::ReadMvendorid>("read_mvendorid"),
                              InstanceMethod<&Machine::ReadMarchid>("read_marchid"),
                              InstanceMethod<&Machine::ReadMimpid>("read_mimpid"),
                              InstanceMethod<&Machine::ReadHtifTohostDev>("read_htif_tohost_dev"),
                              InstanceMethod<&Machine::ReadHtifTohostCmd>("read_htif_tohost_cmd"),
                              InstanceMethod<&Machine::ReadHtifTohostData>("read_htif_tohost_data"),
                              InstanceMethod<&Machine::WritePc>("write_pc"),
                              InstanceMethod<&Machine::WriteFcsr>("write_fcsr"),
                              InstanceMethod<&Machine::WriteMcycle>("write_mcycle"),
                              InstanceMethod<&Machine::WriteIcycleinstret>("write_icycleinstret"),
                              InstanceMethod<&Machine::WriteMstatus>("write_mstatus"),
                              InstanceMethod<&Machine::WriteMenvcfg>("write_menvcfg"),
                              InstanceMethod<&Machine::WriteMtvec>("write_mtvec"),
                              InstanceMethod<&Machine::WriteMscratch>("write_mscratch"),
                              InstanceMethod<&Machine::WriteMepc>("write_mepc"),
                              InstanceMethod<&Machine::WriteMcause>("write_mcause"),
                              InstanceMethod<&Machine::WriteMtval>("write_mtval"),
                              InstanceMethod<&Machine::WriteMisa>("write_misa"),
                              InstanceMethod<&Machine::WriteMie>("write_mie"),
                              InstanceMethod<&Machine::WriteMip>("write_mip"),
                              InstanceMethod<&Machine::WriteMedeleg>("write_medeleg"),
                              InstanceMethod<&Machine::WriteMideleg>("write_mideleg"),
                              InstanceMethod<&Machine::WriteMcounteren>("write_mcounteren"),
                              InstanceMethod<&Machine::WriteStvec>("write_stvec"),
                              InstanceMethod<&Machine::WriteSscratch>("write_sscratch"),
                              InstanceMethod<&Machine::WriteSepc>("write_sepc"),
                              InstanceMethod<&Machine::WriteScause>("write_scause"),
                              InstanceMethod<&Machine::WriteStval>("write_stval"),
                              InstanceMethod<&Machine::WriteSatp>("write_satp"),
                              InstanceMethod<&Machine::WriteScounteren>("write_scounteren"),
                              InstanceMethod<&Machine::WriteSenvcfg>("write_senvcfg"),
                              InstanceMethod<&Machine::WriteIlrsc>("write_ilrsc"),
                              InstanceMethod<&Machine::WriteIflags>("write_iflags"),
                              InstanceMethod<&Machine::WriteHtifTohost>("write_htif_tohost"),
                              InstanceMethod<&Machine::WriteHtifFromhost>("write_htif_fromhost"),
                              InstanceMethod<&Machine::WriteHtifIhalt>("write_htif_ihalt"),
                              InstanceMethod<&Machine::WriteHtifIconsole>("write_htif_iconsole"),
                              InstanceMethod<&Machine::WriteHtifIyield>("write_htif_iyield"),
                              InstanceMethod<&Machine::WriteClintMtimecmp>("write_clint_mtimecmp"),
                              InstanceMethod<&Machine::WriteUarchPc>("write_uarch_pc"),
                              InstanceMethod<&Machine::WriteUarchCycle>("write_uarch_cycle"),
                              InstanceMethod<&Machine::WriteHtifFromhostData>("write_htif_fromhost_data"),
                              InstanceMethod<&Machine::ReadIflagsX>("read_iflags_X"),
                              InstanceMethod<&Machine::ReadIflagsY>("read_iflags_Y"),
                              InstanceMethod<&Machine::ReadIflagsH>("read_iflags_H"),
                              InstanceMethod<&Machine::ReadUarchHaltFlag>("read_uarch_halt_flag"),
                              InstanceMethod<&Machine::SetIflagsX>("set_iflags_X"),
                              InstanceMethod<&Machine::SetIflagsY>("set_iflags_Y"),
                              InstanceMethod<&Machine::SetIflagsH>("set_iflags_H"),
                              InstanceMethod<&Machine::SetUarchHaltFlag>("set_uarch_halt_flag"),
                              InstanceMethod<&Machine::ResetIflagsX>("reset_iflags_X"),
                              InstanceMethod<&Machine::ResetIflagsY>("reset_iflags_Y"),
                              InstanceMethod<&Machine::ResetUarchHaltFlag>("reset_uarch_halt_flag"),

                              InstanceMethod<&Machine::ReadX>("read_x"),
                              InstanceMethod<&Machine::ReadF>("read_f"),
                              InstanceMethod<&Machine::ReadUarchX>("read_uarch_x"),
                              InstanceMethod<&Machine::WriteX>("write_x"),
                              InstanceMethod<&Machine::WriteF>("write_f"),
                              InstanceMethod<&Machine::WriteUarchX>("write_uarch_x"),

                              InstanceMethod<&Machine::ReadCsr>("read_csr"),
                              InstanceMethod<&Machine::WriteCsr>("write_csr"),
                          });

    Napi::FunctionReference *constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.SetInstanceData(constructor);
    exports.Set("Machine", func);
    return exports;
}

Machine::Machine(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Machine>(info)
{
    Napi::Env env = info.Env();
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 1, 2);

    cm_machine *machine = NULL;
    char *err_msg = NULL;

    if (info[0].IsString())
    {
        // load from snapshot
        Napi::String dir = info[0].As<Napi::String>();
        cm_machine_runtime_config runtime_config{}; // TODO: read runtime config
        THROW_IF_CM_ERROR_VOID(env, cm_load_machine(dir.Utf8Value().c_str(), &runtime_config, &machine, &err_msg), err_msg);
    }
    else if (info[0].IsObject())
    {
        // create from config
        cm_machine_config config = GetMachineConfig(info[0].As<Napi::Object>());
        cm_machine_runtime_config runtime_config{}; // TODO: read runtime config
        THROW_IF_CM_ERROR_VOID(env, cm_create_machine(&config, &runtime_config, &machine, &err_msg), err_msg);
    }
    else
    {
        Napi::TypeError::New(info.Env(), "Wrong arguments").ThrowAsJavaScriptException();
        return;
    }
    this->machine = machine;
};

void Machine::Finalize(Napi::Env env)
{
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(env, cm_destroy(this->machine, &err_msg), err_msg);
}

Napi::Value Machine::GetRootHash(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    char *err_msg = NULL;
    Napi::ArrayBuffer buffer = Napi::ArrayBuffer::New(env, sizeof(uint8_t) * CM_MACHINE_HASH_BYTE_SIZE);
    cm_hash *hash = (cm_hash *)buffer.Data();
    THROW_IF_CM_ERROR(env, cm_get_root_hash(this->machine, hash, &err_msg), err_msg);
    return Napi::Uint8Array::New(env, CM_MACHINE_HASH_BYTE_SIZE, buffer, 0);
}

void Machine::Store(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 1, 1);
    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong argument").ThrowAsJavaScriptException();
        return;
    }

    Napi::String directory = info[0].As<Napi::String>();
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(env, cm_store(this->machine, directory.Utf8Value().c_str(), &err_msg), err_msg);
}

Napi::Value Machine::Run(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 1);

    uint64_t mcycle_end = UINT64_MAX;
    if (info.Length() == 1)
    {
        if (!info[0].IsBigInt())
        {
            Napi::TypeError::New(env, "Wrong argument").ThrowAsJavaScriptException();
            return Napi::Value();
        }
        else
        {
            bool lossless;
            mcycle_end = info[0].As<Napi::BigInt>().Uint64Value(&lossless);
        }
    }

    CM_BREAK_REASON break_reason;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(env, cm_machine_run(this->machine, mcycle_end, &break_reason, &err_msg), err_msg);
    return Napi::Number::New(env, break_reason);
}

Napi::Value Machine::RunUarch(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 1);

    uint64_t uarch_cycle_end = UINT64_MAX;
    if (info.Length() == 1)
    {
        if (!info[0].IsBigInt())
        {
            Napi::TypeError::New(env, "Wrong argument").ThrowAsJavaScriptException();
            return Napi::Value();
        }
        else
        {
            bool lossless;
            uarch_cycle_end = info[0].As<Napi::BigInt>().Uint64Value(&lossless);
        }
    }

    CM_UARCH_BREAK_REASON break_reason;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(env, cm_machine_run_uarch(this->machine, uarch_cycle_end, &break_reason, &err_msg), err_msg);
    return Napi::Number::New(env, break_reason);
}

Napi::Value Machine::ReadMemory(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 2, 2);

    if (!info[0].IsBigInt() || !info[1].IsBigInt())
    {
        Napi::TypeError::New(env, "Wrong argument").ThrowAsJavaScriptException();
        return Napi::Value();
    }

    bool lossless;
    uint64_t address = info[0].As<Napi::BigInt>().Int64Value(&lossless);
    uint64_t length = info[1].As<Napi::BigInt>().Int64Value(&lossless);
    Napi::ArrayBuffer buffer = Napi::ArrayBuffer::New(env, length);
    unsigned char *data = (unsigned char *)buffer.Data();
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(env, cm_read_memory(this->machine, address, data, length, &err_msg), err_msg);
    return buffer;
}

void Machine::WriteMemory(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 2, 2);

    if (!info[0].IsBigInt() || !info[1].IsArrayBuffer())
    {
        Napi::TypeError::New(env, "Wrong argument").ThrowAsJavaScriptException();
        return;
    }

    bool lossless;
    uint64_t address = info[0].As<Napi::BigInt>().Int64Value(&lossless);
    Napi::ArrayBuffer buffer = info[1].As<Napi::ArrayBuffer>();
    size_t length = buffer.ByteLength();
    const unsigned char *data = (unsigned char *)buffer.Data();
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(env, cm_write_memory(this->machine, address, data, length, &err_msg), err_msg);
    return;
}

Napi::Value Machine::ReadWord(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 1, 1);

    if (!info[0].IsBigInt())
    {
        Napi::TypeError::New(env, "Wrong argument").ThrowAsJavaScriptException();
        return Napi::Value();
    }

    bool lossless;
    uint64_t address = info[0].As<Napi::BigInt>().Int64Value(&lossless);
    Napi::ArrayBuffer buffer = Napi::ArrayBuffer::New(env, sizeof(uint64_t));
    uint64_t *data = (uint64_t *)buffer.Data();
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(env, cm_read_word(this->machine, address, data, &err_msg), err_msg);
    return buffer;
}

Napi::Value Machine::ReadVirtualMemory(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 2, 2);

    if (!info[0].IsBigInt() || !info[1].IsBigInt())
    {
        Napi::TypeError::New(env, "Wrong argument").ThrowAsJavaScriptException();
        return Napi::Value();
    }

    bool lossless;
    uint64_t address = info[0].As<Napi::BigInt>().Int64Value(&lossless);
    uint64_t length = info[1].As<Napi::BigInt>().Int64Value(&lossless);
    Napi::ArrayBuffer buffer = Napi::ArrayBuffer::New(env, length);
    unsigned char *data = (unsigned char *)buffer.Data();
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(env, cm_read_virtual_memory(this->machine, address, data, length, &err_msg), err_msg);
    return buffer;
}

void Machine::WriteVirtualMemory(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 2, 2);

    if (!info[0].IsBigInt() || !info[1].IsArrayBuffer())
    {
        Napi::TypeError::New(env, "Wrong argument").ThrowAsJavaScriptException();
        return;
    }

    bool lossless;
    uint64_t address = info[0].As<Napi::BigInt>().Int64Value(&lossless);
    Napi::ArrayBuffer buffer = info[1].As<Napi::ArrayBuffer>();
    size_t length = buffer.ByteLength();
    const unsigned char *data = (unsigned char *)buffer.Data();
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(env, cm_write_virtual_memory(this->machine, address, data, length, &err_msg), err_msg);
    return;
}

void Machine::Snapshot(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 0, 0);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(env, cm_snapshot(this->machine, &err_msg), err_msg);
    return;
}

void Machine::Rollback(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 0, 0);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(env, cm_rollback(this->machine, &err_msg), err_msg);
    return;
}

Napi::Value Machine::GetProof(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 2, 2);

    if (!info[0].IsBigInt() || !info[1].IsNumber())
    {
        Napi::TypeError::New(env, "Wrong argument").ThrowAsJavaScriptException();
        return Napi::Value();
    }

    bool lossless;
    uint64_t address = info[0].As<Napi::BigInt>().Int64Value(&lossless);
    uint64_t log2_size = info[1].ToNumber().Int64Value();
    cm_merkle_tree_proof *proof{};
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(env, cm_get_proof(this->machine, address, log2_size, &proof, &err_msg), err_msg);
    const Napi::Object object = MerkleTreeProof::New(env, *proof);
    cm_delete_merkle_tree_proof(proof);
    return object;
}

Napi::Value Machine::VerifyMerkleTree(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    bool result;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(env, cm_verify_merkle_tree(this->machine, &result, &err_msg), err_msg);
    return Napi::Boolean::New(env, result);
}

Napi::Value Machine::VerifyDirtyPageMaps(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    bool result;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(env, cm_verify_dirty_page_maps(this->machine, &result, &err_msg), err_msg);
    return Napi::Boolean::New(env, result);
}

void Machine::ReplaceMemoryRange(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 1, 1);
    THROW_IF_WRONG_TYPE_VOID(info, info[0].IsObject());
    MemoryRangeConfig object = MemoryRangeConfig(info.Env(), info[0].As<Napi::Object>());
    cm_memory_range_config config = {};
    object.Unwrap(config);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_replace_memory_range(this->machine, &config, &err_msg), err_msg);
}

Napi::Value Machine::ReadPc(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_pc(this->machine, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}

Napi::Value Machine::ReadFcsr(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_fcsr(this->machine, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}

Napi::Value Machine::ReadMcycle(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_mcycle(this->machine, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}

Napi::Value Machine::ReadIcycleinstret(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_icycleinstret(this->machine, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}

Napi::Value Machine::ReadMstatus(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_mstatus(this->machine, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}

Napi::Value Machine::ReadMenvcfg(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_menvcfg(this->machine, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}

Napi::Value Machine::ReadMtvec(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_mtvec(this->machine, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}

Napi::Value Machine::ReadMscratch(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_mscratch(this->machine, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}

Napi::Value Machine::ReadMepc(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_mepc(this->machine, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}

Napi::Value Machine::ReadMcause(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_mcause(this->machine, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}

Napi::Value Machine::ReadMtval(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_mtval(this->machine, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}

Napi::Value Machine::ReadMisa(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_misa(this->machine, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}

Napi::Value Machine::ReadMie(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_mie(this->machine, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}

Napi::Value Machine::ReadMip(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_mip(this->machine, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}

Napi::Value Machine::ReadMedeleg(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_medeleg(this->machine, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}

Napi::Value Machine::ReadMideleg(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_mideleg(this->machine, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}

Napi::Value Machine::ReadMcounteren(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_mcounteren(this->machine, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}

Napi::Value Machine::ReadStvec(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_stvec(this->machine, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}

Napi::Value Machine::ReadSscratch(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_sscratch(this->machine, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}

Napi::Value Machine::ReadSepc(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_sepc(this->machine, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}

Napi::Value Machine::ReadScause(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_scause(this->machine, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}

Napi::Value Machine::ReadStval(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_stval(this->machine, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}

Napi::Value Machine::ReadSatp(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_satp(this->machine, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}

Napi::Value Machine::ReadScounteren(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_scounteren(this->machine, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}

Napi::Value Machine::ReadSenvcfg(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_senvcfg(this->machine, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}

Napi::Value Machine::ReadIlrsc(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_ilrsc(this->machine, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}

Napi::Value Machine::ReadIflags(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_iflags(this->machine, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}

Napi::Value Machine::ReadHtifTohost(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_htif_tohost(this->machine, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}

Napi::Value Machine::ReadHtifFromhost(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_htif_fromhost(this->machine, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}

Napi::Value Machine::ReadHtifIhalt(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_htif_ihalt(this->machine, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}

Napi::Value Machine::ReadHtifIconsole(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_htif_iconsole(this->machine, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}

Napi::Value Machine::ReadHtifIyield(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_htif_iyield(this->machine, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}

Napi::Value Machine::ReadClintMtimecmp(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_clint_mtimecmp(this->machine, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}

Napi::Value Machine::ReadUarchPc(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_uarch_pc(this->machine, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}

Napi::Value Machine::ReadUarchCycle(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_uarch_cycle(this->machine, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}

Napi::Value Machine::ReadMvendorid(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_mvendorid(this->machine, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}

Napi::Value Machine::ReadMarchid(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_marchid(this->machine, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}

Napi::Value Machine::ReadMimpid(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_mimpid(this->machine, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}

Napi::Value Machine::ReadHtifTohostDev(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_htif_tohost_dev(this->machine, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}

Napi::Value Machine::ReadHtifTohostCmd(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_htif_tohost_cmd(this->machine, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}

Napi::Value Machine::ReadHtifTohostData(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_htif_tohost_data(this->machine, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}

void Machine::WritePc(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 1, 1);
    THROW_IF_WRONG_TYPE_VOID(info, info[0].IsBigInt());
    bool lossless;
    uint64_t value = info[0].As<Napi::BigInt>().Uint64Value(&lossless);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_write_pc(this->machine, value, &err_msg), err_msg);
}

void Machine::WriteFcsr(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 1, 1);
    THROW_IF_WRONG_TYPE_VOID(info, info[0].IsBigInt());
    bool lossless;
    uint64_t value = info[0].As<Napi::BigInt>().Uint64Value(&lossless);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_write_fcsr(this->machine, value, &err_msg), err_msg);
}

void Machine::WriteMcycle(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 1, 1);
    THROW_IF_WRONG_TYPE_VOID(info, info[0].IsBigInt());
    bool lossless;
    uint64_t value = info[0].As<Napi::BigInt>().Uint64Value(&lossless);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_write_mcycle(this->machine, value, &err_msg), err_msg);
}

void Machine::WriteIcycleinstret(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 1, 1);
    THROW_IF_WRONG_TYPE_VOID(info, info[0].IsBigInt());
    bool lossless;
    uint64_t value = info[0].As<Napi::BigInt>().Uint64Value(&lossless);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_write_icycleinstret(this->machine, value, &err_msg), err_msg);
}

void Machine::WriteMstatus(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 1, 1);
    THROW_IF_WRONG_TYPE_VOID(info, info[0].IsBigInt());
    bool lossless;
    uint64_t value = info[0].As<Napi::BigInt>().Uint64Value(&lossless);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_write_mstatus(this->machine, value, &err_msg), err_msg);
}

void Machine::WriteMenvcfg(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 1, 1);
    THROW_IF_WRONG_TYPE_VOID(info, info[0].IsBigInt());
    bool lossless;
    uint64_t value = info[0].As<Napi::BigInt>().Uint64Value(&lossless);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_write_menvcfg(this->machine, value, &err_msg), err_msg);
}

void Machine::WriteMtvec(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 1, 1);
    THROW_IF_WRONG_TYPE_VOID(info, info[0].IsBigInt());
    bool lossless;
    uint64_t value = info[0].As<Napi::BigInt>().Uint64Value(&lossless);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_write_mtvec(this->machine, value, &err_msg), err_msg);
}

void Machine::WriteMscratch(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 1, 1);
    THROW_IF_WRONG_TYPE_VOID(info, info[0].IsBigInt());
    bool lossless;
    uint64_t value = info[0].As<Napi::BigInt>().Uint64Value(&lossless);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_write_mscratch(this->machine, value, &err_msg), err_msg);
}

void Machine::WriteMepc(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 1, 1);
    THROW_IF_WRONG_TYPE_VOID(info, info[0].IsBigInt());
    bool lossless;
    uint64_t value = info[0].As<Napi::BigInt>().Uint64Value(&lossless);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_write_mepc(this->machine, value, &err_msg), err_msg);
}

void Machine::WriteMcause(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 1, 1);
    THROW_IF_WRONG_TYPE_VOID(info, info[0].IsBigInt());
    bool lossless;
    uint64_t value = info[0].As<Napi::BigInt>().Uint64Value(&lossless);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_write_mcause(this->machine, value, &err_msg), err_msg);
}

void Machine::WriteMtval(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 1, 1);
    THROW_IF_WRONG_TYPE_VOID(info, info[0].IsBigInt());
    bool lossless;
    uint64_t value = info[0].As<Napi::BigInt>().Uint64Value(&lossless);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_write_mtval(this->machine, value, &err_msg), err_msg);
}

void Machine::WriteMisa(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 1, 1);
    THROW_IF_WRONG_TYPE_VOID(info, info[0].IsBigInt());
    bool lossless;
    uint64_t value = info[0].As<Napi::BigInt>().Uint64Value(&lossless);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_write_misa(this->machine, value, &err_msg), err_msg);
}

void Machine::WriteMie(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 1, 1);
    THROW_IF_WRONG_TYPE_VOID(info, info[0].IsBigInt());
    bool lossless;
    uint64_t value = info[0].As<Napi::BigInt>().Uint64Value(&lossless);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_write_mie(this->machine, value, &err_msg), err_msg);
}

void Machine::WriteMip(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 1, 1);
    THROW_IF_WRONG_TYPE_VOID(info, info[0].IsBigInt());
    bool lossless;
    uint64_t value = info[0].As<Napi::BigInt>().Uint64Value(&lossless);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_write_mip(this->machine, value, &err_msg), err_msg);
}

void Machine::WriteMedeleg(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 1, 1);
    THROW_IF_WRONG_TYPE_VOID(info, info[0].IsBigInt());
    bool lossless;
    uint64_t value = info[0].As<Napi::BigInt>().Uint64Value(&lossless);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_write_medeleg(this->machine, value, &err_msg), err_msg);
}

void Machine::WriteMideleg(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 1, 1);
    THROW_IF_WRONG_TYPE_VOID(info, info[0].IsBigInt());
    bool lossless;
    uint64_t value = info[0].As<Napi::BigInt>().Uint64Value(&lossless);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_write_mideleg(this->machine, value, &err_msg), err_msg);
}

void Machine::WriteMcounteren(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 1, 1);
    THROW_IF_WRONG_TYPE_VOID(info, info[0].IsBigInt());
    bool lossless;
    uint64_t value = info[0].As<Napi::BigInt>().Uint64Value(&lossless);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_write_mcounteren(this->machine, value, &err_msg), err_msg);
}

void Machine::WriteStvec(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 1, 1);
    THROW_IF_WRONG_TYPE_VOID(info, info[0].IsBigInt());
    bool lossless;
    uint64_t value = info[0].As<Napi::BigInt>().Uint64Value(&lossless);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_write_stvec(this->machine, value, &err_msg), err_msg);
}

void Machine::WriteSscratch(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 1, 1);
    THROW_IF_WRONG_TYPE_VOID(info, info[0].IsBigInt());
    bool lossless;
    uint64_t value = info[0].As<Napi::BigInt>().Uint64Value(&lossless);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_write_sscratch(this->machine, value, &err_msg), err_msg);
}

void Machine::WriteSepc(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 1, 1);
    THROW_IF_WRONG_TYPE_VOID(info, info[0].IsBigInt());
    bool lossless;
    uint64_t value = info[0].As<Napi::BigInt>().Uint64Value(&lossless);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_write_sepc(this->machine, value, &err_msg), err_msg);
}

void Machine::WriteScause(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 1, 1);
    THROW_IF_WRONG_TYPE_VOID(info, info[0].IsBigInt());
    bool lossless;
    uint64_t value = info[0].As<Napi::BigInt>().Uint64Value(&lossless);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_write_scause(this->machine, value, &err_msg), err_msg);
}

void Machine::WriteStval(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 1, 1);
    THROW_IF_WRONG_TYPE_VOID(info, info[0].IsBigInt());
    bool lossless;
    uint64_t value = info[0].As<Napi::BigInt>().Uint64Value(&lossless);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_write_stval(this->machine, value, &err_msg), err_msg);
}

void Machine::WriteSatp(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 1, 1);
    THROW_IF_WRONG_TYPE_VOID(info, info[0].IsBigInt());
    bool lossless;
    uint64_t value = info[0].As<Napi::BigInt>().Uint64Value(&lossless);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_write_satp(this->machine, value, &err_msg), err_msg);
}

void Machine::WriteScounteren(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 1, 1);
    THROW_IF_WRONG_TYPE_VOID(info, info[0].IsBigInt());
    bool lossless;
    uint64_t value = info[0].As<Napi::BigInt>().Uint64Value(&lossless);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_write_scounteren(this->machine, value, &err_msg), err_msg);
}

void Machine::WriteSenvcfg(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 1, 1);
    THROW_IF_WRONG_TYPE_VOID(info, info[0].IsBigInt());
    bool lossless;
    uint64_t value = info[0].As<Napi::BigInt>().Uint64Value(&lossless);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_write_senvcfg(this->machine, value, &err_msg), err_msg);
}

void Machine::WriteIlrsc(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 1, 1);
    THROW_IF_WRONG_TYPE_VOID(info, info[0].IsBigInt());
    bool lossless;
    uint64_t value = info[0].As<Napi::BigInt>().Uint64Value(&lossless);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_write_ilrsc(this->machine, value, &err_msg), err_msg);
}

void Machine::WriteIflags(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 1, 1);
    THROW_IF_WRONG_TYPE_VOID(info, info[0].IsBigInt());
    bool lossless;
    uint64_t value = info[0].As<Napi::BigInt>().Uint64Value(&lossless);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_write_iflags(this->machine, value, &err_msg), err_msg);
}

void Machine::WriteHtifTohost(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 1, 1);
    THROW_IF_WRONG_TYPE_VOID(info, info[0].IsBigInt());
    bool lossless;
    uint64_t value = info[0].As<Napi::BigInt>().Uint64Value(&lossless);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_write_htif_tohost(this->machine, value, &err_msg), err_msg);
}

void Machine::WriteHtifFromhost(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 1, 1);
    THROW_IF_WRONG_TYPE_VOID(info, info[0].IsBigInt());
    bool lossless;
    uint64_t value = info[0].As<Napi::BigInt>().Uint64Value(&lossless);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_write_htif_fromhost(this->machine, value, &err_msg), err_msg);
}

void Machine::WriteHtifIhalt(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 1, 1);
    THROW_IF_WRONG_TYPE_VOID(info, info[0].IsBigInt());
    bool lossless;
    uint64_t value = info[0].As<Napi::BigInt>().Uint64Value(&lossless);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_write_htif_ihalt(this->machine, value, &err_msg), err_msg);
}

void Machine::WriteHtifIconsole(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 1, 1);
    THROW_IF_WRONG_TYPE_VOID(info, info[0].IsBigInt());
    bool lossless;
    uint64_t value = info[0].As<Napi::BigInt>().Uint64Value(&lossless);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_write_htif_iconsole(this->machine, value, &err_msg), err_msg);
}

void Machine::WriteHtifIyield(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 1, 1);
    THROW_IF_WRONG_TYPE_VOID(info, info[0].IsBigInt());
    bool lossless;
    uint64_t value = info[0].As<Napi::BigInt>().Uint64Value(&lossless);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_write_htif_iyield(this->machine, value, &err_msg), err_msg);
}

void Machine::WriteClintMtimecmp(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 1, 1);
    THROW_IF_WRONG_TYPE_VOID(info, info[0].IsBigInt());
    bool lossless;
    uint64_t value = info[0].As<Napi::BigInt>().Uint64Value(&lossless);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_write_clint_mtimecmp(this->machine, value, &err_msg), err_msg);
}

void Machine::WriteUarchPc(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 1, 1);
    THROW_IF_WRONG_TYPE_VOID(info, info[0].IsBigInt());
    bool lossless;
    uint64_t value = info[0].As<Napi::BigInt>().Uint64Value(&lossless);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_write_uarch_pc(this->machine, value, &err_msg), err_msg);
}

void Machine::WriteUarchCycle(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 1, 1);
    THROW_IF_WRONG_TYPE_VOID(info, info[0].IsBigInt());
    bool lossless;
    uint64_t value = info[0].As<Napi::BigInt>().Uint64Value(&lossless);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_write_uarch_cycle(this->machine, value, &err_msg), err_msg);
}

void Machine::WriteHtifFromhostData(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 1, 1);
    THROW_IF_WRONG_TYPE_VOID(info, info[0].IsBigInt());
    bool lossless;
    uint64_t value = info[0].As<Napi::BigInt>().Uint64Value(&lossless);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_write_htif_fromhost_data(this->machine, value, &err_msg), err_msg);
}

Napi::Value Machine::ReadIflagsX(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    bool result;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_iflags_X(this->machine, &result, &err_msg), err_msg);
    return Napi::Boolean::New(env, result);
}

Napi::Value Machine::ReadIflagsY(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    bool result;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_iflags_Y(this->machine, &result, &err_msg), err_msg);
    return Napi::Boolean::New(env, result);
}

Napi::Value Machine::ReadIflagsH(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    bool result;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_iflags_H(this->machine, &result, &err_msg), err_msg);
    return Napi::Boolean::New(env, result);
}

Napi::Value Machine::ReadUarchHaltFlag(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0, 0);
    bool result;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_uarch_halt_flag(this->machine, &result, &err_msg), err_msg);
    return Napi::Boolean::New(env, result);
}

void Machine::SetIflagsX(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 0, 0);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_set_iflags_X(this->machine, &err_msg), err_msg);
}

void Machine::SetIflagsY(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 0, 0);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_set_iflags_Y(this->machine, &err_msg), err_msg);
}

void Machine::SetIflagsH(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 0, 0);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_set_iflags_H(this->machine, &err_msg), err_msg);
}

void Machine::SetUarchHaltFlag(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 0, 0);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_set_uarch_halt_flag(this->machine, &err_msg), err_msg);
}

void Machine::ResetIflagsX(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 0, 0);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_reset_iflags_X(this->machine, &err_msg), err_msg);
}

void Machine::ResetIflagsY(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 0, 0);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_reset_iflags_Y(this->machine, &err_msg), err_msg);
}

void Machine::ResetUarchHaltFlag(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 0, 0);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_reset_uarch(this->machine, &err_msg), err_msg);
}

Napi::Value Machine::ReadX(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 1, 1);
    THROW_IF_WRONG_TYPE(info, info[0].IsNumber());
    int i = info[0].As<Napi::Number>().Int32Value();
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_x(this->machine, i, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}

Napi::Value Machine::ReadF(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 1, 1);
    THROW_IF_WRONG_TYPE(info, info[0].IsNumber());
    int i = info[0].As<Napi::Number>().Int32Value();
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_f(this->machine, i, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}

Napi::Value Machine::ReadUarchX(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 1, 1);
    THROW_IF_WRONG_TYPE(info, info[0].IsNumber());
    int i = info[0].As<Napi::Number>().Int32Value();
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_uarch_x(this->machine, i, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}

void Machine::WriteX(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 2, 2);
    THROW_IF_WRONG_TYPE_VOID(info, info[0].IsNumber());
    THROW_IF_WRONG_TYPE_VOID(info, info[1].IsBigInt());
    int i = info[0].As<Napi::Number>().Int32Value();
    bool lossless;
    uint64_t value = info[1].As<Napi::BigInt>().Uint64Value(&lossless);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_write_x(this->machine, i, value, &err_msg), err_msg);
}

void Machine::WriteF(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 2, 2);
    THROW_IF_WRONG_TYPE_VOID(info, info[0].IsNumber());
    THROW_IF_WRONG_TYPE_VOID(info, info[1].IsBigInt());
    int i = info[0].As<Napi::Number>().Int32Value();
    bool lossless;
    uint64_t value = info[1].As<Napi::BigInt>().Uint64Value(&lossless);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_write_f(this->machine, i, value, &err_msg), err_msg);
}

void Machine::WriteUarchX(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 2, 2);
    THROW_IF_WRONG_TYPE_VOID(info, info[0].IsNumber());
    THROW_IF_WRONG_TYPE_VOID(info, info[1].IsBigInt());
    int i = info[0].As<Napi::Number>().Int32Value();
    bool lossless;
    uint64_t value = info[1].As<Napi::BigInt>().Uint64Value(&lossless);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_write_uarch_x(this->machine, i, value, &err_msg), err_msg);
}

Napi::Value Machine::ReadCsr(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 1, 1);
    THROW_IF_WRONG_TYPE(info, info[0].IsNumber());
    CM_PROC_CSR r = static_cast<CM_PROC_CSR>(info[0].As<Napi::Number>().Int32Value());
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_csr(this->machine, r, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}

void Machine::WriteCsr(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 2, 2);
    THROW_IF_WRONG_TYPE_VOID(info, info[0].IsNumber());
    THROW_IF_WRONG_TYPE_VOID(info, info[1].IsBigInt());
    CM_PROC_CSR w = static_cast<CM_PROC_CSR>(info[0].As<Napi::Number>().Int32Value());
    bool lossless;
    uint64_t value = info[1].As<Napi::BigInt>().Uint64Value(&lossless);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_write_csr(this->machine, w, value, &err_msg), err_msg);
}
