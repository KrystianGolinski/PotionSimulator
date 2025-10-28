#include "Psykg5FileManager.h"

bool Psykg5FileManager::saveDataToFile(const std::string& filename, const std::string& data)
{
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
        return false;
    }

    file << data;
    file.close();
    std::cout << "Data successfully saved to " << filename << std::endl;
    return true;
}

bool Psykg5FileManager::loadDataFromFile(const std::string& filename, std::string& data)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for reading: " << filename << std::endl;
        return false;
    }

    data.clear();
    std::string line;
    while (std::getline(file, line)) {
        data += line + "\n";
    }

    file.close();
    return true;
}