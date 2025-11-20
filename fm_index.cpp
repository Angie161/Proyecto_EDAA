// Compilación: g++ -std=c++17 -O3 fm_index.cpp -o fm_exec -lsdsl -ldivsufsort -ldivsufsort64
// Ejecución: ./fm_exec <text_file> <t_construct.csv> <t_search.csv> <reps> 
//./fm_exec datasetProp construc.csv busqueda.csv 30

#include <sdsl/suffix_arrays.hpp>
#include <string>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <iterator>
#include <sys/resource.h> 

using namespace sdsl;
namespace fs = std::filesystem;

// ============================
// Lector de archivo
// ============================
std::string read_file(const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo: " << filename << std::endl;
        exit(EXIT_FAILURE);
    }
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    return content;
}

// Concatena todos los archivos en una carpeta
std::pair<std::string, std::vector<std::size_t>> concat_files(const std::string& carpeta) {
    std::vector<fs::path> files;

    for (const auto& entry : fs::directory_iterator(carpeta)) {
        if (entry.is_regular_file()) {
            files.push_back(entry.path());
        }
    }

    std::sort(files.begin(), files.end());

    std::string concat_text;
    std::vector<std::size_t> offsets;

    bool first = true;

    for (const auto& path : files) {
        if (!first) {
            concat_text += '&';
        }
        first = false;

        offsets.push_back(concat_text.size());

        std::string contenido = read_file(path.string());
        concat_text += contenido;
    }

    // Guardar la concatenación en un archivo
    std::ofstream out("concatenacion.txt");
    out << concat_text;
    out.close();

    std::cout << "Archivo concatenacion.txt generado correctamente.\n";
    //std::cout << "Texto concatenado: " << concat_text << "\n";

    return {concat_text, offsets};
}

// ============================
// Medición de memoria (getrusage)
// ============================
double get_memory_usage_mb() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss / 1024.0; 
}

// ============================
// Clase FMIndex
// ============================
class FMIndex {
private:
    csa_wt<wt_huff<>, 128, 128> fm_index;
    std::vector<std::size_t> offsets;
    std::string text;

public:
    // Constructor vacío
    FMIndex() {}

    // Construcción del índice
    void build(const std::string& folder_path) {
        auto [t, offs] = concat_files(folder_path);
        text = t;
        offsets = offs;

        construct(fm_index, "concatenacion.txt", 1);
    }

    std::vector<size_t> doc_locate(const std::string& pattern) {
        std::cout << "Patron buscado: " << pattern << std::endl;                //FLAAAAAAAG
        // Cunta las ocurrencias del patron dentro del texto
        size_t range = count(fm_index, pattern.begin(), pattern.end());
        //std::cout << "occ: " << range << std::endl;                                 //FLAAAAAAAG
        if (range <= 0){
            std::cout << "No hay apariciones del patron" << std::endl;
            return {};
        } 

        // Obtiene las posiciones de ocurrencias
        auto occs = locate(fm_index, pattern.begin(), pattern.end());
        if (occs.empty()){
            std::cout << "No se pudo localizar las apariciones del patron" << std::endl;
            return {};
        }

        std::vector<size_t> docs;
        std::vector<bool> seen(offsets.size(), false); // Para no repetir documentos

        for (auto pos : occs) {
            // Determina a qué documento pertenece la posición
            auto it = std::upper_bound(offsets.begin(), offsets.end(), pos);
            size_t doc_id = std::distance(offsets.begin(), it) - 1;

            // Se agrega el documento a la lista solo si no se ha agregado antes
            if (!seen[doc_id]) {
                docs.push_back(doc_id);
                seen[doc_id] = true;
            }

            // Si ya marcamos todos los documentos, rompemos
            if (docs.size() == offsets.size()) break;
        }
        
        sort(docs.begin(), docs.end());
        return docs;
    }
};

// ============================
// MAIN
// ============================

int main(int argc, char** argv) {
    if (argc < 5) {
        std::cout << "Usage: " << argv[0] << " <folder_name> <t_construct.csv> <t_search.csv> <reps>" << std::endl;
        return 1;
    }

    std::string folder = argv[1];
    std::string cons_file = argv[2];
    std::string time_file = argv[3];
    size_t reps = std::stoi(argv[4]);

    FMIndex fm_index;

    // ------- Construcción --------
    double total_time = 0.0;

    auto start = std::chrono::high_resolution_clock::now();
    fm_index.build(folder);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = end - start;
    total_time = elapsed.count();
    double mem_usage = get_memory_usage_mb();

    std::ofstream cons(cons_file);
    cons << "tiempo_t,memoria_t_mb" << std::endl;
    cons << total_time << "," << mem_usage << std::endl;
    cons.close();

    // ------- Búsqueda --------
    std::vector<std::string> patrones = { "print" };

    std::ofstream timef(time_file);
    timef << "Patron,Longitud,Media(s),Varianza(s^2),cant_matches" << std::endl;

    std::vector<size_t> result;

    for (const std::string& pat : patrones) {
        std::vector<double> tiempos;
        tiempos.reserve(reps);

        // Ejecutar reps repeticiones
        for (size_t i = 0; i < reps; ++i) {
            auto s_start = std::chrono::high_resolution_clock::now();
            result = fm_index.doc_locate(pat);
            auto s_end = std::chrono::high_resolution_clock::now();

            double t = std::chrono::duration<double>(s_end - s_start).count();
            tiempos.push_back(t);
        }

        // ---- Calcular media ----
        double suma = 0.0;
        for (double t : tiempos) suma += t;
        double media = suma / reps;

        // ---- Calcular varianza ----
        double suma_var = 0.0;
        for (double t : tiempos) suma_var += (t - media) * (t - media);
        double varianza = suma_var / reps;   // varianza poblacional

        // ---- Guardar en CSV ----
        timef << pat << "," 
            << pat.size() << "," 
            << media << "," 
            << varianza << "," 
            << result.size()
            << std::endl;
    }

    timef.close();

    for (const auto& pat : patrones) {
        std::cout << "Apariciones del patrón " << pat << ":" << std::endl;
        for (const auto& occD : result){
            std::cout << "Documento " << occD+1 << "; ";
        }
    }
    std::cout << std::endl;
    std::cout << "Resultados guardados en " << cons_file << " y " << time_file << std::endl;
    return 0;
}
