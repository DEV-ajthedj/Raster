#ifndef DRAW_H
#define DRAW_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "fonts.h"

#define FRAME_WIDTH 128
#define FRAME_HEIGHT 160
#define MAX_POINTS 1000
#define COLOR_CHANNELS 3

typedef struct {
    unsigned char color[FRAME_HEIGHT][FRAME_WIDTH][COLOR_CHANNELS];
    unsigned char depth[FRAME_HEIGHT][FRAME_WIDTH];
} frame_buffer_t;

typedef struct {
    float x, y, z;
} vector3_t;

typedef struct {
    float x, y, z, w;
} vector4_t;

typedef struct {
    float m[4][4];
} matrix4_t;

typedef struct {
    vector3_t position;
    vector3_t normal;
} vertex_t;

typedef struct {
    vertex_t *vertices;
    int vertex_count;

    int *indices;
    int index_count;

    int color[COLOR_CHANNELS];
} mesh_t;

typedef struct {
    mesh_t mesh;
    matrix4_t model;
} object_t;

typedef struct {
    object_t *objects;
    int object_count;
} scene_t;

// VARIABLES

extern frame_buffer_t buffer;
extern frame_buffer_t *frame_buffer;
extern scene_t scene;

// FRAME BUFFER

void init_buffer();
void fill_buffer(unsigned char depth, unsigned char color[COLOR_CHANNELS]);
void draw_pixel(int x, int y, unsigned char depth, unsigned char color[COLOR_CHANNELS]);
void draw_string(int x, int y, const char* s, const uint8_t* font, unsigned char color[COLOR_CHANNELS]);

// BASIC MATH

