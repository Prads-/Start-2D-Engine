#ifndef VECTOR_2_H
#define VECTOR_2_H

#include "declarations.h"

struct Vector2i;

struct Vector2f {
	float32 x, y;

	Vector2f();
	Vector2f(const Vector2i &v);
	Vector2f(float x, float y);

	void operator=(const Vector2i &v);
	Vector2f operator-(const Vector2f &v) const;
	Vector2f operator-(const Vector2i &v) const;
	Vector2f operator-(int i) const;
	Vector2f operator-(float f) const;
	Vector2f operator+(const Vector2f &v) const;
	float operator*(const Vector2f &v) const;
	bool operator!=(const Vector2f &v) const;
	bool operator==(const Vector2f &v) const;

	float getAngleBetween(const Vector2f &v) const;
	float getSignedAngleBetween(const Vector2f &v) const;
	int getSignedAngleBetweenInDegrees(const Vector2f &v) const;
	float getSlopeAngle() const;

	float getCrossProduct(const Vector2f &v) const;
	float getMagnitute() const;
	float getDistance(const Vector2f &v) const;
	float getDistanceSquared(const Vector2f &v) const;

	Vector2f getRotate(float sinAngle, float cosAngle) const;
	void rotate(float sinAngle, float cosAngle);
	void translate(float x, float y);

	void normalise();
};

struct Vector2i {
	int32 x, y;

	Vector2i();
	Vector2i(int32 x, int32 y);

	void operator=(const Vector2f &v);
	bool operator!=(const Vector2i &v) const;
};

#endif