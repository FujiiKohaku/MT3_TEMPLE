
#define _USE_MATH_DEFINES
#include "KamataEngine.h"
#include <Novice.h>
#include <assert.h>
#include <cmath>
#include <imgui.h>
#include <math.h>
#include <stdio.h>
const char kWindowTitle[] = "LE2C_25_フジイ_コハク";

#pragma region 構造体定期

struct Matrix4x4 {
  float m[4][4];
};

struct Vector3 {
  float x, y, z;
};

struct Sphere {
  Vector3 center;
  float radius;
};

struct Line {
  Vector3 origin; // 始点
  Vector3 diff;   // 終点への差分ベクトル
};

struct Ray {
  Vector3 origin; // 始点
  Vector3 diff;   // 終点への差分ベクトル
};

struct Segment {
  Vector3 origin; // 始点
  Vector3 diff;   // 終点への差分ベクトル
};
struct Plane {
  Vector3 nomal;  // 法線
  float distance; // 距離
};

#pragma endregion

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
Vector3 Transform(const Vector3 &vector, const Matrix4x4 &matrix) {
  Vector3 result;
  result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] +
             vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];

  result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] +
             vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];

  result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] +
             vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];

  float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] +
            vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];
  assert(w != 0.0f);
  result.x /= w;
  result.y /= w;
  result.z /= w;

  return result;
}
// X軸回転行列（左手座標系）
Matrix4x4 MakeRotateXMatrix(float radian) {
  Matrix4x4 result = {};
  result.m[0][0] = 1.0f;
  result.m[1][1] = cosf(radian);
  result.m[1][2] = sinf(radian);  // ← ここ符号変更
  result.m[2][1] = -sinf(radian); // ← ここ符号変更
  result.m[2][2] = cosf(radian);
  result.m[3][3] = 1.0f;
  return result;
}

// Y軸回転行列（左手座標系）
Matrix4x4 MakeRotateYMatrix(float radian) {
  Matrix4x4 result = {};
  result.m[0][0] = cosf(radian);
  result.m[0][2] = -sinf(radian); // ← ここ符号変更
  result.m[1][1] = 1.0f;
  result.m[2][0] = sinf(radian); // ← ここ符号変更
  result.m[2][2] = cosf(radian);
  result.m[3][3] = 1.0f;
  return result;
}

