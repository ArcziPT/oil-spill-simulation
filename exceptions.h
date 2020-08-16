//
// Created by Mateusz Raczynski on 8/16/2020.
//

#ifndef OILSPILL_EXCEPTIONS_H
#define OILSPILL_EXCEPTIONS_H

#include <exception>

class FilePresentedException: std::exception {};
class InconsistentSizeException: std::exception {};

#endif //OILSPILL_EXCEPTIONS_H
