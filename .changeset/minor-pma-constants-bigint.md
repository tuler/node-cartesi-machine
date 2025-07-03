---
"@tuler/node-cartesi-machine": minor

---

Changed: The `PmaConstant` enum has been replaced with a `const` object to allow the use of `bigint` for address values (`CmioRxBufferStart`, `CmioTxBufferStart`, and `RamStart`). Update any code referencing `PmaConstant` to use the new object and expect `bigint` values for these fields. 