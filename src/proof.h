#ifndef CNODE_MACHINE_PROOF_H
#define CNODE_MACHINE_PROOF_H

#include <napi.h>
#include "machine-c-api.h"

class MerkleTreeProof : public Napi::Object
{
public:
    static MerkleTreeProof New(napi_env env, cm_merkle_tree_proof &proof);
    MerkleTreeProof(napi_env env, napi_value value);
};

#endif
