import { defineConfig } from "tsup";

export default defineConfig({
    clean: true,
    dts: true,
    entry: ["src/index.ts"],
    format: ["cjs", "esm"],
    minify: true,
    sourcemap: true,
});
