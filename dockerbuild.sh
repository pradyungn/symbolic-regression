#!/bin/sh
set -eu

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
IMAGE=${1:-pradyun2/598ape}

cd $SCRIPT_DIR/docker
docker build -t "$IMAGE" .
