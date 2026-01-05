set -eu
original_dir="$(pwd)"
project_root="$(realpath "$(dirname $0)")"
rm -rf "${project_root}/build"
mkdir "${project_root}/build"
clang                                                                     \
-Wall                                                                     \
-Werror                                                                   \
-Os                                                                       \
-mbulk-memory                                                             \
--target=wasm32                                                           \
-nostdlib                                                                 \
-Wl,--strip-all                                                           \
-Wl,--export-dynamic                                                      \
-Wl,--no-entry                                                            \
-Wl,--export=__heap_base                                                  \
-Wl,--initial-memory=6553600                                              \
-o "${project_root}/build/index.wasm"                                     \
"${project_root}/main.c"
cp -r "${project_root}/fonts" "${project_root}/build/fonts"
cp "${project_root}/index.html" "${project_root}/build"
cp -r "${project_root}/assets" "${project_root}/build"