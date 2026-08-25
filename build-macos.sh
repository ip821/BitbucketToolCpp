#!/usr/bin/env bash

set -euo pipefail

project_root="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
build_type="${1:-MinSizeRel}"
jobs="${JOBS:-$(sysctl -n hw.logicalcpu 2>/dev/null || echo 4)}"

case "${build_type}" in
    Debug) build_directory_name="cmake-build-debug" ;;
    Release) build_directory_name="cmake-build-release" ;;
    RelWithDebInfo) build_directory_name="cmake-build-relwithdebinfo" ;;
    MinSizeRel) build_directory_name="cmake-build-minsizerel" ;;
    *)
        echo "Usage: $0 [Debug|Release|RelWithDebInfo|MinSizeRel]" >&2
        exit 2
        ;;
esac

macos_build_dir="${BUILD_DIR:-${project_root}/${build_directory_name}}"

if [[ "$(uname -s)" != "Darwin" ]]; then
    echo "This script must be run on macOS." >&2
    exit 1
fi

for required_tool in cmake ninja; do
    if ! command -v "${required_tool}" >/dev/null 2>&1; then
        echo "Required tool not found in PATH: ${required_tool}" >&2
        echo "Install the build tools with: brew install cmake ninja" >&2
        exit 1
    fi
done

echo "Preparing the macOS ${build_type} build..."
if [[ ! -f "${macos_build_dir}/build.ninja" ]]; then
    cmake \
        -S "${project_root}" \
        -B "${macos_build_dir}" \
        -G Ninja \
        -DCMAKE_BUILD_TYPE="${build_type}"
fi

echo "Building PRToolForBitbucket2.app..."
cmake \
    --build "${macos_build_dir}" \
    --target PRToolForBitbucket2 \
    --parallel "${jobs}"

output="${macos_build_dir}/PRToolForBitbucket2.app"
echo "macOS application: ${output}"
file "${output}/Contents/MacOS/PRToolForBitbucket2"
