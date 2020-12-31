#pragma once
#include "Vector4.h"

#include <assert.h>
#include <math.h>

namespace CU
{
	template<class T>
	class Matrix4x4
	{
	public:
		Matrix4x4<T>();
		Matrix4x4<T>(const Matrix4x4<T>& aMatrix);
		T& operator()(const int row, const int column);
		const T& operator()(const int row, const int column) const;

		Matrix4x4<T> operator+=(Matrix4x4<T> aMatrix4x4);
		Matrix4x4<T> operator-=(Matrix4x4<T> aMatrix4x4);
		Matrix4x4<T> operator*=(Matrix4x4<T> aMatrix4x4);

		void operator=(Matrix4x4<T> aMatrix4x4);
		bool operator==(Matrix4x4<T> aMatrix4x4);

		static Matrix4x4<T> CreateRotationAroundX(T aAngleInRadians);
		static Matrix4x4<T> CreateRotationAroundY(T aAngleInRadians);
		static Matrix4x4<T> CreateRotationAroundZ(T aAngleInRadians);
		static Matrix4x4<T> Transpose(const Matrix4x4<T>& aMatrixToTranspose);
		static Matrix4x4<T> GetFastInverse(const Matrix4x4<T>& aMatrix);

	private:
		T myMatrix[4][4];

	};

	template<class T>
	inline Matrix4x4<T>::Matrix4x4()
	{
		myMatrix[0][0] = 1;
		myMatrix[0][1] = 0;
		myMatrix[0][2] = 0;
		myMatrix[0][3] = 0;
		myMatrix[1][0] = 0;
		myMatrix[1][1] = 1;
		myMatrix[1][2] = 0;
		myMatrix[1][3] = 0;
		myMatrix[2][0] = 0;
		myMatrix[2][1] = 0;
		myMatrix[2][2] = 1;
		myMatrix[2][3] = 0;
		myMatrix[3][0] = 0;
		myMatrix[3][1] = 0;
		myMatrix[3][2] = 0;
		myMatrix[3][3] = 1;
	}


	template<class T>
	inline Matrix4x4<T>::Matrix4x4(const Matrix4x4<T>& aMatrix)
	{
		for (int column = 0; column < 4; ++column)
		{
			for (int row = 0; row < 4; ++row)
			{
				myMatrix[column][row] = aMatrix(column + 1, row + 1);
			}
		}
	}



	template<class T>
	inline T& Matrix4x4<T>::operator()(const int row, const int column)
	{
		assert(row > 0 && column > 0 && row <= 4 && column <= 4 && "index out of range!");

		return myMatrix[row - 1][column - 1];
	}

	template<class T>
	inline const T& Matrix4x4<T>::operator()(const int row, const int column) const
	{
		assert(row > 0 && column > 0 && row <= 4 && column <= 4 && "index out of range!");

		return myMatrix[row - 1][column - 1];
	}

