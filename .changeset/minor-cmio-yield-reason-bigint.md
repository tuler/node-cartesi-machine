---
"@tuler/node-cartesi-machine": minor

---

Changed: The `CmioYieldReason` enum has been replaced with a `const` object to allow the use of `bigint` in all values. Update any code referencing `CmioYieldReason` to use the new object and expect `bigint` values for these fields. 