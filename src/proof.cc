#include <napi.h>
#include "proof.h"

Napi::Uint8Array CreateHash(Napi::Env env, const cm_hash &hash)
{
    Napi::ArrayBuffer buffer = Napi::ArrayBuffer::New(env, sizeof(uint8_t) * CM_MACHINE_HASH_BYTE_SIZE);
    memcpy(buffer.Data(), &hash, sizeof(uint8_t) * CM_MACHINE_HASH_BYTE_SIZE);
    return Napi::Uint8Array::New(env, CM_MACHINE_HASH_BYTE_SIZE, buffer, 0);
}

Napi::Array CreateHashArray(Napi::Env env, const cm_hash_array &hash_array)
{
    Napi::Array array = Napi::Array::New(env, hash_array.count);
    for (size_t i = 0; i < hash_array.count; i++)
    {
        array.Set(i, CreateHash(env, hash_array.entry[i]));
    }
    return array;
}

MerkleTreeProof::MerkleTreeProof(napi_env env, napi_value value) : Napi::Object(env, value){};

MerkleTreeProof MerkleTreeProof::New(napi_env env, cm_merkle_tree_proof &proof)
{
    Napi::Object object = Napi::Object::New(env);
    object.Set("target_address", Napi::BigInt::New(env, proof.target_address));
    object.Set("log2_target_size", Napi::Number::New(env, proof.log2_target_size));
    object.Set("target_hash", CreateHash(env, proof.target_hash));
    object.Set("log2_root_size", Napi::Number::New(env, proof.log2_root_size));
    object.Set("root_hash", CreateHash(env, proof.root_hash));
    object.Set("sibling_hashes", CreateHashArray(env, proof.sibling_hashes));
    return MerkleTreeProof(env, object);
}