	template<class T>
	inline Matrix4x4<T> operator+(const Matrix4x4<T> aMatrix0, const Matrix4x4<T> aMatrix1)
	{
		Matrix4x4<T> tempMatrix;

		for (int column = 0; column < 4; ++column)
		{
			for (int row = 0; row < 4; ++row)
			{
				tempMatrix(column + 1, row + 1) = aMatrix0(column + 1, row + 1) + aMatrix1(column + 1, row + 1);
			}
		}
		return tempMatrix;
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::operator+=(const Matrix4x4<T> aMatrix)
	{
		for (int column = 0; column < 4; ++column)
		{
			for (int row = 0; row < 4; ++row)
			{
				myMatrix[column][row] += aMatrix(column + 1, row + 1);
			}
		}
		return *this;
	}

	template<class T>
	inline Matrix4x4<T> operator-(const Matrix4x4<T> aMatrix0, const Matrix4x4<T> aMatrix1)
	{
		Matrix4x4<T> tempMatrix;

		for (int column = 0; column < 4; ++column)
		{
			for (int row = 0; row < 4; ++row)
			{
				tempMatrix(column + 1, row + 1) = aMatrix0(column + 1, row + 1) - aMatrix1(column + 1, row + 1);
			}
		}
		return tempMatrix;
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::operator-=(const Matrix4x4<T> aMatrix4x4)
	{
		for (int column = 0; column < 4; ++column)
		{
			for (int row = 0; row < 4; ++row)
			{
				myMatrix[column][row] -= aMatrix4x4(column + 1, row + 1);
			}
		}
		return *this;
	}

	template<class T>
	inline Matrix4x4<T> operator*(const Matrix4x4<T> aMatrix0, const Matrix4x4<T> aMatrix1)
	{
		Matrix4x4<T> tempMatrix;

		tempMatrix(1, 1) = aMatrix0(1, 1) * aMatrix1(1, 1) + aMatrix0(1, 2) * aMatrix1(2, 1) + aMatrix0(1, 3) * aMatrix1(3, 1) + aMatrix0(1, 4) * aMatrix1(4, 1);
		tempMatrix(1, 2) = aMatrix0(1, 1) * aMatrix1(1, 2) + aMatrix0(1, 2) * aMatrix1(2, 2) + aMatrix0(1, 3) * aMatrix1(3, 2) + aMatrix0(1, 4) * aMatrix1(4, 2);
		tempMatrix(1, 3) = aMatrix0(1, 1) * aMatrix1(1, 3) + aMatrix0(1, 2) * aMatrix1(2, 3) + aMatrix0(1, 3) * aMatrix1(3, 3) + aMatrix0(1, 4) * aMatrix1(4, 3);
		tempMatrix(1, 4) = aMatrix0(1, 1) * aMatrix1(1, 4) + aMatrix0(1, 2) * aMatrix1(2, 4) + aMatrix0(1, 3) * aMatrix1(3, 4) + aMatrix0(1, 4) * aMatrix1(4, 4);

		tempMatrix(2, 1) = aMatrix0(2, 1) * aMatrix1(1, 1) + aMatrix0(2, 2) * aMatrix1(2, 1) + aMatrix0(2, 3) * aMatrix1(3, 1) + aMatrix0(2, 4) * aMatrix1(4, 1);
		tempMatrix(2, 2) = aMatrix0(2, 1) * aMatrix1(1, 2) + aMatrix0(2, 2) * aMatrix1(2, 2) + aMatrix0(2, 3) * aMatrix1(3, 2) + aMatrix0(2, 4) * aMatrix1(4, 2);
		tempMatrix(2, 3) = aMatrix0(2, 1) * aMatrix1(1, 3) + aMatrix0(2, 2) * aMatrix1(2, 3) + aMatrix0(2, 3) * aMatrix1(3, 3) + aMatrix0(2, 4) * aMatrix1(4, 3);
		tempMatrix(2, 4) = aMatrix0(2, 1) * aMatrix1(1, 4) + aMatrix0(2, 2) * aMatrix1(2, 4) + aMatrix0(2, 3) * aMatrix1(3, 4) + aMatrix0(2, 4) * aMatrix1(4, 4);

		tempMatrix(3, 1) = aMatrix0(3, 1) * aMatrix1(1, 1) + aMatrix0(3, 2) * aMatrix1(2, 1) + aMatrix0(3, 3) * aMatrix1(3, 1) + aMatrix0(3, 4) * aMatrix1(4, 1);
		tempMatrix(3, 2) = aMatrix0(3, 1) * aMatrix1(1, 2) + aMatrix0(3, 2) * aMatrix1(2, 2) + aMatrix0(3, 3) * aMatrix1(3, 2) + aMatrix0(3, 4) * aMatrix1(4, 2);
		tempMatrix(3, 3) = aMatrix0(3, 1) * aMatrix1(1, 3) + aMatrix0(3, 2) * aMatrix1(2, 3) + aMatrix0(3, 3) * aMatrix1(3, 3) + aMatrix0(3, 4) * aMatrix1(4, 3);
		tempMatrix(3, 4) = aMatrix0(3, 1) * aMatrix1(1, 4) + aMatrix0(3, 2) * aMatrix1(2, 4) + aMatrix0(3, 3) * aMatrix1(3, 4) + aMatrix0(3, 4) * aMatrix1(4, 4);

		tempMatrix(4, 1) = aMatrix0(4, 1) * aMatrix1(1, 1) + aMatrix0(4, 2) * aMatrix1(2, 1) + aMatrix0(4, 3) * aMatrix1(3, 1) + aMatrix0(4, 4) * aMatrix1(4, 1);
		tempMatrix(4, 2) = aMatrix0(4, 1) * aMatrix1(1, 2) + aMatrix0(4, 2) * aMatrix1(2, 2) + aMatrix0(4, 3) * aMatrix1(3, 2) + aMatrix0(4, 4) * aMatrix1(4, 2);
		tempMatrix(4, 3) = aMatrix0(4, 1) * aMatrix1(1, 3) + aMatrix0(4, 2) * aMatrix1(2, 3) + aMatrix0(4, 3) * aMatrix1(3, 3) + aMatrix0(4, 4) * aMatrix1(4, 3);
		tempMatrix(4, 4) = aMatrix0(4, 1) * aMatrix1(1, 4) + aMatrix0(4, 2) * aMatrix1(2, 4) + aMatrix0(4, 3) * aMatrix1(3, 4) + aMatrix0(4, 4) * aMatrix1(4, 4);



		return tempMatrix;
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::operator*=(const Matrix4x4<T> aMatrix)
	{
		Matrix4x4<T> tempMatrix = *this;

		myMatrix[0][0] = tempMatrix(1, 1) * aMatrix(1, 1) + tempMatrix(1, 2) * aMatrix(2, 1) + tempMatrix(1, 3) * aMatrix(3, 1) + tempMatrix(1, 4) * aMatrix(4, 1);
		myMatrix[0][1] = tempMatrix(1, 1) * aMatrix(1, 2) + tempMatrix(1, 2) * aMatrix(2, 2) + tempMatrix(1, 3) * aMatrix(3, 2) + tempMatrix(1, 4) * aMatrix(4, 2);
		myMatrix[0][2] = tempMatrix(1, 1) * aMatrix(1, 3) + tempMatrix(1, 2) * aMatrix(2, 3) + tempMatrix(1, 3) * aMatrix(3, 3) + tempMatrix(1, 4) * aMatrix(4, 3);
		myMatrix[0][3] = tempMatrix(1, 1) * aMatrix(1, 4) + tempMatrix(1, 2) * aMatrix(2, 4) + tempMatrix(1, 3) * aMatrix(3, 4) + tempMatrix(1, 4) * aMatrix(4, 4);

		myMatrix[1][0] = tempMatrix(2, 1) * aMatrix(1, 1) + tempMatrix(2, 2) * aMatrix(2, 1) + tempMatrix(2, 3) * aMatrix(3, 1) + tempMatrix(2, 4) * aMatrix(4, 1);
		myMatrix[1][1] = tempMatrix(2, 1) * aMatrix(1, 2) + tempMatrix(2, 2) * aMatrix(2, 2) + tempMatrix(2, 3) * aMatrix(3, 2) + tempMatrix(2, 4) * aMatrix(4, 2);
		myMatrix[1][2] = tempMatrix(2, 1) * aMatrix(1, 3) + tempMatrix(2, 2) * aMatrix(2, 3) + tempMatrix(2, 3) * aMatrix(3, 3) + tempMatrix(2, 4) * aMatrix(4, 3);
		myMatrix[1][3] = tempMatrix(2, 1) * aMatrix(1, 4) + tempMatrix(2, 2) * aMatrix(2, 4) + tempMatrix(2, 3) * aMatrix(3, 4) + tempMatrix(2, 4) * aMatrix(4, 4);

		myMatrix[2][0] = tempMatrix(3, 1) * aMatrix(1, 1) + tempMatrix(3, 2) * aMatrix(2, 1) + tempMatrix(3, 3) * aMatrix(3, 1) + tempMatrix(3, 4) * aMatrix(4, 1);
		myMatrix[2][1] = tempMatrix(3, 1) * aMatrix(1, 2) + tempMatrix(3, 2) * aMatrix(2, 2) + tempMatrix(3, 3) * aMatrix(3, 2) + tempMatrix(3, 4) * aMatrix(4, 2);
		myMatrix[2][2] = tempMatrix(3, 1) * aMatrix(1, 3) + tempMatrix(3, 2) * aMatrix(2, 3) + tempMatrix(3, 3) * aMatrix(3, 3) + tempMatrix(3, 4) * aMatrix(4, 3);
		myMatrix[2][3] = tempMatrix(3, 1) * aMatrix(1, 4) + tempMatrix(3, 2) * aMatrix(2, 4) + tempMatrix(3, 3) * aMatrix(3, 4) + tempMatrix(3, 4) * aMatrix(4, 4);

		myMatrix[3][0] = tempMatrix(4, 1) * aMatrix(1, 1) + tempMatrix(4, 2) * aMatrix(2, 1) + tempMatrix(4, 3) * aMatrix(3, 1) + tempMatrix(4, 4) * aMatrix(4, 1);
		myMatrix[3][1] = tempMatrix(4, 1) * aMatrix(1, 2) + tempMatrix(4, 2) * aMatrix(2, 2) + tempMatrix(4, 3) * aMatrix(3, 2) + tempMatrix(4, 4) * aMatrix(4, 2);
		myMatrix[3][2] = tempMatrix(4, 1) * aMatrix(1, 3) + tempMatrix(4, 2) * aMatrix(2, 3) + tempMatrix(4, 3) * aMatrix(3, 3) + tempMatrix(4, 4) * aMatrix(4, 3);
		myMatrix[3][3] = tempMatrix(4, 1) * aMatrix(1, 4) + tempMatrix(4, 2) * aMatrix(2, 4) + tempMatrix(4, 3) * aMatrix(3, 4) + tempMatrix(4, 4) * aMatrix(4, 4);

		return *this;
	}

	template<class T>
	inline void Matrix4x4<T>::operator=(const Matrix4x4<T> aMatrix)
	{
		for (int column = 0; column < 4; ++column)
		{
			for (int row = 0; row < 4; ++row)
			{
				myMatrix[column][row] = aMatrix(column + 1, row + 1);
			}
		}
	}

	template<class T>
	inline bool Matrix4x4<T>::operator==(const Matrix4x4<T> aMatrix)
	{
		bool isEqual;

		for (int column = 0; column < 4; ++column)
		{
			for (int row = 0; row < 4; ++row)
			{
				isEqual = myMatrix[column][row] == aMatrix(column + 1, row + 1);

				if (!isEqual)
				{
					return false;
				}
			}
		}
		return isEqual;
	}

	template<class T>
	CU::Vector4<T> operator*(const Vector4<T> aVector4, const Matrix4x4<T> aMatrix4x4)
	{
		Vector4<T> tempVector;

		tempVector.x = aVector4.x * aMatrix4x4(1, 1) + aVector4.y * aMatrix4x4(2, 1) + aVector4.z * aMatrix4x4(3, 1) + aVector4.w * aMatrix4x4(4, 1);
		tempVector.y = aVector4.x * aMatrix4x4(1, 2) + aVector4.y * aMatrix4x4(2, 2) + aVector4.z * aMatrix4x4(3, 2) + aVector4.w * aMatrix4x4(4, 2);
		tempVector.z = aVector4.x * aMatrix4x4(1, 3) + aVector4.y * aMatrix4x4(2, 3) + aVector4.z * aMatrix4x4(3, 3) + aVector4.w * aMatrix4x4(4, 3);
		tempVector.w = aVector4.x * aMatrix4x4(1, 4) + aVector4.y * aMatrix4x4(2, 4) + aVector4.z * aMatrix4x4(3, 4) + aVector4.w * aMatrix4x4(4, 4);

		return tempVector;
	}


	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundX(T aAngleInRadians)
	{
		Matrix4x4<T> tempMatrix;

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
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundY(T aAngleInRadians)
	{
		Matrix4x4<T> tempMatrix;

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
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundZ(T aAngleInRadians)
	{
		Matrix4x4<T> tempMatrix;

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
	inline Matrix4x4<T> Matrix4x4<T>::Transpose(const Matrix4x4<T>& aMatrixToTranspose)
	{
		Matrix4x4<T> tempMatrix;

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				tempMatrix(j + 1, i + 1) = aMatrixToTranspose(i + 1, j + 1);
			}
		}

		return tempMatrix;
	}
	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::GetFastInverse(const Matrix4x4<T>& aMatrix)
	{

		Matrix4x4<T> resultMatrix;

		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				resultMatrix(j + 1, i + 1) = aMatrix(i + 1, j + 1);
			}
		}

		Vector4<T> tempVector;
		tempVector.x = -aMatrix(4, 1);
		tempVector.y = -aMatrix(4, 2);
		tempVector.z = -aMatrix(4, 3);

		tempVector.w = static_cast<T>(1.0f);

		Vector4<T> resultVector = tempVector * resultMatrix;

		resultMatrix(4, 1) = resultVector.x;
		resultMatrix(4, 2) = resultVector.y;
		resultMatrix(4, 3) = resultVector.z;
		resultMatrix(4, 4) = tempVector.w;


		return resultMatrix;
	}
}