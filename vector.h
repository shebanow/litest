/**
 * @file vector.h
 * @author m.shebanow
 * @date 11/09/2019
 * @brief basic vector data type.
 */
#ifndef _VECTOR_H_
#define _VECTOR_H_
#include <assert.h>
#include <sstream>
#include <string>

// class forward declarations.
template <typename T> class Matrix_t;
template <typename T> class Tensor_t;

// Vector_t class 
template <typename T>
class Vector_t {
public:
	// Vector Constructor
	Vector_t(int _W) : W(_W) { 
		data = new T[W];
		for (int i = 0; i < W; i++)
			data[i] = 0;
	}

	// Vector Copy Constructor
	Vector_t(const Vector_t<T>& t) : W(t.W) { 
		data = new T[W];
		for (int i = 0; i < W; i++)
			data[i] = t.data[i];
	}

	// Vector destructor
	virtual ~Vector_t() { delete data; }

	// return a reference to a tensor member
	inline T& operator()(int i) { assert(i >= 0 && i < W); return data[i]; }
	inline T& operator[](int i) { assert(i >= 0 && i < W); return data[i]; }

	// dimension methods
	inline const int width() const { return W; }
	inline const int length() const { return W; }

	// Vector Multiply vector * scalar
	Vector_t<T> operator* (const T scalar) const {
		Vector_t<T> result(W); 

		for (int i = 0; i < W; i++)
			result.data[i] = data[i] * scalar;
		return result;
	}

	// Vector Dot product
	T operator* (const Vector_t<T>& v) const {
		T accumulator = 0;

		assert(W == v.W);
		for (int i = 0; i < W; i++)
			accumulator += data[i] * v.data[i];
		return accumulator;
	}

	// Vector: Add a vector to another
	Vector_t<T> operator+ (const Vector_t<T>& v) const {
		Vector_t<T> result(W); 

		assert(W == v.W);
		for (int i = 0; i < W; i++)
			result.data[i] = data[i] + v.data[i];
		return result;
	}

	// Generate a string with vector geoemtry
	std::string operator() () const {
	   	static std::ostringstream buffer;
	    buffer << "[" << W << "]";
	    return buffer.str();
	}

	// set vector to constant
	Vector_t<T>& setVec2constant(const T value) {
		for (int i = 0; i < W; i++)
			data[i] = value;
		return *this;
	}

	// extract vector slice
	template <typename U>
	Vector_t<T>& extractVecSlice(const Vector_t<U>& src, const int offset, const int len) {
		assert(offset >= 0 && (offset + len) <= src.W && len > 0);
		for (int i = 0; i < len; i++)
			data[i] = src.data[i + offset];
		return *this;
	}

    // define pointer type
    typedef Vector_t *VectorPtr_t;

    // allow serialization friend function access to private data
    template <typename U>
	friend void serializeTensor2Vector(Vector_t<U> &, const Tensor_t<U> &);

private:
	T *data;
	const int W;

	template <typename TT> friend class Matrix_t;
	template <typename TT> friend class Tensor_t;
};

// VectorArray_t class
template <typename T>
class VectorArray_t {
public:
	// Constructor
	VectorArray_t(int _N = 1, int _W = 1) : N(_N), W(_W), len(_W) {
		array = new typename Vector_t<T>::VectorPtr_t[N];
		for (int i = 0; i < N; i++)
			array[i] = new Vector_t<T>(W);
	}

	// generic copy constructor
	template <typename U> friend class VectorArray_t;
	template <typename U>
	VectorArray_t(const VectorArray_t<U>& v) : N(v.N), W(v.W), len(v.len) { 
		array = new typename Vector_t<T>::VectorPtr_t[v.N];
		for (int i = 0; i < N; i++)
			array[i] = new Vector_t<T>(*v.array[i]);
	}

	// Destructor
	virtual ~VectorArray_t() {
		for (int i = 0; i < N; i++)
			delete array[i];
		delete[] array;
	}

	// reference to a tensor in the array
	inline Vector_t<T>& operator[] (int i) { return *array[i]; }

	// dimension methods
	inline const int count() const { return N; }
	inline const int width() const { return W; }
	inline const int length() const { return len; }

	// generate a string with geometry info
    std::string operator() () const {
	   	static std::ostringstream buffer;
	    buffer << N << " X [" << W << "]";
	    return buffer.str();
    }

private:
	typename Vector_t<T>::VectorPtr_t *array;
	const int W;
	const int len;
	const int N;
};

#endif // _VECTOR_H_
