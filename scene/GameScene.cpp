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

	camera1.eye = player->GetPosition();
	camera1.eye.y += 20.0f;
	camera1.eye.z -= 0.01;
	camera1.target = player->GetPosition();
	camera1.UpdateMatrix();

	//nPoleBlock.Update(player->GetPosition(), player->GetState(), 4.0f);
	//sPoleBlock.Update(player->GetPosition(), player->GetState(), 4.0f);


	//座標関係更新
	InforUpdate();

	//磁石の移動
	MagnetsUpdate();

	//ブロックとの当たり判定
	MapCollision();

	//磁石と自機の当たり判定(押し戻し処理)
	PosCollision();

	//磁力のON,OFF
	//MagnetPower();

	//座標の更新
	for (int i = 0; i < magnetBlocks.size(); i++) {
		//計算した座標を確定

		magnetBlocks[i].SetPos(setPos[i]);

		debugText_->SetPos(0, 0);
		debugText_->Printf("setPos[0] = (%f,%f,%f)", setPos[0].x, setPos[0].y, setPos[0].z);

		magnetBlocks[i].Update();
	}

	player->Update();

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

	player->SetColX(ColX);
	player->SetColY(ColY);
	player->SetColZ(ColZ);

	//--------磁石-----------
	// 
	//当たらないよう調整する用
	//float adjustPixcelSpeed = player->GetAdjustPixcelSpeed();
	//自機サイズ調整用
	float adjustMagSize = 0.03;


	float leftmagnet[blockSizeMax];
	float downmagnet[blockSizeMax];
	float frontmagnet[blockSizeMax];

	float rightmagnet[blockSizeMax];
	float upmagnet[blockSizeMax];
	float backmagnet[blockSizeMax];

	float magnetSpeedX[blockSizeMax];
	float magnetSpeedZ[blockSizeMax];

	for (int i = 0; i < magnetBlocks.size(); i++) {
		
		//座標を用意
		leftmagnet[i] = bPos[i].x + adjustMagSize;
		downmagnet[i] = bPos[i].y + adjustMagSize;
		frontmagnet[i] = bPos[i].z + adjustMagSize;

		rightmagnet[i] = bPos[i].x + magnetBlocks[i].GetSize() - adjustMagSize;
		upmagnet[i] = bPos[i].y - magnetBlocks[i].GetSize() - adjustMagSize;
		backmagnet[i] = bPos[i].z + magnetBlocks[i].GetSize() - adjustMagSize;

		//当たっているか
		//Vector2 ColX = { 0,0 };
		//Vector2 ColY = { 0,0 };
		//Vector2 ColZ = { 0,0 };

		magnetSpeedX[i] = magnetBlocks[i].GetSpeed().x + adjustPixcelSpeed;
		magnetSpeedZ[i] = magnetBlocks[i].GetSpeed().z + adjustPixcelSpeed;

	}


	/////////////
	////磁石/////
	/////////////

	for (int i = 0; i < magnetBlocks.size(); i++) {

		////右に仮想的に移動して当たったら
		//if (savemap_->mapcol(rightmagnet[i] + magnetSpeedX[i], downmagnet[i] + magnetBlocks[i].GetSize() / 2, frontmagnet[i]) || savemap_->mapcol(rightmagnet[i] + magnetSpeedX[i], downmagnet[i] + magnetBlocks[i].GetSize() / 2, backmagnet[i]))
		//{

		//	if (magnetSpeedX[i] > 0 && mColX[i].x == 0) {
		//		//１ピクセル先に壁が来るまで移動
		//		while (true)
		//		{
		//			if ((savemap_->mapcol(rightmagnet[i] + adjustPixcelSpeed, downmagnet[i] + magnetBlocks[i].GetSize() / 2, frontmagnet[i]) || savemap_->mapcol(rightmagnet[i] + adjustPixcelSpeed, downmagnet[i] + magnetBlocks[i].GetSize() / 2, backmagnet[i]))) {
		//				break;
		//			}

		//			magnetBlocks[i].OnMapCollisionX2();
		//			rightmagnet[i] = magnetBlocks[i].GetPos().x + magnetBlocks[i].GetSize() - adjustMagSize;
		//			leftmagnet[i] = magnetBlocks[i].GetPos().x + adjustMagSize;

		//		}

		//		mColX[i].x = 1;
		//	}

		//}
		//else {
		//	mColX[i].x = 0;
		//}

		////debugText_->SetPos(0,0);
		////debugText_->Printf("RIGHT = %f",mColX.x);

		////左に仮想的に移動して当たったら
		//if (savemap_->mapcol(leftmagnet[i] - magnetSpeedX[i], downmagnet[i] + magnetBlocks[i].GetSize() / 2, frontmagnet[i]) || savemap_->mapcol(leftmagnet[i] - magnetSpeedX[i], downmagnet[i] + magnetBlocks[i].GetSize() / 2, backmagnet[i]))
		//{
		//	if (magnetSpeedX[i] < 0 && mColX[i].y == 0) {
		//		//１ピクセル先に壁が来るまで移動
		//		while (true)
		//		{
		//			if ((savemap_->mapcol(leftmagnet[i] - adjustPixcelSpeed, downmagnet[i] + magnetBlocks[i].GetSize() / 2, frontmagnet[i]) || savemap_->mapcol(leftmagnet[i] - adjustPixcelSpeed, downmagnet[i] + magnetBlocks[i].GetSize() / 2, backmagnet[i]))) {
		//				break;
		//			}

		//			magnetBlocks[i].OnMapCollisionX();
		//			rightmagnet[i] = magnetBlocks[i].GetPos().x + magnetBlocks[i].GetSize() - adjustMagSize;
		//			leftmagnet[i] = magnetBlocks[i].GetPos().x + adjustMagSize;
		//		}

		//		mColX[i].y = 1;

		//	}

		//}
		//else {
		//	mColX[i].y = 0;
		//}


		//debugText_->SetPos(0, 20);
		//debugText_->Printf("LEFT  = %f", mColX.y);

		//leftmagnet[i] = magnet[i]->GetPosition().x + adjust;
		//rightmagnet[i] = magnet[i]->GetPosition().x + magnetBlocks[i].GetSize() - adjust;

		//upmagnet[i] = magnet[i]->GetPosition().y - magnetBlocks[i].GetSize() - adjust;
		//downmagnet[i] = magnet[i]->GetPosition().y + adjust;


		//z軸に対しての当たり判定
		//奥に仮想的に移動して当たったら
		if (savemap_->mapcol(leftmagnet[i], downmagnet[i] + magnetBlocks[i].GetSize() / 2, backmagnet[i]) || savemap_->mapcol(rightmagnet[i], downmagnet[i] + magnetBlocks[i].GetSize() / 2, backmagnet[i] ))
		{
			if (magnetSpeedZ[i] > 0 && mColZ[i].x == 0) {
				//１ピクセル先に壁が来るまで移動
				while (true)
				{
					if ((savemap_->mapcol(leftmagnet[i], downmagnet[i] + magnetBlocks[i].GetSize() / 2, backmagnet[i] + adjustPixcelSpeed) || savemap_->mapcol(rightmagnet[i], downmagnet[i] + magnetBlocks[i].GetSize() / 2, backmagnet[i] + adjustPixcelSpeed))) {
						break;
					}

					magnetBlocks[i].OnMapCollisionZ2();
					frontmagnet[i] = magnetBlocks[i].GetPos().z + adjustMagSize;
					backmagnet[i] = magnetBlocks[i].GetPos().z + magnetBlocks[i].GetSize() - adjustMagSize;
				}

				mColZ[i].x = 1;

			}
		}
		else {
			mColZ[i].x = 0;
		}


		//debugText_->SetPos(0, 40);
		//debugText_->Printf("UP    = %f", mColZ.x);

		//手前に仮想的に移動して当たったら

		if (savemap_->mapcol(leftmagnet[i], downmagnet[i] + magnetBlocks[i].GetSize() / 2, frontmagnet[i] - magnetSpeedZ[i]) || savemap_->mapcol(rightmagnet[i], downmagnet[i] + magnetBlocks[i].GetSize() / 2, frontmagnet[i] - magnetSpeedZ[i]))
		{
			if (magnetSpeedZ[i] < 0 && mColZ[i].y == 0) {
				//１ピクセル先に壁が来るまで移動
				while (true)
				{
					if ((savemap_->mapcol(leftmagnet[i], downmagnet[i] + magnetBlocks[i].GetSize() / 2, frontmagnet[i] - adjustPixcelSpeed) || savemap_->mapcol(rightmagnet[i], downmagnet[i] + magnetBlocks[i].GetSize() / 2, frontmagnet[i] - adjustPixcelSpeed))) {
						break;
					}

					magnetBlocks[i].OnMapCollisionZ();
					frontmagnet[i] = magnetBlocks[i].GetPos().z + adjustMagSize;
					backmagnet[i] = magnetBlocks[i].GetPos().z + magnetBlocks[i].GetSize() - adjustMagSize;
				}

				mColZ[i].y = 1;

			}
		}
		else {
			mColZ[i].y = 0;
		}
	}

}

