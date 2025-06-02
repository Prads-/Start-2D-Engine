#ifndef DYNAMIC_MULTIDIMENSIONAL_ARRAY_H
#define DYNAMIC_MULTIDIMENSIONAL_ARRAY_H

#include "declarations.h"

namespace DynamicMultidimensionalArray {
	template<typename T>
	T **allocDynamicMultidimensionalArray(int row, int column) {
		T **retArray;

		retArray = new T*[column];
		for (int i = 0; i < column; ++i) {
			retArray[i] = new T[row];
		}

		return retArray;
	}

	template<typename T>
	void freeDynamicMultidimensionalArray(T *dmdArray, int column) {
		for (int i = 0; i < column; ++i) {
			delete [] dmdArray[i];
		}
		delete [] dmdArray;
	}
}

#endif