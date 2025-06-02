#include "vector2.h"
#include <cmath>

Vector2i::Vector2i() {
	x = 0;
	y = 0;
}

Vector2i::Vector2i(int32 x, int32 y) {
	this->x = x;
	this->y = y;
}

void Vector2i::operator=(const Vector2f &v) {
	this->x = v.x;
	this->y = v.y;
}

bool Vector2i::operator!=(const Vector2i &v) const {
	return (x != v.x || y != v.y);
}

Vector2f::Vector2f() {
	x = 0.0f;
	y = 0.0f;
}

Vector2f::Vector2f(const Vector2i &v) {
	x = v.x;
	y = v.y;
}

Vector2f::Vector2f(float x, float y) {
	this->x = x;
	this->y = y;
}

void Vector2f::normalise() {
	float length = getMagnitute();
	if (length != 0.0f) {
		x /= length;
		y /= length;
	}
}

void Vector2f::operator=(const Vector2i &v) {
	this->x = v.x;
	this->y = v.y;
}

Vector2f Vector2f::operator-(const Vector2f &v) const {
	return Vector2f(x - v.x, y - v.y);
}

Vector2f Vector2f::operator-(const Vector2i &v) const {
	return Vector2f(x - v.x, y - v.y);
}

float Vector2f::operator*(const Vector2f &v) const {
	return x * v.x + y * v.y;
}

bool Vector2f::operator!=(const Vector2f &v) const {
	return (x != v.x || y != v.y);
}

bool Vector2f::operator==(const Vector2f &v) const {
	return (x == v.x && y == v.y);
}

float Vector2f::getAngleBetween(const Vector2f &v) const {
	return acos((*this * v));
}

float Vector2f::getSignedAngleBetween(const Vector2f &v) const {
	return atan2(getCrossProduct(v), *this * v);
}

int Vector2f::getSignedAngleBetweenInDegrees(const Vector2f &v) const {
	float rotateRadian = getSignedAngleBetween(v);
	if (rotateRadian > 0) {
		return (360.0f - (rotateRadian * 180.0f / _FLOAT_PI));
	} else {
		return (-rotateRadian * 180.0f / _FLOAT_PI);
	}
}

float Vector2f::getSlopeAngle() const {
	return atan2(y, x);
}

float Vector2f::getCrossProduct(const Vector2f &v) const {
	return x * v.y - y * v.x;
}

float Vector2f::getMagnitute() const {
	return sqrt(x * x + y * y);
}

float Vector2f::getDistance(const Vector2f &v) const {
	return sqrt(pow(x - v.x, 2) + pow(y - v.y, 2));
}

float Vector2f::getDistanceSquared(const Vector2f &v) const {
	return pow(x - v.x, 2) + pow(y - v.y, 2);
}

Vector2f Vector2f::operator-(int i) const {
	return Vector2f(x - i, y - i);
}

Vector2f Vector2f::operator-(float f) const {
	return Vector2f(x - f, y - f);
}

Vector2f Vector2f::operator+(const Vector2f &v) const {
	return Vector2f(x + v.x, y + v.y);
}

Vector2f Vector2f::getRotate(float sinAngle, float cosAngle) const {
	return Vector2f(x * cosAngle - y * sinAngle, x * sinAngle + y * cosAngle);
}

void Vector2f::rotate(float sinAngle, float cosAngle) {
	*this = Vector2f(x * cosAngle - y * sinAngle, x * sinAngle + y * cosAngle);
}

void Vector2f::translate(float x, float y) {
	this->x += x;
	this->y += y;
}