void GameScene::PosCollision()
{

	for (int i = 0; i < magnetBlocks.size(); i++) {

		//ブロック 座標

		bPos[i] = setPos[i];

		bPosX1[i] = setPos[i].x - (bSize / 2);
		bPosX2[i] = setPos[i].x + (bSize / 2);

		bPosZ1[i] = setPos[i].z - (bSize / 2);
		bPosZ2[i] = setPos[i].z + (bSize / 2);

	}

	//自機と磁石

	for (int i = 0; i < magnetBlocks.size(); i++) {

		if (pPosX1 < bPosX2[i] && bPosX1[i] < pPosX2) {

			if (pPosZ1 < bPosZ2[i] && bPosZ1[i] < pPosZ2) {

				Vector3 pos1 = pPos;
				Vector3 pos2 = setPos[i];

				float pos1Size = pSize;
				float pos2Size = bSize;

				//調整用
				Vector3 adjust = GetVec(pos1, pos2);

				adjust = ChangeVec(adjust, 0.001f);

				int contact = GetContact(pos1, pos2);

				if (pState != UnMagnet) {

					//押し戻し処理
					while (true)
					{
						if ((magnetBlocks[i].Colision(pos1, pos1Size, pos2, pos2Size))) {

							//まだ重なっていたら離す

							//あたったのがx軸かz軸かで押し戻しを変更
							if (contact == 1 || contact == 2) {
								pos2.y += adjust.y;
								pos2.z += adjust.z;

								//pos1.y -= adjust.y;
								//pos1.z -= adjust.z;
							}
							else {
								pos2.x += adjust.x;
								pos2.y += adjust.y;

								//pos1.x -= adjust.x;
								//pos1.y -= adjust.y;
							}
						}
						else {
							break;
						}

					}

					//当たってないところでセット
					player->SetPos(pos1);
					setPos[i] = pos2;

					//プレイヤー移動分を加算
					setPos[i].x + pMoveVec.x;
					setPos[i].z + pMoveVec.z;

					//あったった面を記録
					if (pState == NorthPole && magnetBlocks[i].GetIsNorth() == 0) {
						magnetBlocks[i].SetContactNum(contact, i);
					}
					else if (pState == SouthPole && magnetBlocks[i].GetIsNorth() == 1) {
						magnetBlocks[i].SetContactNum(contact, i);
					}
					else {
						magnetBlocks[i].ReSetContactNum(contact);
					}

				}
				else {

					////押し戻し処理
					//while (true)
					//{
					//	if ((magnetBlocks[i].Colision(pos1, pos1Size, pos2, pos2Size))) {

					//		//まだ重なっていたら離す

					//		//あたったのがx軸かz軸かで押し戻しを変更
					//		if (contact == 1 || contact == 2) {
					//			pos1.y -= adjust.y;
					//			pos1.z -= adjust.z;
					//		}
					//		else {
					//			pos1.x -= adjust.x;
					//			pos1.y -= adjust.y;
					//		}
					//	}
					//	else {
					//		break;
					//	}

					//}

					//当たってないところでセット
					//player->SetPos(pos1);

					//自機が磁力なしの場合めり込ませないように
					if (contact == 1) {
						ColZ.y = 1;
					}
					else if (contact == 2) {
						ColZ.x = 1;
					}
					else if (contact == 3) {
						ColX.x = 1;
					}
					else if (contact == 4) {
						ColX.y = 1;
					}

					magnetBlocks[i].ReSetContactNum(contact);

					//debugText_->SetPos(120, 0);
					//debugText_->Printf("true");
				}

				//debugText_->SetPos(0, 0);
				//debugText_->Printf("p b[%d] true", i);

			}

		}

	}

	debugText_->SetPos(0, 120);
	debugText_->Printf("pMove = (%f,%f,%f)", pMoveVec.x, pMoveVec.y, pMoveVec.z);

	//----------磁石ブロックと磁石ブロック----------

	//もう当たったかどうか判断するようの配列

	//あったったときの情報などを取得する

	for (int i = 0; i < magnetBlocks.size(); i++) {

		for (int j = 0; j < magnetBlocks.size(); j++) {

			//同じ磁石ともう処理した組み合わせは当たった処理をしないように

			//同じ磁石か

			if (j >= i) {
				break;
			}


			if (bPosX1[j] < bPosX2[i] && bPosX1[i] < bPosX2[j]) {

				if (bPosZ1[j] < bPosZ2[i] && bPosZ1[i] < bPosZ2[j]) {

					//Vector3 pos1 = magnetBlocks[i].GetPos();
					//Vector3 pos2 = magnetBlocks[j].GetPos();

					Vector3 pos1 = setPos[i];
					Vector3 pos2 = setPos[j];

					float pos1Size = bSize;
					float pos2Size = bSize;

					//調整用
					Vector3 adjust = GetVec(pos1, pos2);

					adjust = ChangeVec(adjust, 0.001f);

					int contact1 = GetContact(pos2, pos1);
					int contact2 = GetContact(pos1, pos2);

					//押し戻し処理
					while (true)
					{
						if ((magnetBlocks[i].Colision(pos1, pos1Size, pos2, pos2Size))) {

							//あたったのがx軸かz軸かで押し戻しを変更
							if (contact1 == 1 || contact1 == 2) {

								pos1.y -= adjust.y;
								pos1.z -= adjust.z;

								pos2.y += adjust.y;
								pos2.z += adjust.z;
							}
							else {

								pos1.x -= adjust.x;
								pos1.y -= adjust.y;

								pos2.x += adjust.x;
								pos2.y += adjust.y;

							}

							magnetBlocks[i].SetContactNum(contact1, j);
							magnetBlocks[j].SetContactNum(contact2, i);

						}
						else {
							break;
						}

					}


					//当たってないところでセット
					//magnetBlocks[i].SetPos(pos1);
					//magnetBlocks[j].SetPos(pos2);
					setPos[i] = pos1;
					setPos[j] = pos2;

					//debugText_->SetPos(0, 20);
					//debugText_->Printf("b[%d] b[%d] true", i, j);
				}
			}

		}
	}

	//debugText_->SetPos(0, 0);
	////debugText_->Printf("b[%d] b[%d] = (%f,%f,%f)", i, j, adjust.x, adjust.y, adjust.z);
	//debugText_->Printf("setPos[0] = (%f,%f,%f)", setPos[0].x, setPos[0].y, setPos[0].z);

	for (int k = 1; k < 5; k++) {

		debugText_->SetPos(0 + (100 * (k - 1)), 100);

		debugText_->Printf("%d =  %d ", k, magnetBlocks[0].GetContactNum(k));

	}

	player->SetColX(ColX);
	player->SetColY(ColY);
	player->SetColZ(ColZ);

}

