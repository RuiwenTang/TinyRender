#include <Geometry.h>
#include <iostream>

int main(int argc, const char** argv) {
    TRM::vec<3, float> v1, v2;

    v1[0] = 1.f;
    v1[1] = 1.f;
    v1[2] = 1.f;
    v2[0] = 1.f;
    v2[1] = 1.f;
    v2[2] = 1.f;


    std::cout<<"v1 * v2 = "<<v1 * v2<<std::endl;

    TRM::Vec3f v3;
    v3 = v1;
    std::cout<<"v3[0] = "<<v3[0]<<std::endl;

    TRM::Mat44 mat1;
    mat1.identity();
    mat1[0][0] = 2;
    TRM::Vec4f v4;
    v4[0] = 1.f;
    v4[1] = 2.f;
    v4[2] = 3.f;
    v4[3] = 4.f;

    TRM::Vec4f res = mat1 * v4;

    std::cout<<"v4[0] = "<<v4[0]<<std::endl;

    TRM::mat<3, 3, float> mat3;
    mat3.identity();
    TRM::Vec3f v6;
    v6[0] = 1.f;
    v6[1] = 1.f;
    v6[2] = 1.f;

    mat3 * v6;

    TRM::Vec2i a {0, 0};
    TRM::Vec2i b {2, 0};
    TRM::Vec2i c {2, 2};
    TRM::Vec2i p {1, 1};

    auto ret = TRM::barycentric(a, b, c, p);

    bool outside = (ret[0] < 0 || ret [1] < 0 || ret[2] < 0);
    std::cout << "outside = " << std::boolalpha << outside << std::endl;

    return 0;
}
