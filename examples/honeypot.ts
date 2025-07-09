import {
    BreakReason,
    CleanupCall,
    CmioYieldReason,
    spawn,
} from "@tuler/node-cartesi-machine";
import fs from "node:fs";
import { encodeFunctionData, encodePacked, hexToBytes, parseUnits } from "viem";

const InputsAbi = [
    {
        type: "function",
        name: "EvmAdvance",
        inputs: [
            { name: "chainId", type: "uint256", internalType: "uint256" },
            { name: "appContract", type: "address", internalType: "address" },
            { name: "msgSender", type: "address", internalType: "address" },
            { name: "blockNumber", type: "uint256", internalType: "uint256" },
            {
                name: "blockTimestamp",
                type: "uint256",
                internalType: "uint256",
            },
            { name: "prevRandao", type: "uint256", internalType: "uint256" },
            { name: "index", type: "uint256", internalType: "uint256" },
            { name: "payload", type: "bytes", internalType: "bytes" },
        ],
        outputs: [],
        stateMutability: "nonpayable",
    },
] as const;

const chainId = 1n;
const templateHash =
    "0x615acc9fb8ae058d0e45c0d12fa10e1a6c9e645222c6fd94dfeda194ee427c14";
const snapshotUrl =
    "https://github.com/cartesi/honeypot/releases/download/v2.0.0/honeypot-snapshot-mainnet.tar.gz";
const appContract = "0x4c1E74EF88a75C24e49eddD9f70D82A94D19251c";
const erc20PortalAddress = "0xc700D6aDd016eECd59d989C028214Eaa0fCC0051";
const withdrawAddress = "0x60247492F1538Ed4520e61aE41ca2A8447592Ff5";
const tokenAddress = "0x491604c0fdf08347dd1fa4ee062a822a5dd06b5d";

let blockNumber = 1n;
let index = 0n;
let breakReason: BreakReason;

// now in seconds as bigint
const now = (): bigint => BigInt(Math.floor(Date.now() / 1000));

// check if file exists
const snapshotPath = `snapshots/${templateHash}`;

if (!fs.existsSync(snapshotPath)) {
    console.error(
        `snapshot not found at ${snapshotPath}, download from ${snapshotUrl}`,
    );
    process.exit(1);
}

// load honeypot mainnet machine
const machine = spawn().setCleanupCall(CleanupCall.Shutdown).load(snapshotPath);

// send deposit
machine.sendCmioResponse(
    CmioYieldReason.AdvanceState,
    Buffer.from(
        hexToBytes(
            encodeFunctionData({
                abi: InputsAbi,
                functionName: "EvmAdvance",
                args: [
                    chainId,
                    appContract,
                    erc20PortalAddress,
                    blockNumber++,
                    now(),
                    0n, // prevRandao (not used)
                    index++,
                    encodePacked(
                        ["address", "address", "uint256", "bytes"],
                        [
                            tokenAddress,
                            withdrawAddress,
                            parseUnits("1", 18),
                            "0x",
                        ],
                    ),
                ],
            }),
        ),
    ),
);
console.log(`${index}. deposit`);
breakReason = machine.run();
while (breakReason !== BreakReason.YieldedManually) {
    console.log("\t", machine.receiveCmioRequest());
    breakReason = machine.run();
}
console.log(machine.receiveCmioRequest());

// send withdraw request
machine.sendCmioResponse(
    CmioYieldReason.AdvanceState,
    Buffer.from(
        hexToBytes(
            encodeFunctionData({
                abi: InputsAbi,
                functionName: "EvmAdvance",
                args: [
                    chainId,
                    appContract,
                    withdrawAddress,
                    blockNumber++,
                    now(),
                    0n, // prevRandao (not used)
                    index++,
                    "0x",
                ],
            }),
        ),
    ),
);
console.log(`${index}. withdraw request`);
breakReason = machine.run();
while (breakReason !== BreakReason.YieldedManually) {
    console.log("\t", machine.receiveCmioRequest());
    breakReason = machine.run();
}
console.log(machine.receiveCmioRequest());

machine.shutdown();
