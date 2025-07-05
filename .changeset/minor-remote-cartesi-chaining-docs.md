---
"@tuler/node-cartesi-machine": minor

---

Improved method chaining for `RemoteCartesiMachine`: methods like `load`, `create`, `cloneEmpty`, and `store` now all return a `RemoteCartesiMachine` instance, enabling fluent chaining of these calls. 