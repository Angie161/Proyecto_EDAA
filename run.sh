#!/bin/bash

# TODO: extraer nombre de textos de directorios en datasets y guardar resultados como "results/${TEXTNAME}.csv"

REPS=32

mkdir -p results

# Ejemplos
./build/multidocs datasets/examples/ datasets/examples/patterns/patterns.txt results/examples.csv $REPS
./build/multidocs datasets/proteins/ datasets/proteins/patterns/patterns.txt results/proteins.csv $REPS
./build/multidocs datasets/sources/ datasets/sources/patterns/patterns.txt results/sources.csv $REPS
./build/multidocs datasets/dblp/ datasets/dblp/patterns/patterns.txt results/dblp.csv $REPS
