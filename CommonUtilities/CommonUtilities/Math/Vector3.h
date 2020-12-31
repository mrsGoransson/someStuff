#pragma once
#include <math.h>
#include <assert.h>

namespace CU
{
	template <class T>
	class Vector3
	{
	public:
		T x{};
		T y{};
		T z{};

		Vector3<T>();
		Vector3<T>(const T& aX, const T& aY, const T& aZ);
		Vector3<T>(const Vector3<T>& aVector) = default;
		Vector3<T>& operator=(const Vector3<T>& aVector3) = default;

		//Returns the squared length of the vector
		T LengthSqr() const;
		//Returns the length of the vector
		T Length() const;
		//Returns a normalized copy of this
		Vector3<T> GetNormalized() const;
		//Normalizes the vector
		void Normalize();
		//Returns the dot product of this and aVector
		T Dot(const Vector3<T>& aVector) const;
		//Returns the cross product of this and aVector
		Vector3<T> Cross(const Vector3<T>& aVector) const;
	};

	//Returns the vector sum of aVector0 and aVector1
	template <class T> Vector3<T> operator+(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		Vector3<T> temp;

		temp.x = aVector0.x + aVector1.x;
		temp.y = aVector0.y + aVector1.y;
		temp.z = aVector0.z + aVector1.z;

		return temp;
	}

	//Returns the vector difference of aVector0 and aVector1
	template <class T> Vector3<T> operator-(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		Vector3<T> temp;

		temp.x = aVector0.x - aVector1.x;
		temp.y = aVector0.y - aVector1.y;
		temp.z = aVector0.z - aVector1.z;

		return temp;
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T> Vector3<T> operator*(const Vector3<T>& aVector, const T& aScalar)
	{
		Vector3<T> temp;

		temp.x = aVector.x * aScalar;
		temp.y = aVector.y * aScalar;
		temp.z = aVector.z * aScalar;

		return temp;
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T> Vector3<T> operator*(const T& aScalar, const Vector3<T>& aVector)
	{
		Vector3<T> temp;

		temp.x = aVector.x * aScalar;
		temp.y = aVector.y * aScalar;
		temp.z = aVector.z * aScalar;

		return temp;
	}

	//Returns the vector aVector divided by the scalar aScalar (equivalent to aVector multiplied by 1/aScalar)
	template <class T> Vector3<T> operator/(const Vector3<T>& aVector, const T& aScalar)
	{
		Vector3<T> temp;

		temp.x = aVector.x / aScalar;
		temp.y = aVector.y / aScalar;
		temp.z = aVector.z / aScalar;

		return temp;
	}

	//Equivalent to setting aVector0 to (aVector0 + aVector1)
	template <class T> void operator+=(Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		aVector0.x = aVector0.x + aVector1.x;
		aVector0.y = aVector0.y + aVector1.y;
		aVector0.z = aVector0.z + aVector1.z;
	}

	//Equivalent to setting aVector0 to (aVector0 - aVector1)
	template <class T> void operator-=(Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		aVector0.x = aVector0.x - aVector1.x;
		aVector0.y = aVector0.y - aVector1.y;
		aVector0.z = aVector0.z - aVector1.z;
	}

	//Equivalent to setting aVector to (aVector * aScalar)
	template <class T> void operator*=(Vector3<T>& aVector, const T& aScalar)
	{
		aVector.x = aVector.x * aScalar;
		aVector.y = aVector.y * aScalar;
		aVector.z = aVector.z * aScalar;
	}

	//Equivalent to setting aVector to (aVector / aScalar)
	template <class T> void operator/=(Vector3<T>& aVector, const T& aScalar)
	{
		aVector.x = aVector.x / aScalar;
		aVector.y = aVector.y / aScalar;
		aVector.z = aVector.z / aScalar;
	}


	template<class T>
	inline Vector3<T>::Vector3()
	{}

	template<class T>
	inline Vector3<T>::Vector3(const T& aX, const T& aY, const T& aZ)
	{
		x = aX;
		y = aY;
		z = aZ;
	}

	template<class T>
	inline T Vector3<T>::LengthSqr() const
	{
		return ((x * x) + (y * y) + (z * z));
	}

	template<class T>
	inline T Vector3<T>::Length() const
	{
		T temp = ((x * x) + (y * y) + (z * z));

		return sqrt(temp);
	}

	template<class T>
	inline Vector3<T> Vector3<T>::GetNormalized() const
	{
		assert((x != 0 || y != 0 || z != 0) && "You are trying to divide by zero, you can not normalize a zero vector!");

		Vector3<T> temp;

		temp.x = x / sqrt((x * x) + (y * y) + (z * z));
		temp.y = y / sqrt((x * x) + (y * y) + (z * z));
		temp.z = z / sqrt((x * x) + (y * y) + (z * z));

		return temp;
	}

	template<class T>
	inline void Vector3<T>::Normalize()
	{
		assert((x != 0.0f || y != 0.0f || z != 0.0f) && "You are trying to divide by zero, you can not normalize a zero vector!");

		T tempX = x;
		T tempY = y;
		T tempZ = z;

		x = tempX / sqrt((tempX * tempX) + (tempY * tempY) + (tempZ * tempZ));
		y = tempY / sqrt((tempX * tempX) + (tempY * tempY) + (tempZ * tempZ));
		z = tempZ / sqrt((tempX * tempX) + (tempY * tempY) + (tempZ * tempZ));
	}

	template<class T>
	inline T Vector3<T>::Dot(const Vector3<T>& aVector) const
	{
		return (x * aVector.x) + (y * aVector.y) + (z * aVector.z);
	}

	template<class T>
	inline Vector3<T> Vector3<T>::Cross(const Vector3<T>& aVector) const
	{
		Vector3<T> temp;

		temp.x = (y * aVector.z) - (z * aVector.y);
		temp.y = (z * aVector.x) - (x * aVector.z);
		temp.z = (x * aVector.y) - (y * aVector.x);

		return temp;
	}

	using Vector3f = Vector3<float>;
	using Vector3i = Vector3<int>;
	using Vector3ui = Vector3<unsigned int>;
}
