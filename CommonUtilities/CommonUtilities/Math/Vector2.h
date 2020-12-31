#pragma once
#include <math.h>
#include <assert.h>


namespace CU
{
	template <class T>
	class Vector2
	{
	public:
		T x{};
		T y{};

		Vector2<T>();
		Vector2<T>(const T& aX, const T& aY);
		Vector2<T>(const Vector2<T>& aVector) = default;
		Vector2<T>& operator=(const Vector2<T>& aVector2) = default;

		//Returns the squared length of the vector
		T LengthSqr() const;
		//Returns the length of the vector
		T Length() const;
		//Returns a normalized copy of this
		Vector2<T> GetNormalized() const;
		//Normalizes the vector
		void Normalize();
		//Returns the dot product of this and aVector
		T Dot(const Vector2<T>& aVector) const;
	};

	//Returns the vector sum of aVector0 and aVector1
	template <class T> Vector2<T> operator+(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		Vector2<T> temp;

		temp.x = aVector0.x + aVector1.x;
		temp.y = aVector0.y + aVector1.y;

		return temp;
	}

	//Returns the vector difference of aVector0 and aVector1
	template <class T> Vector2<T> operator-(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		Vector2<T> temp;

		temp.x = aVector0.x - aVector1.x;
		temp.y = aVector0.y - aVector1.y;

		return temp;
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T> Vector2<T> operator*(const Vector2<T>& aVector, const T& aScalar)
	{
		Vector2<T> temp;

		temp.x = aVector.x * aScalar;
		temp.y = aVector.y * aScalar;

		return temp;
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T> Vector2<T> operator*(const T& aScalar, const Vector2<T>& aVector)
	{
		Vector2<T> temp;

		temp.x = aVector.x * aScalar;
		temp.y = aVector.y * aScalar;

		return temp;
	}

	//Returns the vector aVector divided by the scalar aScalar (equivalent to aVector multiplied by 1/aScalar)
	template <class T> Vector2<T> operator/(const Vector2<T>& aVector, const T& aScalar)
	{
		Vector2<T> temp;

		temp.x = aVector.x / aScalar;
		temp.y = aVector.y / aScalar;

		return temp;
	}

	//Equivalent to setting aVector0 to (aVector0 + aVector1)
	template <class T> void operator+=(Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		aVector0.x = aVector0.x + aVector1.x;
		aVector0.y = aVector0.y + aVector1.y;
	}

	//Equivalent to setting aVector0 to (aVector0 - aVector1)
	template <class T> void operator-=(Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		aVector0.x = aVector0.x - aVector1.x;
		aVector0.y = aVector0.y - aVector1.y;
	}

	//Equivalent to setting aVector to (aVector * aScalar)
	template <class T> void operator*=(Vector2<T>& aVector, const T& aScalar)
	{
		aVector.x = aVector.x * aScalar;
		aVector.y = aVector.y * aScalar;
	}

	//Equivalent to setting aVector to (aVector / aScalar)
	template <class T> void operator/=(Vector2<T>& aVector, const T& aScalar)
	{
		aVector.x = aVector.x / aScalar;
		aVector.y = aVector.y / aScalar;
	}


	template<class T>
	inline Vector2<T>::Vector2() {}

	template<class T>
	inline Vector2<T>::Vector2(const T& aX, const T& aY)
	{
		x = aX;
		y = aY;
	}

	template<class T>
	inline T Vector2<T>::LengthSqr() const
	{
		return ((x * x) + (y * y));
	}

	template<class T>
	inline T Vector2<T>::Length() const
	{
		T temp = ((x * x) + (y * y));

		return sqrt(temp);
	}

	template<class T>
	inline Vector2<T> Vector2<T>::GetNormalized() const
	{
		assert((x != 0 || y != 0) && "You are trying to divide by zero, you can not normalize a zero vector!");

		Vector2<T> temp;

		temp.x = static_cast<T>(x / sqrt((x * x) + (y * y)));
		temp.y = static_cast<T>(y / sqrt((x * x) + (y * y)));

		return temp;
	}

	template<class T>
	inline void Vector2<T>::Normalize()
	{
		assert((x != 0 || y != 0) && "You are trying to divide by zero, you can not normalize a zero vector!");

		T tempX = x;
		T tempY = y;

		x = static_cast<T>(tempX / sqrt((tempX * tempX) + (tempY * tempY)));
		y = static_cast<T>(tempY / sqrt((tempX * tempX) + (tempY * tempY)));
	}

	template<class T>
	inline T Vector2<T>::Dot(const Vector2<T>& aVector) const
	{
		return (x * aVector.x) + (y * aVector.y);
	}

	using Vector2f = Vector2<float>;
	using Vector2i = Vector2<int>;
	using Vector2ui = Vector2<unsigned int>;
}
