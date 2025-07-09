import {
    BreakReason,
    CleanupCall,
    CmioYieldReason,
    spawn,
} from "@tuler/node-cartesi-machine";
import fs from "node:fs";
import { encodePacked, parseUnits } from "viem";
import { Sequencer } from "./util";

const templateHash =
    "0x615acc9fb8ae058d0e45c0d12fa10e1a6c9e645222c6fd94dfeda194ee427c14";
const snapshotUrl =
    "https://github.com/cartesi/honeypot/releases/download/v2.0.0/honeypot-snapshot-mainnet.tar.gz";
const appContract = "0x4c1E74EF88a75C24e49eddD9f70D82A94D19251c";
const erc20PortalAddress = "0xc700D6aDd016eECd59d989C028214Eaa0fCC0051";
const withdrawAddress = "0x60247492F1538Ed4520e61aE41ca2A8447592Ff5";
const tokenAddress = "0x491604c0fdf08347dd1fa4ee062a822a5dd06b5d";

let breakReason: BreakReason;

// check if file exists
const snapshotPath = `snapshots/${templateHash}`;

if (!fs.existsSync(snapshotPath)) {
    console.error(
        `snapshot not found at ${snapshotPath}, download from ${snapshotUrl}`,
    );
    process.exit(1);
}

// create an utility sequencer (create EvmAdvance inputs)
const sequencer = new Sequencer(1n, appContract);

// load honeypot mainnet machine
const machine = spawn().setCleanupCall(CleanupCall.Shutdown).load(snapshotPath);

// send deposit
machine.sendCmioResponse(
    CmioYieldReason.AdvanceState,
    sequencer.mine(
        erc20PortalAddress,
        encodePacked(
            ["address", "address", "uint256", "bytes"],
            [tokenAddress, withdrawAddress, parseUnits("1", 18), "0x"],
        ),
    ),
);
console.log(`Deposit`);
breakReason = machine.run();
while (breakReason !== BreakReason.YieldedManually) {
    console.log("\t", machine.receiveCmioRequest());
    breakReason = machine.run();
}
console.log(machine.receiveCmioRequest());

// send withdraw request
machine.sendCmioResponse(
    CmioYieldReason.AdvanceState,
    sequencer.mine(withdrawAddress, "0x"),
);
console.log(`Withdraw request`);
breakReason = machine.run();
while (breakReason !== BreakReason.YieldedManually) {
    console.log("\t", machine.receiveCmioRequest());
    breakReason = machine.run();
}
console.log(machine.receiveCmioRequest());

machine.shutdown();
