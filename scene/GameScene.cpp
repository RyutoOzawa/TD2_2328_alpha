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
	camera1.eye = Vector3(8, 25, 9);
	camera1.target = Vector3(8, 0, 10);
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


	Vector3 nBlockPos{ 8,2,5 };
	Vector3 sBlockPos{ 10,2,5 };

	nPoleBlock.Initialize(nBlockPos, true);
	sPoleBlock.Initialize(sBlockPos, false);

	//ゲームで使うようの配列に格納
	magnetBlocks.push_back(nPoleBlock);
	magnetBlocks.push_back(sPoleBlock);
	MagnetBlock newBlock;
	newBlock.Initialize(Vector3(6, 2, 9), true);
	magnetBlocks.push_back(newBlock);


}

void GameScene::Update() {
	player->Update();

	camera1.eye = player->GetPosition();
	camera1.eye.y += 20.0f;
	camera1.eye.z -= 0.01;
	camera1.target = player->GetPosition();
	camera1.UpdateMatrix();

	nPoleBlock.Update(player->GetPosition(), player->GetState(), 4.0f);
	sPoleBlock.Update(player->GetPosition(), player->GetState(), 4.0f);

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
	for (int i = 0; i < magnetBlocks.size(); i++) {
		magnetBlocks[i].Draw(camera1, northPoleTexture, southPoleTexture);
	}

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

	//当たらないよう調整する用
	float adjustPixcelSpeed = player->GetAdjustPixcelSpeed();
	//自機サイズ調整用
	float adjustPlayerSize = 0.03;

	//座標を用意
	float leftplayer = player->GetPosition().x + adjustPlayerSize;
	float downplayer = player->GetPosition().y + adjustPlayerSize;
	float frontplayer = player->GetPosition().z + adjustPlayerSize;

	float rightplayer = player->GetPosition().x + player->GetSize() - adjustPlayerSize;
	float upplayer = player->GetPosition().y - player->GetSize() - adjustPlayerSize;
	float backplayer = player->GetPosition().z + player->GetSize() - adjustPlayerSize;

	//当たっているか
	//Vector2 ColX = { 0,0 };
	//Vector2 ColY = { 0,0 };
	//Vector2 ColZ = { 0,0 };

	float playerSpeed = player->GetSpeed() + adjustPixcelSpeed;

	/////////////
	//プレイヤー//
	/////////////

	//右に仮想的に移動して当たったら
	if (savemap_->mapcol(rightplayer + playerSpeed, downplayer + player->GetSize() / 2, frontplayer) || savemap_->mapcol(rightplayer + playerSpeed, downplayer + player->GetSize() / 2, backplayer))
	{

		if (player->GetMove().x > 0 && ColX.x == 0) {
			//１ピクセル先に壁が来るまで移動
			while (true)
			{
				if ((savemap_->mapcol(rightplayer + adjustPixcelSpeed, downplayer + player->GetSize() / 2, frontplayer) || savemap_->mapcol(rightplayer + adjustPixcelSpeed, downplayer + player->GetSize() / 2, backplayer))) {
					break;
				}

				player->OnMapCollisionX2();
				rightplayer = player->GetPosition().x + player->GetSize() - adjustPlayerSize;
				leftplayer = player->GetPosition().x + adjustPlayerSize;

			}

			ColX.x = 1;
		}

	}
	else {
		ColX.x = 0;
	}

	//debugText_->SetPos(0,0);
	//debugText_->Printf("RIGHT = %f",ColX.x);

	//左に仮想的に移動して当たったら
	if (savemap_->mapcol(leftplayer - playerSpeed, downplayer + player->GetSize() / 2, frontplayer) || savemap_->mapcol(leftplayer - playerSpeed, downplayer + player->GetSize() / 2, backplayer))
	{
		if (player->GetMove().x < 0 && ColX.y == 0) {
			//１ピクセル先に壁が来るまで移動
			while (true)
			{
				if ((savemap_->mapcol(leftplayer - adjustPixcelSpeed, downplayer + player->GetSize() / 2, frontplayer) || savemap_->mapcol(leftplayer - adjustPixcelSpeed, downplayer + player->GetSize() / 2, backplayer))) {
					break;
				}

				player->OnMapCollisionX();
				rightplayer = player->GetPosition().x + player->GetSize() - adjustPlayerSize;
				leftplayer = player->GetPosition().x + adjustPlayerSize;
			}

			ColX.y = 1;

		}

	}
	else {
		ColX.y = 0;
	}


	//debugText_->SetPos(0, 20);
	//debugText_->Printf("LEFT  = %f", ColX.y);

	//leftplayer = player->GetPosition().x + adjust;
	//rightplayer = player->GetPosition().x + player->GetSize() - adjust;

	//upplayer = player->GetPosition().y - player->GetSize() - adjust;
	//downplayer = player->GetPosition().y + adjust;


	//z軸に対しての当たり判定
	//奥に仮想的に移動して当たったら
	if (savemap_->mapcol(leftplayer, downplayer + player->GetSize() / 2, backplayer + playerSpeed) || savemap_->mapcol(rightplayer, downplayer + player->GetSize() / 2, backplayer + playerSpeed))
	{
		if (player->GetMove().z > 0 && ColZ.x == 0) {
			//１ピクセル先に壁が来るまで移動
			while (true)
			{
				if ((savemap_->mapcol(leftplayer, downplayer + player->GetSize() / 2, backplayer + adjustPixcelSpeed) || savemap_->mapcol(rightplayer, downplayer + player->GetSize() / 2, backplayer + adjustPixcelSpeed))) {
					break;
				}

				player->OnMapCollisionZ2();
				frontplayer = player->GetPosition().z + adjustPlayerSize;
				backplayer = player->GetPosition().z + player->GetSize() - adjustPlayerSize;
			}

			ColZ.x = 1;

		}
	}
	else {
		ColZ.x = 0;
	}


	//debugText_->SetPos(0, 40);
	//debugText_->Printf("UP    = %f", ColZ.x);

	//手前に仮想的に移動して当たったら
	if (savemap_->mapcol(leftplayer, downplayer + player->GetSize() / 2, frontplayer - playerSpeed) || savemap_->mapcol(rightplayer, downplayer + player->GetSize() / 2, frontplayer - playerSpeed))
	{
		if (player->GetMove().z < 0 && ColZ.y == 0) {
			//１ピクセル先に壁が来るまで移動
			while (true)
			{
				if ((savemap_->mapcol(leftplayer, downplayer + player->GetSize() / 2, frontplayer - adjustPixcelSpeed) || savemap_->mapcol(rightplayer, downplayer + player->GetSize() / 2, frontplayer - adjustPixcelSpeed))) {
					break;
				}

				player->OnMapCollisionZ();
				frontplayer = player->GetPosition().z + adjustPlayerSize;
				backplayer = player->GetPosition().z + player->GetSize() - adjustPlayerSize;
			}

			ColZ.y = 1;

		}
	}
	else {
		ColZ.y = 0;
	}


	//debugText_->SetPos(0, 60);
	//debugText_->Printf("DOWN  = %f", ColZ.y);

	//if (ColX.x == 1) {

	//	if (ColZ.x == 1 && ColZ.y == 1) {
	//		ColX.x = 0;
	//	}

	//}

	//if (ColX.y == 1) {

	//	if (ColZ.x == 1) {
	//		ColX.y = 0;
	//	}

	//	if (ColZ.y == 1) {
	//		ColX.y = 0;
	//	}

	//}

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
	int pState = player->GetState();

	float pPosX1 = pPos.x - (pSize / 2);
	float pPosX2 = pPos.x + (pSize / 2);

	float pPosZ1 = pPos.z - (pSize / 2);
	float pPosZ2 = pPos.z + (pSize / 2);

	//向き
	int pInput;

	//プレイヤーと何かしら触れているか
	int playerContact = 0;

	//0なし 1上　2下　3左　4右
	int contact = 0;
	float contactNumX = 0;
	float contactNumZ = 0;

	Vector3 setPos = {};

	//プレイヤーにどちらが近いか
	int nearPlayer = 0;
	float nearPlayerSize0 = 0.0f;
	float nearPlayerSize1 = 0.0f;

	//自機とSブロック

	if (pPosX1 < sPosX2 && sPosX1 < pPosX2) {

		if (pPosZ1 < sPosZ2 && sPosZ1 < pPosZ2) {

			debugText_->Printf("PS");

			//Sブロックの挙動


			//どの面に一番近いか
			if (pPos.x > sPos.x) {
				contact = 3;
				contactNumX = pPos.x - sPos.x;
			}
			else {
				contact = 4;
				contactNumX = sPos.x - pPos.x;
			}

			if (pPos.z > sPos.z) {
				contactNumZ = pPos.z - sPos.z + 0.05;
			}
			else {
				contactNumZ = sPos.z - pPos.z + 0.05;
			}

			if (contactNumX < contactNumZ) {

				if (pPos.z > sPos.z) {
					contact = 2;
				}
				else {
					contact = 1;
				}

			}

			//座標を調節

			if (pState == NorthPole) {

				//自機に隣り合うように
				setPos = sPoleBlock.GetPos();

				if (contact == 3 || contact == 4) {
					setPos.z = pPos.z;
				}
				else if (contact == 1 || contact == 2) {
					setPos.x = pPos.x;
				}

				float playerSpeed = player->GetSpeed();
				Vector3 playerMove = player->GetMove();

				if (playerMove.x > 0) {
					setPos.x += playerSpeed;
				}
				else if (playerMove.x < 0) {
					setPos.x -= playerSpeed;
				}

				if (playerMove.z > 0) {
					setPos.z += playerSpeed;
				}
				else if (playerMove.z < 0) {
					setPos.z -= playerSpeed;
				}

				sPoleBlock.SetPos(setPos);
				sPoleBlock.SetMove(0);
			}

			//自機の挙動

			if (pState != NorthPole) {

				if (contact == 1) {
					ColZ.x = 1;
				}
				else if (contact == 2) {
					ColZ.y = 1;
				}
				else if (contact == 3) {
					ColX.y = 1;
				}
				else if (contact == 4) {
					ColX.x = 1;
				}
			}


			playerContact = 1;

		}
		else {
			sPoleBlock.SetMove(1);

		}

	}
	else {
		sPoleBlock.SetMove(1);

	}

	//debugText_->SetPos(0, 0);
	//debugText_->Printf("1U 2D 3L 4R  = %d", contact);

	//自機とNブロック

	if (pPosX1 < nPosX2 && nPosX1 < pPosX2) {

		if (pPosZ1 < nPosZ2 && nPosZ1 < pPosZ2) {

			debugText_->Printf("PS");

			//Sブロックの挙動


			//どの面に一番近いか
			if (pPos.x > nPos.x) {
				contact = 3;
				contactNumX = pPos.x - nPos.x;
			}
			else {
				contact = 4;
				contactNumX = nPos.x - pPos.x;
			}

			if (pPos.z > nPos.z) {
				contactNumZ = pPos.z - nPos.z + 0.05;
			}
			else {
				contactNumZ = nPos.z - pPos.z + 0.05;
			}

			if (contactNumX < contactNumZ) {

				if (pPos.z > nPos.z) {
					contact = 2;
				}
				else {
					contact = 1;
				}

			}

			//座標を調節

			if (pState == SouthPole) {

				//自機に隣り合うように
				setPos = nPoleBlock.GetPos();

				if (contact == 3 || contact == 4) {
					setPos.z = pPos.z;
				}
				else if (contact == 1 || contact == 2) {
					setPos.x = pPos.x;
				}

				float playerSpeed = player->GetSpeed();
				Vector3 playerMove = player->GetMove();

				if (playerMove.x > 0) {
					setPos.x += playerSpeed;
				}
				else if (playerMove.x < 0) {
					setPos.x -= playerSpeed;
				}

				if (playerMove.z > 0) {
					setPos.z += playerSpeed;
				}
				else if (playerMove.z < 0) {
					setPos.z -= playerSpeed;
				}

				nPoleBlock.SetPos(setPos);
				nPoleBlock.SetMove(0);
			}

			//自機の挙動

			if (pState != SouthPole) {

				if (contact == 1) {
					ColZ.x = 1;
				}
				else if (contact == 2) {
					ColZ.y = 1;
				}
				else if (contact == 3) {
					ColX.y = 1;
				}
				else if (contact == 4) {
					ColX.x = 1;
				}
			}

			playerContact = 1;

		}
		else {
			nPoleBlock.SetMove(1);

		}

	}
	else {
		nPoleBlock.SetMove(1);

	}

	//SブロックとNブロック

	if (sPosX1 < nPosX2 && nPosX1 < sPosX2) {

		if (sPosZ1 < nPosZ2 && nPosZ1 < sPosZ2) {

	/*		debugText_->Printf("NS");*/

			//どの面に一番近いか(触れてるか)
			if (nPos.x > sPos.x) {
				contact = 3;
				contactNumX = nPos.x - sPos.x;
			}
			else {
				contact = 4;
				contactNumX = sPos.x - nPos.x;
			}

			if (nPos.z > sPos.z) {
				contactNumZ = nPos.z - sPos.z + 0.025;
			}
			else {
				contactNumZ = sPos.z - nPos.z + 0.025;
			}

			if (contactNumX < contactNumZ) {

				if (nPos.z > sPos.z) {
					contact = 2;
				}
				else {
					contact = 1;
				}

			}

			//どちらのほうが自機に近いかベクトルを使って計算

			int A1 = sPos.x;
			int A2 = sPos.z;

			int B1 = nPos.x;
			int B2 = nPos.z;

			int P1 = pPos.x;
			int P2 = pPos.z;

			//sPos
			nearPlayerSize0 = sqrt(((P1 - A1) * (P1 - A1)) + ((P2 - A2) * (P2 - A2)));
			//nPos
			nearPlayerSize1 = sqrt(((P1 - B1) * (P1 - B1)) + ((P2 - B2) * (P2 - B2)));

			if (nearPlayerSize0 < nearPlayerSize1) {
				//sPosのほうが近い
				nearPlayer = 0;


				//if (nearPlayerSize1 - nearPlayerSize0 < 2) {
				//	nearPlayer = 2;
				//}
			
			}
			else {
				//nPosのほうが近い
				nearPlayer = 1;

				//if (nearPlayerSize0 - nearPlayerSize1< 2) {
				//	nearPlayer = 2;
				//}

			}

			//座標を調節

			//自機近いほうのブロックに隣り合うように


			if (nearPlayer == 0) {

				setPos = nPoleBlock.GetPos();

				if (contact == 3 || contact == 4) {
					setPos.z = sPos.z;
				}
				else if (contact == 1 || contact == 2) {
					setPos.x = sPos.x;

				}

			}
			else {

				setPos = sPoleBlock.GetPos();

				if (contact == 3 || contact == 4) {
					setPos.z = nPos.z;
				}
				else if (contact == 1 || contact == 2) {
					setPos.x = nPos.x;
				}

			}

			//移動を追加

			float playerSpeed = player->GetSpeed();
			Vector3 playerMove = player->GetMove();

			Vector3 magnetMoveVec = nPoleBlock.GetMoveVec();


			if (playerContact == 0) {

				if (contact == 1 || contact == 2) {
					setPos.z -= magnetMoveVec.z;
				}
				else if (contact == 3 || contact == 4) {
					setPos.x -= magnetMoveVec.x;
				}

			}
			else {

				if (playerMove.x > 0) {
					setPos.x += playerSpeed;
				}
				else if (playerMove.x < 0) {
					setPos.x -= playerSpeed;
				}

				if (playerMove.z > 0) {
					setPos.z += playerSpeed;
				}
				else if (playerMove.z < 0) {
					setPos.z -= playerSpeed;
				}

			}


			if (nearPlayer == 0) {
				nPoleBlock.SetPos(setPos);
				nPoleBlock.SetMove(0);
			}
			else {
				sPoleBlock.SetPos(setPos);
				sPoleBlock.SetMove(0);
			}

		}

	}

	debugText_->SetPos(0, 0);
	debugText_->Printf("S = 0 N = 1  = %d", nearPlayer);

	debugText_->SetPos(0, 20);
	debugText_->Printf("S  = %f", nearPlayerSize0);

	debugText_->SetPos(0, 40);
	debugText_->Printf("N  = %f", nearPlayerSize1);

	debugText_->SetPos(0, 60);
	debugText_->Printf("contact  = %d", contact);

	player->SetColX(ColX);
	player->SetColY(ColY);
	player->SetColZ(ColZ);

}

