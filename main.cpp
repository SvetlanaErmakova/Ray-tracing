#ifndef HM5_5
#define HM5_5
#include <chrono>
#include "header.h"
//
const int BYTES_PER_PIXEL = 3; /// red, green,blue колво байтов на пиксель
const int FILE_HEADER_SIZE = 14;
const int INFO_HEADER_SIZE = 40;

//есть ли точка пересечения с фигурой
//вернет самую ближнюю т пересечения луча(из cam по dir) с фигурами, если т пересеч с фигурами есть
//нормаль к поверхности в точке пересеч с фигурой, если т пересеч с фигурами есть
//соответствующий цвет для пикселя, если т пересеч с фигурами есть
bool Shapes_intersect(const Screen& scr, const vec3& dir, vector<shape*>& Shapes, vec3& hit, vec3& N, vec3& col)
{
    vec3 hit_ = hit;
    vec3 N_ = N;
    double min_dist = std::numeric_limits<double>::max();
    for (size_t i = 0; i < Shapes.size(); i++)
    {
        double temp_dist = std::numeric_limits<double>::max();
        if (Shapes[i]->ray_intersect(scr.cam, dir, temp_dist, hit_, N_) && temp_dist < min_dist)
        {
            min_dist = temp_dist;
            N = N_;
            hit = hit_;
            col = Shapes[i]->get_col();
        }
    }
    return min_dist <= 1000;// scr.limit / (dir * ((-1) * scr.normal));
}

// цвет в кт надо покрасить пиксель учитывая источник света
vec3 p_color(const Screen& scr, const vec3& dir, vector<shape*>& Shapes, const Light& light)
{
    vec3 point, N, col;
    if (Shapes_intersect(scr, dir, Shapes, point, N, col) == false)
    {
        return vec3(250, 218, 221); //фон
    }

    double diffuse_light_intensity = 0;
    vec3 light_dir = (light.position - point).normalize();
    diffuse_light_intensity = light.intensity * std::max(0.0, double(light_dir * N));

    return col * diffuse_light_intensity;

}

unsigned char* createBitmapInfoHeader(int height, int width)
{
    static unsigned char infoHeader[] = {
        0,0,0,0, /// header size
        0,0,0,0, /// image width
        0,0,0,0, /// image height
        0,0,     /// number of color planes
        0,0,     /// bits per pixel
        0,0,0,0, /// compression
        0,0,0,0, /// image size
        0,0,0,0, /// horizontal resolution
        0,0,0,0, /// vertical resolution
        0,0,0,0, /// colors in color table
        0,0,0,0, /// important color count
    };

    infoHeader[0] = (unsigned char)(INFO_HEADER_SIZE);
    infoHeader[4] = (unsigned char)(width);
    infoHeader[5] = (unsigned char)(width >> 8);
    infoHeader[6] = (unsigned char)(width >> 16);
    infoHeader[7] = (unsigned char)(width >> 24);
    infoHeader[8] = (unsigned char)(height);
    infoHeader[9] = (unsigned char)(height >> 8);
    infoHeader[10] = (unsigned char)(height >> 16);
    infoHeader[11] = (unsigned char)(height >> 24);
    infoHeader[12] = (unsigned char)(1);
    infoHeader[14] = (unsigned char)(BYTES_PER_PIXEL * 8);

    return infoHeader;
}
unsigned char* createBitmapFileHeader(int height, int stride)
{
    int fileSize = FILE_HEADER_SIZE + INFO_HEADER_SIZE + (stride * height);

    static unsigned char fileHeader[] = {
        0,0,     /// signature
        0,0,0,0, /// image file size in bytes
        0,0,0,0, /// reserved
        0,0,0,0, /// start of pixel array
    };

    fileHeader[0] = (unsigned char)('B');
    fileHeader[1] = (unsigned char)('M');
    fileHeader[2] = (unsigned char)(fileSize);
    fileHeader[3] = (unsigned char)(fileSize >> 8);
    fileHeader[4] = (unsigned char)(fileSize >> 16);
    fileHeader[5] = (unsigned char)(fileSize >> 24);
    fileHeader[10] = (unsigned char)(FILE_HEADER_SIZE + INFO_HEADER_SIZE);

    return fileHeader;
}
void generateBitmapImage(const unsigned char* image, int height, int width, char* imageFileName)
{
    int widthInBytes = width * BYTES_PER_PIXEL;

    unsigned char padding[3] = { 0, 0, 0 };
    int paddingSize = (4 - (widthInBytes) % 4) % 4;

    int stride = (widthInBytes)+paddingSize;

    //FILE* imageFile = fopen(imageFileName, "wb");
    FILE* imageFile;
    errno_t err;
    err = fopen_s(&imageFile, imageFileName, "wb");
    //if (err == 0)
    //{
    //    printf("The file 'crt_fopen_s.c' was opened\n");
    //}
    //else
    //{
    //    printf("The file 'crt_fopen_s.c' was not opened\n");
    //}


    unsigned char* fileHeader = createBitmapFileHeader(height, stride);
    fwrite(fileHeader, 1, FILE_HEADER_SIZE, imageFile);

    unsigned char* infoHeader = createBitmapInfoHeader(height, width);
    fwrite(infoHeader, 1, INFO_HEADER_SIZE, imageFile);

    int i;
    for (i = 0; i < height; i++) {
        fwrite(image + (i * widthInBytes), BYTES_PER_PIXEL, width, imageFile);
        fwrite(padding, 1, paddingSize, imageFile);
    }

    fclose(imageFile);
}
class FrameBuffer
{
    std::vector<unsigned char> data;
    int width;
public:
    FrameBuffer(int width, int height) :data(width* height * 3), width(width) {

    }
    const unsigned char* ptr() const {
        return data.data();
    }

