#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete model;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();

	block.Initialize();

	camera1.Initialize();

	model = Model::Create();

	//マップ用画像
	textureHandleGround = TextureManager::Load("ground.png");
	textureHandleWall = TextureManager::Load("wall.png");

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

}

void GameScene::Update() {

	camera1.eye = { 0,100,-10 };
	camera1.target = { 0,0,10 };

	camera1.UpdateMatrix();
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

	model->Draw(block, camera1);

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
