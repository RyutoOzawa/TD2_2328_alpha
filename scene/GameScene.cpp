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


	MagnetData nBlockPos{ Vector3(6,2,2),true };
	MagnetData sBlockPos{ Vector3(11,2,2), false };

	MagnetData n2BlockPos{ Vector3(6, 2, 8), true };
	MagnetData s2BlockPos{ Vector3(11, 2, 8), false };

	magnetDatas.push_back(nBlockPos);
	magnetDatas.push_back(sBlockPos);
	magnetDatas.push_back(n2BlockPos);
	magnetDatas.push_back(s2BlockPos);


	for (int i = 0; i < magnetDatas.size(); i++) {

		MagnetBlock newBlock{};
		newBlock.Initialize(magnetDatas[i]);
		//ゲームで使うようの配列に格納
		magnetBlocks.push_back(newBlock);

	}

}

void GameScene::Update() {

	player->Update();

	camera1.eye = player->GetPosition();
	camera1.eye.y += 20.0f;
	camera1.eye.z -= 0.01;
	camera1.target = player->GetPosition();
	camera1.UpdateMatrix();

	//nPoleBlock.Update(player->GetPosition(), player->GetState(), 4.0f);
	//sPoleBlock.Update(player->GetPosition(), player->GetState(), 4.0f);

	for (int i = 0; i < magnetBlocks.size(); i++) {
		magnetBlocks[i].Update(player->GetPosition(), player->GetState(), 4.0f);
	}

	MagnetsUpdate();

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

	//ブロックの最大数
	const int blockSizeMax = 20;

	//nブロック 座標

	Vector3 bPos[blockSizeMax];

	float bPosX1[blockSizeMax] = {};
	float bPosX2[blockSizeMax] = {};

	float bPosZ1[blockSizeMax] = {};
	float bPosZ2[blockSizeMax] = {};

	//ブロックの当たり判定の大きさ(少し大きくしてる)
	float bSize = 2;


	for (int i = 0; i < magnetBlocks.size(); i++) {

		//ブロック 座標

		bPos[i] = magnetBlocks[i].GetPos();

		bPosX1[i] = bPos[i].x - (bSize / 2);
		bPosX2[i] = bPos[i].x + (bSize / 2);

		bPosZ1[i] = bPos[i].z - (bSize / 2);
		bPosZ2[i] = bPos[i].z + (bSize / 2);

		if (magnetBlocks[i].GetIsNorth() == true) {

		}

	}

	//--------自機----------

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

	//自機と磁石

	for (int i = 0; i < magnetBlocks.size(); i++) {

		if (pPosX1 < bPosX2[i] && bPosX1[i] < pPosX2) {

			if (pPosZ1 < bPosZ2[i] && bPosZ1[i] < pPosZ2) {

				//ブロックの挙動

				//どの面に一番近いか
				if (pPos.x > bPos[i].x) {
					contact = 3;
					contactNumX = pPos.x - bPos[i].x;
				}
				else {
					contact = 4;
					contactNumX = bPos[i].x - pPos.x;
				}

				if (pPos.z > bPos[i].z) {
					contactNumZ = pPos.z - bPos[i].z + 0.05;
				}
				else {
					contactNumZ = bPos[i].z - pPos.z + 0.05;
				}

				if (contactNumX < contactNumZ) {

					if (pPos.z > bPos[i].z) {
						contact = 2;
					}
					else {
						contact = 1;
					}

				}

				//磁石がSかNか
				if (magnetBlocks[i].GetIsNorth() == 0) {

					//Sブロックの場合

					//座標を調節

					if (pState == NorthPole) {

						//自機に隣り合うように
						setPos = magnetBlocks[i].GetPos();

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

						magnetBlocks[i].SetPos(setPos);
						magnetBlocks[i].SetMove(0);

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
				}
				else {

					//Nブロックの場合

					if (pState == SouthPole) {

						//自機に隣り合うように
						setPos = magnetBlocks[i].GetPos();

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

						magnetBlocks[i].SetPos(setPos);
						magnetBlocks[i].SetMove(0);
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

				}
				playerContact = 1;

			}
			else {
				magnetBlocks[i].SetMove(1);
			}

		}
		else {
			magnetBlocks[i].SetMove(1);
		}
	}

	//磁石ブロックと磁石ブロック

	//もう当たったかどうか判断するようの配列

	for (int i = 0; i < magnetBlocks.size(); i++) {

		for (int j = 0; j < magnetBlocks.size(); j++) {

			//同じ磁石ともう処理した組み合わせは当たった処理をしないように

			//同じ磁石か

			if (j >= i) {
				break;
			}

			//debugText_->SetPos(60 + i * 40, 60 + j * 40);
			//debugText_->Printf("%d,%d",i,j);


			if (bPosX1[j] < bPosX2[i] && bPosX1[i] < bPosX2[j]) {

				if (bPosZ1[j] < bPosZ2[i] && bPosZ1[i] < bPosZ2[j]) {

					//どの面に一番近いか(触れてるか)
					if (bPos[i].x > bPos[j].x) {
						contact = 3;
						contactNumX = bPos[i].x - bPos[j].x;
					}
					else {
						contact = 4;
						contactNumX = bPos[j].x - bPos[i].x;
					}

					if (bPos[i].z > bPos[j].z) {
						contactNumZ = bPos[i].z - bPos[j].z + 0.025;
					}
					else {
						contactNumZ = bPos[j].z - bPos[i].z + 0.025;
					}

					if (contactNumX < contactNumZ) {

						if (bPos[i].z > bPos[j].z) {
							contact = 2;
						}
						else {
							contact = 1;
						}

					}

					//どちらのほうが自機に近いかベクトルを使って計算

					int A1 = bPos[j].x;
					int A2 = bPos[j].z;

					int B1 = bPos[i].x;
					int B2 = bPos[i].z;

					int P1 = pPos.x;
					int P2 = pPos.z;

					//sPos
					nearPlayerSize0 = sqrt(((P1 - A1) * (P1 - A1)) + ((P2 - A2) * (P2 - A2)));
					//nPos
					nearPlayerSize1 = sqrt(((P1 - B1) * (P1 - B1)) + ((P2 - B2) * (P2 - B2)));

					if (nearPlayerSize0 < nearPlayerSize1) {
						//sPosのほうが近い
						nearPlayer = 0;
					}
					else {
						//nPosのほうが近い
						nearPlayer = 1;
					}

					//座標を調節

					//自機近いほうのブロックに隣り合うように

					if (nearPlayer == 0) {

						setPos = magnetBlocks[i].GetPos();

						if (contact == 3 || contact == 4) {
							setPos.z = bPos[j].z;
						}
						else if (contact == 1 || contact == 2) {
							setPos.x = bPos[j].x;

						}

					}
					else {

						setPos = magnetBlocks[j].GetPos();

						if (contact == 3 || contact == 4) {
							setPos.z = bPos[i].z;
						}
						else if (contact == 1 || contact == 2) {
							setPos.x = bPos[i].x;
						}

					}

					//移動を追加

					float playerSpeed = player->GetSpeed();
					Vector3 playerMove = player->GetMove();

					Vector3 magnetMoveVec;

					if (nearPlayer == 0) {
						magnetMoveVec = magnetBlocks[i].GetMoveVec();
					}
					else {
						magnetMoveVec = magnetBlocks[j].GetMoveVec();
					}

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
						magnetBlocks[j].SetPos(setPos);
						magnetBlocks[j].SetMove(0);
					}
					else {
						magnetBlocks[i].SetPos(setPos);
						magnetBlocks[i].SetMove(0);
					}

				}

			}
		}
	}

	debugText_->SetPos(0, 0);
	debugText_->Printf("S = 0 N = 1  = %d", nearPlayer);

	debugText_->SetPos(0, 20);
	debugText_->Printf("S  = %f", nearPlayerSize0);

	debugText_->SetPos(0, 40);
	debugText_->Printf("N  = %f", nearPlayerSize1);

	//debugText_->SetPos(0, 60);
	//debugText_->Printf("contact  = %d", contact);

	player->SetColX(ColX);
	player->SetColY(ColY);
	player->SetColZ(ColZ);

}

void GameScene::MagnetsUpdate() {

	//配列の最大数-1回for文を回す
	for (int i = 0; i < magnetBlocks.size() - 1; i++) {
		//i個目の磁石に対して、i+1 ~ 配列末尾までのブロックと磁石の判定を行う
		for (int j = i + 1; j < magnetBlocks.size(); j++) {
			bool isSame = false;
			//同極か対極か
			if (magnetBlocks[i].GetIsNorth() == magnetBlocks[j].GetIsNorth()) {
				isSame = true;
			}

			//ブロック同士のベクトル作成
			Vector3 vecMagToMag;
			vecMagToMag.x = magnetBlocks[i].GetPos().x - magnetBlocks[j].GetPos().x;
			vecMagToMag.y = magnetBlocks[i].GetPos().y - magnetBlocks[j].GetPos().y;
			vecMagToMag.z = magnetBlocks[i].GetPos().z - magnetBlocks[j].GetPos().z;
			//ベクトルの長さ取得
			float vecLen = vector3Length(vecMagToMag);
			//i個目の磁石とj個目の磁力による挙動
			if (isSame) {
				if (vecLen <= 4.0f) {
					//ベクトルを正規化+磁石の速さに直す
					vecMagToMag = vector3Normalize(vecMagToMag);
					vecMagToMag *= magnetBlocks[i].GetMoveSpd();
					//それぞれのブロックに加算
					Vector3 pos1, pos2;
					pos1 = magnetBlocks[i].GetPos();
					pos2 = magnetBlocks[j].GetPos();

					pos1.x += vecMagToMag.x;
					pos1.y += vecMagToMag.y;
					pos1.z += vecMagToMag.z;
					pos2.x -= vecMagToMag.x;
					pos2.y -= vecMagToMag.y;
					pos2.z -= vecMagToMag.z;

					magnetBlocks[i].SetPos(pos1);
					magnetBlocks[j].SetPos(pos2);

				}
			}
			else {
				if (vecLen <= 4.0f) {
					//ベクトルを正規化+磁石の速さに直す
					vecMagToMag = vector3Normalize(vecMagToMag);
					vecMagToMag *= magnetBlocks[i].GetMoveSpd();
					//それぞれのブロックに加算
					Vector3 pos1, pos2;
					pos1 = magnetBlocks[i].GetPos();
					pos2 = magnetBlocks[j].GetPos();

					pos1.x -= vecMagToMag.x;
					pos1.y -= vecMagToMag.y;
					pos1.z -= vecMagToMag.z;
					pos2.x += vecMagToMag.x;
					pos2.y += vecMagToMag.y;
					pos2.z += vecMagToMag.z;


					magnetBlocks[i].SetPos(pos1);
					magnetBlocks[j].SetPos(pos2);
				}
			}
		}
	}


}