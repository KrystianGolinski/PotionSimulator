#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

class Psykg5FileManager
{
public:
    static bool saveDataToFile(const std::string& filename, const std::string& data);
    static bool loadDataFromFile(const std::string& filename, std::string& data);
};