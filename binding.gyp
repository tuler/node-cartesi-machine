{
    "targets": [
        {
            "target_name": "<(module_name)",
            "sources": [
                "src/binding.cc",
                "src/config.cc",
                "src/machine.cc",
                "src/proof.cc",
            ],
            "include_dirs": [
                "<!@(node -p \"require('node-addon-api').include_dir\")",
            ],
            "conditions": [
                ['OS=="mac"', {
                    "cflags+": ['-fvisibility=hidden'],
                    "xcode_settings": {
                        "GCC_SYMBOLS_PRIVATE_EXTERN": "YES", # -fvisibility=hidden
                        "MACOSX_DEPLOYMENT_TARGET": "11.0"
                    }
                }]
            ],
            "dependencies": [
                "<!(node -p \"require('node-addon-api').targets\"):node_addon_api",
                "cartesi",
            ],
            "link_settings": {
                "libraries": [ "-Wl,-rpath,@loader_path" ],
            },
            "defines": [],
        },
        {
            "target_name": "cartesi",
            "type": "<(library)",
            "cflags": [
                "-O2",
                "-g",
                "-std=gnu++17",
                "-fvisibility=hidden",
                "-MMD",
                "-fPIC",
                "-fno-strict-aliasing",
                "-fno-strict-overflow",
                "-fno-delete-null-pointer-checks",
                "-Wall",
                "-Wpedantic"
            ],
            "cflags_cc": [
                "-O2",
                "-g",
                "-std=gnu++17",
                "-fvisibility=hidden",
                "-MMD",
                "-fPIC",
                "-fno-strict-aliasing",
                "-fno-strict-overflow",
                "-fno-delete-null-pointer-checks",
                "-Wall",
                "-Wpedantic"
            ],
            "cflags!": [ "-fno-exceptions", "-O3" ],
            "cflags_cc!": [ "-fno-exceptions", "-O3" ],
            "defines": [ "NDEBUG" ],
            "include_dirs": [
                "machine-emulator/src",
                "machine-emulator-artifacts/src",
                "machine-emulator/third-party/downloads",
                "machine-emulator/third-party/llvm-flang-uint128",
                "machine-emulator/third-party/tiny_sha3",
            ],
            "sources": [
                "machine-emulator/src/base64.cpp",
                "machine-emulator/src/clint.cpp",
                "machine-emulator/src/clint-factory.cpp",
                "machine-emulator/src/dtb.cpp",
                "machine-emulator/src/htif.cpp",
                "machine-emulator/src/htif-factory.cpp",
                "machine-emulator/src/interpret.cpp",
                "machine-emulator/src/json-util.cpp",
                "machine-emulator/src/machine.cpp",
                "machine-emulator/src/machine-c-api.cpp",
                "machine-emulator/src/machine-config.cpp",
                "machine-emulator/src/machine-merkle-tree.cpp",
                "machine-emulator/src/os.cpp",
                "machine-emulator/src/pma.cpp",
                "machine-emulator/src/pma-driver.cpp",
                "machine-emulator/src/pristine-merkle-tree.cpp",
                "machine-emulator/src/shadow-state.cpp",
                "machine-emulator/src/shadow-state-factory.cpp",
                "machine-emulator/src/shadow-pmas.cpp",
                "machine-emulator/src/shadow-pmas-factory.cpp",
                "machine-emulator/src/shadow-tlb.cpp",
                "machine-emulator/src/shadow-tlb-factory.cpp",
                "machine-emulator/src/shadow-uarch-state.cpp",
                "machine-emulator/src/shadow-uarch-state-factory.cpp",
                "machine-emulator/src/uarch-interpret.cpp",
                "machine-emulator/src/uarch-machine.cpp",
                "machine-emulator/src/uarch-pristine-state-hash.cpp",
                "machine-emulator/src/uarch-reset-state.cpp",
                "machine-emulator/src/uarch-step.cpp",
                "machine-emulator/src/virtual-machine.cpp",
                "machine-emulator-artifacts/src/uarch-pristine-ram.cpp",
                "machine-emulator-artifacts/src/uarch-pristine-hash.cpp",
                "machine-emulator/third-party/tiny_sha3/sha3.c",
            ],
            "direct_dependent_settings": {
                "include_dirs": [
                    "machine-emulator/src",
                    "machine-emulator-artifacts/src",
                ]
            },
            "conditions": [
                ['OS=="mac"', {
                    "cflags+": ['-fvisibility=hidden'],
                    "xcode_settings": {
                        "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
                        "GCC_SYMBOLS_PRIVATE_EXTERN": "YES", # -fvisibility=hidden
                        "MACOSX_DEPLOYMENT_TARGET": "11.0"
                    }
                }]
            ],
        },
        {
            "target_name": "action_after_build",
            "type": "none",
            "dependencies": [ "<(module_name)", "cartesi" ],
            "conditions": [
                ['OS=="linux"', {
                    "copies": [
                        {
                            "files": [
                                "<(PRODUCT_DIR)/cartesi.so",
                                "<(PRODUCT_DIR)/<(module_name).node"
                            ],
                            "destination": "<(module_path)"
                        }
                    ]
                }],
                ['OS=="mac"', {
                    "copies": [
                        {
                            "files": [
                                "<(PRODUCT_DIR)/cartesi.dylib",
                                "<(PRODUCT_DIR)/<(module_name).node"
                            ],
                            "destination": "<(module_path)"
                        }
                    ]
                }],
            ]
        }
    ],
}
