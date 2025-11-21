#!/bin/bash

set -e # Parar script en error


DATASET_URL='https://drive.usercontent.google.com/download?id=1G5r4vpOqV0hByxSXBopWtx4wIqgp3lV7&confirm'
DATASET_ZIP='datasets.zip'
EXAMPLES_DIR='datasets/examples'

[[ -d "datasets" ]] && exit 0 # Si ya existe no ejecutar


echo "Descargando datasets..."
curl -L "$DATASET_URL" -o "$DATASET_ZIP"
unzip "$DATASET_ZIP"
rm "$DATASET_ZIP"
echo "Datasets listos!"

echo "Generando ejemplos..."
mkdir -p "$EXAMPLES_DIR"
echo 'Búsqueda binaria corre en tiempo logarítmico' > "$EXAMPLES_DIR/D1.txt"
echo 'El árbol binario de búsqueda es una estructura de datos fundamental' > "$EXAMPLES_DIR/D2.txt"
echo 'Sólo quedan 6 semanas para fin de año' > "$EXAMPLES_DIR/D3.txt"
echo "Ejemplos listos!"

