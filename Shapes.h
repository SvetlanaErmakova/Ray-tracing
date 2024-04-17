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
    shape() = default;//êîíñòðóêòîð ïî óìîë÷àíèþ
    shape(const string& type_, const vec3& color_)//êîíñòðóêòîð
    {
        type = type_;
        color = color_;
    }

    //ôöèè
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
    sphere(const string& type, const vec3& color, const vec3& center, const double& radius) : shape(type, color)//êîíñòðóêòîð
    {
        this->center = center;
        this->radius = radius;
    }

    //ôöèè
    bool ray_intersect(const vec3& orig, const vec3& dir, double& t0, vec3& hit, vec3& N)  override
    {
        bool s = true;
        vec3 L = center - orig;
        float tca = L * dir;//ïðîåêöèÿ L íà dir(íîðìèðîâàííûé),åñëè ðàçíîïðàâëåíû òî îòðèö
        float d2 = L * L - tca * tca;//ðàññòîÿíèå îò center äî ëó÷à dir
        if (d2 > radius * radius) s = false;//íå ïåðåñåêàåò øàð
        float thc = sqrtf(radius * radius - d2);//ïîëîâèíà õîðäû ìåæäó äâóìÿ ò ïåðåñå÷åíèÿ øàðà
        t0 = tca - thc;//ðàññòîÿíèå ïî dir äî áëèæíåé òî÷êè ïåðåñå÷(áëèæíåé îò íà÷àëà êîîðä äî øàðà)
        float t1 = tca + thc;//äî äàëüíåé

        if (t0 < 0) t0 = t1;//ëåâàÿ òî÷êà çà íà÷àëîì êîîðäèíàò
        if (t0 < 0) s = false;//âåñü øàð ñ äð ñòîðîíû îò íà÷àëà êîîð-ò - íå ñ òîé ÷òî ýêðàí
        //

        hit = orig + dir * t0;//òî÷êà ïåðåñå÷åíèÿ ñ øàðîì
        N = (hit - get_center()).normalize();//íîðìàëü íàðóæó â äàííîé òî÷êå øàðà: hit

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
        double d = triangle_intersection(orig, dir, A, B, C); //ðàññòîÿíèå ïî ëó÷ó äî ïåðåñå÷ ñ òðåóãîëüíèêîì èëè 0
        if (d == 0)
            d = triangle_intersection(orig, dir, A, C, D);//÷òîáû ðàññì-òü âåñü ïðÿìîóã
        if (d <= 0)
            return -1;
        if (d > t1 && t1 > 0)//ýòî äàëüíÿÿ òî÷êà ïåðåñå÷
            return -1;

        N = cross(B - A, D - A).normalize();
        t1 = d;

        return 0;//íàøëè òî÷êó ïåðåñå÷ ëó÷à ñ ïðÿìîóãîëüíèêîì ãðàíè
    }
    double triangle_intersection(const vec3& orig, const vec3 dir, const vec3& v0, const vec3& v1, const vec3& v2)//àëã ìîëëåðà-òð
    {
        vec3 e1 = v1 - v0;
        vec3 e2 = v2 - v0;
        // Âû÷èñëåíèå âåêòîðà íîðìàëè ê ïëîñêîñòè
        vec3 pvec = cross(dir, e2);
        double det = e1 * pvec;

        // Ëó÷ ïàðàëëåëåí ïëîñêîñòè
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
    box(const string& type, const vec3& color, const vec3& v_min, const vec3& v_max) : shape(type, color)//êîíñòðóêòîð
    {
        this->v_min = v_min;
        this->v_max = v_max;
    }

    //ôöèè
    bool ray_intersect(const vec3& orig, const vec3& dir, double& t0, vec3& hit, vec3& N)  override
    {
        double t1 = -1;
        //êîîðäèíàòû âîñüìè âåðøèí ïàðàëëåëåïèïåäà: íèæ ãðàíü A B C D, âåðõ ãðàíü A1 B1 C1 D1
        vec3 A = v_min;
        vec3 A1 = vec3(v_min[0], v_max[1], v_min[2]);
        vec3 B = vec3(v_max[0], v_min[1], v_min[2]);
        vec3 B1 = vec3(v_max[0], v_max[1], v_min[2]);
        vec3 C = vec3(v_max[0], v_min[1], v_max[2]);
        vec3 C1 = v_max;
        vec3 D = vec3(v_min[0], v_min[1], v_max[2]);
        vec3 D1 = vec3(v_min[0], v_max[1], v_max[2]);

        //îáõîä ñ íèæíåé ëåâîé òî÷êè ïðîòèâ ÷àñîâîé
        int f1 = func(A, B, B1, A1, orig, dir, t1, N); 

        int f2 = func(B, C, C1, B1, orig, dir, t1, N);
        int f3 = func(C, D, D1, C1, orig, dir, t1, N);
        int f4 = func(D, A, A1, D1, orig, dir, t1, N); 
        int f5 = func(A1, B1, C1, D1, orig, dir, t1, N); 

        //int f6 = func(A, B, C, D, orig, dir, t1, N);
        int f6 = func(D, C, B, A, orig, dir, t1, N);

        t0 = t1;
        hit = orig + dir * t0;
        if (f1 * f2 * f3 * f4 * f5 * f6 == 0)//åñòü òî÷êà ïåðåñå÷ ñ ïàðàëë-ì
            return true;
        return false;//íåò òî÷êè ïåðåñå÷ ñ ïàðàëë-ì
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
        double d = triangle_intersection(orig, dir, A, B, C); //ðàññòîÿíèå ïî ëó÷ó äî ïåðåñå÷ ñ òðåóãîëüíèêîì èëè 0
        if (d <= 0)
            return -1;
        if (d > t1 && t1 > 0)//ýòî äàëüíÿÿ òî÷êà ïåðåñå÷
            return -1;

        auto n = cross(B - A, C - B).normalize();
        if (n * (D - C) >= 0)
            n = n * (-1);

        if (n * dir > 0)
            return -1; // íå âèäèò òå òî÷êè

        //n = n * (-1);

        N = n;
        t1 = d;
        return 0;//íàøëè òî÷êó ïåðåñå÷ ëó÷à ñ òðåóãîëüíèêîì ãðàíè
    }
    double triangle_intersection(const vec3& orig, const vec3 dir, const vec3& v0, const vec3& v1, const vec3& v2) //àëã ìîëëåðà-òð
    {
        vec3 e1 = v1 - v0;
        vec3 e2 = v2 - v0;
        // Âû÷èñëåíèå âåêòîðà íîðìàëè ê ïëîñêîñòè
        vec3 pvec = cross(dir, e2);
        double det = e1 * pvec;

        // Ëó÷ ïàðàëëåëåí ïëîñêîñòè
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
    tetra(const string& type, const vec3& color, const vec3& v1, const vec3& v2, const vec3& v3, const vec3& v4) : shape(type, color)//êîíñòðóêòîð
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
        return false;//íåò òî÷êè ïåðåñå÷
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

