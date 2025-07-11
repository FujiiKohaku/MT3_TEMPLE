#define _USE_MATH_DEFINES
#include <Novice.h>
#include <assert.h>
#include <cmath>
#include <imgui.h>
#include <math.h>
#include <stdio.h>

const char kWindowTitle[] = "LE2C_25_フジイ_コハク";
struct Vector3 { // ok
  float x, y, z;
};
struct Vector4 {
  float x, y, z, w;
};
struct Plane {    // ok
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
  Vector3 diff;   // 終点への差分ベクトル
};
struct Ray {
  Vector3 origin; // 始点
  Vector3 diff;   // 終点への差分ベクトル
};
struct Line {
  Vector3 origin; // 始点
  Vector3 diff;   // 終点への差分ベクトル
};
enum class LineType {
  Infinite, // 無限直線
  Ray,      // 半直線
  Segment   // 線分
};

struct Triangle {
  Vector3 vertices[3]; // 頂点
};
#pragma region 関数
// クロス積
Vector3 Cross(const Vector3 &v1, const Vector3 &v2) {
  float cx = v1.y * v2.z - v1.z * v2.y;
  float cy = v1.z * v2.x - v1.x * v2.z;
  float cz = v1.x * v2.y - v1.y * v2.x;

  return Vector3(cx, cy, cz);
}
// 平行移動行列
Matrix4x4 MakeTranslateMatrix(const Vector3 &tlanslate) {
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
Matrix4x4 Matrix4x4MakeScaleMatrix(const Vector3 &s) {
  Matrix4x4 result = {};
  result.m[0][0] = s.x;
  result.m[1][1] = s.y;
  result.m[2][2] = s.z;
  result.m[3][3] = 1.0f;
  return result;
}
// 座標変換
// ベクトルを変換
Vector3 Transform(const Vector3 &v, const Matrix4x4 &m) // ok
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
  return {x, y, z};
}

