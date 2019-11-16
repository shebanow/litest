/**
 * @file vector.h
 * @author m.shebanow
 * @date 11/09/2019
 * @brief basic vector data type.
 */
#ifndef _VECTOR_H_
#define _VECTOR_H_
#include <assert.h>
#include <string>

using namespace std;

template <typename T>
class Vector_t {
public:
	//!< Vector Constructor
	Vector_t(int _W) : W(_W) { 
		data = new T[W];
		for (int i = 0; i < W; i++)
			data[i] = 0;
	}

	//!< Vector Copy Constructor
	Vector_t(const Vector_t<T>& t) : W(t.W) { 
		data = new T[W];
		for (int i = 0; i < W; i++)
			data[i] = t.data[i];
	}

	virtual ~Vector_t() { delete data; }

	// return a reference to a tensor member
	inline T& operator()(int i) { assert(i >= 0 && i < W); return data[i]; }
	inline T& operator[](int i) { assert(i >= 0 && i < W); return data[i]; }

	// dimension methods
	inline const int width() const { return W; }
	inline const int length() const { return W; }

	//!< Vector Multiply vector * scalar
	Vector_t<T> operator* (const T scalar) const {
		Vector_t<T> result(W); 

		for (int i = 0; i < W; i++)
			result.data[i] = data[i] * scalar;
		return result;
	}

	//!< Vector Dot product
	T operator* (const Vector_t<T>& v) const {
		T accumulator = 0;

		assert(W == v.W);
		for (int i = 0; i < W; i++)
			accumulator += data[i] * v.data[i];
		return accumulator;
	}

	//!< Vector: Add a vector to another
	Vector_t<T> operator+ (const Vector_t<T>& v) const {
		Vector_t<T> result(W); 

		assert(W == v.W);
		for (int i = 0; i < W; i++)
			result.data[i] = data[i] + v.data[i];
		return result;
	}

	//!< Generate a string with vector geoemtry
	string operator() () const {
	    static char buffer[16];

	    sprintf(buffer, "[%d]", W);
	    return string(buffer);
	}

private:
	T *data;
	const int W;

	template <typename TT> friend class Matrix_t;
	template <typename TT> friend class Tensor_t;
};

#endif // _VECTOR_H_
