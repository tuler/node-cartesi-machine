#ifndef CNODE_MACHINE_H
#define CNODE_MACHINE_H

#include <napi.h>
#include "machine-c-api.h"

class Machine : public Napi::ObjectWrap<Machine>
{
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    Machine(const Napi::CallbackInfo &info);
    void Finalize(Napi::Env env);

private:
    Napi::Value GetRootHash(const Napi::CallbackInfo &info);
    void Store(const Napi::CallbackInfo &info);
    Napi::Value Run(const Napi::CallbackInfo &info);
    Napi::Value RunUarch(const Napi::CallbackInfo &info);
    Napi::Value ReadMemory(const Napi::CallbackInfo &info);
    void WriteMemory(const Napi::CallbackInfo &info);
    Napi::Value ReadWord(const Napi::CallbackInfo &info);
    Napi::Value ReadVirtualMemory(const Napi::CallbackInfo &info);
    void WriteVirtualMemory(const Napi::CallbackInfo &info);
    void Snapshot(const Napi::CallbackInfo &info);
    void Rollback(const Napi::CallbackInfo &info);
    Napi::Value GetProof(const Napi::CallbackInfo &info);
    Napi::Value VerifyMerkleTree(const Napi::CallbackInfo &info);
    Napi::Value VerifyDirtyPageMaps(const Napi::CallbackInfo &info);
    void ReplaceMemoryRange(const Napi::CallbackInfo &info);

    Napi::Value ReadPc(const Napi::CallbackInfo &info);
    Napi::Value ReadFcsr(const Napi::CallbackInfo &info);
    Napi::Value ReadMcycle(const Napi::CallbackInfo &info);
    Napi::Value ReadIcycleinstret(const Napi::CallbackInfo &info);
    Napi::Value ReadMstatus(const Napi::CallbackInfo &info);
    Napi::Value ReadMenvcfg(const Napi::CallbackInfo &info);
    Napi::Value ReadMtvec(const Napi::CallbackInfo &info);
    Napi::Value ReadMscratch(const Napi::CallbackInfo &info);
    Napi::Value ReadMepc(const Napi::CallbackInfo &info);
    Napi::Value ReadMcause(const Napi::CallbackInfo &info);
    Napi::Value ReadMtval(const Napi::CallbackInfo &info);
    Napi::Value ReadMisa(const Napi::CallbackInfo &info);
    Napi::Value ReadMie(const Napi::CallbackInfo &info);
    Napi::Value ReadMip(const Napi::CallbackInfo &info);
    Napi::Value ReadMedeleg(const Napi::CallbackInfo &info);
    Napi::Value ReadMideleg(const Napi::CallbackInfo &info);
    Napi::Value ReadMcounteren(const Napi::CallbackInfo &info);
    Napi::Value ReadStvec(const Napi::CallbackInfo &info);
    Napi::Value ReadSscratch(const Napi::CallbackInfo &info);
    Napi::Value ReadSepc(const Napi::CallbackInfo &info);
    Napi::Value ReadScause(const Napi::CallbackInfo &info);
    Napi::Value ReadStval(const Napi::CallbackInfo &info);
    Napi::Value ReadSatp(const Napi::CallbackInfo &info);
    Napi::Value ReadScounteren(const Napi::CallbackInfo &info);
    Napi::Value ReadSenvcfg(const Napi::CallbackInfo &info);
    Napi::Value ReadIlrsc(const Napi::CallbackInfo &info);
    Napi::Value ReadIflags(const Napi::CallbackInfo &info);
    Napi::Value ReadHtifTohost(const Napi::CallbackInfo &info);
    Napi::Value ReadHtifFromhost(const Napi::CallbackInfo &info);
    Napi::Value ReadHtifIhalt(const Napi::CallbackInfo &info);
    Napi::Value ReadHtifIconsole(const Napi::CallbackInfo &info);
    Napi::Value ReadHtifIyield(const Napi::CallbackInfo &info);
    Napi::Value ReadClintMtimecmp(const Napi::CallbackInfo &info);
    Napi::Value ReadUarchPc(const Napi::CallbackInfo &info);
    Napi::Value ReadUarchCycle(const Napi::CallbackInfo &info);
    Napi::Value ReadMvendorid(const Napi::CallbackInfo &info);
    Napi::Value ReadMarchid(const Napi::CallbackInfo &info);
    Napi::Value ReadMimpid(const Napi::CallbackInfo &info);
    Napi::Value ReadHtifTohostDev(const Napi::CallbackInfo &info);
    Napi::Value ReadHtifTohostCmd(const Napi::CallbackInfo &info);
    Napi::Value ReadHtifTohostData(const Napi::CallbackInfo &info);
    void WritePc(const Napi::CallbackInfo &info);
    void WriteFcsr(const Napi::CallbackInfo &info);
    void WriteMcycle(const Napi::CallbackInfo &info);
    void WriteIcycleinstret(const Napi::CallbackInfo &info);
    void WriteMstatus(const Napi::CallbackInfo &info);
    void WriteMenvcfg(const Napi::CallbackInfo &info);
    void WriteMtvec(const Napi::CallbackInfo &info);
    void WriteMscratch(const Napi::CallbackInfo &info);
    void WriteMepc(const Napi::CallbackInfo &info);
    void WriteMcause(const Napi::CallbackInfo &info);
    void WriteMtval(const Napi::CallbackInfo &info);
    void WriteMisa(const Napi::CallbackInfo &info);
    void WriteMie(const Napi::CallbackInfo &info);
    void WriteMip(const Napi::CallbackInfo &info);
    void WriteMedeleg(const Napi::CallbackInfo &info);
    void WriteMideleg(const Napi::CallbackInfo &info);
    void WriteMcounteren(const Napi::CallbackInfo &info);
    void WriteStvec(const Napi::CallbackInfo &info);
    void WriteSscratch(const Napi::CallbackInfo &info);
    void WriteSepc(const Napi::CallbackInfo &info);
    void WriteScause(const Napi::CallbackInfo &info);
    void WriteStval(const Napi::CallbackInfo &info);
    void WriteSatp(const Napi::CallbackInfo &info);
    void WriteScounteren(const Napi::CallbackInfo &info);
    void WriteSenvcfg(const Napi::CallbackInfo &info);
    void WriteIlrsc(const Napi::CallbackInfo &info);
    void WriteIflags(const Napi::CallbackInfo &info);
    void WriteHtifTohost(const Napi::CallbackInfo &info);
    void WriteHtifFromhost(const Napi::CallbackInfo &info);
    void WriteHtifIhalt(const Napi::CallbackInfo &info);
    void WriteHtifIconsole(const Napi::CallbackInfo &info);
    void WriteHtifIyield(const Napi::CallbackInfo &info);
    void WriteClintMtimecmp(const Napi::CallbackInfo &info);
    void WriteUarchPc(const Napi::CallbackInfo &info);
    void WriteUarchCycle(const Napi::CallbackInfo &info);
    void WriteHtifFromhostData(const Napi::CallbackInfo &info);
    Napi::Value ReadIflagsX(const Napi::CallbackInfo &info);
    Napi::Value ReadIflagsY(const Napi::CallbackInfo &info);
    Napi::Value ReadIflagsH(const Napi::CallbackInfo &info);
    Napi::Value ReadUarchHaltFlag(const Napi::CallbackInfo &info);
    void SetIflagsX(const Napi::CallbackInfo &info);
    void SetIflagsY(const Napi::CallbackInfo &info);
    void SetIflagsH(const Napi::CallbackInfo &info);
    void SetUarchHaltFlag(const Napi::CallbackInfo &info);
    void ResetIflagsX(const Napi::CallbackInfo &info);
    void ResetIflagsY(const Napi::CallbackInfo &info);
    void ResetUarchHaltFlag(const Napi::CallbackInfo &info);

