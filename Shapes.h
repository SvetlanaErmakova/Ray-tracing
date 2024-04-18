#ifndef SHAPE
#define SHAPE

#include "header.h"
#include <algorithm>
class shape
{
protected:
    string type = "sphere";
    vec3 color = { 225,0,0 };

public:
    shape() = default;//конструктор по умолчанию
    shape(const string& type_, const vec3& color_)//конструктор
    {
        type = type_;
        color = color_;
    }

    //фции
    virtual bool ray_intersect(const vec3& orig, const vec3& dir, double& t0, vec3& hit, vec3& N) = 0;
    vec3 get_col()
    {
        return color;
    }
    string get_type()
    {
        return type;
    }
};

class sphere : public shape
{
private:
    vec3 center = { 0.0, 0.0, 0.0 };
    double radius = 2.0;
public:
    sphere() = default;
    sphere(const string& type, const vec3& color, const vec3& center, const double& radius) : shape(type, color)//конструктор
    {
        this->center = center;
        this->radius = radius;
    }

    //фции
    bool ray_intersect(const vec3& orig, const vec3& dir, double& t0, vec3& hit, vec3& N)  override
    {
        bool s = true;
        vec3 L = center - orig;
        float tca = L * dir;//проекция L на dir(нормированный),если разноправлены то отриц
        float d2 = L * L - tca * tca;//расстояние от center до луча dir
        if (d2 > radius * radius) s = false;//не пересекает шар
        float thc = sqrtf(radius * radius - d2);//половина хорды между двумя т пересечения шара
        t0 = tca - thc;//расстояние по dir до ближней точки пересеч(ближней от начала коорд до шара)
        float t1 = tca + thc;//до дальней

        if (t0 < 0) t0 = t1;//левая точка за началом координат
        if (t0 < 0) s = false;//весь шар с др стороны от начала коор-т - не с той что экран
        //

        hit = orig + dir * t0;//точка пересечения с шаром
        N = (hit - get_center()).normalize();//нормаль наружу в данной точке шара: hit

        return s;
    }
    vec3 get_center()
    {
        return center;
    }
    double get_radius()
    {
        return radius;
    }

};
class box : public shape
{
private:
    vec3 v_min = { 0.0, 0.0, 0.0 };
    vec3 v_max = { 2.0, 2.0, 2.0 };

    int func(const vec3& A, const vec3& B, const vec3& C, const vec3& D, const vec3& orig, const vec3& dir, double& t1, vec3& N)
    {
        double d = triangle_intersection(orig, dir, A, B, C); //расстояние по лучу до пересеч с треугольником или 0
        if (d == 0)
            d = triangle_intersection(orig, dir, A, C, D);//чтобы рассм-ть весь прямоуг
        if (d <= 0)
            return -1;
        if (d > t1 && t1 > 0)//это дальняя точка пересеч
            return -1;

        N = cross(B - A, D - A).normalize();
        t1 = d;

        return 0;//нашли точку пересеч луча с прямоугольником грани
    }
    double triangle_intersection(const vec3& orig, const vec3 dir, const vec3& v0, const vec3& v1, const vec3& v2)//алг моллера-тр
    {
        vec3 e1 = v1 - v0;
        vec3 e2 = v2 - v0;
        // Вычисление вектора нормали к плоскости
        vec3 pvec = cross(dir, e2);
        double det = e1 * pvec;

        // Луч параллелен плоскости
        if (det < 1e-8 && det > -1e-8)
        {
            return 0;
        }

        double inv_det = 1 / det;
        vec3 tvec = orig - v0;
        double u = (tvec * pvec) * inv_det;
        if (u < 0 || u > 1) {
            return 0;
        }

        vec3 qvec = cross(tvec, e1);
        double v = (dir * qvec) * inv_det;
        if (v < 0 || u + v > 1) {
            return 0;
        }
        return (e2 * qvec) * inv_det;
    }

public:
    box() = default;
    box(const string& type, const vec3& color, const vec3& v_min, const vec3& v_max) : shape(type, color)//конструктор
    {
        this->v_min = v_min;
        this->v_max = v_max;
    }

