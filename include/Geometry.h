#pragma once

#include <vector>
#include <cmath>
#include <iostream>
#include <cassert>
#include <limits>
#include <algorithm>

namespace TRM
{

template <size_t DIM, typename T>
class vec
{
  public:
    vec()
    {
        for (size_t i = 0; i < DIM; i++)
            data[i] = static_cast<T>(0);
    }

    vec(const vec<DIM, T> &v1)
    {
        for (size_t i = 0; i < DIM; i++)
            data[i] = v1[i];
    }

    virtual ~vec() {}

    template <size_t COL>
    vec(const vec<COL, T> &v1)
    {
        size_t max = std::min(DIM, COL);
        for (size_t i = 0; i < max; i++)
            data[i] = v1[i];
    }

    T &operator[](size_t index)
    {
        assert(index < DIM);
        return data[index];
    }

    const T &operator[](size_t index) const
    {
        assert(index < DIM);
        return data[index];
    }

    const vec<DIM, T> &operator=(const vec<DIM, T> &v1)
    {
        for (size_t i = 0; i < DIM; i++)
            data[i] = v1[i];
        return *this;
    }

    T norm() {
        T res(0);
        for(size_t i = 0; i < DIM; i++) {
            res += data[i] * data[i];
        }
        return std::sqrt(res);
    }

    vec<DIM, T>& normalize() {
        *this = (*this)*(static_cast<T>(1) /norm());
        return *this;
    }

  protected:
    T data[DIM];
};

template <size_t ROW, size_t COL, typename T>
class mat
{
  public:
    mat() {}
    mat(const mat<ROW, COL, T>& m1) {
        for(size_t i = 0; i < ROW; i++) {
            for(size_t j = 0; j < COL; j++){
                data[i][j] = m1[i][j];
            }
        }
    }

    ~mat() {}

    void identity() {
        for(size_t i = 0; i < ROW; i++)
            for(size_t j = 0; j < COL; j++)
                if (i == j) data[i][j] = static_cast<T>(1);
    }

    vec<COL, T>& operator[](const size_t index) {
        return data[index];
    }

    const vec<COL, T>& operator[](const size_t index) const {
        return data[index];
    }

    const mat<ROW, COL, T>& operator=(const mat<ROW, COL, T>& m1) {
        for(size_t i = 0; i < ROW; i++) {
            for(size_t j = 0; j < COL; j++){
                data[i][j] = m1[i][j];
            }
        }
        return *this;
    }

