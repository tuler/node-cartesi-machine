# @tuler/node-cartesi-machine

## 0.2.0

### Minor Changes

- 797f2b2: The `spawn` function's `address` parameter is now optional and defaults to `"127.0.0.1:0"`. This makes it easier to spawn a remote machine server with sensible defaults.

  No breaking changes.

  **New Behavior:**

  - `spawn()` - Uses default address `"127.0.0.1:0"`
  - `spawn("127.0.0.1:5000")` - Uses specified address
  - `spawn(undefined, 5000)` - Uses default address with custom timeout

- 6683739: Add `MAX_MCYCLE` constant (0xffffffffffffffffffn) for use as the default maximum cycle count. The `mcycleEnd` parameter of `run` is now optional and defaults to `MAX_MCYCLE`.

## 0.1.0

### Minor Changes

- 06cfe86: binding for emulator 0.19.0 using FFI

## 0.0.8

### Patch Changes

- 251bb48: fix typings

## 0.0.7

### Patch Changes

- 89ad28d: fix typescript packaging

## 0.0.6

### Patch Changes

- 8c431e0: fix release version number
- 5918c10: fix npm package contents

## 0.0.5

### Patch Changes

- c508985: fix: add header files to npm package

## 0.0.4

### Patch Changes

- 915b22e: fix publishing

## 0.0.3

### Patch Changes

- 142914a: remove compilation warnings

## 0.0.2

### Patch Changes

- 4040226: release automation
- 28f9950: pre-compiled binaries