    Napi::Value ReadX(const Napi::CallbackInfo &info);
    Napi::Value ReadF(const Napi::CallbackInfo &info);
    Napi::Value ReadUarchX(const Napi::CallbackInfo &info);
    void WriteX(const Napi::CallbackInfo &info);
    void WriteF(const Napi::CallbackInfo &info);
    void WriteUarchX(const Napi::CallbackInfo &info);

    Napi::Value ReadCsr(const Napi::CallbackInfo &info);
    void WriteCsr(const Napi::CallbackInfo &info);

    cm_machine *machine;
};

/*
missing API

CM_API int cm_log_uarch_step(cm_machine *m, cm_access_log_type log_type, bool one_based, cm_access_log **access_log, char **err_msg);
CM_API int cm_verify_uarch_step_log(const cm_access_log *log, const cm_machine_runtime_config *runtime_config, bool one_based, char **err_msg);
CM_API int cm_verify_uarch_step_state_transition(const cm_hash *root_hash_before, const cm_access_log *log, const cm_hash *root_hash_after, const cm_machine_runtime_config *runtime_config, bool one_based, char **err_msg);
CM_API int cm_verify_uarch_reset_state_transition(const cm_hash *root_hash_before, const cm_access_log *log, const cm_hash *root_hash_after, const cm_machine_runtime_config *runtime_config, bool one_based, char **err_msg);
CM_API int cm_verify_uarch_reset_log(const cm_access_log *log, const cm_machine_runtime_config *runtime_config, bool one_based, char **err_msg);
CM_API int cm_log_uarch_reset(cm_machine *m, cm_access_log_type log_type, bool one_based, cm_access_log **access_log, char **err_msg);

CM_API uint64_t cm_get_csr_address(CM_PROC_CSR w);
CM_API uint64_t cm_get_x_address(int i);
CM_API uint64_t cm_get_uarch_x_address(int i);
CM_API uint64_t cm_get_f_address(int i);
CM_API uint64_t cm_packed_iflags(int PRV, int X, int Y, int H);

CM_API void cm_delete_memory_range_config(const cm_memory_range_config *config);
CM_API void cm_delete_machine_runtime_config(const cm_machine_runtime_config *config);
CM_API void cm_delete_uarch_ram_config(const cm_uarch_ram_config *config);
CM_API void cm_delete_uarch_config(const cm_uarch_config *config);
CM_API void cm_delete_semantic_version(const cm_semantic_version *version);

CM_API int cm_get_initial_config(const cm_machine *m, const cm_machine_config **config, char **err_msg);
CM_API int cm_get_default_config(const cm_machine_config **config, char **err_msg);
CM_API int cm_get_memory_ranges(cm_machine *m, cm_memory_range_descr_array **mrda, char **err_msg);
*/

#endif