void GameScene::MagnetsUpdate() {

	MagToMagUpdate();

	MagToPlayerUpdate();

}

void GameScene::MagToMagUpdate()
{	//配列の最大数-1回for文を回す
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

			float moveSpd = 0.025;

			//距離で磁力の強さを変化させる
			if (4.0f > vecLen) {
				moveSpd = ((4.0f / 1000) - (vecLen / 1000)) / 0.01;
			}
			

			//i個目の磁石とj個目の磁力による挙動
			if (isSame) {
				if (vecLen <= 4.0f) {
					//ベクトルを正規化+磁石の速さに直す
					vecMagToMag = vector3Normalize(vecMagToMag);
					vecMagToMag *= moveSpd;
					//それぞれのブロックに加算
					Vector3 pos1, pos2;
					pos1 = magnetBlocks[i].GetPos();
					pos2 = magnetBlocks[j].GetPos();

					if (mColX[i].x == 0) {
						setPos[i].x += vecMagToMag.x;
					}

					setPos[i].y += vecMagToMag.y;
					
					if (mColZ[i].x == 0) {
						setPos[i].z += vecMagToMag.z;
					}

					if (mColX[j].y == 0) {

						setPos[j].x -= vecMagToMag.x;
					}

					setPos[j].y -= vecMagToMag.y;

					if (mColZ[j].y == 0) {

						setPos[j].z -= vecMagToMag.z;
					}

					//magnetBlocks[i].SetPos(pos1);
					//magnetBlocks[j].SetPos(pos2);

					magnetBlocks[i].SetSpeed(-vecMagToMag);
					magnetBlocks[j].SetSpeed(+vecMagToMag);

				}
			}
			else {
				if (vecLen <= 4.0f && vecLen) {
					//ベクトルを正規化+磁石の速さに直す
					vecMagToMag = vector3Normalize(vecMagToMag);
					vecMagToMag *= moveSpd;
					//それぞれのブロックに加算
					Vector3 pos1, pos2;
					pos1 = magnetBlocks[i].GetPos();
					pos2 = magnetBlocks[j].GetPos();

					if (mColX[i].y == 0) {
						setPos[i].x -= vecMagToMag.x;
					}

					setPos[i].y -= vecMagToMag.y;

					if (mColZ[i].y == 0) {
						setPos[i].z -= vecMagToMag.z;
					}

					if (mColX[j].x == 0) {
						setPos[j].x += vecMagToMag.x;
					}

					setPos[j].y += vecMagToMag.y;

					if (mColZ[j].x == 0) {
						setPos[j].z += vecMagToMag.z;

					}


					magnetBlocks[i].SetSpeed(vecMagToMag);
					magnetBlocks[j].SetSpeed(-vecMagToMag);

					//magnetBlocks[i].SetPos(pos1);
					//magnetBlocks[j].SetPos(pos2);

					debugText_->SetPos(0, 20);
					debugText_->Printf("b[%d] b[%d] = (%f,%f,%f)", i, j, vecMagToMag.x, vecMagToMag.y, vecMagToMag.z);

				}
			}
		}


	}

}

