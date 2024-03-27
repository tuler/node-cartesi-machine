import { toPascalCase } from "js-convert-case";

const readwrite = [
    "pc",
    "fcsr",
    "mcycle",
    "icycleinstret",
    "mstatus",
    "menvcfg",
    "mtvec",
    "mscratch",
    "mepc",
    "mcause",
    "mtval",
    "misa",
    "mie",
    "mip",
    "medeleg",
    "mideleg",
    "mcounteren",
    "stvec",
    "sscratch",
    "sepc",
    "scause",
    "stval",
    "satp",
    "scounteren",
    "senvcfg",
    "ilrsc",
    "iflags",
    "htif_tohost",
    "htif_fromhost",
    "htif_ihalt",
    "htif_iconsole",
    "htif_iyield",
    "clint_mtimecmp",
    "uarch_pc",
    "uarch_cycle",
];
const read = [
    "mvendorid",
    "marchid",
    "mimpid",
    "htif_tohost_dev",
    "htif_tohost_cmd",
    "htif_tohost_data",
];
const write = ["htif_fromhost_data"];
const readSetReset = ["iflags_X", "iflags_Y", "iflags_H", "uarch_halt_flag"];

// TODO
const i_rw = ["x", "f", "uarch_x"];

const instanceMethodRead = (name: string) =>
    `InstanceMethod<&Machine::Read${toPascalCase(name)}>("read_${name}"),`;
const instanceMethodWrite = (name: string) =>
    `InstanceMethod<&Machine::Write${toPascalCase(name)}>("write_${name}"),`;
const instanceMethodSet = (name: string) =>
    `InstanceMethod<&Machine::Set${toPascalCase(name)}>("set_${name}"),`;
const instanceMethodReset = (name: string) =>
    `InstanceMethod<&Machine::Reset${toPascalCase(name)}>("reset_${name}"),`;

const defRead = (name: string) =>
    `Napi::Value Read${toPascalCase(name)}(const Napi::CallbackInfo &info);`;
const defWrite = (name: string) =>
    `void Write${toPascalCase(name)}(const Napi::CallbackInfo &info);`;
const defSet = (name: string) =>
    `void Set${toPascalCase(name)}(const Napi::CallbackInfo &info);`;
const defReset = (name: string) =>
    `void Reset${toPascalCase(name)}(const Napi::CallbackInfo &info);`;

const implementationRead = (name: string) => `
Napi::Value Machine::Read${toPascalCase(name)}(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0);
    uint64_t value;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_${name}(this->machine, &value, &err_msg), err_msg);
    return Napi::BigInt::New(info.Env(), value);
}`;

const implementationWrite = (name: string) => `
void Machine::Write${toPascalCase(name)}(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 1);
    THROW_IF_WRONG_TYPE_VOID(info, info[0].IsBigInt());
    bool lossless;
    uint64_t value = info[0].As<Napi::BigInt>().Uint64Value(&lossless);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_write_${name}(this->machine, value, &err_msg), err_msg);
}`;

const implementationReadBool = (name: string) => `
Napi::Value Machine::Read${toPascalCase(name)}(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS(info, 0);
    bool result;
    char *err_msg = NULL;
    THROW_IF_CM_ERROR(info.Env(), cm_read_${name}(this->machine, &result, &err_msg), err_msg);
    return Napi::Boolean::New(env, result);
}`;

const implementationSetBool = (name: string) => `
void Machine::Set${toPascalCase(name)}(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 0);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_set_${name}(this->machine, &err_msg), err_msg);
}`;

const implementationResetBool = (name: string) => `
void Machine::Reset${toPascalCase(name)}(const Napi::CallbackInfo &info)
{
    THROW_IF_WRONG_NUMBER_OF_ARGUMENTS_VOID(info, 0);
    char *err_msg = NULL;
    THROW_IF_CM_ERROR_VOID(info.Env(), cm_reset_${name}(this->machine, &err_msg), err_msg);
}`;

console.log(readwrite.map(defRead).join("\n"));
console.log(read.map(defRead).join("\n"));
console.log(readwrite.map(defWrite).join("\n"));
console.log(write.map(defWrite).join("\n"));
console.log(readSetReset.map(defRead).join("\n"));
console.log(readSetReset.map(defSet).join("\n"));
console.log(readSetReset.map(defReset).join("\n"));

console.log(readwrite.map(instanceMethodRead).join("\n"));
console.log(read.map(instanceMethodRead).join("\n"));
console.log(readwrite.map(instanceMethodWrite).join("\n"));
console.log(write.map(instanceMethodWrite).join("\n"));
console.log(readSetReset.map(instanceMethodRead).join("\n"));
console.log(readSetReset.map(instanceMethodSet).join("\n"));
console.log(readSetReset.map(instanceMethodReset).join("\n"));

console.log(readwrite.map(implementationRead).join("\n"));
console.log(read.map(implementationRead).join("\n"));
console.log(readwrite.map(implementationWrite).join("\n"));
console.log(write.map(implementationWrite).join("\n"));
console.log(readSetReset.map(implementationReadBool).join("\n"));
console.log(readSetReset.map(implementationSetBool).join("\n"));
console.log(readSetReset.map(implementationResetBool).join("\n"));
