#include "SuffixArray.hpp"
#include "utils.hpp"

#include <algorithm>

using namespace std;

void SuffixArray::build(const string &folder_path) {
    auto [t, offs] = concat_files(folder_path);
    text = t;
    offsets = offs;

    sa.width(sdsl::bits::hi(text.size()) + 1);
    sa.resize(text.size());

    sdsl::algorithm::calculate_sa((const unsigned char *)text.c_str(), text.size(), sa);
}

int SuffixArray::compare_suffix(const string &pattern, size_t suffix_idx) const {
    // if (suffix_idx >= text.size()) return -1;

    const char *suffix_ptr = text.c_str() + suffix_idx;
    const char *pattern_ptr = pattern.c_str();

    int cmp = strncmp(pattern_ptr, suffix_ptr, pattern.size());

    if (cmp < 0) return -1; // pattern < suffix
    if (cmp > 0) return +1; // pattern > suffix
    return 0; // Coinciden
}

vector<size_t> SuffixArray::doc_locate(const string &pattern) {
    if (pattern.empty())
        return {};

    long left = 0;
    long right = sa.size() - 1;
    long start_idx = -1;

    // búsqueda binaria para encontrar el primer sufijo (bot lim)
    while (left <= right) {
        long mid = left + (right - left) / 2;
        int cmp = compare_suffix(pattern, sa[mid]);

        if (cmp <= 0) {
            if (cmp == 0)
                start_idx = mid; // posible candidato, pero seguimos buscando a la izquierda
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }

    if (start_idx == -1) {
        // no se encontró ninguna coincidencia
        return {};
    }

    // búsqueda binaria para encontrar el último sufijo (top lim)
    left = start_idx;
    right = sa.size() - 1;
    long end_idx = -1;

    while (left <= right) {
        long mid = left + (right - left) / 2;
        int cmp = compare_suffix(pattern, sa[mid]);

        if (cmp == 0) {
            end_idx = mid; // candidato, seguimos buscando a la derecha
            left = mid + 1;
        } else if (cmp < 0) { // patrón < texto
            right = mid - 1;
        } else { // patrón > texto
            left = mid + 1;
        }
    }

    // recuperar documentos únicos en el rango
    vector<size_t> docs;
    vector<bool> seen(offsets.size(), false);

    // recorremos el rango en el SA
    for (long i = start_idx; i <= end_idx; ++i) {
        size_t text_pos = sa[i];

        auto it = upper_bound(offsets.begin(), offsets.end(), text_pos);
        size_t doc_id = distance(offsets.begin(), it) - 1;

        if (!seen[doc_id]) {
            docs.push_back(doc_id);
            seen[doc_id] = true;
        }

        if (docs.size() == offsets.size())
            break;
    }

    sort(docs.begin(), docs.end());
    return docs;
}
