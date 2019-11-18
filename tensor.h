/**
 * @file tensor.h
 * @author m.shebanow
 * @date 11/09/2019
 * @brief general 3D tensor data type.
 */
#ifndef _TENSORH_
#define _TENSORH_
#include <iostream>
#include <fstream>
#include <assert.h>
#include <sstream>
#include <string>
#include <type_traits>
#include "vector.h"
#include "matrix.h"

// Tensor_t class
template <typename T>
class Tensor_t {
public:
	// constructor
	Tensor_t(int _W = 1, int _H = 1, int _D = 1) : W(_W), H(_H), D(_D), len(_W * _H * _D) {
		data = new T[len];
		T *dst = data;

		for (int i = 0; i < W; i++)
			for (int j = 0; j < H; j++)
				for (int k = 0; k < D; k++)
					*dst++ = 0;
	}

	// generic copy constructor
	template <typename U> friend class Tensor_t;
	template <typename U>
	Tensor_t(const Tensor_t<U>& t) : W(t.W), H(t.H), D(t.D), len(t.len) { 
		data = new T[t.length()];
		T *dst = data;
		U *src = t.data;

		for (int i = 0; i < W; i++)
			for (int j = 0; j < H; j++)
				for (int k = 0; k < D; k++)
					*dst++ = (T) *src++;
	}

	// destructor
	virtual ~Tensor_t() { delete[] data; }

	// dimension methods
	inline const int width() const { return W; }
	inline const int height() const { return H; }
	inline const int depth() const { return D; }
	inline const int length() const { return len; }
	
	// reference to a tensor member
	inline T& operator()(int i, int j, int k) const { 
		assert(i >= 0 && i < W && j >= 0 && j < H && k >= 0 && k < D); 
		return data[((k * H) + j) * W + i]; 
	}

	// Extract a subtensor slice from tensor of spatial face h by w, full depth
	Tensor_t<T> extractSubtensor(const int ii, const int jj, const int kk, const int w, const int h, const int d) const {
		Tensor_t<T> result(w, h, d);

		// parameter check
		assert(ii >= 0 && jj >= 0 && kk >= 0 &&								// offsets >= 0
			ii < W && jj < H && kk < D && 									// offsets less than dimensions
			w >= 1 && h >= 1 && d >= 1 && 									// output dimensions positive and non-zero
			(ii + w) <= W && (jj + h) <= H && (kk + d) <= D);				// output not sliced beyond limits

		// We copy from rows innermost, to columns, to depth. 
		// Since we are extracting a partial row, we align to start of next offset when copy is complete (when we started, we were at ii offset; 
		// we increment during the copy to now be at "ii + w" offset - row end is at "W - ii - w" distance; we want to take that distance and
		// then add ii to offset at the next row => "W - ii - w + ii" = "W - w" adder. Since we are copying complete depth, we don't need to do this
		// for the loop after stride H. 
		for (int i = 0; i < w; i++)
			for (int j = 0; j < h; j++) 
				for (int k = 0; k < d; k++) 
					result(i, j, k) = (*this)(ii+i, jj+j, kk+k);
		return result;
	}

	// Dot two tensors (dot product)
	T dot(const Tensor_t<T>& t) const {
		T sum = 0;
		T *src1 = t.data;
		T *src2 = data;

		assert(W == t.W && H == t.H && D == t.D);
		for (int k = 0; k < D; k++)
			for (int j = 0; j < H; j++)
				for (int i = 0; i < W; i++)
					sum += *src1++ * *src2++;
		return sum;
	}

	// generate a string with geometry info
    std::string operator() () const {
	   	static std::ostringstream buffer;
	    buffer << "[" << W << "," << H << "," << D << "]";
	    return buffer.str();
    }

