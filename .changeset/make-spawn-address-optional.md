---
"@tuler/node-cartesi-machine": minor
---

The `spawn` function's `address` parameter is now optional and defaults to `"127.0.0.1:0"`. This makes it easier to spawn a remote machine server with sensible defaults.

No breaking changes.

**New Behavior:**
- `spawn()` - Uses default address `"127.0.0.1:0"`
- `spawn("127.0.0.1:5000")` - Uses specified address
- `spawn(undefined, 5000)` - Uses default address with custom timeout
