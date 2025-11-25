#pragma once

#include <string>
#include <vector>

// Delimitador de patrones
const std::string DELIMITER = "$$$$";
const std::string CONCAT_FILE = "concat.txt";

// Validar argumentos y guardarlos
void validate_args(int argc, char *argv[], std::string &docs_path, std::string &patterns_path, std::string &csv_path,
                   unsigned &reps);

// Cargar patrones desde archivo
std::vector<std::string> load_patterns(const std::string dir);
std::vector<std::string> load_patterns_from_file(const std::string &filename);

// Concatenar archivos y retornar concatenación junto offsets
std::pair<std::string, std::vector<size_t>> concat_files(const std::string &carpeta);


