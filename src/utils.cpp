#include "utils.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>

using namespace std;
namespace fs = filesystem;

void validate_args(int argc, char *argv[], string &docs_path, string &patterns_path, string &csv_path, unsigned &reps) {
    if (argc < 5) {
        cerr << argv[0] << " <docs_path> <patterns_path> <csv_path> <reps>\n";
        exit(1);
    }
    docs_path = argv[1];
    if (!fs::exists(docs_path) || !fs::is_directory(docs_path)) {
        cerr << "Error: invalid " << quoted(docs_path) << " docs directory\n";
        exit(1);
    }
    patterns_path = argv[2];
    if (!fs::exists(patterns_path) || !fs::is_regular_file(patterns_path)) {
        cerr << "Error: invalid " << quoted(patterns_path) << " patterns file\n";
        exit(1);
    }
    csv_path = argv[3];
    if (!fs::exists(fs::path(csv_path).parent_path())) {
        cerr << "Error: invalid " << quoted(csv_path) << " csv path\n";
        exit(1);
    }

    int i = atoi(argv[4]);
    if (i < 0) {
        cerr << "Error: reps must be a positive number\n";
        exit(1);
    }
    reps = i;
}

static string read_file(const string &filename) {
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
        string extension = entry.path().extension();
        if (entry.is_regular_file() && (extension == ".xml" || extension == ".txt")) {
            files.push_back(entry.path());
        }
    }

    if (files.empty()) {
        cerr << "Error: No se encontraron archivos en la carpeta: " << carpeta << endl;
        exit(1);
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
    ofstream out(CONCAT_FILE);
    out << concat_text;
    out.close();

    cout << "Archivo " << quoted(CONCAT_FILE) << " generado correctamente.\n";
    // cout << "Texto concatenado: " << concat_text << "\n";

    return {concat_text, offsets};
}

vector<string> load_patterns(const string &folder) {
    string pattern_file = folder + "/patterns/patterns.txt";

    ifstream in(pattern_file);
    if (!in.is_open()) {
        cerr << "ERROR: No se pudo abrir archivo de patrones: " << pattern_file << endl;
        exit(1);
    }

    string content((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
    in.close();

    vector<string> patterns;
    string separator = "$$$$$";
    size_t pos = 0;

    while (true) {
        size_t next = content.find(separator, pos);
        if (next == string::npos)
            break;

        string pat = content.substr(pos, next - pos);
        if (!pat.empty())
            patterns.push_back(pat);

        pos = next + separator.size();
    }

    return patterns;
}

vector<string> load_patterns_from_file(const string &filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: No se pudo abrir el archivo de patrones: " << filename << endl;
        exit(1);
    }

    // Leer todo el contenido
    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    vector<string> patterns;
    string separator = "$$$$$"; // Mismo separador definido en setup.sh y utils.cpp
    size_t pos = 0;

    while (true) {
        size_t next = content.find(separator, pos);
        if (next == string::npos)
            break;

        // Extraer patrón ignorando cadenas vacías
        if (next > pos) {
            string pat = content.substr(pos, next - pos);
            patterns.push_back(pat);
        }

        pos = next + separator.size();
    }
    return patterns;
}
