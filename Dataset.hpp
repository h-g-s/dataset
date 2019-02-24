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

class Dataset {
public:
	Dataset(const char *fileName);
	virtual ~Dataset();

	std::vector<std::string> features;

};

#endif /* DATASET_HPP_ */
