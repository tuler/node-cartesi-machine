import { BreakReason, CmioYieldReason } from "./cartesi-machine";
import { spawn, type RemoteCartesiMachine } from "./remote-cartesi-machine";
import type { MachineRuntimeConfig } from "./types";

/**
 * Custom error class to signal the rollup has entered an invalid state
 */
export class RollupsFatalError extends Error {
    constructor(message: string) {
        super(message);
        this.name = "RollupsFatalError";
    }
}

/**
 * Custom error class to signal an input was rejected
 */
export class RollupsInputRejectedError extends Error {
    constructor() {
        super("Input rejected");
        this.name = "RollupsInputRejectedError";
    }
}

export interface RollupsMachine {
    advance(input: Buffer): { outputs: Buffer[]; reports: Buffer[] };
    inspect(query: Buffer): { reports: Buffer[] };
}

/**
 * Create a rollups machine from a remote machine.
 * @param machine - The remote machine.
 * @returns A rollups machine.
 */
function rollupsFromRemote(machine: RemoteCartesiMachine): RollupsMachine {
    return new RollupsMachineImpl(machine);
}

/**
 * Create a rollups machine from a store directory.
 * @param dir - The directory containing the store.
 * @param runtimeConfig - The runtime configuration.
 * @param address - The address of the remote machine.
 * @param timeout - The timeout for the remote machine.
 * @description This function spawns a new machine, and loads the stored snapshot on it.
 * @returns A rollups machine.
 */
function rollupsFromStore(
    dir: string,
    runtimeConfig?: MachineRuntimeConfig,
    address: string = "127.0.0.1:0",
    timeout: number = -1,
): RollupsMachine {
    const machine = spawn(address, timeout).load(dir, runtimeConfig);
    return rollupsFromRemote(machine);
}

/**
 * Create a rollups machine from a remote machine.
 * @param machine - The remote machine.
 * @returns A rollups machine.
 */
export function rollups(machine: RemoteCartesiMachine): RollupsMachine;

/**
 * Create a rollups machine from a store directory.
 * @param dir - The directory containing the store.
 * @param runtimeConfig - The runtime configuration.
 * @param address - The address of the remote machine.
 * @param timeout - The timeout for the remote machine.
 * @description This function spawns a new machine, and loads the stored snapshot on it.
 * @returns A rollups machine.
 */
export function rollups(
    dir: string,
    runtimeConfig?: MachineRuntimeConfig,
    address?: string,
    timeout?: number,
): RollupsMachine;

export function rollups(
    arg1: RemoteCartesiMachine | string,
    runtimeConfig?: MachineRuntimeConfig,
    address: string = "127.0.0.1:0",
    timeout: number = -1,
): RollupsMachine {
    if (typeof arg1 === "string") {
        return rollupsFromStore(arg1, runtimeConfig, address, timeout);
    } else {
        return rollupsFromRemote(arg1);
    }
}

class RollupsMachineImpl implements RollupsMachine {
    private machine: RemoteCartesiMachine;

    constructor(machine: RemoteCartesiMachine) {
        this.machine = machine;
    }

    advance(input: Buffer): { outputs: Buffer[]; reports: Buffer[] } {
        // start by creating a backup fork, in case input gets rejected
        const fork = this.machine.fork();

        // write input
        this.machine.sendCmioResponse(CmioYieldReason.AdvanceState, input);

        const outputs: Buffer[] = [];
        const reports: Buffer[] = [];

        while (true) {
            // run machine until it yields
            const breakReason = this.machine.run();

            if (breakReason === BreakReason.YieldedManually) {
                const { reason, data } = this.machine.receiveCmioRequest();
                if (reason === CmioYieldReason.ManualRxAccepted) {
                    // input was accepted, shutdown the backup fork
                    fork.shutdown();

                    // data should have root hash
                    return { outputs, reports };
                }
                if (reason === CmioYieldReason.ManualRxRejected) {
                    // input was rejected, shutdown the machine and replace by the fork
                    this.machine.shutdown();
                    this.machine = fork;

                    throw new RollupsInputRejectedError();
                }
                if (reason === CmioYieldReason.ManualTxException) {
                    // exception, shutdown the current machine, and replace by the preserved fork
                    // XXX: is this correct?
                    this.machine.shutdown();
                    this.machine = fork;

                    const description = data.toString("utf-8"); // XXX: is this correct?
                    throw new RollupsFatalError(description);
                }
                throw new RollupsFatalError(
                    `Unexpected yield reason: ${reason}`,
                );
            }

            if (breakReason === BreakReason.YieldedAutomatically) {
                const { reason, data } = this.machine.receiveCmioRequest();
                if (reason === CmioYieldReason.AutomaticTxOutput) {
                    // collect output
                    outputs.push(data);
                } else if (reason === CmioYieldReason.AutomaticTxReport) {
                    // collect report
                    reports.push(data);
                }
                continue;
            }

            throw new RollupsFatalError(
                `Unexpected break reason: ${breakReason}`,
            );
        }
    }

    inspect(query: Buffer): { reports: Buffer[] } {
        // start by creating a backup fork, to preserve the state of the machine
        const fork = this.machine.fork();

        // write query
        this.machine.sendCmioResponse(CmioYieldReason.InspectState, query);

        const reports: Buffer[] = [];

        while (true) {
            // run machine until it yields
            const breakReason = this.machine.run();

            if (breakReason === BreakReason.YieldedManually) {
                const { reason, data } = this.machine.receiveCmioRequest();
                if (reason === CmioYieldReason.ManualRxAccepted) {
                    // input was accepted, shutdown the backup fork
                    fork.shutdown();

                    // data should have root hash
                    return { reports };
                }
                if (reason === CmioYieldReason.ManualRxRejected) {
                    // input was rejected, shutdown the machine and replace by the fork
                    this.machine.shutdown();
                    this.machine = fork;
                    throw new RollupsInputRejectedError();
                }
                if (reason === CmioYieldReason.ManualTxException) {
                    // exception, shutdown the current machine, and replace by the preserved fork
                    // XXX: is this correct?
                    this.machine.shutdown();
                    this.machine = fork;

                    const description = data.toString("utf-8"); // XXX: is this correct?
                    throw new RollupsFatalError(description);
                }
                throw new RollupsFatalError(
                    `Unexpected yield reason: ${reason}`,
                );
            }

            if (breakReason === BreakReason.YieldedAutomatically) {
                const { reason, data } = this.machine.receiveCmioRequest();
                if (reason === CmioYieldReason.AutomaticTxOutput) {
                    // ignore output
                } else if (reason === CmioYieldReason.AutomaticTxReport) {
                    // collect report
                    reports.push(data);
                }
                continue;
            }

            throw new RollupsFatalError(
                `Unexpected break reason: ${breakReason}`,
            );
        }
    }
}
