//
// Created by arczipt on 29.09.2020.
//

#ifndef OILSPILL_ARRAYREADER_H
#define OILSPILL_ARRAYREADER_H


#include <vector>
#include <string>

class ArrayReader {
public:
    static std::vector<std::vector<double>> read(const std::string& path, bool isScalar = true);
};


#endif //OILSPILL_ARRAYREADER_H
