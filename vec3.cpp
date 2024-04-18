#include "header.h"

vec3::vec3()//конструктор по умолчанию
{
	x = 0.0; y = 0.0; z = 0.0;
}
vec3::vec3(double x_, double y_, double z_)//конструктор
{
	x = x_; y = y_; z = z_;
}
vec3& vec3::operator=(const vec3& other)
{
	x = other.x; y = other.y; z = other.z;
	return *this;
}
const double& vec3::operator[](const size_t i) const
{
	if (i == 0)
		return x;
	else if (i == 1)
		return y;
	else if (i == 2)
		return z;
}
double vec3::lenght()
{
	return sqrt(x * x + y * y + z * z);
}

vec3& vec3::normalize()
{
	double l = this->lenght();
	x = x / l;
	y = y / l;
	z = z / l;
	return *this;
}


//операторы

vec3 operator-(const vec3& v1, const vec3& v2)
{
	return vec3(v1[0] - v2[0], v1[1] - v2[1], v1[2] - v2[2]);
}
vec3 operator+(const vec3& v1, const vec3& v2)
{
	return vec3(v1[0] + v2[0], v1[1] + v2[1], v1[2] + v2[2]);
}
double operator*(const vec3& v1, const vec3& v2)
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}
vec3 operator*(const vec3& v1, double F)
{
	return vec3(v1[0] * F, v1[1] * F, v1[2] * F);
}
vec3 operator*(double F, const vec3& v1)
{
	return vec3(v1[0] * F, v1[1] * F, v1[2] * F);
}
vec3 cross(const vec3& x, const vec3& y)
{
	return vec3(x[1] * y[2] - x[2] * y[1], x[2] * y[0] - x[0] * y[2], x[0] * y[1] - x[1] * y[0]);
}