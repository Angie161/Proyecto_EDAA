#include "FMIndex.hpp"

#include <filesystem>

namespace fs = std::filesystem;
using namespace std;

// Lector de archivo
string read_file(const string &filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error al abrir el archivo: " << filename << endl;
        exit(EXIT_FAILURE);
    }
    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    return content;
}

pair<string, vector<size_t>> concat_files(const string &carpeta) {
    vector<fs::path> files;
    
    for (const auto &entry : fs::directory_iterator(carpeta)) {
        if (entry.is_regular_file() && entry.path().extension() == ".xml") {
            files.push_back(entry.path());
        }
    }

    sort(files.begin(), files.end());

    string concat_text;
    vector<size_t> offsets;

    bool first = true;

    for (const auto &path : files) {
        if (!first) {
            concat_text += '$';
        }
        first = false;

        offsets.push_back(concat_text.size());

        string contenido = read_file(path.string());
        concat_text += contenido;
    }

    // Guardar la concatenación en un archivo
    ofstream out("concatenacion.txt");
    out << concat_text;
    out.close();

    cout << "Archivo concatenacion.txt generado correctamente.\n";
    // cout << "Texto concatenado: " << concat_text << "\n";

    return {concat_text, offsets};
}

void FMIndex::build(const string &folder_path) {
    auto [t, offs] = concat_files(folder_path);
    text = t;
    offsets = offs;

    construct(fm_index, "concatenacion.txt", 1);
}

vector<size_t> FMIndex::doc_locate(const string &pattern) {
    // Cuenta las ocurrencias del patron dentro del texto
    size_t range = count(fm_index, pattern.begin(), pattern.end());
    //cout << "occ: " << range << endl;                                 //FLAAAAAAAG
    if (range <= 0) {
        cout << "No hay apariciones del patron" << endl;
        return {};
    }

    // Obtiene las posiciones de ocurrencias
    auto occs = locate(fm_index, pattern.begin(), pattern.end());
    if (occs.empty()) {
        cout << "No se pudo localizar las apariciones del patron" << endl;
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
