#ifndef __FACTORY
#define __FACTORY
#include "homework5_7.h"

class Factory
{
public:
	virtual shape* create(const string& str) = 0;
};

class Factory_sphere : public Factory
{
public:
	shape* create(const string& str) override
	{
		istringstream iss(str);
		double num; vec3 col, cent; double rad;
		iss >> col.x;
		iss >> col.y;
		iss >> col.z;
		iss >> cent.x;
		iss >> cent.y;
		iss >> cent.z;
		iss >>rad;
		
		return new sphere("sphere", col, cent, rad); 
	}
};

class Factory_box : public Factory
{
public:
	shape* create(const string& str) override
	{
		istringstream iss(str);
		double num; vec3 col, v_min, v_max; double rad;
		iss >> col.x;
		iss >> col.y;
		iss >> col.z;
		iss >> v_min.x;
		iss >> v_min.y;
		iss >> v_min.z;
		iss >> v_max.x;
		iss >> v_max.y;
		iss >> v_max.z;
		return new box("box", col, v_min, v_max);
	}
};
class Factory_tetra : public Factory
{
public:
	shape* create(const string& str) override
	{
		istringstream iss(str);
		double num; vec3 col, v1, v2, v3, v4; 
		iss >> col.x;
		iss >> col.y;
		iss >> col.z;
		iss >> v1.x;
		iss >> v1.y;
		iss >> v1.z;
		iss >> v2.x;
		iss >> v2.y;
		iss >> v2.z;
		iss >> v3.x;
		iss >> v3.y;
		iss >> v3.z;
		iss >> v4.x;
		iss >> v4.y;
		iss >> v4.z;

		return new tetra("tetra", col, v1, v2, v3, v4);
	}
};


#endif 