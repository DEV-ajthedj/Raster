#include "draw.h"

frame_buffer_t buffer;
frame_buffer_t *frame_buffer = &buffer;
scene_t scene;

// FRAME BUFFER

void init_buffer() {
    for (int y = 0; y < FRAME_HEIGHT; y++) {
        for (int x = 0; x < FRAME_WIDTH; x++) {
            for (int c = 0; c < COLOR_CHANNELS; c++) {
                frame_buffer->color[y][x][c] = 0;
            }
            frame_buffer->depth[y][x] = 255;
        }
    }
}

void fill_buffer(unsigned char depth, unsigned char color[COLOR_CHANNELS]) {
    for (int y = 0; y < FRAME_HEIGHT; y++) {
        for (int x = 0; x < FRAME_WIDTH; x++) {
            for (int c = 0; c < COLOR_CHANNELS; c++) {
                frame_buffer->color[y][x][c] = color[c];
            }
            frame_buffer->depth[y][x] = depth;
        }
    }
}

void draw_pixel(int x, int y, unsigned char depth, unsigned char color[COLOR_CHANNELS]) {
    if (x >= 0 && x < FRAME_WIDTH && y >= 0 && y < FRAME_HEIGHT) {
        for (int c = 0; c < COLOR_CHANNELS; c++) {
            frame_buffer->color[y][x][c] = color[c];
        }
        frame_buffer->depth[y][x] = depth;
    }
}

static void draw_char(int x, int y, char c, const uint8_t* font, unsigned char color[COLOR_CHANNELS]) {
    if(c < font[3] || c > font[4]) return;

    uint32_t parts_per_line = (font[0] >> 3) + ((font[0] & 7) > 0);
    for (uint8_t w = 0; w < font[1]; w++) {
        uint32_t pp = (c - font[3]) * font[1] * parts_per_line + w * parts_per_line + 5;
        for (uint32_t lp = 0; lp < parts_per_line; lp++) {
            uint8_t line = font[pp];
            for (int8_t j = 0; j < 8; ++j, line >>= 1) {
                if (line & 1) {
                    draw_pixel(x + w, y + (lp << 3) + j, 0, color);
                }
            }
            pp++;
        }
    }
}

void draw_string(int x, int y, const char* s, const uint8_t* font, unsigned char color[COLOR_CHANNELS]) {
    for (int32_t x_n = x; *s; x_n += (font[1] + font[2])) {
        draw_char(x_n, y, *(s++), font, color);
    }
}

// BASIC MATH

