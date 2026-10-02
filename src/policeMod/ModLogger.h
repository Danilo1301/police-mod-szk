#pragma once

#include <mutex>
#include <string>

// class ModLogger
// {
//   public:
//     bool debugEnabled = true;

//     // Construtor: nome do mod será o nome do arquivo de log
//     ModLogger(const std::string &modName);

//     // Métodos de log
//     void Log(const std::string &message);   // INFO
//     void Warn(const std::string &message);  // WARN
//     void Error(const std::string &message); // ERROR
//     void Debug(const std::string &message);

//     void Clear();

//   private:
//     std::string logFolder; // pasta de logs
//     std::string logFile;   // arquivo de log
//     std::mutex mtx;        // para threads

//     // Função auxiliar que escreve no arquivo
//     void Write(const std::string &level, const std::string &message);
// };