void GameScene::MagToPlayerUpdate()
{

	for (int i = 0; i < magnetBlocks.size(); i++) {

		if (magnetBlocks[i].GetIsMove()) {

			Vector3 pos = magnetBlocks[i].GetPos();

			//自機の状態が磁石なら引き寄せ等の処理を行う
			if (pState != UnMagnet) {
				bool isPlayerNorth = false;
				if (pState == NorthPole) {
					isPlayerNorth = true;
				}
				else if (pState == SouthPole) {
					isPlayerNorth = false;
				}
				bool isPull;
				if (isPlayerNorth == magnetBlocks[i].GetIsNorth()) {

					isPull = false;
				}
				else {
					isPull = true;
				}

				//自機座標を参照し、自機と磁石の距離を計算
				Vector3 vecPlayerToblock;
				vecPlayerToblock.x = pPos.x - pos.x;
				vecPlayerToblock.y = pPos.y - pos.y;
				vecPlayerToblock.z = pPos.z - pos.z;
				//ベクトルの長さは移動開始距離以下なら自機、磁石の磁力を使って引き寄せ等の処理
				float vecLength = vector3Length(vecPlayerToblock);

				float moveSpd = 0.025;

				//距離で磁力の強さを変化させる
				if (4.0f > vecLength) {
					moveSpd = ((4.0f / 1000) - (vecLength / 1000)) / 0.01;
				}

				if (isPull) {

					if (vecLength <= 4.0f) {
						/*Vector3 moveVec;*/
						bMoveVec = vector3Normalize(vecPlayerToblock);
						bMoveVec *= moveSpd;
						if (mColX[i].x == 0) {
							setPos[i].x += bMoveVec.x;
						}
						setPos[i].y += bMoveVec.y;

						if (mColZ[i].x == 0) {
							setPos[i].z += bMoveVec.z;
						}

						magnetBlocks[i].SetSpeed(-bMoveVec);

					}
				}
				else {

					if (vecLength <= 4.0f) {
						/*Vector3 moveVec;*/
						bMoveVec = vector3Normalize(vecPlayerToblock);
						bMoveVec *= moveSpd;

						if (mColX[i].y == 0) {
							setPos[i].x -= bMoveVec.x;
						}

						setPos[i].y -= bMoveVec.y;

						if (mColZ[i].x == 0) {
							setPos[i].z -= bMoveVec.z;
						}


						magnetBlocks[i].SetSpeed(-bMoveVec);

					}
				}
			}

			//magnetBlocks[i].SetPos(pos);

		}

	}
}