float clamp(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

vector3_t vec3_add(vector3_t v1, vector3_t v2) {
    return (vector3_t){v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
}

vector3_t vec3_sub(vector3_t v1, vector3_t v2) {
    return (vector3_t){v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
}

float vec3_dot(vector3_t v1, vector3_t v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

vector3_t vec3_cross(vector3_t v1, vector3_t v2) {
    return (vector3_t){
        v1.y * v2.z - v1.z * v2.y,
        v1.z * v2.x - v1.x * v2.z,
        v1.x * v2.y - v1.y * v2.x
    };
}

float vec3_mag(vector3_t v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

vector3_t vec3_norm(vector3_t v) {
    float mag = vec3_mag(v);
    if (mag == 0) return (vector3_t){0, 0, 0};
    return (vector3_t){v.x / mag, v.y / mag, v.z / mag};
}

vector3_t vec3_mul_scalar(vector3_t v, float s) {
    return (vector3_t){v.x * s, v.y * s, v.z * s};
}

vector3_t vec4_to_vec3(vector4_t v) {
    if (v.w != 0) {
        return (vector3_t){v.x / v.w, v.y / v.w, v.z / v.w};
    } else {
        return (vector3_t){v.x, v.y, v.z};
    }
}

vector4_t vec4_add(vector4_t v1, vector4_t v2) {
    return (vector4_t){v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w};
}

vector4_t vec4_sub(vector4_t v1, vector4_t v2) {
    return (vector4_t){v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w};
}

float vec4_dot(vector4_t v1, vector4_t v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}

float vec4_mag(vector4_t v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

vector4_t vec4_norm(vector4_t v) {
    float mag = vec4_mag(v);
    if (mag == 0) return (vector4_t){0, 0, 0, 0};
    return (vector4_t){v.x / mag, v.y / mag, v.z / mag, v.w / mag};
}

vector4_t vec4_mul_scalar(vector4_t v, float scalar) {
    return (vector4_t){v.x * scalar, v.y * scalar, v.z * scalar, v.w * scalar};
}

vector4_t vec3_to_vec4(vector3_t v) {
    return (vector4_t){v.x, v.y, v.z, 1.0f};
}

matrix4_t mat4_identity() {
    return (matrix4_t){{
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    }};
}

vector4_t mat4_mul_vec4(matrix4_t m, vector4_t v) {
    vector4_t result;
    result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w;
    result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w;
    result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w;
    result.w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w;
    return result;
}

matrix4_t mat4_mul(matrix4_t m1, matrix4_t m2) {
    matrix4_t result = {{{0}}};
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 4; k++) {
                result.m[i][j] += m1.m[i][k] * m2.m[k][j];
            }
        }
    }
    return result;
}

matrix4_t mat4_translate(float x, float y, float z) {
    matrix4_t result = mat4_identity();
    result.m[0][3] = x;
    result.m[1][3] = y;
    result.m[2][3] = z;
    return result;
}

matrix4_t mat4_scale(float x, float y, float z) {
    matrix4_t result = mat4_identity();
    result.m[0][0] = x;
    result.m[1][1] = y;
    result.m[2][2] = z;
    return result;
}

matrix4_t mat4_rotate(float x, float y, float z) {
    matrix4_t rx = mat4_identity();
    matrix4_t ry = mat4_identity();
    matrix4_t rz = mat4_identity();

    float cx = cosf(x), sx = sinf(x);
    float cy = cosf(y), sy = sinf(y);
    float cz = cosf(z), sz = sinf(z);

    rx.m[1][1] = cx; rx.m[1][2] = -sx;
    rx.m[2][1] = sx; rx.m[2][2] = cx;

    ry.m[0][0] = cy; ry.m[0][2] = sy;
    ry.m[2][0] = -sy; ry.m[2][2] = cy;

    rz.m[0][0] = cz; rz.m[0][1] = -sz;
    rz.m[1][0] = sz; rz.m[1][1] = cz;

    return mat4_mul(mat4_mul(rz, ry), rx);
}

matrix4_t mat4_perspective(float fov, float aspect, float near, float far) {
    matrix4_t result = {{{0}}};
    float f = 1.0f / tanf(fov / 2.0f);
    result.m[0][0] = f / aspect;
    result.m[1][1] = f;
    result.m[2][2] = (far + near) / (near - far);
    result.m[2][3] = (2 * far * near) / (near - far);
    result.m[3][2] = -1.0f;
    return result;
}

matrix4_t look_at(vector3_t eye, vector3_t center, vector3_t up) {
    vector3_t f = vec3_norm(vec3_sub(center, eye));
    vector3_t s = vec3_norm(vec3_cross(f, up));
    vector3_t u = vec3_cross(s, f);

    matrix4_t result = mat4_identity();
    result.m[0][0] = s.x; result.m[0][1] = s.y; result.m[0][2] = s.z;
    result.m[1][0] = u.x; result.m[1][1] = u.y; result.m[1][2] = u.z;
    result.m[2][0] = -f.x; result.m[2][1] = -f.y; result.m[2][2] = -f.z;
    result.m[0][3] = -vec3_dot(s, eye);
    result.m[1][3] = -vec3_dot(u, eye);
    result.m[2][3] = vec3_dot(f, eye);
    return result;
}

matrix4_t mat4_inv(matrix4_t m) {
    matrix4_t inv;
    float det;
    int i;

    inv.m[0][0] = m.m[1][1] * m.m[2][2] * m.m[3][3] - m.m[1][1] * m.m[2][3] * m.m[3][2] - m.m[2][1] * m.m[1][2] * m.m[3][3] + m.m[2][1] * m.m[1][3] * m.m[3][2] + m.m[3][1] * m.m[1][2] * m.m[2][3] - m.m[3][1] * m.m[1][3] * m.m[2][2];
    inv.m[0][1] = -m.m[0][1] * m.m[2][2] * m.m[3][3] + m.m[0][1] * m.m[2][3] * m.m[3][2] + m.m[2][1] * m.m[0][2] * m.m[3][3] - m.m[2][1] * m.m[0][3] * m.m[3][2] - m.m[3][1] * m.m[0][2] * m.m[2][3] + m.m[3][1] * m.m[0][3] * m.m[2][2];
    inv.m[0][2] = m.m[0][1] * m.m[1][2] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[3][2] - m.m[1][1] * m.m[0][2] * m.m[3][3] + m.m[1][1] * m.m[0][3] * m.m[3][2] + m.m[3][1] * m.m[0][2] * m.m[1][3] - m.m[3][1] * m.m[0][3] * m.m[1][2];
    inv.m[0][3] = -m.m[0][1] * m.m[1][2] * m.m[2][3] + m.m[0][1] * m.m[1][3] * m.m[2][2] + m.m[1][1] * m.m[0][2] * m.m[2][3] - m.m[1][1] * m.m[0][3] * m.m[2][2] - m.m[2][1] * m.m[0][2] * m.m[1][3] + m.m[2][1] * m.m[0][3] * m.m[1][2];
    det = m.m[0][0] * inv.m[0][0] + m.m[0][1] * inv.m[0][1] + m.m[0][2] * inv.m[0][2] + m.m[0][3] * inv.m[0][3];
    if (det == 0) {
        // Handle non-invertible matrix case (return identity or zero matrix)
        return mat4_identity(); // or return a zero matrix
    }
    det = 1.0f / det;
    for (i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            inv.m[i][j] *= det;
        }
    }
    return inv;
}

// TESTING

void print_frame() {
    for (int y = 0; y < FRAME_HEIGHT; y += 1) {
        for (int x = 0; x < FRAME_WIDTH; x += 1) {
            printf("%3d ", frame_buffer->depth[y][x]);
        }
        printf("\n");
    }
}