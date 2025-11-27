#include "FMIndex.hpp"

#include "utils.hpp"

using namespace std;

// Lector de archivo
void FMIndex::build(const string &folder_path) {
    auto [t, offs] = concat_files(folder_path);
    offsets = offs;

    construct(fm_index, CONCAT_FILE, 1);
}

vector<size_t> FMIndex::doc_locate(const string &pattern) {
    // Cuenta las ocurrencias del patron dentro del texto
    size_t range = count(fm_index, pattern.begin(), pattern.end());
    // cout << "occ: " << range << endl;                                 //FLAAAAAAAG
    if (range == 0) {
        // cout << "No hay apariciones del patron" << endl;
        return {};
    }

    // Obtiene las posiciones de ocurrencias
    auto occs = locate(fm_index, pattern.begin(), pattern.end());
    if (occs.empty()) {
        // cout << "No se pudo localizar las apariciones del patron" << endl;
        return {};
    }

    vector<size_t> docs;
    vector<bool> seen(offsets.size(), false); // Para no repetir documentos

    for (auto pos : occs) {
        // Determina a qué documento pertenece la posición
        auto it = upper_bound(offsets.begin(), offsets.end(), pos);
        size_t doc_id = distance(offsets.begin(), it) - 1;

        // Se agrega el documento a la lista solo si no se ha agregado antes
        if (!seen[doc_id]) {
            docs.push_back(doc_id);
            seen[doc_id] = true;
        }

        // Si ya marcamos todos los documentos, rompemos
        if (docs.size() == offsets.size())
            break;
    }

    sort(docs.begin(), docs.end());
    return docs;
}
