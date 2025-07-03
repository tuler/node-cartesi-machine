---
"@tuler/node-cartesi-machine": minor
---

Add `MAX_MCYCLE` constant (0xffffffffffffffffffn) for use as the default maximum cycle count. The `mcycleEnd` parameter of `run` is now optional and defaults to `MAX_MCYCLE`.