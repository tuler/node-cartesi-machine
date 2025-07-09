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

/**
 * Advance requests can yield either an output or a report, indicated by the type field.
 * The data field contains the output or report data.
 */
export type AdvanceYield =
    | { type: "output"; data: Buffer }
    | { type: "report"; data: Buffer }
    | { type: "progress"; data: number };

export interface RollupsMachine {
    advance(input: Buffer): IterableIterator<AdvanceYield>;
    inspect(query: Buffer): IterableIterator<Buffer>;
    shutdown(): void;
    store(dir: string): RollupsMachine;
}

/**
 * Create a rollups machine from a remote machine.
 * @param machine - The remote machine.
 * @returns A rollups machine.
 */
function rollupsFromRemote(
    machine: RemoteCartesiMachine,
    options?: { noRollback?: boolean },
): RollupsMachine {
    return new RollupsMachineImpl(machine, options?.noRollback ?? false);
}

const DEFAULT_ADDRESS = "127.0.0.1:0";
const DEFAULT_TIMEOUT = -1;

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
    options?: {
        noRollback?: boolean;
        runtimeConfig?: MachineRuntimeConfig;
        address?: string;
        timeout?: number;
    },
): RollupsMachine {
    const { runtimeConfig } = options ?? {
        address: DEFAULT_ADDRESS,
        timeout: DEFAULT_TIMEOUT,
        runtimeConfig: undefined,
    };
    const address = options?.address ?? DEFAULT_ADDRESS;
    const timeout = options?.timeout ?? DEFAULT_TIMEOUT;
    const machine = spawn(address, timeout).load(dir, runtimeConfig);
    return rollupsFromRemote(machine, options);
}

/**
 * Create a rollups machine from a remote machine.
 * @param machine - The remote machine.
 * @returns A rollups machine.
 */
export function rollups(
    machine: RemoteCartesiMachine,
    options?: { noRollback: boolean },
): RollupsMachine;

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
    options?: {
        noRollback?: boolean;
        runtimeConfig?: MachineRuntimeConfig;
        address?: string;
        timeout?: number;
    },
): RollupsMachine;

export function rollups(
    arg1: RemoteCartesiMachine | string,
    options?: {
        noRollback?: boolean;
        runtimeConfig?: MachineRuntimeConfig;
        address?: string;
        timeout?: number;
    },
): RollupsMachine {
    options = options ?? {
        noRollback: false,
        address: DEFAULT_ADDRESS,
        timeout: DEFAULT_TIMEOUT,
    };

    if (typeof arg1 === "string") {
        return rollupsFromStore(arg1, options);
    } else {
        return rollupsFromRemote(arg1, options);
    }
}

class RollupsMachineImpl implements RollupsMachine {
    private machine: RemoteCartesiMachine;
    private noRollback: boolean;

    constructor(machine: RemoteCartesiMachine, noRollback: boolean = false) {
        this.machine = machine;
        this.noRollback = noRollback;
    }

    *advance(input: Buffer): IterableIterator<AdvanceYield> {
        // start by creating a backup fork, in case input gets rejected
        const fork = this.noRollback ? undefined : this.machine.fork();

        // write input
        this.machine.sendCmioResponse(CmioYieldReason.AdvanceState, input);

        while (true) {
            // run machine until it yields or halts
            const breakReason = this.machine.run();

            switch (breakReason) {
                case BreakReason.YieldedManually: {
                    const { reason, data } = this.machine.receiveCmioRequest();
                    switch (reason) {
                        case CmioYieldReason.ManualRxAccepted: {
                            // input was accepted
                            // shutdown the backup fork if it exists
                            fork?.shutdown();
                            return;
                        }
                        case CmioYieldReason.ManualRxRejected: {
                            // input was rejected
                            if (fork) {
                                // shutdown the machine and replace by the fork
                                this.machine.shutdown();
                                this.machine = fork;
                            }

                            throw new RollupsInputRejectedError();
                        }
                        case CmioYieldReason.ManualTxException: {
                            // exception
                            if (fork) {
                                // shutdown the current machine, and replace by the preserved fork
                                this.machine.shutdown();
                                this.machine = fork;
                            }

                            const description = data.toString("utf-8"); // XXX: is this correct?
                            throw new RollupsFatalError(description);
                        }
                        default: {
                            throw new RollupsFatalError(
                                `Unexpected yield reason: ${reason}`,
                            );
                        }
                    }
                }
                case BreakReason.YieldedAutomatically: {
                    const { reason, data } = this.machine.receiveCmioRequest();
                    switch (reason) {
                        case CmioYieldReason.AutomaticProgress: {
                            try {
                                const progress = data.readUInt32LE();
                                yield {
                                    type: "progress",
                                    data: progress,
                                };
                            } catch {
                                // just ignore the progress in case cannot read it
                            }
                            break;
                        }
                        case CmioYieldReason.AutomaticTxOutput: {
                            yield { type: "output", data };
                            break;
                        }
                        case CmioYieldReason.AutomaticTxReport: {
                            yield { type: "report", data };
                            break;
                        }
                    }
                    continue; // run again
                }
                default: {
                    throw new RollupsFatalError(
                        `Unexpected break reason: ${breakReason}`,
                    );
                }
            }
        }
    }

    *inspect(query: Buffer): IterableIterator<Buffer> {
        // depending on the noRollback flag, we either use the current machine or a fork
        // default behavior is to create a fork, to preserve the state of the machine
        const machine = this.noRollback ? this.machine : this.machine.fork();

        // write query
        machine.sendCmioResponse(CmioYieldReason.InspectState, query);

        while (true) {
            // run machine until it yields or halts
            const breakReason = machine.run();

            switch (breakReason) {
                case BreakReason.YieldedManually: {
                    const { reason, data } = machine.receiveCmioRequest();
                    switch (reason) {
                        case CmioYieldReason.ManualRxAccepted: {
                            // input was accepted
                            if (!this.noRollback) {
                                // shutdown the fork and return gracefully
                                machine.shutdown();
                            }
                            return;
                        }
                        case CmioYieldReason.ManualRxRejected: {
                            // input was rejected
                            if (!this.noRollback) {
                                // shutdown the fork and throw
                                machine.shutdown();
                            }
                            throw new RollupsInputRejectedError();
                        }
                        case CmioYieldReason.ManualTxException: {
                            // exception
                            if (!this.noRollback) {
                                // shutdown the fork, and throw
                                machine.shutdown();
                            }

                            const description = data.toString("utf-8"); // XXX: is this correct?
                            throw new RollupsFatalError(description);
                        }
                        default: {
                            throw new RollupsFatalError(
                                `Unexpected yield reason: ${reason}`,
                            );
                        }
                    }
                }
                case BreakReason.YieldedAutomatically: {
                    const { reason, data } = machine.receiveCmioRequest();
                    switch (reason) {
                        case CmioYieldReason.AutomaticProgress: {
                            // ignore progress
                            break;
                        }
                        case CmioYieldReason.AutomaticTxOutput: {
                            // ignore output
                            break;
                        }
                        case CmioYieldReason.AutomaticTxReport: {
                            // yield report
                            yield data;
                            break;
                        }
                    }
                    continue; // run again
                }
                default: {
                    throw new RollupsFatalError(
                        `Unexpected break reason: ${breakReason}`,
                    );
                }
            }
        }
    }

    shutdown(): void {
        this.machine.shutdown();
    }

    store(dir: string): RollupsMachine {
        this.machine.store(dir);
        return this;
    }
}
