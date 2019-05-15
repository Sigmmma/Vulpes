#pragma once
#include <cstdint>

#define CONCAT(x, y) x ## y
#define CONCAT2(x, y) CONCAT(x, y)
#define PAD(s) uint8_t CONCAT2(padding, __LINE__)[s]
#define BITPAD(type,s) type CONCAT2(padding, __LINE__) : s

#define NULL __null

// Class for references in Halo memory. Not the same as TagRef.
class MemRef {
public:
union {
    struct {
    uint16_t local;
    uint16_t salt;  //Like a global id, the game assigns one of these to everything that has an id.
                    //It just counts up for each assignment until it loops over
    }id;

    uint32_t raw;
};
    bool valid();
    // TODO: Implement functions to retrieve corresponding memory objects.
};

class Vec2d{
public:
    float x;
    float y;

    float get_magnitude();
};

class Vec3d{
public:
    float x;
    float y;
    float z;

    float get_magnitude();
};

class Euler2d {
public:
    float i;
    float j;

    void normalize();
};

class Euler3d {
public:
    float i;
    float j;
    float k;

    void normalize();
};

class Quaternion {
public:
    float i;
    float j;
    float k;
    float w;

    Quaternion(){
        i=0.0;j=0.0;k=0.0;w=1.0;
    }

    void normalize();
};

class RGBFloat {
public:
    float red;
    float green;
    float blue;

    RGBFloat(float r = 1.0, float g = 1.0, float b = 1.0){red=r;green=g;blue=b;}
};

class ARGBFloat {
public:
    float alpha;
    float red;
    float green;
    float blue;

    ARGBFloat(float a = 1.0, float r = 1.0, float g = 1.0, float b = 1.0){alpha=a;red=r;green=g;blue=b;}
};

class Rectangle2d {
public:
    float t;
    float l;
    float b;
    float r;
};

class Plane3d {
public:
    float x;
    float y;
    float z;
    float d;
};
