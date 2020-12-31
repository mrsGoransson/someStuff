#pragma once
#include "Matrix4x4.h"
#include "Vector3.h"

#include <assert.h>
#include <math.h>

namespace CU
{
	template<class T>
	class Matrix3x3
	{
	public:
		Matrix3x3<T>();
		Matrix3x3<T>(const Matrix3x3<T>& aMatrix);
		Matrix3x3<T>(const Matrix4x4<T>& aMatrix);
		T& operator()(const int row, const int column);
		const T& operator()(const int row, const int column) const;

		Matrix3x3<T> operator+=(Matrix3x3<T> aMatrix3x3);
		Matrix3x3<T> operator-=(Matrix3x3<T> aMatrix3x3);
		Matrix3x3<T> operator*=(Matrix3x3<T> aMatrix3x3);

		void operator=(Matrix3x3<T> aMatrix3x3);
		bool operator==(Matrix3x3<T> aMatrix3x3);

		static Matrix3x3<T> CreateRotationAroundX(T aAngleInRadians);
		static Matrix3x3<T> CreateRotationAroundY(T aAngleInRadians);
		static Matrix3x3<T> CreateRotationAroundZ(T aAngleInRadians);
		static Matrix3x3<T> Transpose(const Matrix3x3<T>& aMatrixToTranspose);

	private:
		T myMatrix[3][3];
	};


	template<class T>
	inline Matrix3x3<T>::Matrix3x3()
	{
		myMatrix[0][0] = 1;
		myMatrix[0][1] = 0;
		myMatrix[0][2] = 0;

		myMatrix[1][0] = 0;
		myMatrix[1][1] = 1;
		myMatrix[1][2] = 0;

		myMatrix[2][0] = 0;
		myMatrix[2][1] = 0;
		myMatrix[2][2] = 1;

	}

	template<class T>
	inline Matrix3x3<T>::Matrix3x3(const Matrix3x3<T>& aMatrix)
	{
		for (int column = 0; column < 3; ++column)
		{
			for (int row = 0; row < 3; ++row)
			{
				myMatrix[column][row] = aMatrix(column + 1, row + 1);
			}
		}
	}


	template<class T>
	inline Matrix3x3<T>::Matrix3x3(const Matrix4x4<T>& aMatrix)
	{
		for (int column = 0; column < 3; ++column)
		{
			for (int row = 0; row < 3; ++row)
			{
				myMatrix[column][row] = aMatrix(column + 1, row + 1);
			}
		}
	}

	template<class T>
	inline T& Matrix3x3<T>::operator()(const int row, const int column)
	{
		assert(row > 0 && column > 0 && row <= 3 && column <= 3 && "index out of range!");

		return myMatrix[row - 1][column - 1];
	}

	template<class T>
	inline const T& Matrix3x3<T>::operator()(const int row, const int column) const
	{
		assert(row > 0 && column > 0 && row <= 3 && column <= 3 && "index out of range!");

		return myMatrix[row - 1][column - 1];
	}

	template<class T>
	inline Matrix3x3<T> operator+(Matrix3x3<T> aMatrix0, Matrix3x3<T> aMatrix1)
	{
		Matrix3x3<T> tempMatrix;

		for (int column = 0; column < 3; ++column)
		{
			for (int row = 0; row < 3; ++row)
			{
				tempMatrix(column + 1, row + 1) = aMatrix0(column + 1, row + 1) + aMatrix1(column + 1, row + 1);
			}
		}
		return tempMatrix;
	}


	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::operator+=(Matrix3x3<T> aMatrix3x3)
	{
		for (int column = 0; column < 3; ++column)
		{
			for (int row = 0; row < 3; ++row)
			{
				myMatrix[column][row] += aMatrix3x3(column + 1, row + 1);
			}
		}
		return *this;
	}



