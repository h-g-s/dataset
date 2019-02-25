/*
 * Instance.hpp
 *
 *  Created on: 24 de fev de 2019
 *      Author: haroldo
 */

#ifndef DATASET_HPP_
#define DATASET_HPP_

#include <string>
#include <vector>

enum Datatype { Char=0, 
                Short, 
                Integer, 
                Float, 
                String, 
                Empty, 
                N_DATA_TYPES };

class Dataset {
public:
	Dataset(const char *fileName);
	virtual ~Dataset();

        std::vector< std::string > headers;

        std::vector< enum Datatype > cTypes;
        std::vector< size_t > cSizes; // in bytes
private:
        size_t rowSize;
        void *data;
        size_t rows;
};

#endif /* DATASET_HPP_ */
