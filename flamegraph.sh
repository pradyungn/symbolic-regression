#!/bin/bash
# sourced from https://nodejs.org/learn/diagnostics/flame-graphs

FGPATH=../FlameGraph

COMMAND=""
case $1 in
    "diabetes")
         COMMAND="./genetic_benchmark diabetes"
    ;;

    "cancer")
         COMMAND="./genetic_benchmark cancer"
    ;;

    "housing")
         COMMAND="./genetic_benchmark housing"
    ;;

    "silk")
         COMMAND="./genetic_benchmark silk-wave"
    ;;

    "square")
         COMMAND="./genetic_benchmark square-wave"
    ;;

    *)
        echo "fahhhhh"
        exit 1
        ;;
esac

if [ "$#" -eq 2 ]; then
    $COMMAND
else
    perf record -e cycles:u -g -- $COMMAND
    perf script > perfs.out
    cat perfs.out | ./$FGPATH/stackcollapse-perf.pl | ./$FGPATH/flamegraph.pl --colors=js > $1.svg
fi