// X軸回転行列
Matrix4x4 MakeRotateXMatrix(const float radian) {
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
Matrix4x4 MakeRotateYMatrix(const float radian) {
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
Matrix4x4 MakeRotateZMatrix(const float radian) {
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
void MatrixScreenPrintf(const int x, const int y, const Matrix4x4 &matirix,
                        const char *label) {
  Novice::ScreenPrintf(x, y, "%s", label);
  for (int row = 0; row < 4; ++row) {
    for (int column = 0; column < 4; ++column) {
      Novice::ScreenPrintf(x + column * kColumnWidth,
                           y + (row + 1) * kRowHeight, "%6.02f",
                           matirix.m[row][column]);
    }
  }
}
// 行列の積
Matrix4x4 Multiply(const Matrix4x4 &m1, const Matrix4x4 &m2) {
  Matrix4x4 result{};
  for (int i = 0; i < 4; ++i)
    for (int j = 0; j < 4; ++j)
      for (int k = 0; k < 4; ++k)
        result.m[i][j] += m1.m[i][k] * m2.m[k][j];
  return result;
}

Matrix4x4 MakeAffineMatrix(const Vector3 &scale, const Vector3 &rotate,
                           const Vector3 &translate) {
  Matrix4x4 scaleMatrix = Matrix4x4MakeScaleMatrix(scale);
  Matrix4x4 rotateX = MakeRotateXMatrix(rotate.x);
  Matrix4x4 rotateY = MakeRotateYMatrix(rotate.y);
  Matrix4x4 rotateZ = MakeRotateZMatrix(rotate.z);
  Matrix4x4 rotateMatrix = Multiply(Multiply(rotateX, rotateY), rotateZ);
  Matrix4x4 translateMatrix = MakeTranslateMatrix(translate);

  Matrix4x4 worldMatrix =
      Multiply(Multiply(scaleMatrix, rotateMatrix), translateMatrix);
  return worldMatrix;
}

// 転置行列
Matrix4x4 Transpose(const Matrix4x4 &m) {
  Matrix4x4 result{};
  for (int i = 0; i < 4; ++i)
    for (int j = 0; j < 4; ++j)
      result.m[i][j] = m.m[j][i];
  return result;
}

// 単位行列の作成
Matrix4x4 MakeIdentity4x4() {
  Matrix4x4 result{};
  for (int i = 0; i < 4; ++i)
    result.m[i][i] = 1.0f;
  return result;
}

// 4x4 行列の逆行列を計算する関数
Matrix4x4 Inverse(const Matrix4x4 m) {
  Matrix4x4 result;
  float det;
  int i;

  result.m[0][0] =
      m.m[1][1] * m.m[2][2] * m.m[3][3] - m.m[1][1] * m.m[2][3] * m.m[3][2] -
      m.m[2][1] * m.m[1][2] * m.m[3][3] + m.m[2][1] * m.m[1][3] * m.m[3][2] +
      m.m[3][1] * m.m[1][2] * m.m[2][3] - m.m[3][1] * m.m[1][3] * m.m[2][2];

  result.m[0][1] =
      -m.m[0][1] * m.m[2][2] * m.m[3][3] + m.m[0][1] * m.m[2][3] * m.m[3][2] +
      m.m[2][1] * m.m[0][2] * m.m[3][3] - m.m[2][1] * m.m[0][3] * m.m[3][2] -
      m.m[3][1] * m.m[0][2] * m.m[2][3] + m.m[3][1] * m.m[0][3] * m.m[2][2];

  result.m[0][2] =
      m.m[0][1] * m.m[1][2] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[3][2] -
      m.m[1][1] * m.m[0][2] * m.m[3][3] + m.m[1][1] * m.m[0][3] * m.m[3][2] +
      m.m[3][1] * m.m[0][2] * m.m[1][3] - m.m[3][1] * m.m[0][3] * m.m[1][2];

  result.m[0][3] =
      -m.m[0][1] * m.m[1][2] * m.m[2][3] + m.m[0][1] * m.m[1][3] * m.m[2][2] +
      m.m[1][1] * m.m[0][2] * m.m[2][3] - m.m[1][1] * m.m[0][3] * m.m[2][2] -
      m.m[2][1] * m.m[0][2] * m.m[1][3] + m.m[2][1] * m.m[0][3] * m.m[1][2];

  result.m[1][0] =
      -m.m[1][0] * m.m[2][2] * m.m[3][3] + m.m[1][0] * m.m[2][3] * m.m[3][2] +
      m.m[2][0] * m.m[1][2] * m.m[3][3] - m.m[2][0] * m.m[1][3] * m.m[3][2] -
      m.m[3][0] * m.m[1][2] * m.m[2][3] + m.m[3][0] * m.m[1][3] * m.m[2][2];

  result.m[1][1] =
      m.m[0][0] * m.m[2][2] * m.m[3][3] - m.m[0][0] * m.m[2][3] * m.m[3][2] -
      m.m[2][0] * m.m[0][2] * m.m[3][3] + m.m[2][0] * m.m[0][3] * m.m[3][2] +
      m.m[3][0] * m.m[0][2] * m.m[2][3] - m.m[3][0] * m.m[0][3] * m.m[2][2];

  result.m[1][2] =
      -m.m[0][0] * m.m[1][2] * m.m[3][3] + m.m[0][0] * m.m[1][3] * m.m[3][2] +
      m.m[1][0] * m.m[0][2] * m.m[3][3] - m.m[1][0] * m.m[0][3] * m.m[3][2] -
      m.m[3][0] * m.m[0][2] * m.m[1][3] + m.m[3][0] * m.m[0][3] * m.m[1][2];

  result.m[1][3] =
      m.m[0][0] * m.m[1][2] * m.m[2][3] - m.m[0][0] * m.m[1][3] * m.m[2][2] -
      m.m[1][0] * m.m[0][2] * m.m[2][3] + m.m[1][0] * m.m[0][3] * m.m[2][2] +
      m.m[2][0] * m.m[0][2] * m.m[1][3] - m.m[2][0] * m.m[0][3] * m.m[1][2];

  result.m[2][0] =
      m.m[1][0] * m.m[2][1] * m.m[3][3] - m.m[1][0] * m.m[2][3] * m.m[3][1] -
      m.m[2][0] * m.m[1][1] * m.m[3][3] + m.m[2][0] * m.m[1][3] * m.m[3][1] +
      m.m[3][0] * m.m[1][1] * m.m[2][3] - m.m[3][0] * m.m[1][3] * m.m[2][1];

  result.m[2][1] =
      -m.m[0][0] * m.m[2][1] * m.m[3][3] + m.m[0][0] * m.m[2][3] * m.m[3][1] +
      m.m[2][0] * m.m[0][1] * m.m[3][3] - m.m[2][0] * m.m[0][3] * m.m[3][1] -
      m.m[3][0] * m.m[0][1] * m.m[2][3] + m.m[3][0] * m.m[0][3] * m.m[2][1];

  result.m[2][2] =
      m.m[0][0] * m.m[1][1] * m.m[3][3] - m.m[0][0] * m.m[1][3] * m.m[3][1] -
      m.m[1][0] * m.m[0][1] * m.m[3][3] + m.m[1][0] * m.m[0][3] * m.m[3][1] +
      m.m[3][0] * m.m[0][1] * m.m[1][3] - m.m[3][0] * m.m[0][3] * m.m[1][1];

  result.m[2][3] =
      -m.m[0][0] * m.m[1][1] * m.m[2][3] + m.m[0][0] * m.m[1][3] * m.m[2][1] +
      m.m[1][0] * m.m[0][1] * m.m[2][3] - m.m[1][0] * m.m[0][3] * m.m[2][1] -
      m.m[2][0] * m.m[0][1] * m.m[1][3] + m.m[2][0] * m.m[0][3] * m.m[1][1];

  result.m[3][0] =
      -m.m[1][0] * m.m[2][1] * m.m[3][2] + m.m[1][0] * m.m[2][2] * m.m[3][1] +
      m.m[2][0] * m.m[1][1] * m.m[3][2] - m.m[2][0] * m.m[1][2] * m.m[3][1] -
      m.m[3][0] * m.m[1][1] * m.m[2][2] + m.m[3][0] * m.m[1][2] * m.m[2][1];

  result.m[3][1] =
      m.m[0][0] * m.m[2][1] * m.m[3][2] - m.m[0][0] * m.m[2][2] * m.m[3][1] -
      m.m[2][0] * m.m[0][1] * m.m[3][2] + m.m[2][0] * m.m[0][2] * m.m[3][1] +
      m.m[3][0] * m.m[0][1] * m.m[2][2] - m.m[3][0] * m.m[0][2] * m.m[2][1];

  result.m[3][2] =
      -m.m[0][0] * m.m[1][1] * m.m[3][2] + m.m[0][0] * m.m[1][2] * m.m[3][1] +
      m.m[1][0] * m.m[0][1] * m.m[3][2] - m.m[1][0] * m.m[0][2] * m.m[3][1] -
      m.m[3][0] * m.m[0][1] * m.m[1][2] + m.m[3][0] * m.m[0][2] * m.m[1][1];

  result.m[3][3] =
      m.m[0][0] * m.m[1][1] * m.m[2][2] - m.m[0][0] * m.m[1][2] * m.m[2][1] -
      m.m[1][0] * m.m[0][1] * m.m[2][2] + m.m[1][0] * m.m[0][2] * m.m[2][1] +
      m.m[2][0] * m.m[0][1] * m.m[1][2] - m.m[2][0] * m.m[0][2] * m.m[1][1];

  det = m.m[0][0] * result.m[0][0] + m.m[0][1] * result.m[1][0] +
        m.m[0][2] * result.m[2][0] + m.m[0][3] * result.m[3][0];

  if (det == 0)
    return Matrix4x4{}; // またはエラー処理

  det = 1.0f / det;

  for (i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      result.m[i][j] = result.m[i][j] * det;

  return result;
}

// 透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(const float fovY, const float aspectRatio,
                                   const float nearClip, const float farClip) {
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
                                 const float nearClip, const float farClip) {
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
                             const float minDepth, const float maxDepth) {
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
Vector3 Add(const Vector3 &v1, const Vector3 &v2) {
  Vector3 result;
  result.x = v1.x + v2.x;
  result.y = v1.y + v2.y;
  result.z = v1.z + v2.z;
  return result;
}
// 減算
Vector3 Subtract(const Vector3 &v1, const Vector3 &v2) {
  Vector3 result;
  result.x = v1.x - v2.x;
  result.y = v1.y - v2.y;
  result.z = v1.z - v2.z;
  return result;
}
// スカラー倍
Vector3 Multiply(const float scalar, const Vector3 &v2) {
  Vector3 result;
  result.x = scalar * v2.x;
  result.y = scalar * v2.y;
  result.z = scalar * v2.z;
  return result;
}
// 内積
float Dot(const Vector3 &v1, const Vector3 &v2) // ok
{
  return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}
// 長さ（ノルム）
float Length(const Vector3 &v) // ok
{
  return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

// 正規化
Vector3 Nomalize(const Vector3 &v) // ok
{
  float length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
  if (length == 0.0f) {
    return {0.0f, 0.0f, 0.0f};
  }
  return {v.x / length, v.y / length, v.z / length};
}

// 球体作成関数
void DrawSphere(const Sphere &sphere, const Matrix4x4 &viewProjectionMatrix,
                const Matrix4x4 &viewportMatrix, uint32_t color) {
  const uint32_t kSubdivision = 16; // 分割数（大きいほどなめらか）
  const float kLonEvery =
      2.0f * float(M_PI) / float(kSubdivision); // 経度の1つ分の角度
  const float kLatEvery =
      float(M_PI) / float(kSubdivision); // 緯度の1つ分の角度

  for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
    float lat = -float(M_PI) / 2.0f + kLatEvery * latIndex; // 現在の緯度
    float nextLat = lat + kLatEvery;                        // 次の緯度

    for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
      float lon = lonIndex * kLonEvery; // 現在の経度
      float nextLon = lon + kLonEvery;  // 次の経度

      // 球面上の3点を求める
      Vector3 a = {sphere.center.x + sphere.radius * cosf(lat) * cosf(lon),
                   sphere.center.y + sphere.radius * sinf(lat),
                   sphere.center.z + sphere.radius * cosf(lat) * sinf(lon)};
      Vector3 b = {sphere.center.x + sphere.radius * cosf(nextLat) * cosf(lon),
                   sphere.center.y + sphere.radius * sinf(nextLat),
                   sphere.center.z + sphere.radius * cosf(nextLat) * sinf(lon)};
      Vector3 c = {sphere.center.x + sphere.radius * cosf(lat) * cosf(nextLon),
                   sphere.center.y + sphere.radius * sinf(lat),
                   sphere.center.z + sphere.radius * cosf(lat) * sinf(nextLon)};

      // ワールド座標→スクリーン座標へ変換
      Vector3 screenA =
          Transform(Transform(a, viewProjectionMatrix), viewportMatrix);
      Vector3 screenB =
          Transform(Transform(b, viewProjectionMatrix), viewportMatrix);
      Vector3 screenC =
          Transform(Transform(c, viewProjectionMatrix), viewportMatrix);

      // abとacに線を引く
      Novice::DrawLine(int(screenA.x), int(screenA.y), int(screenB.x),
                       int(screenB.y), color);

      Novice::DrawLine(int(screenA.x), int(screenA.y), int(screenC.x),
                       int(screenC.y), color);
    }
  }
}
// Grid描画関数
void DrawGrid(const Matrix4x4 &viewProjectionMatrix,
              const Matrix4x4 &viewportMatrix) {
  const float kGridHalfwidth = 2.0f; // グリッドの半分の幅
  const uint32_t kSubdivision = 10;  // 分割数
  const float kGridEvery =
      (kGridHalfwidth * 2.0f) / float(kSubdivision); // 一つ分の長さ

  // 奥から手前への線を順々に引いていく
  for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {
    float x = -kGridHalfwidth + kGridEvery * xIndex;

    // ワールド座標系の支点と終点
    Vector3 start = {x, 0.0f, -kGridHalfwidth};
    Vector3 end = {x, 0.0f, kGridHalfwidth};

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

    Vector3 start = {-kGridHalfwidth, 0.0f, z};
    Vector3 end = {kGridHalfwidth, 0.0f, z};

    Vector3 startScreen = Transform(start, viewProjectionMatrix);
    startScreen = Transform(startScreen, viewportMatrix);

    Vector3 endScreen = Transform(end, viewProjectionMatrix);
    endScreen = Transform(endScreen, viewportMatrix);

    uint32_t color = (z == 0.0f) ? 0x000000FF : 0xAAAAAAFF;

    Novice::DrawLine(int(startScreen.x), int(startScreen.y), int(endScreen.x),
                     int(endScreen.y), color);
  }
}
// 入力されたベクトルに対してそれと垂直になるようなベクトルを返している。
Vector3 Perpendicular(const Vector3 &vector) {
  if (vector.x != 0.0f || vector.y != 0.0f) {

    return {-vector.y, vector.x, 0.0f};
  }
  return {0.0f, -vector.z, vector.y};
}

void DrawPlane(const Plane &plane, const Matrix4x4 &vieprojectionMatrix,
               const Matrix4x4 &viewportMatrix, uint32_t color) {
  // 平面上の点を求めるすでに分かっているdistanceとnormal法線を利用する
  Vector3 center = Multiply(plane.distance, plane.normal); // 1
  // 垂直方向のベクトル(例：平面上に広がる4つの点)を保持する配列
  Vector3 perpendiculars[4];
  // plane.normalに垂直なベクトルを返してNormalizeで正規化したものを0番目の配列に格納
  // なんか平面の向きが決まるらしいね
  perpendiculars[0] = Nomalize(Perpendicular(plane.normal)); // 2
  // 逆ベクトルを求めます(perpendiculars[0]の)
  // 逆ベクトルを求めることによって平面の中心から両側に広がる形を作れる
  perpendiculars[1] = {-perpendiculars[0].x, -perpendiculars[0].y,
                       -perpendiculars[0].z}; // 3
  // 平面の法線と、すでにある垂直ベクトルとの外積で、平面上のもう1つの直交方向ベクトルを求めました
  // 四角を作るため
  perpendiculars[2] = Nomalize(Cross(plane.normal, perpendiculars[0])); // 4
  /// ここまで法線を中心に縦横のベクトルを求めていた
  // 縦方向ベクトルの逆を作って平面の中心から上下左右すべての方向に広がるようにするため
  perpendiculars[3] = {-perpendiculars[2].x, -perpendiculars[2].y,
                       -perpendiculars[2].z};

  Vector3 points[4];

  for (int index = 0; index < 4; ++index) {
    // 単位ベクトル(長さ=1)を2倍に伸ばしている
    //  perpendiculars[]この配列の中どれが右とかない
    //  だけど向いている方向が違うから四角形が作れる
    Vector3 extend = Multiply(2.0f, perpendiculars[index]);
    // 中心からある方向に伸ばした位置にある一点を計算している
    Vector3 point = Add(center, extend);
    points[index] =
        Transform(Transform(point, vieprojectionMatrix), viewportMatrix);
  }
  // 線を引く
  Novice::DrawLine((int)points[3].x, (int)points[3].y, (int)points[1].x,
                   (int)points[1].y, color); // top → right
  Novice::DrawLine((int)points[1].x, (int)points[1].y, (int)points[2].x,
                   (int)points[2].y, color); // right → bottom
  Novice::DrawLine((int)points[2].x, (int)points[2].y, (int)points[0].x,
                   (int)points[0].y, color); // bottom → left
  Novice::DrawLine((int)points[0].x, (int)points[0].y, (int)points[3].x,
                   (int)points[3].y, color); // left → top
}
// 三角形描画関数
void DrawTriangle(const Triangle &triangle,
                  const Matrix4x4 &worldViewProjectionMatrix,
                  const Matrix4x4 &viewportMatrix, uint32_t color) {
  Vector3 screenVertices[3];

  for (int i = 0; i < 3; ++i) {
    // カメラ・投影（ワールド→カメラ→NDC）
    Vector3 v = Transform(triangle.vertices[i], worldViewProjectionMatrix);

    // ビューポート変換（NDC→スクリーン）
    v = Transform(v, viewportMatrix);

    screenVertices[i] = v;
  }

  // 画面上に描画（整数座標に変換）
  Novice::DrawTriangle(static_cast<int>(screenVertices[0].x),
                       static_cast<int>(screenVertices[0].y),
                       static_cast<int>(screenVertices[1].x),
                       static_cast<int>(screenVertices[1].y),
                       static_cast<int>(screenVertices[2].x),
                       static_cast<int>(screenVertices[2].y), color,
                       kFillModeWireFrame);
}

#pragma endregion

bool IsCollision(const Vector3 &origin, const Vector3 &diff,
                 const Plane &plane) {

  // 平面の法線と線の方向ベクトルの内積
  float dot = plane.normal.x * diff.x + plane.normal.y * diff.y +
              plane.normal.z * diff.z;

  // 線と平面が平行なら交差しない（内積0）
  if (dot == 0.0f) {
    return false;
  }

  // 交点の媒介変数tを求める
  float t =
      (plane.distance - (origin.x * plane.normal.x + origin.y * plane.normal.y +
                         origin.z * plane.normal.z)) /
      dot;
  return (t >= 0.0f && t <= 1.0f);
}

// 三角形と線の当たり判定
bool isCollisionTriangle(const Triangle &triangle, const Segment &segment) {
  Vector3 origin = segment.origin;
  Vector3 diff = segment.diff;

  Vector3 edge1 = Subtract(triangle.vertices[1], triangle.vertices[0]);
  Vector3 edge2 = Subtract(triangle.vertices[2], triangle.vertices[0]);
  Vector3 normal = Cross(edge1, edge2);

  float distance = Dot(normal, triangle.vertices[0]);

  // 平面の法線と線の方向ベクトルの内積
  float dot = normal.x * diff.x + normal.y * diff.y + normal.z * diff.z;
  // 線と平面が平行なら交差しない（内積0）
  if (dot == 0.0f) {
    return false;
  }

  // 交点の媒介変数tを求める
  float t = (distance - (origin.x * normal.x + origin.y * normal.y +
                         origin.z * normal.z)) /
            dot;

  // tが線分上にあるかチェック
  if (t < 0.0f || t > 1.0f) {
    return false;
  }

  // 交点pを計算
  Vector3 p;
  p.x = origin.x + diff.x * t;
  p.y = origin.y + diff.y * t;
  p.z = origin.z + diff.z * t;

  // 三角形の辺ベクトル
  Vector3 edge01 = Subtract(triangle.vertices[1], triangle.vertices[0]);
  Vector3 edge12 = Subtract(triangle.vertices[2], triangle.vertices[1]);
  Vector3 edge20 = Subtract(triangle.vertices[0], triangle.vertices[2]);

  // 交点pと各頂点を結ぶベクトル
  Vector3 c0 = Subtract(p, triangle.vertices[0]);
  Vector3 c1 = Subtract(p, triangle.vertices[1]);
  Vector3 c2 = Subtract(p, triangle.vertices[2]);

  // クロス積を計算
  Vector3 cross01 = Cross(edge01, c0);
  Vector3 cross12 = Cross(edge12, c1);
  Vector3 cross20 = Cross(edge20, c2);

  // クロス積の向きを比較（内積）
  if (Dot(cross01, normal) >= 0.0f && Dot(cross12, normal) >= 0.0f &&
      Dot(cross20, normal) >= 0.0f) {
    return true;
  }

  return false; // 衝突なし
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

  // ライブラリの初期化
  Novice::Initialize(kWindowTitle, 1280, 720);

  // キー入力結果を受け取る箱
  char keys[256] = {0};
  char preKeys[256] = {0};
  int color = WHITE;
  // 初期化,定義
  Vector3 rotate = {};
  Vector3 translate = {};
  Vector3 cameraTransLate{0.0f, 1.9f, -6.49f};
  Vector3 cameraRotate{0.26f, 0.0f, 0.0f};
  int kWindowWidth = 1280;
  int kWindowHeight = 720;
  static Plane plane = {{0.0f, 1.0f, 0.0f}, 0.0f};
  static Segment segment = {{0.0f, 1.0f, -1.0f}, {0.0f, -2.0f, 2.0f}};
  static Triangle triangle = {{
      {-0.5f, -0.5f, 2.0f}, // 左下
      {0.5f, -0.5f, 2.0f},  // 右下
      {0.0f, 0.5f, 2.0f}    // 上
  }};
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
    plane.normal = Nomalize(plane.normal);
    // スケール × 回転 × 平行移動 = カメラの世界行列（位置と向き）
    Matrix4x4 cameraMatrix =
        MakeAffineMatrix({1.0f, 1.0f, 1.0f}, cameraRotate, cameraTransLate);
    // 視点座標系に変換（カメラの逆変換）＝ビュー行列
    Matrix4x4 viewMatrix = Inverse(cameraMatrix);
    // 透視投影（遠近感を加える）
    // 透視投影行列の計算
    Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(
        float(0.45), float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
    // NDC（-1〜+1）を画面ピクセル座標に変換する
    Matrix4x4 viewportMatrix = MakeViewportMatrix(
        0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);
    Matrix4x4 WorldViewProjectionMatrix =
        Multiply(viewMatrix, projectionMatrix);

    // 線
    Matrix4x4 worldMatrix =
        MakeAffineMatrix({1.0f, 1.0f, 1.0f}, rotate, translate);
    Matrix4x4 LineViewProjectionMatrix =
        Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
    Vector3 start = Transform(
        Transform(segment.origin, LineViewProjectionMatrix), viewportMatrix);
    Vector3 end = Transform(
        Transform(Add(segment.origin, segment.diff), LineViewProjectionMatrix),
        viewportMatrix);

    // int hit = IsCollision(segment.origin, segment.diff, plane);

    int hitTriangle = isCollisionTriangle(triangle, segment);
    if (hitTriangle) {
      color = RED;

    } else {
      color = WHITE;
    }
    ImGui::Begin("Control Panel");

    // Planeの法線ベクトル（正規化された値なので単位ベクトルに注意）
    ImGui::Text("Plane");
    ImGui::DragFloat3("Plane Normal", &plane.normal.x, 0.01f, -1.0f, 1.0f);
    ImGui::DragFloat("Plane Distance", &plane.distance, 0.01f, -10.0f, 10.0f);

    // Segmentの始点と終点方向
    ImGui::Separator();
    ImGui::Text("Segment");
    ImGui::DragFloat3("Segment Origin", &segment.origin.x, 0.01f, -10.0f,
                      10.0f);
    ImGui::DragFloat3("Segment Diff", &segment.diff.x, 0.01f, -10.0f, 10.0f);
    ImGui::Separator();
    ImGui::Text("Triangle");
    ImGui::DragFloat3("Vertex 0", &triangle.vertices[0].x, 0.01f);
    ImGui::DragFloat3("Vertex 1", &triangle.vertices[1].x, 0.01f);
    ImGui::DragFloat3("Vertex 2", &triangle.vertices[2].x, 0.01f);
    ImGui::Text("Camera");
    ImGui::DragFloat3("cameraX", &cameraRotate.x, 0.01f);
    ImGui::DragFloat3("cameraTransLate", &cameraTransLate.x, 0.01f);
    ImGui::End();
    /// ↑更新処理ここまで
    ///

    ///
    /// ↓描画処理ここから
    ///
    DrawPlane(plane, WorldViewProjectionMatrix, viewportMatrix, RED);
    Novice::DrawLine(int(start.x), int(start.y), int(end.x), int(end.y), WHITE);
    DrawGrid(WorldViewProjectionMatrix, viewportMatrix);
    // 三角形
    // viewportMatirixにviewportが入っていてうまく描画できなかったので注意
    DrawTriangle(triangle, WorldViewProjectionMatrix, viewportMatrix, color);
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
