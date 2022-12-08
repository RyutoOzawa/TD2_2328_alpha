#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene() {
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();

	camera1.Initialize();
	camera1.eye = Vector3(0, 50, -1);
	camera1.target = Vector3(0, 0, 0);
	//camera1.up = Vector3(0, -1, 0);
	camera1.UpdateMatrix();

	player = new Player();
	player->Initialize();

	model = Model::Create();

	//マップ用画像
	textureHandleGround = TextureManager::Load("ground.png");
	textureHandleWall = TextureManager::Load("wall.png");

	northPoleTexture = TextureManager::Load("red1x1.png");
	southPoleTexture = TextureManager::Load("blue1x1.png");

	map_->Loding("map/map1.csv");
	savemap_->Loding("map/map1.csv");

	//マップの座標の初期化
	for (int i = 0; i < blockY; i++)
	{
		for (int j = 0; j < blockZ; j++)
		{
			for (int k = 0; k < blockX; k++)
			{
				worldTransform_[i][j][k].Initialize();
				worldTransform_[i][j][k].translation_.x = k * blockSize * blockScale;
				worldTransform_[i][j][k].translation_.y = i * blockSize * blockScale;
				worldTransform_[i][j][k].translation_.z = j * blockSize * blockScale;
				worldTransform_[i][j][k].scale_ = { blockScale,blockScale,blockScale };
				worldTransformUpdate(&worldTransform_[i][j][k]);
			}
		}
	}


	Vector3 nBlockPos{ 5,2,5 };
	Vector3 sBlockPos{ 5,2,15 };

	nPoleBlock.Initialize(nBlockPos,true);
	sPoleBlock.Initialize(sBlockPos,false);

}