  protected:
    vec<COL, T> data[ROW];
};

template <size_t U, typename V>
vec<U, V> operator+(const vec<U, V>& v1, const vec<U, V>& v2) {
	vec<U, V> res;

	for (size_t i = 0; i < U; i++) {
		res[i] = v1[i] + v2[i];
	}

	return res;
}

template <size_t U, typename V>
vec<U, V> operator-(const vec<U, V>& v1, const vec<U, V>& v2) {
	vec<U, V> res;

	for (size_t i = 0; i < U; i++) {
		res[i] = v1[i] - v2[i];
	}

	return res;
}

template <size_t U, typename V>
V operator*(const vec<U, V> &v1, const vec<U, V> &v2)
{
    V res(0);

    for (size_t i = 0; i < U; i++)
        res += v1[i] * v2[i];

    return res;
}

template <size_t U, typename V>
vec<U, V> operator*(const vec<U, V> &v1, V n)
{
    vec<U, V> res;
    for (size_t i = 0; i < U; i++)
        res[i] = v1[i] * n;
    return res;
}

template <size_t U, typename V>
vec<U, V> operator*(V n, const vec<U, V> &v1)
{
    vec<U, V> res;
    for (size_t i = 0; i < U; i++)
        res[i] = v1[i] * n;
    return res;
}


template <size_t ROW, size_t COL, typename T>
vec<COL, T> operator* (const mat<ROW, COL, T>& m1, const vec<COL, T>& v1)
{
    vec<COL, T> res;

    for(size_t j = 0; j < COL; j++) {
        for(size_t i = 0; i < ROW; i++) {
            res[j] += m1[j][i] * v1[i];
        }
    }
    
    return res;
}

template <size_t ROW, size_t COL, typename T>
vec<COL, T> operator* (const vec<ROW, T>& v1, const mat<ROW, COL, T>& m1) {
    vec<COL, T> res;
    for(size_t i = 0; i < COL; i++) {
        for(size_t j = 0; j < ROW; j++) {
            res[i] += v1[j] * m1[j][i];
        }
    }

    return res;
}

template<typename T>
vec<3, T> cross(const vec<3, T>& v1, const vec<3, T>& v2) {
    vec<3, T> res;
    res[0] = v1[1] * v2[2] - v1[2] * v2[1];
    res[1] = v1[2] * v2[0] - v1[0] * v2[2];
    res[2] = v1[0] * v2[1] - v1[1] * v2[0];
    return res;
}

template <size_t U, size_t V, size_t W, typename T>
mat<U, W, T> operator*(const mat<U, V, T>& m1, const mat<V, W, T>& m2) {
    mat<U, W, T> res;
    for(size_t i = 0; i < U; i++) {
        for(size_t j = 0; j < V; j++) {
            for(size_t k = 0; k < W; k++) {
                res[i][j] += m1[i][k] * m2[k][j];
            }
        }
    }

    return res;
}

template <typename T>
vec<3, T> operator^(const vec<3, T>& v1, const vec<3, T>& v2) {
    vec<3, T> ret;
    T l = v1[0];
    T m = v1[1];
    T n = v1[2];
    T o = v2[0];
    T p = v2[1];
    T q = v2[2];

    ret[0] = (m * q - n * p);
    ret[1] = (n * o - l * q);
    ret[2] = (l * p - m * o);
    return ret;
};

template <size_t DIM,typename T>
vec<3, float> barycentric(const vec<DIM, T>& a, const vec<DIM, T>& b, const vec<DIM, T>& c, const vec<DIM, T>& p) {
	assert(DIM >= 2);

    vec<3, float> res;

	/*
		P = (1 - u - v) x A + u x B + v x C 
	    AP = u x AC + v x AB 
		u x AC + v x AB + PA = 0
		u x (c - a) + v x (b - a) + (a - p) = 0

		| u x (c - a)[0] + v x (b - a)[0] + (a - p)[0] = 0
		| u x (c - a)[1] + v x (b - a)[1] + (a - p)[1] = 0
	*/
	vec<DIM, T> AC = c - a;
	vec<DIM, T> AB = b - a;
	vec<DIM, T> PA = a - p;
	vec<3, float> v1, v2;
	
	v1[0] = AC[0];
	v1[1] = AB[0];
	v1[2] = PA[0];

	v2[0] = AC[1];
	v2[1] = AB[1];
	v2[2] = PA[1];

	res = cross(v1, v2);
	vec<3, float> u;
	if (std::abs(res[2] > 1e-2)) {
		u[0] = 1.f - (float)(res[0] + res[1]) / float(res[2]);
		u[1] = float(res[1]) / float(res[2]);
		u[2] = float(res[0]) / float(res[2]);
	}
	else {
		u[0] = -1.f;
		u[1] = -1.f;
		u[2] = -1.f;
	}
	return u;
}


template <size_t DIM, typename T>
void bbox(vec<2, T>& leftTop, vec<2, T>& rightBottom, vec<DIM, T>* pts) {
	if (pts == nullptr) return;

	leftTop[0] = std::numeric_limits<T>::max();
	leftTop[1] = std::numeric_limits<T>::max();

	rightBottom[0] = -std::numeric_limits<T>::max();
	rightBottom[1] = -std::numeric_limits<T>::max();

	for (size_t i = 0; i < 3; i++) {
		for (size_t j = 0; j < 2; j++) {
			leftTop[j] = std::max(static_cast<T>(0), std::min(leftTop[j], pts[i][j]));
			rightBottom[j] = std::min(std::numeric_limits<T>::max(), std::max(rightBottom[j], pts[i][j]));
		}
	}
}

typedef vec<2, float> Vec2f;
typedef vec<3, float> Vec3f;
typedef vec<4, float> Vec4f;
typedef vec<2, int> Vec2i;
typedef vec<3, int> Vec3i;
typedef vec<4, int> Vec4i;
typedef mat<4, 4, float> Mat44;
}
