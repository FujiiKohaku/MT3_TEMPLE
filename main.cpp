#include <Novice.h>
#include <math.h>
#include <stdio.h>
const char kWindowTitle[] = "LC1C_18_フジイ_コハク";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

  // ライブラリの初期化
  Novice::Initialize(kWindowTitle, 1280, 720);

  // キー入力結果を受け取る箱
  char keys[256] = {0};
  char preKeys[256] = {0};

  //==定義==//
  // プレイヤーの位置
  int posX;
  int posY;
  // プレイヤー縦横幅
  int width = 64;
  int height = 64;
  // プレイヤーの速度
  int speed;
  //==初期化==//
  posX = 640; // ウィンドウの中心に配置
  posY = 360; // ウィンドウの中心に配置
  speed = 5;  // プレイヤーの移動速度
  //==初期化ここまで==//

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

    // プレイヤーの移動処理
    posX += speed;
    // 右壁に衝突したときの処理
    if (posX >= 1280 - width) {
      speed *= -1; // 速度を反転
    }
    // 左壁に衝突したときの処理
    if (posX <= 0) {

      speed *= -1; // 速度を反転
    }

    ///
    /// ↑更新処理ここまで
    ///

    ///
    /// ↓描画処理ここから
    ///
    Novice::DrawBox(posX, posY, width, height, 0.0f, WHITE, kFillModeSolid);
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