void GameScene::MagnetPower()
{
	//磁力のON,OFF

//4面調べてあったっている方向の磁石とは反応しないように


	float bSize = 2; // (2 * 0.99)

	float pSize = 2;

	//配列の最大数-1回for文を回す
	for (int i = 0; i < magnetBlocks.size() - 1; i++) {
		//i個目の磁石に対して、i+1 ~ 配列末尾までのブロックと磁石の判定を行う
		for (int j = i + 1; j < magnetBlocks.size(); j++) {

			for (int k = 1; k < 5; k++) {

				//自機と磁石

				if (magnetBlocks[i].GetContactNum(k) == i) {

					debugText_->SetPos(0, 80);
					debugText_->Printf("qqqqqqqqqqqq ");

					if (k == 1) {
						if (magnetBlocks[i].GetPos().z + (bSize / 2) <= player->GetPosition().z - (pSize / 2)) {
							magnetBlocks[i].SetIsMove(false);
						}
						else {
							magnetBlocks[i].SetIsMove(true);
						}
					}

					if (k == 2) {
						if (magnetBlocks[i].GetPos().z - (bSize / 2) > player->GetPosition().z + (pSize / 2)) {
							magnetBlocks[i].SetIsMove(false);
						}
						else {
							magnetBlocks[i].SetIsMove(true);
						}
					}


					if (k == 3) {
						if (magnetBlocks[i].GetPos().x - (bSize / 2) >= player->GetPosition().x + (pSize / 2)) {
							magnetBlocks[i].SetIsMove(false);
						}
						else {
							magnetBlocks[i].SetIsMove(true);
						}
					}


					if (k == 4) {
						if (magnetBlocks[i].GetPos().x + (bSize / 2) < player->GetPosition().x - (pSize / 2)) {
							magnetBlocks[i].SetIsMove(false);
						}
						else {
							magnetBlocks[i].SetIsMove(true);
						}
					}

				}
				else {
					magnetBlocks[i].SetIsMove(true);
				}

				//磁石同士

				if (magnetBlocks[i].GetContactNum(k) == 100) {
					continue;
				}

				if (magnetBlocks[i].GetContactNum(k) != j && magnetBlocks[i].GetContactNum(k) != i) {

					if (k == 1) {
						if (magnetBlocks[i].GetPos().z + (bSize / 2) <= magnetBlocks[j].GetPos().z) {
							magnetBlocks[i].SetIsMagMove(j, false);
						}
						else {
							magnetBlocks[i].SetIsMagMove(j, true);
						}
					}

					if (k == 2) {
						if (magnetBlocks[i].GetPos().z - (bSize / 2) > magnetBlocks[j].GetPos().z) {
							magnetBlocks[i].SetIsMagMove(j, false);
						}
						else {
							magnetBlocks[i].SetIsMagMove(j, true);
						}
					}


					if (k == 3) {
						if (magnetBlocks[i].GetPos().x - (bSize / 2) >= magnetBlocks[j].GetPos().x) {
							magnetBlocks[i].SetIsMagMove(j, false);
						}
						else {
							magnetBlocks[i].SetIsMagMove(j, true);
						}
					}


					if (k == 4) {
						if (magnetBlocks[i].GetPos().x + (bSize / 2) < magnetBlocks[j].GetPos().x) {
							magnetBlocks[i].SetIsMagMove(j, false);
						}
						else {
							magnetBlocks[i].SetIsMagMove(j, true);
						}
					}

					if (i == 0) {
						debugText_->SetPos(0 + (60 * k - 1), 60);
						debugText_->Printf("b[%d] b[%d] = ", i, j);
					}

				}

			}

			//引き寄せ処理がどちらもONの場合引き寄せるように

			//	bool isMagMove = magnetBlocks[i].GetIsMagMove(j);

			//if (isMagMove) {
			//	isMagMove = magnetBlocks[j].GetIsMagMove(i);
			//}
		}

	}
}

