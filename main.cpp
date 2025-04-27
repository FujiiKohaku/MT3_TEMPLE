#include <Novice.h>
#include <assert.h>
#include <cmath>
#include <math.h>
#include <stdio.h>
const char kWindowTitle[] = "LE2C_25_フジイ_コハク";

struct Matrix4x4 {
  float m[4][4];
};

struct Vector3 {
  float x, y, z;
};

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
// X軸回転行列
Matrix4x4 MakeRotateXMatrix(float radian) {
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
Matrix4x4 MakeRotateYMatrix(float radian) {
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
Matrix4x4 MakeRotateZMatrix(float radian) {
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
void MatrixScreenPrintf(int x, int y, const Matrix4x4 &matirix,
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
Matrix4x4 Inverse(Matrix4x4 m) {
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

  result.m[0][0] = f / aspectRatio;
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
// 表示関数
// VectorScreenPrintf関数
static const int kColumWidth = 60;
void VectorScreenPrintf(int x, int y, const Vector3 &vector,
                        const char *label) {
  Novice::ScreenPrintf(x, y, "%.02f", vector.x);
  Novice::ScreenPrintf(x + kColumWidth, y, "%.02f", vector.y);
  Novice::ScreenPrintf(x + kColumWidth * 2, y, "%.02f", vector.z);
  Novice::ScreenPrintf(x + kColumWidth * 3, y, "%s", label);
}
/// 変数
Vector3 rotate{};
Vector3 translate{};
Vector3 cameraposition{0.0f, 0.0f, -5.0f};
int kWindowWidth = 1280;
int kWindowHeight = 720;
Vector3 kLocalVertices[3] = {
    {0.0f, 1.0f, 0.0f}, {-1.0f, -0.5f, 0.0f}, {1.0f, -0.5f, 0.0f}

};

// クロス積の確認用
Vector3 v1{1.2f, -3.9f, 2.5f};
Vector3 v2{2.8f, 0.4f, -1.3f};

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

  // ライブラリの初期化
  Novice::Initialize(kWindowTitle, 1280, 720);

  // キー入力結果を受け取る箱
  char keys[256] = {0};
  char preKeys[256] = {0};

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
    // WSキーで前後に,ADキーで左右に三角形を動かす.Y軸回転させるという処理をここに書く.translateとrotateの値を変更すればいい

    // WSキーで前後移動
    if (keys[DIK_W]) {
      translate.z += 0.1f; // 前に進む（カメラ座標系）
    }
    if (keys[DIK_S]) {
      translate.z -= 0.1f; // 後ろに下がる
    }

    // ADキーで左右移動
    if (keys[DIK_A]) {
      translate.x -= 0.1f; // 左に移動
      rotate.y -= 0.1f;   // 左に回転
    }
    if (keys[DIK_D]) {
      translate.x += 0.1f; // 右に移動
      rotate.y += 0.1f;   // 右に回転
    }

 

    // 各種行列の計算
    Matrix4x4 worldMatrix =
        MakeAffineMatrix({1.0f, 1.0f, 1.0f}, rotate, translate);
    Matrix4x4 cameraMatrix = MakeAffineMatrix(
        {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, cameraposition);
    Matrix4x4 viewMatrix = Inverse(cameraMatrix);
    Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(
        float(0.45), float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
    Matrix4x4 worldViewProjectionMatrix =
        Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
    Matrix4x4 viewportMatrix = MakeViewportMatrix(
        0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);
    Vector3 screenVertices[3];
    for (uint32_t i = 0; i < 3; i++) {
      Vector3 ndcVertex =
          Transform(kLocalVertices[i], worldViewProjectionMatrix);
      screenVertices[i] = Transform(ndcVertex, viewportMatrix);
    }

    Vector3 cross = Cross(v1, v2);
    ///
    /// ↑更新処理ここまで
    ///

    ///
    /// ↓描画処理ここから
    ///
    Novice::DrawTriangle(int(screenVertices[0].x), int(screenVertices[0].y),
                         int(screenVertices[1].x), int(screenVertices[1].y),
                         int(screenVertices[2].x), int(screenVertices[2].y),
                         RED, kFillModeSolid);
    VectorScreenPrintf(0, 0, cross, "Cross");
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
