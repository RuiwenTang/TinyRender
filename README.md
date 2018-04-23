# TinyRender
> A project to learn how OpenGL Works.    
>Use TGA Image to hold render result, so not depend on any platform Window system.
### BasicTriangle Render Algrithom
. Use Triangle center of gravity to decided whether a pixle is inside a Triangle.
The algrithom is like this:
```c++
/*
P = (1 - u - v) x A + u x B + v x C 
AP = u x AC + v x AB 
u x AC + v x AB + PA = 0
u x (c - a) + v x (b - a) + (a - p) = 0

| u x (c - a)[0] + v x (b - a)[0] + (a - p)[0] = 0
| u x (c - a)[1] + v x (b - a)[1] + (a - p)[1] = 0
*/
```
the full code is in [Triangle.cpp](./example/Triangle.cpp)    
Result like this:    
![BasicTriangle](./screenshoots/basictriangle.png)
### ColorTriangle Render
. The center of gravity algrithom can also be used to do color interpolation. Just like glsl keyworkd `varying`    
The code segment like this:    
```c++
Vec2i p;
for (p[0] = leftTop[0]; p[0] <= rightBottom[0]; p[0]++) {
    for (p[1] = leftTop[1]; p[1] <= rightBottom[1]; p[1]++) {
        Vec3f bc_screen = barycentric(a, b, c, p);
        if (bc_screen[0] < 0 || bc_screen[1] < 0 || bc_screen[2] < 0) continue;

        mImage->set(p[0], p[1], color);
    }
}
```
And full code is in [ColorTriangle.cpp](./example/ColorTriangle.cpp)
Result like this:    
![ColorTriangle](./screenshoots/colortriangle.png)