void GameScene::Update() {
	player->Update();

	nPoleBlock.Update();
	sPoleBlock.Update();

	MapCollision();
	PosCollision();
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	player->Draw(camera1);

	//磁石描画
	nPoleBlock.Draw(camera1, northPoleTexture, southPoleTexture);
	sPoleBlock.Draw(camera1, northPoleTexture, southPoleTexture);

	//マップの描画
	for (int i = 0; i < blockY; i++)
	{
		for (int j = 0; j < blockZ; j++)
		{
			for (int k = 0; k < blockX; k++)
			{
				if (savemap_->map[i][j][k] == 1)
				{
					if (i == 1) {
						model->Draw(worldTransform_[i][j][k], camera1, textureHandleWall);
					}
					else {
						model->Draw(worldTransform_[i][j][k], camera1, textureHandleGround);
					}
				}
			}
		}
	}


	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

//判定
void GameScene::MapCollision()
{
	//座標を用意
	float leftplayer = player->GetPosition().x;
	float downplayer = player->GetPosition().y;
	float frontplayer = player->GetPosition().z;
	float rightplayer = player->GetPosition().x + player->GetSize();
	float upplayer = player->GetPosition().y - player->GetSize();
	float backplayer = player->GetPosition().z + player->GetSize();

	//当たっているか
	Vector2 ColX = { 0,0 };
	Vector2 ColY = { 0,0 };
	Vector2 ColZ = { 0,0 };

	float playerSpeed = player->GetSpeed() + 0.01;

	/////////////
	//プレイヤー//
	/////////////

	//右に仮想的に移動して当たったら
	if (savemap_->mapcol(rightplayer + playerSpeed, downplayer + player->GetSize() / 2, frontplayer) || savemap_->mapcol(rightplayer + playerSpeed, downplayer + player->GetSize() / 2, backplayer))
	{

		if (player->GetMove().x > 0) {
			//１ピクセル先に壁が来るまで移動
			while ((savemap_->mapcol(rightplayer, downplayer + player->GetSize() / 2, frontplayer) || savemap_->mapcol(rightplayer, downplayer + player->GetSize() / 2, backplayer)))
			{
				player->OnMapCollisionX2();
				rightplayer = player->GetPosition().x + player->GetSize();
				leftplayer = player->GetPosition().x;
			}
			ColX.x = 1;
		}
		else {
			ColX.x = 1;
		}

	}


	//左に仮想的に移動して当たったら
	if (savemap_->mapcol(leftplayer - playerSpeed, downplayer + player->GetSize() / 2, frontplayer) || savemap_->mapcol(leftplayer - playerSpeed, downplayer + player->GetSize() / 2, backplayer))
	{
		if (player->GetMove().x < 0) {
			//１ピクセル先に壁が来るまで移動
			while ((savemap_->mapcol(leftplayer, downplayer + player->GetSize() / 2, frontplayer) || savemap_->mapcol(leftplayer, downplayer + player->GetSize() / 2, backplayer)))
			{
				player->OnMapCollisionX();
				rightplayer = player->GetPosition().x + player->GetSize();
				leftplayer = player->GetPosition().x;
			}
			ColX.y = 1;
		}
		else {
			ColX.y = 1;
		}
	}


	//debugText_->Printf("%f", ColX.y);


	leftplayer = player->GetPosition().x;
	rightplayer = player->GetPosition().x + player->GetSize();

	////下(床)に仮想的に移動して当たったら
	//if (savemap_->mapcol(leftplayer, downplayer, frontplayer + playerSpeed))
	//{
	//	//１ピクセル先に壁が来るまで移動
	//	while ((savemap_->mapcol(leftplayer, downplayer, frontplayer + playerSpeed)))
	//	{
	//		player->OnMapCollisionY();
	//		upplayer = player->GetPosition().y - player->GetSize();
	//		downplayer = player->GetPosition().y;
	//	}
	//}


	upplayer = player->GetPosition().y - player->GetSize();
	downplayer = player->GetPosition().y;


	//z軸に対しての当たり判定
	//奥に仮想的に移動して当たったら
	if (savemap_->mapcol(leftplayer, downplayer + player->GetSize() / 2, backplayer + playerSpeed) || savemap_->mapcol(rightplayer, downplayer + player->GetSize() / 2, backplayer + playerSpeed))
	{
		if (player->GetMove().z > 0) {
			//１ピクセル先に壁が来るまで移動
			while ((savemap_->mapcol(leftplayer, downplayer + player->GetSize() / 2, backplayer) || savemap_->mapcol(rightplayer, downplayer + player->GetSize() / 2, backplayer)))
			{
				player->OnMapCollisionZ2();
				frontplayer = player->GetPosition().z;
				backplayer = player->GetPosition().z + player->GetSize();
			}
			ColZ.x = 1;
		}
		else {
			ColZ.x = 1;
		}

	}



	//手前に仮想的に移動して当たったら
	if (savemap_->mapcol(leftplayer, downplayer + player->GetSize() / 2, frontplayer - playerSpeed) || savemap_->mapcol(rightplayer, downplayer + player->GetSize() / 2, frontplayer - playerSpeed))
	{
		if (player->GetMove().z < 0) {
			//１ピクセル先に壁が来るまで移動
			while ((savemap_->mapcol(leftplayer, downplayer + player->GetSize() / 2, frontplayer) || savemap_->mapcol(rightplayer, downplayer + player->GetSize() / 2, frontplayer)))
			{
				player->OnMapCollisionZ();
				frontplayer = player->GetPosition().z;
				backplayer = player->GetPosition().z + player->GetSize();
			}
			ColZ.y = 1;
		}
		else {
			ColZ.y = 1;
		}
	}


	player->SetColX(ColX);
	player->SetColY(ColY);
	player->SetColZ(ColZ);

}

void GameScene::PosCollision()
{

	//nブロック

	Vector3 nPos = nPoleBlock.GetPos();
	float nSize = 2;

	float nPosX1 = nPos.x - (nSize / 2);
	float nPosX2 = nPos.x + (nSize / 2);

	float nPosZ1 = nPos.z - (nSize / 2);
	float nPosZ2 = nPos.z + (nSize / 2);

	//sブロック

	Vector3 sPos = sPoleBlock.GetPos();
	float sSize = 2;

	float sPosX1 = sPos.x - (sSize / 2);
	float sPosX2 = sPos.x + (sSize / 2);

	float sPosZ1 = sPos.z - (sSize / 2);
	float sPosZ2 = sPos.z + (sSize / 2);

	//自機

	Vector3 pPos = player->GetPosition();
	float pSize = player->GetSize();

	float pPosX1 = pPos.x - (pSize / 2);
	float pPosX2 = pPos.x + (pSize / 2);

	float pPosZ1 = pPos.z - (pSize / 2);
	float pPosZ2 = pPos.z + (pSize / 2);

	//向き
	int pInput;

	//自機とSブロック

	if (pPosX1 < sPosX2 && sPosX1 < pPosX2) {

		if (pPosZ1 < sPosZ2 && sPosZ1 < pPosZ2) {

			debugText_->Printf("PS");

		}

	}

	//自機とNブロック

	if (pPosX1 < nPosX2 && nPosX1 < pPosX2) {

		if (pPosZ1 < nPosZ2 && nPosZ1 < pPosZ2) {

			debugText_->Printf("PN");

		}

	}

	//SブロックとNブロック

	if (sPosX1 < nPosX2 && nPosX1 < sPosX2) {

		if (sPosZ1 < nPosZ2 && nPosZ1 < sPosZ2) {

			debugText_->Printf("NS");

		}

	}

}

