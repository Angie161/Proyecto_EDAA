#!/bin/bash
# chmod +x setup.sh
# ./setup.sh

set -e

DATASET_URL='https://drive.usercontent.google.com/download?id=1G5r4vpOqV0hByxSXBopWtx4wIqgp3lV7&confirm=t'
DATASET_ZIP='datasets.zip'
EXAMPLES_DIR='datasets/examples'

N_PATTERNS=10
# N_PATTERNS=50
# PATTERNS_LEN=(5 10 20 50)
DELIMITER='$$$$$'

###############################
# 1) DESCARGA DE DATASETS
###############################
download_texts() {
    echo "Descargando datasets..."
    curl -L "$DATASET_URL" -o "$DATASET_ZIP"

    # Comprobar si zip fue descargado correctamente
    if unzip -t "$DATASET_ZIP" &>/dev/null; then
        unzip -q "$DATASET_ZIP"
        rm "$DATASET_ZIP"
        echo "Datasets listos!"
    else
        echo "Error: El archivo descargado no es un ZIP válido."
        rm -f $DATASET_ZIP # limpiar zip invalido
        exit 1
    fi

    echo "Generando ejemplos..."
    mkdir -p "$EXAMPLES_DIR"
    echo 'Búsqueda binaria corre en tiempo logarítmico' >"$EXAMPLES_DIR/D1.txt"
    echo 'El árbol binario de búsqueda es una estructura de datos fundamental' >"$EXAMPLES_DIR/D2.txt"
    echo 'Sólo quedan 6 semanas para fin de año' >"$EXAMPLES_DIR/D3.txt"
}

###############################
# 2) NORMALIZACIÓN
###############################
normalize_file() {
    local FILE="$1"
    if [ ! -f "$FILE" ]; then return; fi # Saltar archivos no validos

    echo "Normalizando $FILE..."

    # Convertir a ASCII y eliminar caracteres no imprimibles + saltos de línea
    #iconv -f ISO-8859-1 -t ASCII//TRANSLIT -c < "$FILE" | \
    iconv -f UTF-8 -t ASCII//TRANSLIT -c < "$FILE" | \
        # tr -d '\r' | tr '\n' ' ' | \
        awk '{gsub(/[^[:print:]]/, ""); printf "%s", $0}' > "$FILE.tmp"

    mv "$FILE.tmp" "$FILE"
}

###############################
# 3) GENERACIÓN DE PATRONES
###############################
generate_patterns_for_folder() {
    local FOLDER="$1"

    # if [[ "$FOLDER" == *"examples" ]]; then return; fi

    if [[ "$FOLDER" == *"patterns"* ]]; then return; fi

    echo "Procesando carpeta: $FOLDER"

    FIRST_FILE=$(find "$FOLDER" -maxdepth 1 -type f -not -name ".*" | sort -V | head -n 1)

    if [ -z "$FIRST_FILE" ]; then
        echo " No se encontraron archivos en $FOLDER"
        return
    fi

    echo " Fuente de patrones: $FIRST_FILE"

    # Obtener tamaño del texto en bytes sin leer texto completo
    TEXT_LEN=$(stat -c%s "$FIRST_FILE")

    PATTERN_DIR="$FOLDER/patterns"
    mkdir -p "$PATTERN_DIR"
    OUT_FILE="$PATTERN_DIR/patterns.txt"
    > "$OUT_FILE" # eliminar contenido previo

    echo " Generando $N_PATTERNS patrones..."


    # # Generar patrones para cada longitud definida
    # for LEN in "${LENGTHS[@]}"; do
    #     # Generamos varios patrones por cada longitud
    #     for k in {1..10}; do
    #         # Elegir un punto de inicio aleatorio. 
    #         # Aseguramos que start + len < TEXT_LEN
    #         MAX_START=$((TEXT_LEN - LEN - 1))
    #         if [ "$MAX_START" -gt 0 ]; then
    #             START_POS=$(shuf -i 0-"$MAX_START" -n 1)
    #
    #             # Extraer substring
    #             # dd es más seguro para bytes exactos que head/tail combinados
    #             dd if="$FIRST_FILE" bs=1 skip="$START_POS" count="$LEN" 2>/dev/null >> "$OUT_FILE"
    #
    #             # Añadir delimitador
    #             printf "$DELIMITER" >> "$OUT_FILE"
    #         fi
    #     done
    # done

    for i in $(seq 1 $N_PATTERNS); do
        # tamaño del patron
        size=$((TEXT_LEN * i / N_PATTERNS))
        if [ "$size" -eq 0 ]; then continue; fi

        # Escribimos el patrón y los $$$$$ sin saltos de línea extra
        # head -c n file: lee los primeros n bytes de un archivo
        # evitamos guardar patrón en variables debido a posibles tamaños gigantes
        head -c "$size" "$FIRST_FILE" >>"$OUT_FILE"
        printf "$DELIMITER" >>"$OUT_FILE"

        echo " Generado patrón de tamaño $size"
    done

    echo " Patrones guardados en $OUT_FILE"
}

###############################
# 4) MAIN, PUNTO DE PARTIDA
###############################
main() {
    # Si no existe el directorio de textos, descargar todos los textos
    if [ ! -d "datasets" ]; then
        download_texts
    else
        echo 'Ya existe el directorio de los textos, saltando.'
    fi

    echo -e "Normalizando todos los archivos dentro directorio datasets..."
    # find datasets -type f -not -path "*/examples/*" -not -name ".*" | while read -r FILE; do
    find datasets -type f  -not -name ".*" | while read -r FILE; do
        normalize_file "$FILE"
    done
    echo -e "\nTodos los archivos normalizados.\n"

    echo -e "\nGenerando patrones…"
    find datasets -type d -not -path "*/patterns" | while read -r DIR; do
        generate_patterns_for_folder "$DIR"
    done

    echo -e "\nCompilando todo..."
    make

    echo -e "\nProceso COMPLETO."
}

main