//Clamp a value between a minimum and maximum range
//@param value The value to clamp
//@param min The minimum value to clamp to
//@param max The maximum value to clamp to
//@return The clamped value
float clamp(float value, float min, float max);
//Add two 3-dimensional vectors together
//@param v1 The first 3D vector
//@param v2 The second 3D vector
//@return The resulting 3D vector of the addition
vector3_t vec3_add(vector3_t v1, vector3_t v2);
//Subtract one 3-dimensional vector from another
//@param v1 The first 3D vector
//@param v2 The second 3D vector to subtract
//@return The resulting 3D vector of the subtraction
vector3_t vec3_sub(vector3_t v1, vector3_t v2);
//Calculate the dot product of two 3-dimensional vectors
//@param v1 The first 3D vector
//@param v2 The second 3D vector
//@return The dot product of the two 3D vectors
float vec3_dot(vector3_t v1, vector3_t v2);
//Calculate the cross product of two 3-dimensional vectors
//@param v1 The first 3D vector
//@param v2 The second 3D vector
//@return The cross product of the two 3D vectors
vector3_t vec3_cross(vector3_t v1, vector3_t v2);
//Calculate the magnitude of a 3-dimensional vector
//@param v The 3D vector to calculate the magnitude of
//@return The magnitude of the 3D vector
float vec3_mag(vector3_t v);
//Normalize a 3-dimensional vector to have a magnitude of 1
//@param v The 3D vector to normalize
//@return The normalized 3D vector
vector3_t vec3_norm(vector3_t v);
//Multiply a 3-dimensional vector by a scalar value
//@param v The 3D vector to multiply
//@param s The scalar value to multiply the vector by
//@return The resulting 3D vector of the multiplication
vector3_t vec3_mul_scalar(vector3_t v, float s);
//Convert a 4-dimensional vector to a 3-dimensional vector
//@param v The 4D vector to convert. If `w` is non-zero, a perspective divide is applied (x/w, y/w, z/w)
//@return The resulting 3D vector
vector3_t vec4_to_vec3(vector4_t v);
//Add two 4-dimensional vectors together
//@param v1 The first 4D vector
//@param v2 The second 4D vector
//@return The resulting 4D vector of the addition
vector4_t vec4_add(vector4_t v1, vector4_t v2);
//Subtract one 4-dimensional vector from another
//@param v1 The first 4D vector
//@param v2 The second 4D vector to subtract
//@return The resulting 4D vector of the subtraction
vector4_t vec4_sub(vector4_t v1, vector4_t v2);
//Calculate the dot product of two 4-dimensional vectors
//@param v1 The first 4D vector
//@param v2 The second 4D vector
//@return The dot product of the two 4D vectors
float vec4_dot(vector4_t v1, vector4_t v2);
//Calculate the magnitude of a 4-dimensional vector
//@param v The 4D vector to calculate the magnitude of
//@return The magnitude of the 4D vector
float vec4_mag(vector4_t v);
//Normalize a 4-dimensional vector to have a magnitude of 1
//@param v The 4D vector to normalize
//@return The normalized 4D vector
vector4_t vec4_norm(vector4_t v);
//Multiply a 4-dimensional vector by a scalar value
//@param v The 4D vector to multiply
//@param scalar The scalar value to multiply the vector by
//@return The resulting 4D vector of the multiplication
vector4_t vec4_mul_scalar(vector4_t v, float scalar);
//Create a 4-dimensional vector from a 3-dimensional vector
//@param v The 3D vector to convert
//@return The resulting 4D vector with `w` set to 1.0
vector4_t vec3_to_vec4(vector3_t v);
//Generate a 4x4 identity matrix
//@return A `matrix4_t` representing the identity transform
matrix4_t mat4_identity();
//Multiply a 4x4 matrix by a 4-dimensional vector
//@param m The 4x4 matrix
//@param v The 4D vector
//@return The resulting 4D vector from the multiplication
vector4_t mat4_mul_vec4(matrix4_t m, vector4_t v);
//Multiply two 4x4 matrices (m1 * m2)
//@param m1 The left-hand 4x4 matrix
//@param m2 The right-hand 4x4 matrix
//@return The resulting 4x4 matrix of the multiplication
matrix4_t mat4_mul(matrix4_t m1, matrix4_t m2);
//Create a translation matrix
//@param x Translation along the X axis
//@param y Translation along the Y axis
//@param z Translation along the Z axis
//@return A `matrix4_t` that translates by `(x, y, z)`
matrix4_t mat4_translate(float x, float y, float z);
//Create a scale matrix
//@param x Scaling factor along the X axis
//@param y Scaling factor along the Y axis
//@param z Scaling factor along the Z axis
//@return A `matrix4_t` that scales by `(x, y, z)`
matrix4_t mat4_scale(float x, float y, float z);
//Create a rotation matrix from Euler angles
//@param x Rotation angle around the X axis (radians)
//@param y Rotation angle around the Y axis (radians)
//@param z Rotation angle around the Z axis (radians)
//@return A `matrix4_t` representing the combined rotation (X, then Y, then Z)
matrix4_t mat4_rotate(float x, float y, float z);
//Create a perspective projection matrix
//@param fov Field of view in radians (vertical FOV)
//@param aspect Aspect ratio (width / height)
//@param near Distance to the near clipping plane (positive)
//@param far Distance to the far clipping plane (positive)
//@return A `matrix4_t` that maps camera-space into clip space using a perspective projection
matrix4_t perspective(float fov, float aspect, float near, float far);
//Create a view matrix that looks from `eye` towards `target` using `up` as the up direction
//@param eye The camera position in world space
//@param target The point the camera is looking at
//@param up The up direction vector (usually {0,1,0})
//@return A `matrix4_t` representing the camera view transform
matrix4_t look_at(vector3_t eye, vector3_t target, vector3_t up);
//Compute the inverse of a 4x4 matrix
//@param m The matrix to invert
//@return The inverse of `m`. If `m` is non-invertible, the behavior depends on the implementation
matrix4_t mat4_inv(matrix4_t m);

// WORLD DATA
/*
void init_world();
void draw_point(vector3_t p, unsigned char color[COLOR_CHANNELS]);
void draw_line(vector3_t pt0, vector3_t pt1, unsigned char color[COLOR_CHANNELS]);
void draw_sphere(vector3_t center, float radius, unsigned char color[COLOR_CHANNELS]);

// LIGHTING

void set_ambient_light(unsigned char intensity);
void set_directional_light(vector3_t origin, vector3_t target, unsigned char intensity);
*/
// TESTING

void print_frame();


#endif