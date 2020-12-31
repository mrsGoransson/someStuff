#pragma once
#include <math.h>
#include <assert.h>

namespace CU
{
	template <class T>
	class Vector4
	{
	public:
		T x{};
		T y{};
		T z{};
		T w{};


		Vector4<T>();
		Vector4<T>(const T& aX, const T& aY, const T& aZ, const T& aW);
		Vector4<T>(const Vector4<T>& aVector) = default;
		Vector4<T>& operator=(const Vector4<T>& aVector4) = default;

		//Returns the squared length of the vector
		T LengthSqr() const;
		//Returns the length of the vector
		T Length() const;
		//Returns a normalized copy of this
		Vector4<T> GetNormalized() const;
		//Normalizes the vector
		void Normalize();
		//Returns the dot product of this and aVector
		T Dot(const Vector4<T>& aVector) const;
	};

	//Returns the vector sum of aVector0 and aVector1
	template <class T> Vector4<T> operator+(const Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		Vector4<T> temp;

		temp.x = aVector0.x + aVector1.x;
		temp.y = aVector0.y + aVector1.y;
		temp.z = aVector0.z + aVector1.z;
		temp.w = aVector0.w + aVector1.w;

		return temp;
	}

	//Returns the vector difference of aVector0 and aVector1
	template <class T> Vector4<T> operator-(const Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		Vector4<T> temp;

		temp.x = aVector0.x - aVector1.x;
		temp.y = aVector0.y - aVector1.y;
		temp.z = aVector0.z - aVector1.z;
		temp.w = aVector0.w - aVector1.w;

		return temp;
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T> Vector4<T> operator*(const Vector4<T>& aVector, const T& aScalar)
	{
		Vector4<T> temp;

		temp.x = aVector.x * aScalar;
		temp.y = aVector.y * aScalar;
		temp.z = aVector.z * aScalar;
		temp.w = aVector.w * aScalar;

		return temp;
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T> Vector4<T> operator*(const T& aScalar, const Vector4<T>& aVector)
	{
		Vector4<T> temp;

		temp.x = aVector.x * aScalar;
		temp.y = aVector.y * aScalar;
		temp.z = aVector.z * aScalar;
		temp.w = aVector.w * aScalar;

		return temp;
	}

	//Returns the vector aVector divided by the scalar aScalar (equivalent to aVector multiplied by 1/aScalar)
	template <class T> Vector4<T> operator/(const Vector4<T>& aVector, const T& aScalar)
	{
		Vector4<T> temp;

		temp.x = aVector.x / aScalar;
		temp.y = aVector.y / aScalar;
		temp.z = aVector.z / aScalar;
		temp.w = aVector.w / aScalar;

		return temp;
	}

	//Equivalent to setting aVector0 to (aVector0 + aVector1)
	template <class T> void operator+=(Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		aVector0.x = aVector0.x + aVector1.x;
		aVector0.y = aVector0.y + aVector1.y;
		aVector0.z = aVector0.z + aVector1.z;
		aVector0.w = aVector0.w + aVector1.w;
	}

	//Equivalent to setting aVector0 to (aVector0 - aVector1)
	template <class T> void operator-=(Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		aVector0.x = aVector0.x - aVector1.x;
		aVector0.y = aVector0.y - aVector1.y;
		aVector0.z = aVector0.z - aVector1.z;
		aVector0.w = aVector0.w - aVector1.w;
	}

	//Equivalent to setting aVector to (aVector * aScalar)
	template <class T> void operator*=(Vector4<T>& aVector, const T& aScalar)
	{
		aVector.x = aVector.x * aScalar;
		aVector.y = aVector.y * aScalar;
		aVector.z = aVector.z * aScalar;
		aVector.w = aVector.w * aScalar;
	}

	//Equivalent to setting aVector to (aVector / aScalar)
	template <class T> void operator/=(Vector4<T>& aVector, const T& aScalar)
	{
		aVector.x = aVector.x / aScalar;
		aVector.y = aVector.y / aScalar;
		aVector.z = aVector.z / aScalar;
		aVector.w = aVector.w / aScalar;
	}


	template<class T>
	inline Vector4<T>::Vector4()
	{
		x = NULL;
		y = NULL;
		z = NULL;
		w = NULL;
	}

	template<class T>
	inline Vector4<T>::Vector4(const T& aX, const T& aY, const T& aZ, const T& aW)
	{
		x = aX;
		y = aY;
		z = aZ;
		w = aW;
	}

	template<class T>
	inline T Vector4<T>::LengthSqr() const
	{
		return ((x * x) + (y * y) + (z * z) + (w * w));
	}

	template<class T>
	inline T Vector4<T>::Length() const
	{
		T temp = ((x * x) + (y * y) + (z * z) + (w * w));

		return sqrt(temp);
	}

	template<class T>
	inline Vector4<T> Vector4<T>::GetNormalized() const
	{
		assert((x != 0 || y != 0 || z != 0 || w != 0) && "You are trying to divide by zero, you can not normalize a zero vector!");

		Vector4<T> temp;

		temp.x = x / sqrt((x * x) + (y * y) + (z * z) + (w * w));
		temp.y = y / sqrt((x * x) + (y * y) + (z * z) + (w * w));
		temp.z = z / sqrt((x * x) + (y * y) + (z * z) + (w * w));
		temp.w = w / sqrt((x * x) + (y * y) + (z * z) + (w * w));

		return temp;
	}

	template<class T>
	inline void Vector4<T>::Normalize()
	{
		assert((x != 0 || y != 0 || z != 0) && "You are trying to divide by zero, you can not normalize a zero vector!");

		T tempX = x;
		T tempY = y;
		T tempZ = z;
		T tempW = w;

		x = tempX / sqrt((tempX * tempX) + (tempY * tempY) + (tempZ * tempZ) + (tempW * tempW));
		y = tempY / sqrt((tempX * tempX) + (tempY * tempY) + (tempZ * tempZ) + (tempW * tempW));
		z = tempZ / sqrt((tempX * tempX) + (tempY * tempY) + (tempZ * tempZ) + (tempW * tempW));
		w = tempW / sqrt((tempX * tempX) + (tempY * tempY) + (tempZ * tempZ) + (tempW * tempW));
	}

	template<class T>
	inline T Vector4<T>::Dot(const Vector4<T>& aVector) const
	{
		return ((x * aVector.x) + (y * aVector.y) + (z * aVector.z) + (w * aVector.w));
	}

	using Vector4f = Vector4<float>;
	using Vector4i = Vector4<int>;
	using Vector4ui = Vector4<unsigned int>;
}

