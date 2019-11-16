/**
 * @file tensor.h
 * @author m.shebanow
 * @date 11/09/2019
 * @brief general 3D tensor data type.
 */
#ifndef _TENSORH_
#define _TENSORH_
#include <assert.h>
#include <string>
#include "vector.h"
#include "matrix.h"

using namespace std;

/********************* Tensor_t class ********************/
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

	// dimension methods
	inline const int width() { return W; }
	inline const int length() { return W; }

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
	
	//!< reference to a tensor member
	inline T& operator()(int i, int j, int k) { 
		assert(i >= 0 && i < W && j >= 0 && j < H && k >= 0 && k < D); 
		return data[((k * H) + j) * W + i]; 
	}

	//!< Extract a row vector slice from tensor
	Vector_t<T> rowVector(const int j, const int k) const {
		Vector_t<T> result(W);
		T *dst = result.data;
		T *src = &data[((k * H) + j) * W];

		assert(j >= 0 && j < H && k >= 0 && k < D);
		for (int i = 0; i < W; i++)
			*dst++ = *src++;
	}

	//!< Extract a column vector slice from tensor
	Vector_t<T> colVector(const int i, const int k) const {
		Vector_t<T> result(H);
		T *dst = result.data;
		T *src = &data[k * H * W + i];
		int stride = W;

		assert(i >= 0 && i < W && k >= 0 && k < D);
		for (int j = 0; j < H; j++) {
			*dst++ = *src;
			src += stride;
		}
		return result;
	}

	//!< Extract a depth vector slice from tensor
	Vector_t<T> depthVector(const int i, const int j) const {
		Vector_t<T> result(D);
		T *dst = result.data;
		T *src = &data[j * W + i];
		int stride = H * W;

		assert(i >= 0 && i < W && j >= 0 && j < H);
		for (int k = 0; k < D; k++) {
			*dst++ = *src;
			src += stride;
		}
		return result;
	}

	//!< Extract a subtensor slice from tensor of spatial face h by w, full depth
	Tensor_t<T> extractSpatialSubtensor(const int ii, const int jj, const int w, const int h) const {
		Tensor_t<T> result(w, h, D);
		T *dst = result.data;
		T *src = &data[jj * W + ii];
		int stride = W - w;

		// parameter check
		assert(ii >= 0 && ii < W && jj >= 0 && jj < H && w >= 1 && (ii + w) <= W && (jj + h) <= H);

		// We copy from rows innermost, to columns, to depth. 
		// Since we are extracting a partial row, we align to start of next offset when copy is complete (when we started, we were at ii offset; 
		// we increment during the copy to now be at "ii + w" offset - row end is at "W - ii - w" distance; we want to take that distance and
		// then add ii to offset at the next row => "W - ii - w + ii" = "W - w" adder. Since we are copying complete depth, we don't need to do this
		// for the loop after stride H. 
		for (int k = 0; k < D; k++) {
			for (int j = jj; j < (jj + h); j++) {
				for (int i = ii; i < (ii + w); i++)
					*dst++ = *src++;
				src += stride;
			}
		}
		return result;
	}

	//!< Convolve two tensors (dot product)
	T convolve(const Tensor_t<T>& t) const {
		T sum = 0;
		T *src1 = t.data;
		T *src2 = data;

		assert(W = t.W && H == t.H && D == t.D);
		for (int k = 0; k < D; k++)
			for (int j = 0; j < H; j++)
				for (int i = 0; i < W; i++)
					sum += *src1++ * *src2++;
		return sum;
	}

	// generate a string with geometry info
    string operator() () const {
	    char buffer[64];

	    sprintf(buffer, "[%d,%d,%d]", W, H, D);
	    return string(buffer);
    }

    // define pointer type
    typedef Tensor_t *TensorPtr_t;

private:
	T *data;
	const int W, H, D;
	const int len;
}; 

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

	// generate a string with geometry info
    string operator() () const {
	    char buffer[64];

	    sprintf(buffer, "%d X [%d,%d,%d]", N, W, H, D);
	    return string(buffer);
    }

private:
	typename Tensor_t<T>::TensorPtr_t *array;
	const int W, H, D;
	const int len;
	const int N;
}; 

#endif // _TENSORH_
