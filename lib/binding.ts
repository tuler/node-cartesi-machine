var addon = require("bindings")("binding");

export type ProcessorConfig = {
    x?: number[];
    f?: number[];
    pc?: number;
    fcsr?: number;
    mvendorid?: number;
    marchid?: number;
    mimpid?: number;
    mcycle?: number;
    icycleinstret?: number;
    mstatus?: number;
    mtvec?: number;
    mscratch?: number;
    mepc?: number;
    mcause?: number;
    mtval?: number;
    misa?: number;
    mie?: number;
    mip?: number;
    medeleg?: number;
    mideleg?: number;
    mcounteren?: number;
    menvcfg?: number;
    stvec?: number;
    sscratch?: number;
    sepc?: number;
    scause?: number;
    stval?: number;
    satp?: number;
    scounteren?: number;
    senvcfg?: number;
    ilrsc?: number;
    iflags?: number;
};

export type RamConfig = {
    length?: bigint;
    image_filename?: string;
};

export type DtbConfig = {
    bootargs?: string;
    init?: string;
    entrypoint?: string;
    image_filename?: string;
};

export type MemoryRangeConfig = {
    start?: bigint;
    length?: bigint;
    shared?: boolean;
    image_filename?: string;
};

export type TlbConfig = {
    image_filename?: string;
};

export type ClintConfig = {
    mtimecmp?: number;
};

export type HtifConfig = {
    fromhost?: number;
    tohost?: number;
    console_getchar?: boolean;
    yield_manual?: boolean;
    yield_automatic?: boolean;
};

export type RollupConfig = {
    has_value?: boolean;
    rx_buffer?: MemoryRangeConfig;
    tx_buffer?: MemoryRangeConfig;
    input_metadata?: MemoryRangeConfig;
    voucher_hashes?: MemoryRangeConfig;
    notice_hashes?: MemoryRangeConfig;
};

export type UarchRamConfig = {
    image_filename?: string;
};

export type UarchProcessorConfig = {
    x?: number[];
    pc?: number;
    cycle?: number;
    halt_flag?: boolean;
};

export type UarchConfig = {
    processor?: UarchProcessorConfig;
    ram?: UarchRamConfig;
};

export type MachineConfig = {
    processor?: ProcessorConfig;
    ram?: RamConfig;
    dtb?: DtbConfig;
    flash_drive?: MemoryRangeConfig[];
    tlb?: TlbConfig;
    clint?: ClintConfig;
    htif?: HtifConfig;
    rollup?: RollupConfig;
    uarch?: UarchConfig;
};

export type ConcurrencyRuntimeConfig = {
    update_merkle_tree?: bigint;
};

export type HtifRuntimeConfig = {
    no_console_putchar?: boolean;
};

export type MachineRuntimeConfig = {
    concurrency?: ConcurrencyRuntimeConfig;
    htif?: HtifRuntimeConfig;
    skip_root_hash_check?: boolean;
    skip_version_check?: boolean;
    soft_yield?: boolean;
};

export type Proof = {
    target_address: bigint;
    log2_target_size: number;
    target_hash: Uint8Array;
    log2_root_size: number;
    root_hash: Uint8Array;
    sibling_hashes: Uint8Array[];
};

export enum BreakReason {
    FAILED,
    HALTED,
    YIELDED_MANUALLY,
    YIELDED_AUTOMATICALLY,
    YIELDED_SOFTLY,
    REACHED_TARGET_MCYCLE,
}

export enum MachineError {
    OK = 0,
    INVALID_ARGUMENT,
    DOMAIN_ERROR,
    LENGTH_ERROR,
    OUT_OF_RANGE,
    LOGIC_ERROR,
    LOGIC_ERROR_END,
    BAD_OPTIONAL_ACCESS,
    RUNTIME_ERROR,
    RANGE_ERROR,
    OVERFLOW_ERROR,
    UNDERFLOW_ERROR,
    REGEX_ERROR,
    SYSTEM_IOS_BASE_FAILURE,
    FILESYSTEM_ERROR,
    ATOMIC_TX_ERROR,
    NONEXISTING_LOCAL_TIME,
    AMBIGUOUS_LOCAL_TIME,
    FORMAT_ERROR,
    RUNTIME_ERROR_END,
    BAD_TYPEID,
    BAD_CAST,
    BAD_ANY_CAST,
    BAD_WEAK_PTR,
    BAD_FUNCTION_CALL,
    BAD_ALLOC,
    BAD_ARRAY_NEW_LENGTH,
    BAD_EXCEPTION,
    BAD_VARIANT_ACCESS,
    EXCEPTION,
    OTHER_ERROR_END,
    UNKNOWN,
}

export interface Machine {
    get_root_hash(): Uint8Array;
    store(directory: string): void;
    run(max_mcycle?: bigint): BreakReason;
    read_memory(address: bigint, length: bigint): ArrayBuffer;
    write_memory(address: bigint, data: ArrayBuffer): void;
    read_virtual_memory(address: bigint, length: bigint): ArrayBuffer;
    write_virtual_memory(address: bigint, data: ArrayBuffer): void;
    get_proof(address: bigint, log2_size: number): Proof;
    snapshot(): void;
    rollback(): void;
    verify_merkle_tree(): boolean;
}

export enum MachineErrorCode {}

export interface CartesiMachineError extends Error {
    code: MachineError;
}

class MachineWrapper implements Machine {
    private machine: any;
    constructor(machine: any) {
        this.machine = machine;
    }

    get_root_hash(): Uint8Array {
        return this.machine.get_root_hash();
    }

    store(directory: string): void {
        this.machine.store(directory);
    }

    run(max_mcycle?: bigint | undefined): BreakReason {
        return this.machine.run(max_mcycle);
    }

    read_memory(address: bigint, length: bigint): ArrayBuffer {
        return this.machine.read_memory(address, length);
    }

    write_memory(address: bigint, data: ArrayBuffer): void {
        this.machine.write_memory(address, data);
    }

    read_virtual_memory(address: bigint, length: bigint): ArrayBuffer {
        return this.machine.read_virtual_memory(address, length);
    }

    write_virtual_memory(address: bigint, data: ArrayBuffer): void {
        this.machine.write_virtual_memory(address, data);
    }

    get_proof(address: bigint, log2_size: number): Proof {
        return this.machine.get_proof(address, log2_size);
    }

    snapshot(): void {
        this.machine.snapshot();
    }

    rollback(): void {
        this.machine.rollback();
    }

    verify_merkle_tree(): boolean {
        return this.machine.verify_merkle_tree();
    }
}

export const create = (
    config: MachineConfig,
    runtimeConfig: MachineRuntimeConfig,
): Machine => {
    return new MachineWrapper(new addon.Machine(config, runtimeConfig));
};

export const load = (
    directory: string,
    runtimeConfig: MachineRuntimeConfig,
): Machine => {
    return new MachineWrapper(new addon.Machine(directory, runtimeConfig));
};
