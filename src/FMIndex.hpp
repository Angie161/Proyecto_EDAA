#pragma once

#include <sdsl/suffix_arrays.hpp>

class FMIndex {
  private:
    sdsl::csa_wt<sdsl::wt_huff<>, 32, 32> fm_index;
    std::vector<std::size_t> offsets;
    std::string text;

  public:
    // Construcción del índice
    void build(const std::string &folder_path);

    // Búsqueda de documentos con coincidencia
    std::vector<size_t> doc_locate(const std::string &pattern);

    // Retornar tamaño en megabytes
    size_t get_size_mb() { return sdsl::size_in_mega_bytes(fm_index); };
};
