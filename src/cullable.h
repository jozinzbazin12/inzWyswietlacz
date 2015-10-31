/*
 * cullable.h
 *
 *  Created on: 25 paü 2015
 *      Author: Grzegurz
 */

#ifndef SRC_CULLABLE_H_
#define SRC_CULLABLE_H_

class Cullable {
public:
	double range;

	virtual double* getPos()=0;

	virtual ~Cullable() {

	}
};

#endif /* SRC_CULLABLE_H_ */
