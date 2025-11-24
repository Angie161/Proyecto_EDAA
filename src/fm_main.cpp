// ./build/fm_exec <folder_name> <t_construct.csv> <t_search.csv> <reps>
// ./build/fm_exec datasets/sources construc.csv busqueda.csv 1

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

// Cargar patrones desde folder/patterns/patterns.txt
vector<string> load_patterns(const string &folder) {
    string pattern_file = folder + "/patterns/patterns.txt";

    ifstream in(pattern_file);
    if (!in.is_open()) {
        cerr << "ERROR: No se pudo abrir archivo de patrones: " 
             << pattern_file << endl;
        exit(1);
    }

    string content((istreambuf_iterator<char>(in)),
                    istreambuf_iterator<char>());
    in.close();

    vector<string> patterns;
    string separator = "$$$$$";
    size_t pos = 0;

    while (true) {
        size_t next = content.find(separator, pos);
        if (next == string::npos) break;

        string pat = content.substr(pos, next - pos);
        if (!pat.empty())
            patterns.push_back(pat);

        pos = next + separator.size();
    }

    return patterns;
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

    // ------- Cargar patrones --------
    vector<string> patrones = load_patterns(folder);
    cout << "Se cargaron " << patrones.size() << " patrones desde " << folder << "/patterns/patterns.txt\n";

    // ------- Búsqueda --------
    ofstream timef(time_file);
    timef << "Longitud_Pat,Media(s),Varianza(s^2),cant_matches" << endl;

    // Guarda TODOS los resultados de todos los patrones
    vector<vector<size_t>> all_results;

    for (const string &pat : patrones) {
        vector<double> tiempos;
        tiempos.reserve(reps);

        vector<size_t> result;

        for (size_t i = 0; i < reps; ++i) {
            auto s_start = chrono::high_resolution_clock::now();
            result = fm_index.doc_locate(pat);
            auto s_end = chrono::high_resolution_clock::now();

            double t = chrono::duration<double>(s_end - s_start).count();
            tiempos.push_back(t);
        }

        // Calcular media
        double suma = 0.0;
        for (double t : tiempos)
            suma += t;
        double media = suma / reps;

        // Calcular varianza
        double suma_var = 0.0;
        for (double t : tiempos)
            suma_var += (t - media) * (t - media);
        double varianza = suma_var / reps;

        // Guardar en CSV
        timef << pat.size() << "," << media << "," << varianza << "," << result.size() << endl;

        // GUARDAR EL RESULTADO DEL PATRÓN ACTUAL
        all_results.push_back(result);
    }

    timef.close();

    // ------- Imprimir todas las apariciones por patrón -------
    for (size_t i = 0; i < all_results.size(); i++) {
        cout << "Apariciones del patrón " << i +1 << ": ";
        for (auto doc : all_results[i]) {
            cout << "Documento " << doc + 1 << "; ";
        }
        cout << endl;
    }

    cout << "Resultados guardados en " << cons_file << " y " << time_file << endl;
    return 0;
}
