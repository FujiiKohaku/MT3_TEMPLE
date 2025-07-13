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
  float posX;
  float posY;
  // プレイヤー縦横幅
  int radius = 32; // 楕円の半径
  // プレイヤーの速度
  float speed;
  float length;
  // 方向ベクトル
  float moveX;    // プレイヤーの移動方向X
  float moveY;    // プレイヤーの移動方向Y
  float newMoveX; // プレイヤーの新しい移動方向X
  float newMoveY; // プレイヤーの新しい移動方向Y
  //==初期化==//
  posX = 640;   // ウィンドウの中心に配置
  posY = 360;   // ウィンドウの中心に配置
  speed = 5;    // プレイヤーの移動速度
  length = 0;   // プレイヤーの移動距離
  moveX = 0;    // プレイヤーの移動方向X
  moveY = 0;    // プレイヤーの移動方向Y
  newMoveX = 0; // プレイヤーの新しい移動方向X
  newMoveY = 0; // プレイヤーの新しい移動方向Y

  // 弾
  float bulletPosX = 0;
  float BulletPosY = 0;
  float bulletSpeed = 8;
  int bulletRadius = 16;
  int isBulletShot = false;

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
    moveX = 0.0f;    // 長さが0の場合は移動しない
    moveY = 0.0f;    // 長さが0の場合は移動しない
    newMoveX = 0.0f; // 長さが0の場合は移動しない
    newMoveY = 0.0f; // 長さが0の場合は移動しない
    // プレイヤーの移動処理
    if (keys[DIK_W]) {
      moveY = -1.0f; // 上に移動
    }
    if (keys[DIK_A]) {
      moveX = -1.0f; // 左に移動
    }
    if (keys[DIK_S]) {
      moveY = 1.0f; // 下に移動
    }
    if (keys[DIK_D]) {
      moveX = 1.0f; // 右に移動
    }
    //==スぺスキー押したら弾を発射==//
    if (!isBulletShot) {

      if (keys[DIK_SPACE] && !preKeys[DIK_SPACE]) {
        bulletPosX = posX; // プレイヤーの位置を弾の初期位置に設定
        BulletPosY = posY; // プレイヤーの位置を弾の初期位置に設定

        isBulletShot = true;
      }
      //フラグがONになったら弾の移動処理
    }
    if (isBulletShot) {
      BulletPosY -= bulletSpeed;
    }

    if (BulletPosY <= 0-bulletRadius) {
      isBulletShot = false; // 弾が画面外に出たら発射状態を解除
    }
    // 移動ベクトルの長さ（距離）
    length = sqrtf(moveX * moveX + moveY * moveY);

    if (length != 0.0f) {
      newMoveX = moveX / length; // 正規化
      newMoveY = moveY / length; // 正規化
    }

    posX += newMoveX * speed; // 新しい位置に移動
    posY += newMoveY * speed; // 新しい位置に移動

    ///
    /// ↑更新処理ここまで
    ///

    ///
    /// ↓描画処理ここから
    ///
    Novice::DrawEllipse(static_cast<int>(posX), static_cast<int>(posY), radius,
                        radius, 0.0f, WHITE, kFillModeSolid);

    Novice::DrawEllipse(static_cast<int>(bulletPosX),
                        static_cast<int>(BulletPosY), bulletRadius,
                        bulletRadius, 0.0f, RED, kFillModeSolid);
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
