#!/bin/bash
# chmod +x setup.sh
# ./setup.sh

set -e

DATASET_URL='https://drive.usercontent.google.com/download?id=1G5r4vpOqV0hByxSXBopWtx4wIqgp3lV7&confirm'
DATASET_ZIP='datasets.zip'
EXAMPLES_DIR='datasets/examples'

###############################
# 1) DESCARGA DE DATASETS
###############################
if [ ! -d "datasets" ]; then
    echo "Descargando datasets..."
    curl -L "$DATASET_URL" -o "$DATASET_ZIP"
    unzip "$DATASET_ZIP"
    rm "$DATASET_ZIP"
    echo "Datasets listos!"

    echo "Generando ejemplos..."
    mkdir -p "$EXAMPLES_DIR"
    echo 'Búsqueda binaria corre en tiempo logarítmico' > "$EXAMPLES_DIR/D1.txt"
    echo 'El árbol binario de búsqueda es una estructura fundamental' > "$EXAMPLES_DIR/D2.txt"
    echo 'Sólo quedan 6 semanas para fin de año' > "$EXAMPLES_DIR/D3.txt"
fi

###############################
# 2) NORMALIZACIÓN
###############################
normalize_file() {
    local FILE="$1"

    echo "Normalizando $FILE..."

    # Convertir a ASCII y eliminar caracteres no imprimibles + saltos de línea
    iconv -f ISO-8859-1 -t ASCII//TRANSLIT < "$FILE" | \
        awk '{gsub(/[^[:print:]]/, ""); printf "%s", $0}' > "$FILE.tmp"

    mv "$FILE.tmp" "$FILE"
}

echo ""
echo "Normalizando todos los archivos dentro de datasets..."

for DIR in datasets/*; do
    if [ -d "$DIR" ]; then
        for FILE in "$DIR"/*; do
            if [ -f "$FILE" ]; then
                normalize_file "$FILE"
            fi
        done
    fi
done

echo ""
echo "Todos los archivos normalizados."
echo ""

###############################
# 3) GENERACIÓN DE PATRONES
###############################
generate_patterns_for_folder() {
    local FOLDER="$1"

    # evitar la carpeta "examples"
    if [[ "$FOLDER" == *"examples" ]]; then
        return
    fi

    echo "Procesando carpeta: $FOLDER"

    FIRST_FILE=$(ls "$FOLDER" | sort -V | head -n 1)
    FILE_PATH="$FOLDER/$FIRST_FILE"

    echo " Primer archivo seleccionado: $FIRST_FILE"

    TEXT_CONTENT=$(cat "$FILE_PATH")
    TEXT_LEN=${#TEXT_CONTENT}

    N_PATTERNS=10

    PATTERN_DIR="$FOLDER/patterns"
    mkdir -p "$PATTERN_DIR"

    OUT_FILE="$PATTERN_DIR/patterns.txt"
    > "$OUT_FILE"

    echo "Generando patrones..."

    for i in $(seq 1 $N_PATTERNS); do
        size=$(( TEXT_LEN * i / N_PATTERNS ))
        pattern="${TEXT_CONTENT:0:$size}"

        # Escribimos el patrón y los $$$$$ sin saltos de línea extra
        printf "%s" "$pattern" >> "$OUT_FILE"
        printf "\$\$\$\$\$" >> "$OUT_FILE"

        echo " Generado patrón de tamaño $size"
    done

}

echo ""
echo "Generando patrones…"
for DIR in datasets/*; do
    [ -d "$DIR" ] && generate_patterns_for_folder "$DIR"
done

echo ""
echo "Proceso COMPLETO."