void GameScene::InforUpdate()
{

	//nブロック 座標

	for (int i = 0; i < magnetBlocks.size(); i++) {

		//ブロック 座標

		bPos[i] = magnetBlocks[i].GetPos();

		bPosX1[i] = bPos[i].x - (bSize / 2);
		bPosX2[i] = bPos[i].x + (bSize / 2);

		bPosZ1[i] = bPos[i].z - (bSize / 2);
		bPosZ2[i] = bPos[i].z + (bSize / 2);

		setPos[i] = magnetBlocks[i].GetPos();

		for (int j = 0; j < 5; j++) {
			magnetBlocks[i].ReSetContactNum(j);
		}
	}

	//--------自機----------

	pPos = player->GetPosition();
	pSize = player->GetSize();
	pState = player->GetState();

	pPosX1 = pPos.x - (pSize / 2);
	pPosX2 = pPos.x + (pSize / 2);

	pPosZ1 = pPos.z - (pSize / 2);
	pPosZ2 = pPos.z + (pSize / 2);

	pMoveVec = player->GetMove();
}

int GameScene::GetContact(Vector3 mainPos, Vector3 subPos)
{
	//0なし 1上　2下　3左　4右
	int contact = 0;

	//X軸 Z軸判断用
	float contactNumX = 0;
	float contactNumZ = 0;

	if (mainPos.x > subPos.x) {
		contact = 4;
		contactNumX = mainPos.x - subPos.x;
	}
	else {
		contact = 3;
		contactNumX = subPos.x - mainPos.x;
	}

	if (mainPos.z > subPos.z) {
		contactNumZ = mainPos.z - subPos.z + 0.05;
	}
	else {
		contactNumZ = subPos.z - mainPos.z + 0.05;
	}

	if (contactNumX < contactNumZ) {

		if (mainPos.z > subPos.z) {
			contact = 1;
		}
		else {
			contact = 2;
		}

	}

	return contact;
}

Vector3 GameScene::GetVec(Vector3 pos1, Vector3 pos2)
{
	return Vector3(pos2.x - pos1.x, pos2.y - pos1.y, pos2.z - pos1.z);
}

Vector3 GameScene::ChangeVec(Vector3 changeVec, float size)
{
	//正規化

	Vector3 vec = changeVec;

	float l = 1 / lengthVec(vec);

	vec.x *= l;
	vec.y *= l;
	vec.z += l;

	//sizeをかける

	vec.x *= size;
	vec.y *= size;
	vec.z *= size;


	return vec;
}

float GameScene::lengthVec(Vector3 vec)
{
	return sqrt((vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z));
}
