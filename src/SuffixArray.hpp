#pragma once

#include <sdsl/suffix_arrays.hpp>
#include <string>
#include <vector>

class SuffixArray {
  private:
    sdsl::int_vector<> sa;       // contenedor sdsl de suffix array
    std::vector<size_t> offsets; // arreglo auxiliar para guardar posiciones de inicio de cada doc
    std::string text;            // texto concatenado completo (NOTE: no escala bien con archivos grandes (1GB))

    // función auxiliar de búsqueda binaria: compara patrón P con sufijo que inicia en text[suffix_idx]
    // retorna: 0 si son iguales, < 0 si P es menor y > 0 si P es mayor
    int compare_suffix(const std::string &pattern, size_t suffix_idx) const;

  public:
    // construcción de índice (concat + compute SA)
    void build(const std::string &folder_path);

    // búsqueda de documentos que contienen el patrón
    std::vector<size_t> doc_locate(const std::string &pattern);

    // retorna tamaño en megabytes
    size_t get_size_mb() { return sdsl::size_in_mega_bytes(sa) + text.size(); }; };
