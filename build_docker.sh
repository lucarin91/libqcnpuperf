#!/bin/bash
set -e

OUTPUT_DIR="${OUTPUT_DIR:-$(pwd)/dist}"

echo "=== Building qcnpuperf_cli (arm64) — FastRPC ${FASTRPC_VERSION} ==="
mkdir -p "${OUTPUT_DIR}"

docker build \
    --platform linux/arm64 \
    --target export \
    --output "type=local,dest=${OUTPUT_DIR}" \
    .

echo "=== Build complete ==="
echo "Binary: ${OUTPUT_DIR}/qcnpuperf_cli"
