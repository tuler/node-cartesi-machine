import { defineConfig } from "tsup";

export default defineConfig({
    clean: true,
    dts: true,
    entry: ["lib/binding.ts"],
    format: ["cjs", "esm"],
    splitting: false,
    sourcemap: true,
});
