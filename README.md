# node-cartesi-machine

This is a JavaScript/TypeScript library for interacting with Cartesi Machines.

## Installation

```shell
npm install @tuler/node-cartesi-machine
# yarn add @tuler/node-cartesi-machine
# pnpm add @tuler/node-cartesi-machine
```

Download the kernel and rootfs used in the code examples below:

```shell
wget -O linux.bin https://github.com/cartesi/image-kernel/releases/download/v0.19.1/linux-6.5.9-ctsi-1-v0.19.1.bin
wget -O rootfs.ext2 https://github.com/cartesi/machine-emulator-tools/releases/download/v0.14.1/rootfs-tools-v0.14.1.ext2
```

## How to use

```typescript
import { create, load } from "@tuler/node-cartesi-machine";

// creating a machine
const bootargs =
    "no4lvl quiet earlycon=sbi console=hvc0 rootfstype=ext2 root=/dev/pmem0 rw init=/usr/sbin/cartesi-init";
const entrypoint = "uname -a; echo Hello from Cartesi Machine from Node.js";
const machine = create(
    {
        ram: {
            image_filename: "linux.bin",
            length: 128n * 1024n * 1024n,
        },
        dtb: { bootargs, entrypoint },
        flash_drive: [
            {
                image_filename: "rootfs.ext2",
                start: BigInt("0x80000000000000"),
                length: BigInt("0xffffffffffffffff"),
                shared: false,
            },
        ],
    },
    {},
);

// calculate machine root hash
const hash = Buffer.from(machine.get_root_hash()).toString("hex");

// execute the machine until it halts
machine.run();

// store machine snapshot
machine.store("./snapshot");

// load machine from snapshot
const machine2 = load("./snapshot");
```

## TypeScript

The library contains TypeScript type definitions.
Not the entire API is exposed though.

## Building

The binding is built in two layers.

The C++ layer uses [node-addon-api](https://github.com/nodejs/node-addon-api) and exposes 95% of the Cartesi Machine C API.

Then there is a TypeScript layer which exposes a limited and possibly more user friendly API.

## License

This binding code is license under Apache-2 license.

The machine-emulator repository and all contributions to it are licensed under the LGPL 3.0, unless otherwise specified below or in subdirectory LICENSE / COPYING files. Please review our COPYING file for the LGPL 3.0 license.

## Additional Notes

This project may include or link to other software components with different licenses. Contributors and users are responsible for ensuring compliance with each component's licensing terms. For detailed information, please refer to the individual LICENSE files within each directory or submodule, and for the Debian packages, please review the respective copyright and licensing details as mentioned above.