    unsigned char& operator()(int i, int j, int plane) {
        return data[(i + j * width) * 3 + plane];
    }

};
void create_img(vector<shape*>& Shapes, Light& light, const Screen& scr)
{
    const int height = scr.height;
    const int width = scr.width;
    vec3 te = scr.normal;
    vec3 t = te.normalize();
    vec3 up = scr.up;
    vec3 f = cross(up, t).normalize();
    vec3 a = cross(t, f);
    double d = scr.screen;
    double alpha = scr.alpha;
    double fov = alpha / 180 * PI;
    double w = d * tan(fov / 2.);
    double h = w * (double(height) / double(width));
    //
    std::vector<vec3> pixels(width * height);

    FrameBuffer image(width, height);

    auto start_for = std::chrono::steady_clock::now();
#pragma omp parallel for
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {

            double dir_[3];
            for (int k = 0; k < 3; k++) {
                dir_[k] = t[k] * d - w * f[k] - h * a[k] + (i * 2 * w) * f[k] / (width - 1) + ((height - j) * 2 * h) * a[k] / (height - 1);
            }
            vec3 dir = vec3(dir_[0], dir_[1], dir_[2]).normalize();

            pixels[i + j * width] = p_color(scr, dir, Shapes, light);
        }
    }
    auto end_for = std::chrono::steady_clock::now();
    std::cout << "Time is " << std::chrono::duration<double, std::milli>(end_for - start_for).count() << " ms" << "\n";


    char* FileName = (char*)"rez.bmp";

    int i, j;
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            image(i, j, 2) = pixels[i + j * width][2]; ///red
            image(i, j, 1) = pixels[i + j * width][1]; ///green
            image(i, j, 0) = pixels[i + j * width][0]; ///blue
        }
    }
    generateBitmapImage(image.ptr(), height, width, FileName);
}
//
int main()
{
    try
    {
        //cout << "Input file name: \n";
        //string file_name;
        //cin >> file_name;

        //ifstream inp(file_name);
        //if (!inp)
        //{
        //    throw runtime_error("Error! Cannot open file!");
        //}

        string file_name = "data_f1.txt";// "data_f1.txt";
        ifstream inp;
        inp.open(file_name);

        std::vector<shape*> Shapes;
        string I;

        map<string, Factory*> factoryMap;
        factoryMap["sphere"] = new Factory_sphere;
        factoryMap["box"] = new Factory_box;
        factoryMap["tetra"] = new Factory_tetra;

        while (!inp.eof())
        {
            inp >> I;

            auto Creator = factoryMap.find(I);
            if (Creator == factoryMap.end())
                throw runtime_error("There is no such class!");
            string stroka;
            getline(inp, stroka);

            Shapes.push_back(Creator->second->create(stroka));

        }
        inp.close();

        //
        //cout << "Input file name: \n";
        //string file_name;
        //cin >> file_name;

        //ifstream inp(file_name);
        //if (!inp)
        //{
        //    throw runtime_error("Error! Cannot open file!");
        //}

        string file_name1 = "data_s1.txt";// "data_s1.txt";
        ifstream inp1;
        inp1.open(file_name1);
        vec3 cam, light, normal, up;
        double screen;
        double limit;
        double alpha;
        double width;
        double height;
        string A;
        while (!inp1.eof())
        {
            inp1 >> A;
            if (A == "cam")
            {
                inp1 >> cam.x; inp1 >> cam.y; inp1 >> cam.z;
            }
            else if (A == "normal")
            {
                inp1 >> normal.x; inp1 >> normal.y; inp1 >> normal.z;
            }
            else if (A == "up")
            {
                inp1 >> up.x; inp1 >> up.y; inp1 >> up.z;
            }
            else if (A == "screen")
                inp1 >> screen;
            else if (A == "limit")
                inp1 >> limit;
            else if (A == "alpha")
                inp1 >> alpha;
            else if (A == "width")
                inp1 >> width;
            else if (A == "height")
                inp1 >> height;
            else if (A == "light")
            {
                inp1 >> light.x; inp1 >> light.y; inp1 >> light.z;
            }

        }
        inp1.close();
        //
        Light light_ = { light, 1.0 };
        Screen scr = { cam, normal.normalize(), up, screen, limit, alpha, width, height };

        //Shapes.push_back(new sphere("sp", vec3(186, 85, 211), vec3(-3, 0, -16), 2.0));
        //Shapes.push_back(new sphere("sp", vec3(221, 160, 221), vec3(0, 10, -50), 2.0));
        //Shapes.push_back(new box("box", vec3(225, 85, 211), vec3(-5, -3, -10), vec3(-7, -7, -30)));
        //Shapes.push_back(new tetra("tetra", vec3(34, 173, 214), vec3(-2, 2, -15), vec3(0, -2, -18), vec3(6, 0.0, -20), vec3(0, 2, -15)));

        create_img(Shapes, light_, scr);
    }
    catch (const exception& e)
    {
        cerr << e.what() << '\n';
        return 1;
    }
}

#endif