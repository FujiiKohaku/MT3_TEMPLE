#define _USE_MATH_DEFINES
#include <Novice.h>
#include <algorithm>
#include <assert.h>
#include <cmath>
#include <imgui.h>
#include <math.h>
#include <stdio.h>

const char kWindowTitle[] = "LE2B_21_フジイ_コハク";

struct Vector3 { // ok
    float x, y, z;
};
struct Vector4 {
    float x, y, z, w;
};
struct Plane { // ok
    Vector3 normal; // 法線
    float distance; // 距離
};
struct Matrix4x4 { // ok
    float m[4][4];
};
struct Sphere { // ok
    Vector3 center;
    float radius;
};

struct Segment {
    Vector3 origin; // 始点
    Vector3 diff; // 終点への差分ベクトル
};
struct Ray {
    Vector3 origin; // 始点
    Vector3 diff; // 終点への差分ベクトル
};
struct Line {
    Vector3 origin; // 始点
    Vector3 diff; // 終点への差分ベクトル
};
enum class LineType {
    Infinite, // 無限直線
    Ray, // 半直線
    Segment // 線分
};

struct AABB {
    Vector3 min;
    Vector3 max;
};
struct Triangle {
    Vector3 vertices[3]; // 頂点
};


#pragma region 関数
// クロス積
Vector3 Cross(const Vector3& v1, const Vector3& v2)
{
    float cx = v1.y * v2.z - v1.z * v2.y;
    float cy = v1.z * v2.x - v1.x * v2.z;
    float cz = v1.x * v2.y - v1.y * v2.x;

    return Vector3(cx, cy, cz);
}
// 平行移動行列
Matrix4x4 MakeTranslateMatrix(const Vector3& tlanslate)
{
    Matrix4x4 result = {};
    result.m[0][0] = 1.0f;
    result.m[1][1] = 1.0f;
    result.m[2][2] = 1.0f;
    result.m[3][3] = 1.0f;
    result.m[3][0] = tlanslate.x;
    result.m[3][1] = tlanslate.y;
    result.m[3][2] = tlanslate.z;

    return result;
}
// 拡大縮小行列
Matrix4x4 Matrix4x4MakeScaleMatrix(const Vector3& s)
{
    Matrix4x4 result = {};
    result.m[0][0] = s.x;
    result.m[1][1] = s.y;
    result.m[2][2] = s.z;
    result.m[3][3] = 1.0f;
    return result;
}
// 座標変換
// ベクトルを変換
Vector3 Transform(const Vector3& v, const Matrix4x4& m) // ok
{
    float x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + m.m[3][0];
    float y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + m.m[3][1];
    float z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + m.m[3][2];
    float w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + m.m[3][3];
    if (w != 0.0f) {
        x /= w;
        y /= w;
        z /= w;
    }
    return { x, y, z };
}