// Z軸回転行列（左右手共通）
Matrix4x4 MakeRotateZMatrix(float radian) {
  Matrix4x4 result = {};
  result.m[0][0] = cosf(radian);
  result.m[0][1] = sinf(radian);  // 符号が逆
  result.m[1][0] = -sinf(radian); // 符号が逆
  result.m[1][1] = cosf(radian);
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
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio,
                                   float nearClip, float farClip) {
  Matrix4x4 result = {};

  float f = 1.0f / std::tan(fovY / 2.0f);

  result.m[0][0] = f / aspectRatio; // ここ疑問
  result.m[1][1] = f;
  result.m[2][2] = farClip / (farClip - nearClip);
  result.m[2][3] = 1.0f;
  result.m[3][2] = (-nearClip * farClip) / (farClip - nearClip);

  return result;
}
// 正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right,
                                 float bottom, float nearClip, float farClip) {
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
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height,
                             float minDepth, float maxDepth) {
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
float Dot(const Vector3 &v1, const Vector3 &v2) {
  return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}
// 長さ（ノルム）
float Length(const Vector3 &v) {
  return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

// 正規化
Vector3 Nomalize(const Vector3 &v) {
  float length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
  if (length == 0.0f) {
    return {0.0f, 0.0f, 0.0f};
  }
  return {v.x / length, v.y / length, v.z / length};
}

// 球体作成関数
void DrawSphere(const Sphere &sphere, const Matrix4x4 &viewProjectionMatrix,
                const Matrix4x4 &viewportMatrix, uint32_t color) {

  const uint32_t kSubdivision = 16; // 分割数
  const float kLonEvery =
      2.0f * float(M_PI) / kSubdivision; // 経度ステップ（0～2π）
  const float kLatEvery =
      float(M_PI) / kSubdivision; // 緯度ステップ（-π/2～π/2）

  // 緯度方向にループ -π/2 ～ +π/2
  for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
    float lat = -float(M_PI) / 2.0f + kLatEvery * latIndex;

    // 経度方向にループ 0 ～ 2π
    for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
      float lon = lonIndex * kLonEvery;

      // 球面上の3点（a, b, c）を求める
      Vector3 a = {sphere.center.x + sphere.radius * cosf(lat) * cosf(lon),
                   sphere.center.y + sphere.radius * sinf(lat),
                   sphere.center.z + sphere.radius * cosf(lat) * sinf(lon)};

      Vector3 b = {
          sphere.center.x + sphere.radius * cosf(lat + kLatEvery) * cosf(lon),
          sphere.center.y + sphere.radius * sinf(lat + kLatEvery),
          sphere.center.z + sphere.radius * cosf(lat + kLatEvery) * sinf(lon)};

      Vector3 c = {
          sphere.center.x + sphere.radius * cosf(lat) * cosf(lon + kLonEvery),
          sphere.center.y + sphere.radius * sinf(lat),
          sphere.center.z + sphere.radius * cosf(lat) * sinf(lon + kLonEvery)};

      // スクリーン座標に変換
      Vector3 screenA =
          Transform(Transform(a, viewProjectionMatrix), viewportMatrix);
      Vector3 screenB =
          Transform(Transform(b, viewProjectionMatrix), viewportMatrix);
      Vector3 screenC =
          Transform(Transform(c, viewProjectionMatrix), viewportMatrix);

      // ab, ac に線を引く
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
// 垂直ベクトルを取得
Vector3 Perpendicular(const Vector3 &vector) {
  if (vector.x != 0.0f || vector.y != 0.0) {
    return {-vector.y, vector.x, 0.0f};
  }
  return {0.0f, -vector.z, vector.y};
}

void DrawPlane(const Plane &plane, const Matrix4x4 &viewProjectionMatrix,
               const Matrix4x4 &viewportMatrix, uint32_t color) {
  // 平面上の中心点
  Vector3 center = Multiply(plane.distance, plane.nomal); // 1

  // 平面上に広がる2軸を作る
  Vector3 perpendiculars[4];
  perpendiculars[0] = Nomalize(Perpendicular(plane.nomal)); // X軸方向
  perpendiculars[1] = {-perpendiculars[0].x, -perpendiculars[0].y,
                       -perpendiculars[0].z}; // -X方向
  perpendiculars[2] =
      Cross(plane.nomal, perpendiculars[0]); // Y軸方向（法線と直交）
  perpendiculars[3] = {-perpendiculars[2].x, -perpendiculars[2].y,
                       -perpendiculars[2].z}; // -Y方向

  // 平面の4点を定義
  Vector3 points[4];
  for (int32_t index = 0; index < 4; ++index) {
    Vector3 extend =
        Multiply(2.0f, perpendiculars[index]); // 大きさ調整（広がり）
    Vector3 point = Add(center, extend);       // 中心 + オフセット

    // 各頂点をスクリーン座標に変換
    points[index] = Transform(Transform(point, viewProjectionMatrix),
                              viewportMatrix); // w除算はTransform内で1回だけ
  }

  // 外枠線を描画
  Novice::DrawLine((int)points[0].x, (int)points[0].y, (int)points[2].x,
                   (int)points[2].y, color); // 0-2
  Novice::DrawLine((int)points[2].x, (int)points[2].y, (int)points[1].x,
                   (int)points[1].y, color); // 2-1
  Novice::DrawLine((int)points[1].x, (int)points[1].y, (int)points[3].x,
                   (int)points[3].y, color); // 1-3
  Novice::DrawLine((int)points[3].x, (int)points[3].y, (int)points[0].x,
                   (int)points[0].y, color); // 3-0

  // 塗りつぶし（三角形2枚で四角形を描画）
  Novice::DrawTriangle((int)points[0].x, (int)points[0].y, (int)points[2].x,
                       (int)points[2].y, (int)points[1].x, (int)points[1].y,
                       color, kFillModeWireFrame);

  Novice::DrawTriangle((int)points[1].x, (int)points[1].y, (int)points[3].x,
                       (int)points[3].y, (int)points[0].x, (int)points[0].y,
                       color, kFillModeWireFrame);
}

// 平面と球体の当たり判定の関数
bool isCollision(const Sphere &sphere, const Plane &plane) {
  // 球の中心と平面の法線の内積を取り、平面までの距離を求める
  float distance = Dot(sphere.center, plane.nomal) - plane.distance;

  // 距離の絶対値が半径以下であれば、球は平面と衝突していると判定する
  return std::fabs(distance) <= sphere.radius;
}

#pragma endregion

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

  // ライブラリの初期化
  Novice::Initialize(kWindowTitle, 1280, 720);

  // キー入力結果を受け取る箱
  char keys[256] = {0};
  char preKeys[256] = {0};

#pragma region 初期化
  Plane groundPlane;
  groundPlane.nomal = {0.0f, 1.0f, 0.0f};
  groundPlane.distance = 0.0f;
  Sphere sphere; // ok
  sphere.center = {0.0f, 0.0f, 0.0f};
  sphere.radius = 2;
  Vector3 rotate{};                            // ok
  Vector3 translatePlane{};                    // ok
  Vector3 translateSphere{};                   // ok
  Vector3 cameraTransLate{0.0f, 1.9f, -6.49f}; // ok
  Vector3 cameraRotate{0.26f, 0.0f, 0.0f};     // ok
  int kWindowWidth = 1280;
  int kWindowHeight = 720;

  bool isHit = false;

  // GRID計算
  //

#pragma endregion
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

    // 万国共通カメラマトリックス
    Matrix4x4 cameraMatrix =
        MakeAffineMatrix({1.0f, 1.0f, 1.0f}, cameraRotate, cameraTransLate);
    // カメラを逆さにviewMatrix
    Matrix4x4 viewMatrix = Inverse(cameraMatrix);
    // portを追加
    Matrix4x4 viewportMatrix = MakeViewportMatrix(
        0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);
    // 透視投影行列の計算
    Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(
        float(0.45), float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
    // GRID計算
    // Grid用のWorldViewProjection行列（ワールド行列は単位行列）
    Matrix4x4 gridWorldMatrix = MakeIdentity4x4(); // 単位行列（何も変換しない）
    Matrix4x4 gridWorldViewProjectionMatrix =
        Multiply(gridWorldMatrix, Multiply(viewMatrix, projectionMatrix));
    // 矩形
    Matrix4x4 planeWorldMatrix =
        MakeAffineMatrix({1.0f, 1.0f, 1.0f}, rotate, translatePlane);
    //
    Matrix4x4 planeWorldViewProjectionMatrix =
        Multiply(planeWorldMatrix, Multiply(viewMatrix, projectionMatrix));

    // 球体
    Matrix4x4 sphereWorldMatrix =
        MakeAffineMatrix({1.0f, 1.0f, 1.0f}, rotate, translateSphere);

    Matrix4x4 sphereWorldViewProjectionMatrix =
        Multiply(sphereWorldMatrix, Multiply(viewMatrix, projectionMatrix));

    if (isCollision(sphere, groundPlane)) {
      isHit = true;
    } else {
      isHit = false;
    }

    if (keys[DIK_W]) {
      translateSphere.y++;
    }
    if (keys[DIK_S]) {
      translateSphere.y--;
    }
    if (keys[DIK_A]) {
      translatePlane.z += 0.01f;
    }
    if (keys[DIK_D]) {
      translatePlane.z -= 0.01f;
    }

    // デバックのやつ
    ImGui::Begin("window");
    ImGui::DragFloat3("CameraTranslate", &cameraTransLate.x, 0.01f);
    ImGui::DragFloat3("CameraRotate", &cameraRotate.x, 0.01f);
    ImGui::DragFloat3("SpherCenter", &sphere.center.x, 0.01f);
    ImGui::DragFloat3("SpherTranslate", &translateSphere.x, 0.01f);
    ImGui::DragFloat("SphereRadius", &sphere.radius, 0.01f);
    ImGui::End();
    // 球2個目計算
    ///
    /// ↑更新処理ここまで
    ///

    ///
    /// ↓描画処理ここから
    ///
    // グリッド描画
    DrawGrid(gridWorldViewProjectionMatrix, viewportMatrix);
    // 矩形描画
    DrawPlane(groundPlane, planeWorldViewProjectionMatrix, viewportMatrix,
              WHITE);
    if (isHit) {

      DrawSphere(sphere, sphereWorldViewProjectionMatrix, viewportMatrix,
                 WHITE);
    }
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