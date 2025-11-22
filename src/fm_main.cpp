// ./build/fm_exec <text_file> <t_construct.csv> <t_search.csv> <reps>
//./build/fm_exec datasetProp construc.csv busqueda.csv 30

#include "FMIndex.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <sys/resource.h>

using namespace sdsl;
using namespace std;

// Medición de memoria (getrusage)
double get_memory_usage_mb() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss / 1024.0;
}

int main(int argc, char **argv) {
    if (argc < 5) {
        cout << "Usage: " << argv[0] << " <folder_name> <t_construct.csv> <t_search.csv> <reps>" << endl;
        return 1;
    }

    string folder = argv[1];
    string cons_file = argv[2];
    string time_file = argv[3];
    size_t reps = stoi(argv[4]);

    FMIndex fm_index;

    // ------- Construcción --------
    double total_time = 0.0;

    auto start = chrono::high_resolution_clock::now();
    fm_index.build(folder);
    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double> elapsed = end - start;
    total_time = elapsed.count();
    double mem_usage = get_memory_usage_mb();

    ofstream cons(cons_file);
    cons << "tiempo_t,memoria_t_mb" << endl;
    cons << total_time << "," << mem_usage << endl;
    cons.close();

    // ------- Búsqueda --------
    vector<string> patrones = {"print"};

    ofstream timef(time_file);
    timef << "Patron,Longitud,Media(s),Varianza(s^2),cant_matches" << endl;

    vector<size_t> result;

    for (const string &pat : patrones) {
        vector<double> tiempos;
        tiempos.reserve(reps);

        // Ejecutar reps repeticiones
        for (size_t i = 0; i < reps; ++i) {
            auto s_start = chrono::high_resolution_clock::now();
            result = fm_index.doc_locate(pat);
            auto s_end = chrono::high_resolution_clock::now();

            double t = chrono::duration<double>(s_end - s_start).count();
            tiempos.push_back(t);
        }

        // ---- Calcular media ----
        double suma = 0.0;
        for (double t : tiempos)
            suma += t;
        double media = suma / reps;

        // ---- Calcular varianza ----
        double suma_var = 0.0;
        for (double t : tiempos)
            suma_var += (t - media) * (t - media);
        double varianza = suma_var / reps; // varianza poblacional

        // ---- Guardar en CSV ----
        timef << pat << "," << pat.size() << "," << media << "," << varianza << "," << result.size() << endl;
    }

    timef.close();

    for (const auto &pat : patrones) {
        cout << "Apariciones del patrón " << pat << ":" << endl;
        for (const auto &occD : result) {
            cout << "Documento " << occD + 1 << "; ";
        }
    }

    cout << endl;
    cout << "Resultados guardados en " << cons_file << " y " << time_file << endl;
    return 0;
}
