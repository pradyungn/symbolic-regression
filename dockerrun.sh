#!/bin/sh
set -eu

if docker ps | rg apeshit; then
    true
elif docker ps -a | rg apeshit; then
    docker start apeshit
else
    docker run -dit --security-opt seccomp=unconfined \
        --name apeshit \
        -v $HOME:/host pradyun2/598ape /bin/bash
fi

if [ "$#" -eq 0 ]; then
    echo "Go apeshit"
else
    docker exec -it -w "/host/symreg" apeshit $@
fi
