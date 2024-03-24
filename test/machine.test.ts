import fs from "fs";
import path from "path";
import { describe, expect, it } from "vitest";
import { create, load } from "../lib/binding";

describe("cartesi-machine", () => {
    it("create-store-load", () => {
        const bootargs =
            "no4lvl quiet earlycon=sbi console=hvc0 rootfstype=ext2 root=/dev/pmem0 rw init=/usr/sbin/cartesi-init";
        const entrypoint =
            "uname -a; echo Hello from Cartesi Machine from Node.js";
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

        const hash = Buffer.from(machine.get_root_hash()).toString("hex");
        expect(hash).toEqual(
            "84cd4796ca9c3907375b1dc49676537d896479f8b3568d5b8b4143e7c0d71b0a",
        );

        // set tup store directory
        const storeDir = path.join("test", "snapshot");
        // fs.rmSync(storeDir, { recursive: true, force: true });

        // store snapshot
        try {
            machine.store(storeDir);
        } catch (e) {
            console.log("Store Error", e, typeof e, e instanceof Error);
        }

        // execute machine
        // const breakReason = machine.run();
        const data = machine.read_memory(BigInt("0x80000000000000"), 1024n);
        console.log("data", data);

        // load snapshot
        const loadedMachine = load(storeDir, {});
        const loadedHash = Buffer.from(loadedMachine.get_root_hash()).toString(
            "hex",
        );

        expect(hash).toEqual(loadedHash);
    });
});
