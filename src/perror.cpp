/*
 error.cpp

 Copyright (c) 2014 Terumasa Tadano

 This file is distributed under the terms of the MIT license.
 Please see the file 'LICENCE.txt' in the root directory 
 or http://opensource.org/licenses/mit-license.php for information.
*/

#include <iostream>
#include <cstdlib>
#include <string>
#include "perror.h"

using namespace PMA_NS;

PError::PError(PMA *pma): PPointers(pma) {}

PError::~PError() {}

void PError::warn(const char *file, const char *message)
{
    std::cout << " WARNING in " << file << "  MESSAGE: " << message << std::endl;
}

void PError::exit(const char *file, const char *message)
{
    std::cout << " ERROR in " << file << "  MESSAGE: " << message << std::endl;
    std::exit(EXIT_FAILURE);
}

void PError::exit(const char *file, const char *message, int info)
{
    std::cout << " ERROR in " << file << "  MESSAGE: " << message << info << std::endl;
    std::exit(EXIT_FAILURE);
}

void PError::exit(const char *file, const char *message, const char *info)
{
    std::cout << " ERROR in " << file << "  MESSAGE: " << message << info << std::endl;
    std::exit(EXIT_FAILURE);
}

