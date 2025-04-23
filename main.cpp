#include <Novice.h>
#include <assert.h>
#include <cmath>
const char kWindowTitle[] = "LE2C_25_フジイ_コハク";

struct Matrix4x4 {
  float m[4][4];
};
struct Vector3 {
  float x, y, z;
};

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
  assert(w != 1.0f);
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
int kRowHeight = 20;
int kColumnWidth = 60;
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

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
  Vector3 scale{1.2f, 0.79f, -2.1f};
  Vector3 rotate{0.4f, 1.43f, 0.8f};
  Vector3 translate{2.7f, -4.15f, 1.57f};

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
    Matrix4x4 worldMatrix = MakeAffineMatrix(scale, rotate, translate);
    ///
    /// ↑更新処理ここまで
    ///
    MatrixScreenPrintf(0, 0, worldMatrix, "worldMatrix");
    ///
    /// ↓描画処理ここから
    ///

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
