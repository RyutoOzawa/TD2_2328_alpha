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

	//磁石の引き寄せ、引き離しの更新処理
	void MagnetsUpdate();

	//磁石同士の引き寄せ、引き離しの更新処理
	void MagToMagUpdate();

	//磁石とプレイヤーの引き寄せ、引き離しの更新処理
	void MagToPlayerUpdate();

	//磁力のON,OFF
	void MagnetPower();

	//座標などの情報を更新
	void InforUpdate();

	//mainPos のどの面にsubPosにあったっか
	int GetContact(Vector3 mainPos,Vector3 subPos);

	//pos2 - pos1　で pos1　から pos2　のベクトルを求める
	Vector3 GetVec(Vector3 pos1, Vector3 pos2);

	//ベクトルを正規化してsizeをかける関数
	Vector3 ChangeVec(Vector3 changeVec,float size);

	float lengthVec(Vector3 vec);



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

	//自機当たっているか
	Vector2 ColX = { 0,0 };
	Vector2 ColY = { 0,0 };
	Vector2 ColZ = { 0,0 };

	//くっついているブロックの番号
	bool stickBlockMass[20];

	//くっついているブロックの中でどれが1番プレイヤーに近いか
	int mostNearPlayerBlock = 0;
	bool mostNear;

	//周りの4面のみ
	int mostNearPlayerBlockFour = 0;
	bool mostNearFour;

	int a;

	//ブロックの最大数
	static const int blockSizeMax = 20;

	//nブロック 座標

	Vector3 bPos[blockSizeMax];

	float bPosX1[blockSizeMax] = {};
	float bPosX2[blockSizeMax] = {};

	float bPosZ1[blockSizeMax] = {};
	float bPosZ2[blockSizeMax] = {};

	//ブロックの当たり判定の大きさ
	float bSize = 1.98; // (2 * 0.99)


	Vector3 setPos[blockSizeMax] = {};

	Vector3 bMoveVec;


	//--------自機----------

	Vector3 pPos;
	float pSize;
	int pState;

	float pPosX1;
	float pPosX2;

	float pPosZ1;
	float pPosZ2;

	Vector3 pMoveVec;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
};
