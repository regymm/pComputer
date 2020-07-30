#!/bin/bash
set -e
cat > coe/result_$1.coe << EOF
memory_initialization_radix  = 16;
memory_initialization_vector =
EOF
xxd -c 4 -p $1.bin >> coe/result_$1.coe
