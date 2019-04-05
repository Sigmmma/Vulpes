#pragma once

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
    // TODO: Implement functions to retrieve corresponding memory objects.
};

class Vec2d{
public:
    float x;
    float y;

    //float get_magnitude();
    //float scale_by(float factor);
};

class Vec3d{
public:
    float x;
    float y;
    float z;

    //float get_magnitude();
    //float scale_by(float factor);
};

class Euler2d {
    float i;
    float j;

    //void normalize();
};

class Euler3d {
    float i;
    float j;
    float k;

    //void normalize();
};

class Quaternion {
    float i;
    float j;
    float k;
    float w;

    //void normalize();
};