// X軸回転行列
Matrix4x4 MakeRotateXMatrix(const float radian)
{
    Matrix4x4 result = {};

    result.m[0][0] = 1.0f;
    result.m[1][1] = std::cos(radian);
    result.m[1][2] = std::sin(radian);
    result.m[2][1] = -std::sin(radian);
    result.m[2][2] = std::cos(radian);
    result.m[3][3] = 1.0f;

    return result;
}
// Y軸回転行列
Matrix4x4 MakeRotateYMatrix(const float radian)
{
    Matrix4x4 result = {};

    result.m[0][0] = std::cos(radian);
    result.m[0][2] = std::sin(radian);
    result.m[1][1] = 1.0f;
    result.m[2][0] = -std::sin(radian);
    result.m[2][2] = std::cos(radian);
    result.m[3][3] = 1.0f;

    return result;
}
// Z軸回転行列
Matrix4x4 MakeRotateZMatrix(const float radian)
{
    Matrix4x4 result = {};

    result.m[0][0] = std::cos(radian);
    result.m[0][1] = -std::sin(radian);
    result.m[1][0] = std::sin(radian);
    result.m[1][1] = std::cos(radian);
    result.m[2][2] = 1.0f;
    result.m[3][3] = 1.0f;

    return result;
}
// 表示関数
int kColumnWidth = 60;
int kRowHeight = 20;
void MatrixScreenPrintf(const int x, const int y, const Matrix4x4& matirix,
    const char* label)
{
    Novice::ScreenPrintf(x, y, "%s", label);
    for (int row = 0; row < 4; ++row) {
        for (int column = 0; column < 4; ++column) {
            Novice::ScreenPrintf(x + column * kColumnWidth,
                y + (row + 1) * kRowHeight, "%6.03f",
                matirix.m[row][column]);
        }
    }
}
// 行列の積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2)
{
    Matrix4x4 result {};
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            for (int k = 0; k < 4; ++k)
                result.m[i][j] += m1.m[i][k] * m2.m[k][j];
    return result;
}

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate,
    const Vector3& translate)
{
    Matrix4x4 scaleMatrix = Matrix4x4MakeScaleMatrix(scale);
    Matrix4x4 rotateX = MakeRotateXMatrix(rotate.x);
    Matrix4x4 rotateY = MakeRotateYMatrix(rotate.y);
    Matrix4x4 rotateZ = MakeRotateZMatrix(rotate.z);
    Matrix4x4 rotateMatrix = Multiply(Multiply(rotateX, rotateY), rotateZ);
    Matrix4x4 translateMatrix = MakeTranslateMatrix(translate);

    Matrix4x4 worldMatrix = Multiply(Multiply(scaleMatrix, rotateMatrix), translateMatrix);
    return worldMatrix;
}

// 転置行列
Matrix4x4 Transpose(const Matrix4x4& m)
{
    Matrix4x4 result {};
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            result.m[i][j] = m.m[j][i];
    return result;
}

// 単位行列の作成
Matrix4x4 MakeIdentity4x4()
{
    Matrix4x4 result {};
    for (int i = 0; i < 4; ++i)
        result.m[i][i] = 1.0f;
    return result;
}

