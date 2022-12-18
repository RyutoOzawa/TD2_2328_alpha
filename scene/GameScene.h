#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "DebugText.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include"Player.h"

#include "Map.h"
#include "Matrix.h"
#include"MagnetBlock.h"
#include<vector>

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

  public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	//当たり判定
	//マップ参照
	void MapCollision();

	//座標参照
	void PosCollision();

	//磁石同士の引き寄せ、引き離しの更新処理
	void MagnetsUpdate();

	//mainPos のどの面がsubPosにあったっか
	int GetContact(Vector3 mainPos,Vector3 subPos);

  private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	DebugText* debugText_ = nullptr;

	ViewProjection camera1;

	Player* player = nullptr;

	//マップ

	Model* model = nullptr;

	Map* map_ = new Map;
	Map* savemap_ = new Map;

	//マップの座標
	WorldTransform worldTransform_[10][10][10];
	Map* map[blockY][blockZ][blockX] = {};

	//マップ用画像

	uint32_t textureHandleGround = 0;
	uint32_t textureHandleWall = 0;

	//N極、S極のブロック関係
	uint32_t northPoleTexture = 0;
	uint32_t southPoleTexture = 0;

	std::vector<MagnetBlock> magnetBlocks;
	std::vector<MagnetData> magnetDatas;

	Vector3 vecMagToMag;

	//当たっているか
	Vector2 ColX = { 0,0 };
	Vector2 ColY = { 0,0 };
	Vector2 ColZ = { 0,0 };

	/// <summary>
	/// ゲームシーン用
	/// </summary>
};
