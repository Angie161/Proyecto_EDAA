#include "FMIndex.hpp"
#include "SuffixArray.hpp"
#include "utils.hpp"

#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;
using namespace std::chrono;

int main(int argc, char *argv[]) {
    string docs_path, patterns_path, csv_path;
    unsigned reps;
    validate_args(argc, argv, docs_path, patterns_path, csv_path, reps);

    cout << "=== Inicio del Proyecto 2 EDAA ===" << endl;
    cout << "Documentos: " << docs_path << endl;
    cout << "Patrones: " << patterns_path << endl;
    cout << "Repeticiones: " << reps << endl;

    // Construcción
    cout << "\n* Construyendo índices..." << endl;

    cout << "Construyendo Suffix Array..." << endl;
    SuffixArray sa;
    auto t_start = high_resolution_clock::now();
    sa.build(docs_path);
    auto t_end = high_resolution_clock::now();
    long time_build_sa = duration_cast<milliseconds>(t_end - t_start).count();
    cout << "   Tiempo SA: " << time_build_sa << " ms" << endl;

    cout << "Construyendo FM-Index..." << endl;
    FMIndex fm;
    t_start = high_resolution_clock::now();
    fm.build(docs_path);
    t_end = high_resolution_clock::now();
    long time_build_fm = duration_cast<milliseconds>(t_end - t_start).count();
    cout << "   Tiempo FM: " << time_build_fm << " ms" << endl;

    // Carga de patrones
    cout << "\n* Cargando patrones..." << endl;
    vector<string> patterns = load_patterns_from_file(patterns_path);
    cout << "Se cargaron " << patterns.size() << " patrones." << endl;

    // Ejecución de experimentos
    cout << "\n* Ejecutando búsquedas y guardando en CSV..." << endl;
    ofstream csv(csv_path);
    if (!csv.is_open()) {
        cerr << "Error al crear archivo CSV en: " << csv_path << endl;
        return 1;
    }

    csv << "type,text,pattern_len,time_ns,found_count\n";
    for (const string &p : patterns) {
        vector<size_t> results;

        // SA
        long total_time = 0.0;
        for (unsigned i = 0; i < reps; i++) {
            t_start = high_resolution_clock::now();
            results = sa.doc_locate(p);
            t_end = high_resolution_clock::now();
            long ns = duration_cast<nanoseconds>(t_end - t_start).count();
            total_time += ns;
        }
        csv << "SA," << docs_path << ',' << p.size() << ',' << total_time / reps << ',' << results.size() << '\n';

        // FM
        total_time = 0.0;
        for (unsigned i = 0; i < reps; i++) {
            t_start = high_resolution_clock::now();
            results = fm.doc_locate(p);
            t_end = high_resolution_clock::now();
            long ns = duration_cast<nanoseconds>(t_end - t_start).count();
            total_time += ns;
        }
        csv << "FM," << docs_path << ',' << p.size() << ',' << total_time / reps << ',' << results.size() << '\n';
    }

    csv.close();
    cout << "\nExperimento finalizado. Resultados guardados en: " << csv_path << endl;

    return 0;
}