// 4x4 行列の逆行列を計算する関数
Matrix4x4 Inverse(const Matrix4x4 m)
{
    Matrix4x4 result;
    float det;
    int i;

    result.m[0][0] = m.m[1][1] * m.m[2][2] * m.m[3][3] - m.m[1][1] * m.m[2][3] * m.m[3][2] - m.m[2][1] * m.m[1][2] * m.m[3][3] + m.m[2][1] * m.m[1][3] * m.m[3][2] + m.m[3][1] * m.m[1][2] * m.m[2][3] - m.m[3][1] * m.m[1][3] * m.m[2][2];

    result.m[0][1] = -m.m[0][1] * m.m[2][2] * m.m[3][3] + m.m[0][1] * m.m[2][3] * m.m[3][2] + m.m[2][1] * m.m[0][2] * m.m[3][3] - m.m[2][1] * m.m[0][3] * m.m[3][2] - m.m[3][1] * m.m[0][2] * m.m[2][3] + m.m[3][1] * m.m[0][3] * m.m[2][2];

    result.m[0][2] = m.m[0][1] * m.m[1][2] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[3][2] - m.m[1][1] * m.m[0][2] * m.m[3][3] + m.m[1][1] * m.m[0][3] * m.m[3][2] + m.m[3][1] * m.m[0][2] * m.m[1][3] - m.m[3][1] * m.m[0][3] * m.m[1][2];

    result.m[0][3] = -m.m[0][1] * m.m[1][2] * m.m[2][3] + m.m[0][1] * m.m[1][3] * m.m[2][2] + m.m[1][1] * m.m[0][2] * m.m[2][3] - m.m[1][1] * m.m[0][3] * m.m[2][2] - m.m[2][1] * m.m[0][2] * m.m[1][3] + m.m[2][1] * m.m[0][3] * m.m[1][2];

    result.m[1][0] = -m.m[1][0] * m.m[2][2] * m.m[3][3] + m.m[1][0] * m.m[2][3] * m.m[3][2] + m.m[2][0] * m.m[1][2] * m.m[3][3] - m.m[2][0] * m.m[1][3] * m.m[3][2] - m.m[3][0] * m.m[1][2] * m.m[2][3] + m.m[3][0] * m.m[1][3] * m.m[2][2];

    result.m[1][1] = m.m[0][0] * m.m[2][2] * m.m[3][3] - m.m[0][0] * m.m[2][3] * m.m[3][2] - m.m[2][0] * m.m[0][2] * m.m[3][3] + m.m[2][0] * m.m[0][3] * m.m[3][2] + m.m[3][0] * m.m[0][2] * m.m[2][3] - m.m[3][0] * m.m[0][3] * m.m[2][2];

    result.m[1][2] = -m.m[0][0] * m.m[1][2] * m.m[3][3] + m.m[0][0] * m.m[1][3] * m.m[3][2] + m.m[1][0] * m.m[0][2] * m.m[3][3] - m.m[1][0] * m.m[0][3] * m.m[3][2] - m.m[3][0] * m.m[0][2] * m.m[1][3] + m.m[3][0] * m.m[0][3] * m.m[1][2];

    result.m[1][3] = m.m[0][0] * m.m[1][2] * m.m[2][3] - m.m[0][0] * m.m[1][3] * m.m[2][2] - m.m[1][0] * m.m[0][2] * m.m[2][3] + m.m[1][0] * m.m[0][3] * m.m[2][2] + m.m[2][0] * m.m[0][2] * m.m[1][3] - m.m[2][0] * m.m[0][3] * m.m[1][2];

    result.m[2][0] = m.m[1][0] * m.m[2][1] * m.m[3][3] - m.m[1][0] * m.m[2][3] * m.m[3][1] - m.m[2][0] * m.m[1][1] * m.m[3][3] + m.m[2][0] * m.m[1][3] * m.m[3][1] + m.m[3][0] * m.m[1][1] * m.m[2][3] - m.m[3][0] * m.m[1][3] * m.m[2][1];

    result.m[2][1] = -m.m[0][0] * m.m[2][1] * m.m[3][3] + m.m[0][0] * m.m[2][3] * m.m[3][1] + m.m[2][0] * m.m[0][1] * m.m[3][3] - m.m[2][0] * m.m[0][3] * m.m[3][1] - m.m[3][0] * m.m[0][1] * m.m[2][3] + m.m[3][0] * m.m[0][3] * m.m[2][1];

    result.m[2][2] = m.m[0][0] * m.m[1][1] * m.m[3][3] - m.m[0][0] * m.m[1][3] * m.m[3][1] - m.m[1][0] * m.m[0][1] * m.m[3][3] + m.m[1][0] * m.m[0][3] * m.m[3][1] + m.m[3][0] * m.m[0][1] * m.m[1][3] - m.m[3][0] * m.m[0][3] * m.m[1][1];

    result.m[2][3] = -m.m[0][0] * m.m[1][1] * m.m[2][3] + m.m[0][0] * m.m[1][3] * m.m[2][1] + m.m[1][0] * m.m[0][1] * m.m[2][3] - m.m[1][0] * m.m[0][3] * m.m[2][1] - m.m[2][0] * m.m[0][1] * m.m[1][3] + m.m[2][0] * m.m[0][3] * m.m[1][1];

    result.m[3][0] = -m.m[1][0] * m.m[2][1] * m.m[3][2] + m.m[1][0] * m.m[2][2] * m.m[3][1] + m.m[2][0] * m.m[1][1] * m.m[3][2] - m.m[2][0] * m.m[1][2] * m.m[3][1] - m.m[3][0] * m.m[1][1] * m.m[2][2] + m.m[3][0] * m.m[1][2] * m.m[2][1];

    result.m[3][1] = m.m[0][0] * m.m[2][1] * m.m[3][2] - m.m[0][0] * m.m[2][2] * m.m[3][1] - m.m[2][0] * m.m[0][1] * m.m[3][2] + m.m[2][0] * m.m[0][2] * m.m[3][1] + m.m[3][0] * m.m[0][1] * m.m[2][2] - m.m[3][0] * m.m[0][2] * m.m[2][1];

    result.m[3][2] = -m.m[0][0] * m.m[1][1] * m.m[3][2] + m.m[0][0] * m.m[1][2] * m.m[3][1] + m.m[1][0] * m.m[0][1] * m.m[3][2] - m.m[1][0] * m.m[0][2] * m.m[3][1] - m.m[3][0] * m.m[0][1] * m.m[1][2] + m.m[3][0] * m.m[0][2] * m.m[1][1];

    result.m[3][3] = m.m[0][0] * m.m[1][1] * m.m[2][2] - m.m[0][0] * m.m[1][2] * m.m[2][1] - m.m[1][0] * m.m[0][1] * m.m[2][2] + m.m[1][0] * m.m[0][2] * m.m[2][1] + m.m[2][0] * m.m[0][1] * m.m[1][2] - m.m[2][0] * m.m[0][2] * m.m[1][1];

    det = m.m[0][0] * result.m[0][0] + m.m[0][1] * result.m[1][0] + m.m[0][2] * result.m[2][0] + m.m[0][3] * result.m[3][0];

    if (det == 0)
        return Matrix4x4 {}; // またはエラー処理

    det = 1.0f / det;

    for (i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            result.m[i][j] = result.m[i][j] * det;

    return result;
}

// 透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(const float fovY, const float aspectRatio,
    const float nearClip, const float farClip)
{
    Matrix4x4 result = {};

    float f = 1.0f / std::tan(fovY / 2.0f);

    result.m[0][0] = f / aspectRatio;
    result.m[1][1] = f;
    result.m[2][2] = farClip / (farClip - nearClip);
    result.m[2][3] = 1.0f;
    result.m[3][2] = (-nearClip * farClip) / (farClip - nearClip);

    return result;
}
// 正射影行列
Matrix4x4 MakeOrthographicMatrix(const float left, const float top,
    const float right, const float bottom,
    const float nearClip, const float farClip)
{
    Matrix4x4 m = {};

    m.m[0][0] = 2.0f / (right - left);
    m.m[1][1] = 2.0f / (top - bottom);
    m.m[2][2] = 1.0f / (farClip - nearClip);
    m.m[3][0] = -(right + left) / (right - left);
    m.m[3][1] = -(top + bottom) / (top - bottom);
    m.m[3][2] = -nearClip / (farClip - nearClip);
    m.m[3][3] = 1.0f;

    return m;
}
// ビューポート変換行列
Matrix4x4 MakeViewportMatrix(const float left, const float top,
    const float width, const float height,
    const float minDepth, const float maxDepth)
{
    Matrix4x4 m = {};

    // 行0：X方向スケーリングと移動
    m.m[0][0] = width / 2.0f;
    m.m[3][0] = left + width / 2.0f;
    m.m[1][1] = -height / 2.0f;
    m.m[3][1] = top + height / 2.0f;
    m.m[2][2] = maxDepth - minDepth;
    m.m[2][3] = minDepth;
    m.m[3][3] = 1.0f;

    return m;
}
// 加算
Vector3 Add(const Vector3& v1, const Vector3& v2)
{
    Vector3 result;
    result.x = v1.x + v2.x;
    result.y = v1.y + v2.y;
    result.z = v1.z + v2.z;
    return result;
}
// 減算
Vector3 Subtract(const Vector3& v1, const Vector3& v2)
{
    Vector3 result;
    result.x = v1.x - v2.x;
    result.y = v1.y - v2.y;
    result.z = v1.z - v2.z;
    return result;
}
// スカラー倍
Vector3 Multiply(const float scalar, const Vector3& v2)
{
    Vector3 result;
    result.x = scalar * v2.x;
    result.y = scalar * v2.y;
    result.z = scalar * v2.z;
    return result;
}
// 内積
float Dot(const Vector3& v1, const Vector3& v2) // ok
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}
// 長さ（ノルム）
float Length(const Vector3& v) // ok
{
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

// 正規化
Vector3 Nomalize(const Vector3& v) // ok
{
    float length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    if (length == 0.0f) {
        return { 0.0f, 0.0f, 0.0f };
    }
    return { v.x / length, v.y / length, v.z / length };
}

// 球体作成関数
void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix,
    const Matrix4x4& viewportMatrix, uint32_t color)
{
    const uint32_t kSubdivision = 16;
    const float pi = 3.1415926535f;
    const float kLatEvery = pi / kSubdivision;
    const float kLonEvery = 2 * pi / kSubdivision;

    for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
        float lat = -pi / 2.0f + kLatEvery * latIndex;
        float nextLat = lat + kLatEvery;

        for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
            float lon = lonIndex * kLonEvery;
            float nextLon = lon + kLonEvery;

            Vector3 a = { sphere.center.x + sphere.radius * cosf(lat) * cosf(lon),
                sphere.center.y + sphere.radius * sinf(lat),
                sphere.center.z + sphere.radius * cosf(lat) * sinf(lon) };
            Vector3 b = { sphere.center.x + sphere.radius * cosf(nextLat) * cosf(lon),
                sphere.center.y + sphere.radius * sinf(nextLat),
                sphere.center.z + sphere.radius * cosf(nextLat) * sinf(lon) };
            Vector3 c = { sphere.center.x + sphere.radius * cosf(lat) * cosf(nextLon),
                sphere.center.y + sphere.radius * sinf(lat),
                sphere.center.z + sphere.radius * cosf(lat) * sinf(nextLon) };

            Vector3 screenA = Transform(Transform(a, viewProjectionMatrix), viewportMatrix);
            Vector3 screenB = Transform(Transform(b, viewProjectionMatrix), viewportMatrix);
            Vector3 screenC = Transform(Transform(c, viewProjectionMatrix), viewportMatrix);

            Novice::DrawLine(int(screenA.x), int(screenA.y), int(screenB.x),
                int(screenB.y), color);
            Novice::DrawLine(int(screenA.x), int(screenA.y), int(screenC.x),
                int(screenC.y), color);
        }
    }
}
// Grid描画関数
void DrawGrid(const Matrix4x4& viewProjectionMatrix,
    const Matrix4x4& viewportMatrix)
{
    const float kGridHalfwidth = 2.0f; // グリッドの半分の幅
    const uint32_t kSubdivision = 10; // 分割数
    const float kGridEvery = (kGridHalfwidth * 2.0f) / float(kSubdivision); // 一つ分の長さ

    // 奥から手前への線を順々に引いていく
    for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {
        float x = -kGridHalfwidth + kGridEvery * xIndex;

        // ワールド座標系の支点と終点
        Vector3 start = { x, 0.0f, -kGridHalfwidth };
        Vector3 end = { x, 0.0f, kGridHalfwidth };

        // 変換（ワールド→スクリーン）
        Vector3 startScreen = Transform(start, viewProjectionMatrix);
        startScreen = Transform(startScreen, viewportMatrix);

        Vector3 endScreen = Transform(end, viewProjectionMatrix);
        endScreen = Transform(endScreen, viewportMatrix);

        // 色を決める（原点の線は黒、それ以外は薄い灰色）
        uint32_t color = (x == 0.0f) ? 0x000000FF : 0xAAAAAAFF;

        // 線を引く
        Novice::DrawLine(int(startScreen.x), int(startScreen.y), int(endScreen.x),
            int(endScreen.y), color);
    }
    // 左から右への線を順々に引いていく
    for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex) {
        float z = -kGridHalfwidth + kGridEvery * zIndex;

        Vector3 start = { -kGridHalfwidth, 0.0f, z };
        Vector3 end = { kGridHalfwidth, 0.0f, z };

        Vector3 startScreen = Transform(start, viewProjectionMatrix);
        startScreen = Transform(startScreen, viewportMatrix);

        Vector3 endScreen = Transform(end, viewProjectionMatrix);
        endScreen = Transform(endScreen, viewportMatrix);

        uint32_t color = (z == 0.0f) ? 0x000000FF : 0xAAAAAAFF;

        Novice::DrawLine(int(startScreen.x), int(startScreen.y), int(endScreen.x),
            int(endScreen.y), color);
    }
}
Vector3 operator+(const Vector3& v1, const Vector3& v2) { return Add(v1, v2); }
Vector3 operator-(const Vector3& v1, const Vector3& v2) { return Subtract(v1, v2); }
Vector3 operator*(float s, const Vector3& v) { return Multiply(s, v); }
Vector3 operator*(const Vector3& v, float s) { return s * v; }
Matrix4x4 operator*(const Matrix4x4 m1, const Matrix4x4& m2) { return Multiply(m1, m2); }

