//
// Created by arczipt on 29.09.2020.
//

#include <fstream>
#include "ArrayReader.h"

std::vector<std::vector<double>> ArrayReader::read(const std::string& path, bool isScalar){
    std::ifstream file(path);

    int rows=0, cols=0;

    std::string line;
    std::getline(file, line, ' ');
    rows = std::stoi(line);
    std::getline(file, line, '\n');
    cols = std::stoi(line);

    if(!isScalar)
        cols *= 2;

    auto ret = std::vector<std::vector<double>>(rows, std::vector<double>(cols, 0));

    for(int i=0; i<rows; i++){
        for(int j=0; j<cols; j++){
            if(j == cols-1)
                std::getline(file, line, '\n');
            else
                std::getline(file, line, ' ');

            ret[i][j] = std::stod(line);
        }
    }

    return ret;
}