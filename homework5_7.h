#ifndef HM5_7
#define HM5_7

#include <string>

#include <sstream>

#include <fstream>
#include <iostream>
using namespace std;
#include <map>
#include <vector>
#include <cmath>
#define _USE_MATH_DEFINES
#define PI 3.14159265358979323846
#include <math.h>

//#include "homework5_7.cpp"
#include "vec3.h"
#include "Shapes.h"

#include "Factory.h"

struct Light
{
    vec3 position;
    float intensity;
};

struct Screen
{
    vec3 cam;
    vec3 normal;
    vec3 up;
    double screen;
    double limit;
    double alpha;
    double width;
    double height;
};
//
bool Shapes_intersect(const vec3& cam, const vec3& dir, vector<shape*>& Shapes, vec3& hit, vec3& N, vec3& col);
vec3 p_color(const vec3& cam, const vec3& dir, vector<shape*>& Shapes, const Light& light);
unsigned char* createBitmapInfoHeader(int height, int width);
unsigned char* createBitmapFileHeader(int height, int stride);
void generateBitmapImage(unsigned char* image, int height, int width, char* imageFileName);
void create_img(vector<shape*>& Shapes, Light& light);

#endif