Matrix4x4 MakeRotateAxisAngle(const Vector3& axis, float angle)
{
    // 軸ベクトルを正規化
    Vector3 n = Nomalize(axis);
    float x = n.x;
    float y = n.y;
    float z = n.z; 

    float c = std::cos(angle);
    float s = std::sin(angle);
    float k = 1.0f - c;

    Matrix4x4 result = {};

    // ここが「行ベクトル用 R」の式
    result.m[0][0] = x * x * k + c;
    result.m[0][1] = x * y * k + z * s;
    result.m[0][2] = x * z * k - y * s;
    result.m[0][3] = 0.0f;

    result.m[1][0] = x * y * k - z * s;
    result.m[1][1] = y * y * k + c;
    result.m[1][2] = y * z * k + x * s;
    result.m[1][3] = 0.0f;

    result.m[2][0] = x * z * k + y * s;
    result.m[2][1] = y * z * k - x * s;
    result.m[2][2] = z * z * k + c;
    result.m[2][3] = 0.0f;

    result.m[3][0] = 0.0f;
    result.m[3][1] = 0.0f;
    result.m[3][2] = 0.0f;
    result.m[3][3] = 1.0f;

    return result;
}


#pragma endregion

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{

    // ライブラリの初期化
    Novice::Initialize(kWindowTitle, 1280, 720);

    // キー入力結果を受け取る箱
    char keys[256] = { 0 };
    char preKeys[256] = { 0 };

    // 初期化,定義
    Vector3 rotate = {};
    Vector3 translate = {};
    Vector3 cameraTransLate { 0.0f, 1.9f, -6.49f };
    Vector3 cameraRotate { 0.26f, 0.0f, 0.0f };
    int kWindowWidth = 1280;
    int kWindowHeight = 720;

    // ウィンドウの×ボタンが押されるまでループ
    while (Novice::ProcessMessage() == 0) {
        // フレームの開始
        Novice::BeginFrame();

        // キー入力を受け取る
        memcpy(preKeys, keys, 256);
        Novice::GetHitKeyStateAll(keys);

        ///
        /// ↓更新処理ここから
        ///

#pragma region カメラ計算
        // スケール × 回転 × 平行移動 = カメラの世界行列（位置と向き）
        Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, cameraRotate, cameraTransLate);
        // 視点座標系に変換（カメラの逆変換）＝ビュー行列
        Matrix4x4 viewMatrix = Inverse(cameraMatrix);
        // 透視投影（遠近感を加える）
        // 透視投影行列の計算
        Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(float(0.45), float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
        // NDC（-1〜+1）を画面ピクセル座標に変換する
        Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);
        Matrix4x4 WorldViewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
#pragma endregion

    /*    Vector3 a { 0.2f, 1.0f, 0.0f };
        Vector3 b { 2.4f, 3.1f, 1.2f };
        Vector3 c = a + b;
        Vector3 d = a - b;
        Vector3 e = a * 2.4f;

        Vector3 rotate1 { 0.4f, 1.43f, -0.8f };

        Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rotate1.x);
        Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rotate1.y);
        Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotate1.z);

        Matrix4x4 rotateMatrix = rotateXMatrix * rotateYMatrix * rotateZMatrix;*/

        ImGui::Begin("Control Panel");

        ImGui::End();

        /// ↑更新処理ここまで
        ///

        Vector3 axis = Nomalize({ 1.0f, 1.0f, 1.0f }); // 回転軸
        float angle = 0.44f; // ラジアン角

        Matrix4x4 rotateMatrix = MakeRotateAxisAngle(axis, angle);

        MatrixScreenPrintf(0, 0, rotateMatrix, "rotateMatrix");

        ///
        /// ↓描画処理ここから
        ///

        /// ↑描画処理ここまで
        ///

        // フレームの終了
        Novice::EndFrame();

        // ESCキーが押されたらループを抜ける
        if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
            break;
        }
    }

    // ライブラリの終了
    Novice::Finalize();
    return 0;
}