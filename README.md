# Proyecto 2: "Detección de patrones en múltiples documentos"

## Descripción

Segundo proyecto de _Estructura de datos y algoritmos avanzados_, donde se compararán dos implementaciones para búsquedas de patrones en multiples documentos.

## Uso

### Dependencias

- `g++` con soporte C++17
- `make` para la compilación
- `python3`/`pip`, con librerías `pandas`, `matplotlib`, `numpy`, para gráficos

Es posible obtener todo, en distros tipo Debian (Ubuntu/PopOS), usando:

```bash
sudo apt update
sudo apt install build-essential python3-pip
pip3 install pandas matplotlib numpy
```

### Comandos

```bash
# Preparar experimentos (compilar)
./setup.sh

# Ejecutar todos los experimentos
./run.sh

# Graficar resultados
python3 graph.py results/
```
