#ifndef VEC3
#define VEC3

class vec3
{
public:
    double x;
    double y;
    double z;

    vec3();//конструктор по умолчанию
    vec3(double x, double y, double z);//конструктор 
    vec3(const vec3& a)
    {
        x = a.x;
        y = a.y;
        z = a.z;
    }

    vec3& operator=(const vec3& other);
    const double& operator[](size_t i) const;

    //фции
    double lenght();
    vec3& normalize();
};

vec3 operator+(const vec3& v1, const vec3& v2);
vec3 operator-(const vec3& v1, const vec3& v2);
double operator*(const vec3& v1, const vec3& v2);
vec3 operator*(const vec3& v1, double F);
vec3 operator*(double F, const vec3& v1);
vec3 cross(const vec3& x, const vec3& y);

#endif


