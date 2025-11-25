#!/bin/bash

# TODO: extraer nombre de textos de directorios en datasets y guardar resultados como "results/${TEXTNAME}.csv"

REPS=32

# Ejemplos
./build/multidocs datasets/examples/ datasets/examples/patterns/patterns.txt results/examples_ns.csv $REPS
./build/multidocs datasets/proteins/ datasets/proteins/patterns/patterns.txt results/proteins_ns.csv $REPS
