import { defineConfig } from "tsup";

export default defineConfig({
    clean: true,
    entry: ["lib/binding.ts"],
    splitting: false,
    sourcemap: true,
});
