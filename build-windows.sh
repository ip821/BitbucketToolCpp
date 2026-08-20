#!/usr/bin/env bash

set -euo pipefail

project_root="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
build_type="${1:-Release}"
toolchain_file="${project_root}/cmake/toolchains/mingw-w64-x86_64.cmake"
jobs="${JOBS:-$(sysctl -n hw.logicalcpu 2>/dev/null || echo 4)}"

case "${build_type}" in
    Debug) build_directory_name="cmake-build-win-debug" ;;
    Release) build_directory_name="cmake-build-win-release" ;;
    RelWithDebInfo) build_directory_name="cmake-build-win-relwithdebinfo" ;;
    MinSizeRel) build_directory_name="cmake-build-win-minsizerel" ;;
    *)
        echo "Usage: $0 [Debug|Release|RelWithDebInfo|MinSizeRel]" >&2
        exit 2
        ;;
esac

windows_build_dir="${BUILD_DIR:-${project_root}/${build_directory_name}}"

for required_tool in cmake ninja x86_64-w64-mingw32-gcc x86_64-w64-mingw32-g++ x86_64-w64-mingw32-windres; do
    if ! command -v "${required_tool}" >/dev/null 2>&1; then
        echo "Required tool not found in PATH: ${required_tool}" >&2
        echo "Install the toolchain with: brew install cmake ninja mingw-w64" >&2
        exit 1
    fi
done

echo "Preparing the Windows x86-64 ${build_type} build..."
if [[ ! -f "${windows_build_dir}/CMakeCache.txt" ]]; then
    cmake_options=(
        -S "${project_root}"
        -B "${windows_build_dir}"
        -G Ninja
        -DCMAKE_BUILD_TYPE="${build_type}"
        -DCMAKE_TOOLCHAIN_FILE="${toolchain_file}"
    )
    if [[ -n "${HOST_WXRC:-}" ]]; then
        cmake_options+=(-DHOST_WXRC="${HOST_WXRC}")
    fi
    cmake "${cmake_options[@]}"
fi

echo "Building PRToolForBitbucket2.exe..."
cmake \
    --build "${windows_build_dir}" \
    --target PRToolForBitbucket2 \
    --parallel "${jobs}"

output="${windows_build_dir}/PRToolForBitbucket2.exe"
echo "Windows executable: ${output}"
file "${output}"