    //фции
    bool ray_intersect(const vec3& orig, const vec3& dir, double& t0, vec3& hit, vec3& N)  override
    {
        double t1 = -1;
        //координаты восьми вершин параллелепипеда: ниж грань A B C D, верх грань A1 B1 C1 D1
        vec3 A = v_min;
        vec3 A1 = vec3(v_min[0], v_max[1], v_min[2]);
        vec3 B = vec3(v_max[0], v_min[1], v_min[2]);
        vec3 B1 = vec3(v_max[0], v_max[1], v_min[2]);
        vec3 C = vec3(v_max[0], v_min[1], v_max[2]);
        vec3 C1 = v_max;
        vec3 D = vec3(v_min[0], v_min[1], v_max[2]);
        vec3 D1 = vec3(v_min[0], v_max[1], v_max[2]);

        //обход с нижней левой точки против часовой
        int f1 = func(A, B, B1, A1, orig, dir, t1, N);

        int f2 = func(B, C, C1, B1, orig, dir, t1, N);
        int f3 = func(C, D, D1, C1, orig, dir, t1, N);
        int f4 = func(D, A, A1, D1, orig, dir, t1, N);
        int f5 = func(A1, B1, C1, D1, orig, dir, t1, N);

        //int f6 = func(A, B, C, D, orig, dir, t1, N);
        int f6 = func(D, C, B, A, orig, dir, t1, N);

        t0 = t1;
        hit = orig + dir * t0;
        if (f1 * f2 * f3 * f4 * f5 * f6 == 0)//есть точка пересеч с паралл-м
            return true;
        return false;//нет точки пересеч с паралл-м
    }
    vec3 get_v_min()
    {
        return v_min;
    }
    vec3 get_v_max()
    {
        return v_max;
    }
};
//
class tetra : public shape
{
private:
    vec3 v1 = { 0.0, 0.0, 0.0 };
    vec3 v2 = { 2.0, 2.0, 0.0 };
    vec3 v3 = { 0.0, 2.0, 2.0 };
    vec3 v4 = { 2.0, 0.0, 2.0 };
    int func(const vec3& A, const vec3& B, const vec3& C, const vec3& D, const vec3& orig, const vec3& dir, double& t1, vec3& N)
    {
        double d = triangle_intersection(orig, dir, A, B, C); //расстояние по лучу до пересеч с треугольником или 0
        if (d <= 0)
            return -1;
        if (d > t1 && t1 > 0)//это дальняя точка пересеч
            return -1;

        auto n = cross(B - A, C - B).normalize();
        if (n * (D - C) >= 0)
            n = n * (-1);

        if (n * dir > 0)
            return -1; // не видит те точки

        //n = n * (-1);

        N = n;
        t1 = d;
        return 0;//нашли точку пересеч луча с треугольником грани
    }
    double triangle_intersection(const vec3& orig, const vec3 dir, const vec3& v0, const vec3& v1, const vec3& v2) //алг моллера-тр
    {
        vec3 e1 = v1 - v0;
        vec3 e2 = v2 - v0;
        // Вычисление вектора нормали к плоскости
        vec3 pvec = cross(dir, e2);
        double det = e1 * pvec;

        // Луч параллелен плоскости
        if (det < 1e-8 && det > -1e-8)
        {
            return 0;
        }

        double inv_det = 1 / det;
        vec3 tvec = orig - v0;
        double u = (tvec * pvec) * inv_det;
        if (u < 0 || u > 1) {
            return 0;
        }

        vec3 qvec = cross(tvec, e1);
        double v = (dir * qvec) * inv_det;
        if (v < 0 || u + v > 1) {
            return 0;
        }
        return (e2 * qvec) * inv_det;
    }

public:
    tetra() = default;
    tetra(const string& type, const vec3& color, const vec3& v1, const vec3& v2, const vec3& v3, const vec3& v4) : shape(type, color)//конструктор
    {
        this->v1 = v1; this->v2 = v2; this->v3 = v3; this->v4 = v4;
    }


    bool ray_intersect(const vec3& orig, const vec3& dir, double& t0, vec3& hit, vec3& N)  override
    {
        double t1 = -1;


        int f1 = func(v1, v2, v3, v4, orig, dir, t1, N); //N = N * (-1);
        int f2 = func(v2, v3, v4, v1, orig, dir, t1, N);
        int f3 = func(v1, v4, v3, v2, orig, dir, t1, N);
        int f4 = func(v1, v2, v4, v3, orig, dir, t1, N); //N = N * (-1);

        t0 = t1;
        hit = orig + dir * t0;
        if (f1 * f2 * f3 * f4 == 0)
            return true;
        return false;//нет точки пересеч
    }

    vec3 get_v1()
    {
        return v1;
    }
    vec3 get_v2()
    {
        return v2;
    }
    vec3 get_v3()
    {
        return v3;
    }
    vec3 get_v4()
    {
        return v4;
    }
};

#endif

