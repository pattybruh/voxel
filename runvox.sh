#!/bin/bash

set -e

mkdir -p build
cd build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
ln -sf compile_commands.json ../compile_commands.json
make

echo "========== Running Voxel =========="
./Voxel
