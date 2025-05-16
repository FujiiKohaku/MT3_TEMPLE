



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
const char kWindowTitle[] = "LC2C_フジイ_コハク";
// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

  // ライブラリの初期化
  Novice::Initialize(kWindowTitle, 1280, 720);

  // キー入力結果を受け取る箱
  char keys[256] = {0};
  char preKeys[256] = {0};

  Plane groundPlane;
  groundPlane.nomal = {0.0f, 1.0f, 0.0f};
  groundPlane.distance = 0.0f;
  Vector3 rotate{};
  Vector3 translatePlane{};
  Vector3 cameraTransLate{0.0f, 1.9f, -6.49f};
  Vector3 cameraRotate{0.26f, 0.0f, 0.0f};
  int kWindowWidth = 1280;
  int kWindowHeight = 720;
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
  // GRID計算
  //
  Matrix4x4 viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
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
    // 球1個目計算
    Matrix4x4 planeWorldMatrix =
        MakeAffineMatrix({1.0f, 1.0f, 1.0f}, rotate, translatePlane);
    //
    Matrix4x4 planeWorldViewProjectionMatrix =
        Multiply(planeWorldMatrix, Multiply(viewMatrix, projectionMatrix));
    // 球2個目計算
    ///
    /// ↑更新処理ここまで
    ///

    ///
    /// ↓描画処理ここから
    ///
    DrawGrid(gridWorldViewProjectionMatrix, viewportMatrix);

    DrawPlane(groundPlane, viewProjectionMatrix, viewportMatrix, WHITE);

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