    // CSV dump
    void csvDump(std::ostream& os, const char* name = NULL) {
    	// print optional array name
    	if (name != NULL)
    		os << name << std::endl;

    	// print header
    	os << std::dec;
     	os << "WH";
	   	for (int k = 0; k < D; k++)
    		os << "," << k;
    	os << std::endl;

    	// print tensor
    	for (int i = 0; i < W; i++) {
    		for (int j = 0; j < H; j++) {
    			os << i << "x" << j;
		    	for (int k = 0; k < D; k++) {
		    		if (std::is_floating_point<T>::value)
		    			os << "," << (*this)(i, j, k);
		    		else
		    			os << "," << (int) (*this)(i, j, k);
		    	}
		    	os << std::endl;
	   		}
    	}
    }

    // define pointer type
    typedef Tensor_t *TensorPtr_t;

	// friend function to serialize a Tensor into a vector
    template <typename U>
	friend void serializeTensor2Vector(Vector_t<U> &, const Tensor_t<U> &);

private:
	T *data;
	const int W, H, D;
	const int len;

	template <typename TT> friend class Vector_t;
	template <typename TT> friend class Matrix_t;
}; 

/********************* Tensor_t Friend Functions ********************/
// friend function to serialize a Tensor into a vector
// The data in a tensor is already serialized by row-col-depth, so all we have to do is copy it. 
// (a bit of overhead since we zero-initialize a vector in the constructor)
template <typename T> 
inline void serializeTensor2Vector(Vector_t<T>& dst, const Tensor_t<T>& src) {
	assert(dst.length() == src.length());
	memcpy(dst.data, src.data, src.length());
}

/********************* TensorArray_t class ********************/
template <typename T>
class TensorArray_t {
public:
	// Constructor
	TensorArray_t(int _N = 1, int _W = 1, int _H = 1, int _D = 1) : N(_N), W(_W), H(_H), D(_D), len(_W * _H * _D) {
		array = new typename Tensor_t<T>::TensorPtr_t[N];
		for (int i = 0; i < N; i++)
			array[i] = new Tensor_t<T>(W, H, D);
	}

	// generic copy constructor
	template <typename U> friend class TensorArray_t;
	template <typename U>
	TensorArray_t(const TensorArray_t<U>& t) : N(t.N), W(t.W), H(t.H), D(t.D), len(t.len) { 
		array = new typename Tensor_t<T>::TensorPtr_t[t.N];
		for (int i = 0; i < N; i++)
			array[i] = new Tensor_t<T>(*t.array[i]);
	}

	// Destructor
	virtual ~TensorArray_t() {
		for (int i = 0; i < N; i++)
			delete array[i];
		delete[] array;
	}

	// reference to a tensor in the array
	inline Tensor_t<T>& operator[] (int i) { return *array[i]; }

	// dimension methods
	inline const int count() const { return N; }
	inline const int width() const { return W; }
	inline const int height() const { return H; }
	inline const int depth() const { return D; }
	inline const int length() const { return len; }

	// CSV dump
    void csvDump(std::ostream& os, const char* name = NULL) {
    	// print optional array name
    	if (name != NULL)
    		os << name << std::endl;

    	// print header
     	os << "N,WH";
	   	for (int k = 0; k < D; k++)
    		os << "," << k;
    	os << std::endl;

    	// print tensor
    	for (int n = 0; n < N; n++) {
	    	for (int i = 0; i < W; i++) {
	    		for (int j = 0; j < H; j++) {
	    			os << n << "," << i << "x" << j;
			    	for (int k = 0; k < D; k++)
			    		if (std::is_floating_point<T>::value)
			    			os << "," << (*this)[n](i, j, k);
			    		else
			    			os << "," << (int) (*this)[n](i, j, k);
			    	os << std::endl;
		   		}
	    	}
	    }
    }

	// generate a string with geometry info
    std::string operator() () const {
	   	static std::ostringstream buffer;
	    buffer << N << " X [" << W << "," << H << "," << D << "]";
	    return buffer.str();
    }

private:
	typename Tensor_t<T>::TensorPtr_t *array;
	const int W, H, D;
	const int len;
	const int N;
}; 

#endif // _TENSORH_
