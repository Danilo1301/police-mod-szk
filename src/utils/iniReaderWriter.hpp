#pragma once

#include "aml-psdk/gta_base/Vector.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

class IniReaderWriter
{
  public:
    // ordem preservada dentro de cada seção
    using Section = std::vector<std::pair<std::string, std::string>>;
    using Data = std::unordered_map<std::string, Section>;

    // -------------------------------------------------------------
    // Load / Save
    // -------------------------------------------------------------

    bool LoadFromFile(const std::string &path)
    {
        std::ifstream file(path);
        if (!file.is_open())
            return false;

        std::stringstream ss;
        ss << file.rdbuf();
        return LoadFromString(ss.str());
    }

    bool SaveToFile(const std::string &path)
    {
        std::ofstream file(path);
        if (!file.is_open())
            return false;

        for (const auto &sectionPair : data)
        {
            file << "[" << sectionPair.first << "]\n";

            for (const auto &kv : sectionPair.second)
            {
                const std::string &value = kv.second;

                if (IsNumberOrBool(value))
                {
                    // salva sem aspas
                    file << kv.first << " = " << value << "\n";
                }
                else
                {
                    // salva com aspas
                    file << kv.first << " = \"" << value << "\"\n";
                }
            }

            file << "\n";
        }

        return true;
    }

    bool LoadFromString(const std::string &content)
    {
        data.clear();
        std::istringstream iss(content);
        std::string line, currentSection;

        while (std::getline(iss, line))
        {
            Trim(line);

            if (line.empty())
                continue;
            if (line[0] == '#' || line[0] == ';')
                continue;

            // seção
            if (line.front() == '[' && line.back() == ']')
            {
                currentSection = line.substr(1, line.size() - 2);
                Trim(currentSection);
                continue;
            }

            // chave=valor
            auto eq = line.find('=');
            if (eq == std::string::npos)
                continue;

            std::string key = line.substr(0, eq);
            std::string value = line.substr(eq + 1);

            Trim(key);
            Trim(value);

            // -----------------------------------------
            // REMOVER ASPAS EXTERNAS DE FORMA ROBUSTA
            // -----------------------------------------
            if (!value.empty() && value.front() == '"')
                value.erase(value.begin());

            if (!value.empty() && value.back() == '"')
                value.pop_back();

            // salvar no mapa
            Set(currentSection, key, value);
        }

        return true;
    }

    // -------------------------------------------------------------
    // Getters
    // -------------------------------------------------------------

    std::string Get(const std::string &section, const std::string &key, const std::string &def = "") const
    {
        auto sit = data.find(section);
        if (sit == data.end())
            return def;

        // procurar dentro do vector
        for (const auto &kv : sit->second)
        {
            if (kv.first == key)
                return kv.second;
        }

        return def;
    }

    int GetInt(const std::string &section, const std::string &key, int def = 0) const
    {
        std::string value = Get(section, key, "");
        if (value.empty())
            return def;

        try
        {
            return std::stoi(value);
        }
        catch (...)
        {
            return def;
        }
    }

    double GetDouble(const std::string &section, const std::string &key, double def = 0.0) const
    {
        std::string value = Get(section, key, "");
        if (value.empty())
            return def;

        try
        {
            return std::stod(value);
        }
        catch (...)
        {
            return def;
        }
    }

    CVector GetCVector(const std::string &section, const std::string &key, CVector def) const
    {
        float x = GetDouble(section, key + "_x", def.x);
        float y = GetDouble(section, key + "_y", def.y);
        float z = GetDouble(section, key + "_z", def.z);

        return CVector(x, y, z);
    }

    bool GetBool(const std::string &section, const std::string &key, bool def = false) const
    {
        std::string value = Get(section, key, "");
        if (value.empty())
            return def;

        std::transform(value.begin(), value.end(), value.begin(), ::tolower);

        return (value == "1" || value == "true");
    }

    // -------------------------------------------------------------
    // Setters (preservam a ordem)
    // -------------------------------------------------------------

    void Set(const std::string &section, const std::string &key, const std::string &value)
    {
        auto &list = data[section];

        // atualizar se já existir
        for (auto &kv : list)
        {
            if (kv.first == key)
            {
                kv.second = value;
                return;
            }
        }

        // adicionar no fim (mantém ordem)
        list.emplace_back(key, value);
    }

    void SetInt(const std::string &section, const std::string &key, int value)
    {
        Set(section, key, std::to_string(value));
    }

    void SetDouble(const std::string &section, const std::string &key, double value)
    {
        Set(section, key, std::to_string(value));
    }

    void SetBool(const std::string &section, const std::string &key, bool value)
    {
        Set(section, key, value ? "true" : "false");
    }

    void SetCVector(const std::string &section, const std::string &key, const CVector &value)
    {
        SetDouble(section, key + "_x", value.x);
        SetDouble(section, key + "_y", value.y);
        SetDouble(section, key + "_z", value.z);
    }

    Data *GetData() { return &data; }

  private:
    // trim
    static void Trim(std::string &s)
    {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
    }

    static bool IsNumberOrBool(const std::string &s)
    {
        if (s == "true" || s == "false" || s == "1" || s == "0")
            return true;

        // Tentar detectar número
        bool decimalFound = false;
        bool digitFound = false;

        for (size_t i = 0; i < s.size(); ++i)
        {
            char c = s[i];

            if (i == 0 && (c == '-' || c == '+'))
                continue;

            if (c == '.')
            {
                if (decimalFound)
                    return false;
                decimalFound = true;
                continue;
            }

            if (!std::isdigit(c))
                return false;

            digitFound = true;
        }

        return digitFound;
    }

    Data data;
};