	template<class T>
	inline Matrix3x3<T> operator-(Matrix3x3<T> aMatrix0, Matrix3x3<T> aMatrix1)
	{
		Matrix3x3<T> tempMatrix;

		for (int column = 0; column < 3; ++column)
		{
			for (int row = 0; row < 3; ++row)
			{
				tempMatrix(column + 1, row + 1) = aMatrix0(column + 1, row + 1) - aMatrix1(column + 1, row + 1);
			}
		}
		return tempMatrix;
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::operator-=(Matrix3x3<T> aMatrix3x3)
	{
		for (int column = 0; column < 3; ++column)
		{
			for (int row = 0; row < 3; ++row)
			{
				myMatrix[column][row] -= aMatrix3x3(column + 1, row + 1);
			}
		}
		return *this;
	}


	template<class T>
	inline Matrix3x3<T> operator*(Matrix3x3<T> aMatrix0, Matrix3x3<T> aMatrix1)
	{
		Matrix3x3<T> tempMatrix;

		tempMatrix(1, 1) = aMatrix0(1, 1) * aMatrix1(1, 1) + aMatrix0(1, 2) * aMatrix1(2, 1) + aMatrix0(1, 3) * aMatrix1(3, 1);
		tempMatrix(1, 2) = aMatrix0(1, 1) * aMatrix1(1, 2) + aMatrix0(1, 2) * aMatrix1(2, 2) + aMatrix0(1, 3) * aMatrix1(3, 2);
		tempMatrix(1, 3) = aMatrix0(1, 1) * aMatrix1(1, 3) + aMatrix0(1, 2) * aMatrix1(2, 3) + aMatrix0(1, 3) * aMatrix1(3, 3);

		tempMatrix(2, 1) = aMatrix0(2, 1) * aMatrix1(1, 1) + aMatrix0(2, 2) * aMatrix1(2, 1) + aMatrix0(2, 3) * aMatrix1(3, 1);
		tempMatrix(2, 2) = aMatrix0(2, 1) * aMatrix1(1, 2) + aMatrix0(2, 2) * aMatrix1(2, 2) + aMatrix0(2, 3) * aMatrix1(3, 2);
		tempMatrix(2, 3) = aMatrix0(2, 1) * aMatrix1(1, 3) + aMatrix0(2, 2) * aMatrix1(2, 3) + aMatrix0(2, 3) * aMatrix1(3, 3);

		tempMatrix(3, 1) = aMatrix0(3, 1) * aMatrix1(1, 1) + aMatrix0(3, 2) * aMatrix1(2, 1) + aMatrix0(3, 3) * aMatrix1(3, 1);
		tempMatrix(3, 2) = aMatrix0(3, 1) * aMatrix1(1, 2) + aMatrix0(3, 2) * aMatrix1(2, 2) + aMatrix0(3, 3) * aMatrix1(3, 2);
		tempMatrix(3, 3) = aMatrix0(3, 1) * aMatrix1(1, 3) + aMatrix0(3, 2) * aMatrix1(2, 3) + aMatrix0(3, 3) * aMatrix1(3, 3);

		return tempMatrix;
	}


	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::operator*=(Matrix3x3<T> aMatrix3x3)
	{
		Matrix3x3<T> tempMatrix = *this;

		myMatrix[0][0] = tempMatrix(1, 1) * aMatrix3x3(1, 1) + tempMatrix(1, 2) * aMatrix3x3(2, 1) + tempMatrix(1, 3) * aMatrix3x3(3, 1);
		myMatrix[0][1] = tempMatrix(1, 1) * aMatrix3x3(1, 2) + tempMatrix(1, 2) * aMatrix3x3(2, 2) + tempMatrix(1, 3) * aMatrix3x3(3, 2);
		myMatrix[0][2] = tempMatrix(1, 1) * aMatrix3x3(1, 3) + tempMatrix(1, 2) * aMatrix3x3(2, 3) + tempMatrix(1, 3) * aMatrix3x3(3, 3);

		myMatrix[1][0] = tempMatrix(2, 1) * aMatrix3x3(1, 1) + tempMatrix(2, 2) * aMatrix3x3(2, 1) + tempMatrix(2, 3) * aMatrix3x3(3, 1);
		myMatrix[1][1] = tempMatrix(2, 1) * aMatrix3x3(1, 2) + tempMatrix(2, 2) * aMatrix3x3(2, 2) + tempMatrix(2, 3) * aMatrix3x3(3, 2);
		myMatrix[1][2] = tempMatrix(2, 1) * aMatrix3x3(1, 3) + tempMatrix(2, 2) * aMatrix3x3(2, 3) + tempMatrix(2, 3) * aMatrix3x3(3, 3);

		myMatrix[2][0] = tempMatrix(3, 1) * aMatrix3x3(1, 1) + tempMatrix(3, 2) * aMatrix3x3(2, 1) + tempMatrix(3, 3) * aMatrix3x3(3, 1);
		myMatrix[2][1] = tempMatrix(3, 1) * aMatrix3x3(1, 2) + tempMatrix(3, 2) * aMatrix3x3(2, 2) + tempMatrix(3, 3) * aMatrix3x3(3, 2);
		myMatrix[2][2] = tempMatrix(3, 1) * aMatrix3x3(1, 3) + tempMatrix(3, 2) * aMatrix3x3(2, 3) + tempMatrix(3, 3) * aMatrix3x3(3, 3);

		return *this;
	}


	template<class T>
	inline void Matrix3x3<T>::operator=(Matrix3x3<T> aMatrix3x3)
	{
		for (int column = 0; column < 3; ++column)
		{
			for (int row = 0; row < 3; ++row)
			{
				myMatrix[column][row] = aMatrix3x3(column + 1, row + 1);
			}
		}
	}


	template<class T>
	inline bool Matrix3x3<T>::operator==(Matrix3x3<T> aMatrix3x3)
	{
		bool isEqual;

		for (int column = 0; column < 3; ++column)
		{
			for (int row = 0; row < 3; ++row)
			{
				isEqual = myMatrix[column][row] == aMatrix3x3(column + 1, row + 1);

				if (!isEqual)
				{
					return false;
				}
			}
		}
		return isEqual;
	}


	template<class T>
	CU::Vector3<T> operator*(const Vector3<T> aVector3, const Matrix3x3<T> aMatrix3x3)
	{
		Vector3<T> tempVector;

		tempVector.x = aVector3.x * aMatrix3x3(1, 1) + aVector3.y * aMatrix3x3(2, 1) + aVector3.z * aMatrix3x3(3, 1);
		tempVector.y = aVector3.x * aMatrix3x3(1, 2) + aVector3.y * aMatrix3x3(2, 2) + aVector3.z * aMatrix3x3(3, 2);
		tempVector.z = aVector3.x * aMatrix3x3(1, 3) + aVector3.y * aMatrix3x3(2, 3) + aVector3.z * aMatrix3x3(3, 3);

		return tempVector;
	}


	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundX(T aAngleInRadians)
	{
		Matrix3x3<T> tempMatrix;

		tempMatrix(1, 1) = 1;
		tempMatrix(1, 2) = 0;
		tempMatrix(1, 3) = 0;

		tempMatrix(2, 1) = 0;
		tempMatrix(2, 2) = cos(aAngleInRadians);
		tempMatrix(2, 3) = sin(aAngleInRadians);

		tempMatrix(3, 1) = 0;
		tempMatrix(3, 2) = -sin(aAngleInRadians);
		tempMatrix(3, 3) = cos(aAngleInRadians);

		return tempMatrix;
	}


	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundY(T aAngleInRadians)
	{
		Matrix3x3<T> tempMatrix;

		tempMatrix(1, 1) = cos(aAngleInRadians);
		tempMatrix(1, 2) = 0;
		tempMatrix(1, 3) = -sin(aAngleInRadians);

		tempMatrix(2, 1) = 0;
		tempMatrix(2, 2) = 1;
		tempMatrix(2, 3) = 0;

		tempMatrix(3, 1) = sin(aAngleInRadians);
		tempMatrix(3, 2) = 0;
		tempMatrix(3, 3) = cos(aAngleInRadians);

		return tempMatrix;
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundZ(T aAngleInRadians)
	{
		Matrix3x3<T> tempMatrix;

		tempMatrix(1, 1) = cos(aAngleInRadians);
		tempMatrix(1, 2) = sin(aAngleInRadians);
		tempMatrix(1, 3) = 0;

		tempMatrix(2, 1) = -sin(aAngleInRadians);
		tempMatrix(2, 2) = cos(aAngleInRadians);
		tempMatrix(2, 3) = 0;

		tempMatrix(3, 1) = 0;
		tempMatrix(3, 2) = 0;
		tempMatrix(3, 3) = 1;

		return tempMatrix;
	}


	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::Transpose(const Matrix3x3<T>& aMatrixToTranspose)
	{
		Matrix3x3<T> tempMatrix;

		for (int i = 1; i < 4; i++)
		{
			for (int j = 1; j < 4; j++)
			{
				tempMatrix(j, i) = aMatrixToTranspose(i, j);
			}
		}

		return tempMatrix;
	}
}