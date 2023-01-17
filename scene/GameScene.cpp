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

	//ブロックの当たり判定の大きさ
	float bSize = 1.98; // (2 * 0.99)


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

	//0なし 1上　2下　3左　4右
	int contact = 0;
	int contact1 = 0;
	int contact2 = 0;

	float contactNumX = 0;
	float contactNumZ = 0;

	Vector3 setPos = { 0,2,0 };

	//プレイヤーにどちらが近いか
	int nearPlayer = 0;

	//自機と磁石

	for (int i = 0; i < magnetBlocks.size(); i++) {

		if (pPosX1 < bPosX2[i] && bPosX1[i] < pPosX2) {

			if (pPosZ1 < bPosZ2[i] && bPosZ1[i] < pPosZ2) {

				//ブロックの挙動

				//どの面に一番近いか
				contact = GetContact(pPos, bPos[i]);

				//磁石がSかNか

				//Sブロックの場合
				if (magnetBlocks[i].GetIsNorth() == 0) {

					//座標を調節

					if (pState == NorthPole) {

						//magnetBlocks[i].SetPos(setPos);
						magnetBlocks[i].SetIsMove(0);

						magnetBlocks[i].SetIsStickPlayer(true);
						magnetBlocks[i].SetStickContact(i, contact);
						//magnetBlocks[i].SetIsStickContact(contact,true);

					}
					else {

						//めり込まないように動きを止める

						//自機の挙動

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

						magnetBlocks[i].SetIsStickPlayer(false);
						magnetBlocks[i].SetIsMove(true);

					}

				}
				else {

					//Nブロックの場合

					if (pState == SouthPole) {
						//magnetBlocks[i].SetPos(setPos);
						magnetBlocks[i].SetIsMove(0);

						magnetBlocks[i].SetIsStickPlayer(true);
						magnetBlocks[i].SetStickContact(i, contact);
						//magnetBlocks[i].SetIsStickContact(contact,true);

					}
					else {

						//自機の挙動

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

						magnetBlocks[i].SetIsStickPlayer(false);
						magnetBlocks[i].SetIsMove(true);

					}

				}

			}
			else {

			}

		}
		else {

		}
	}

	//自機にくっついたら
	for (int i = 0; i < magnetBlocks.size(); i++) {

		//自機とは違う極か
		bool isDiffMag = 1;

		if (magnetBlocks[i].GetIsNorth() == 1) {
			if (player->GetState() != SouthPole) {
				isDiffMag = 0;
				magnetBlocks[i].SetIsStickPlayer(false);
			}
		}
		else {
			if (player->GetState() != NorthPole) {
				isDiffMag = 0;
				magnetBlocks[i].SetIsStickPlayer(false);
			}
		}

		if (magnetBlocks[i].GetIsStickPlayer() == true && player->GetState() != UnMagnet) {

			if (isDiffMag) {

				Vector3 setPos = magnetBlocks[i].GetPos();
				contact = magnetBlocks[i].GetStickContact(i);

				Vector3 stickPos = pPos;

				int blockSize = 2;

				if (contact == 1) {
					setPos.z = (stickPos.z + blockSize);
					setPos.x = stickPos.x;
				}
				else if (contact == 2) {
					setPos.z = (stickPos.z - blockSize);
					setPos.x = stickPos.x;
				}
				else if (contact == 3) {
					setPos.x = (stickPos.x - blockSize);
					setPos.z = stickPos.z;
				}
				else if (contact == 4) {
					setPos.x = (stickPos.x + blockSize);
					setPos.z = stickPos.z;
				}

				magnetBlocks[i].SetPos(setPos);
			}

		}

	}

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

			//debugText_->SetPos(60 + i * 40, 60 + j * 40);
			//debugText_->Printf("%d,%d",i,j);


			if (bPosX1[j] < bPosX2[i] && bPosX1[i] < bPosX2[j]) {

				if (bPosZ1[j] < bPosZ2[i] && bPosZ1[i] < bPosZ2[j]) {

					nearPlayer = NearPlayerJudge(bPos[j], bPos[i], pPos);

					//どの面に一番近いか(触れてるか)


					//どの面が触れているかの判断
					contact1 = GetContact(bPos[i], bPos[j]);

					contact2 = GetContact(bPos[j], bPos[i]);


					//座標を調節

					//めり込まないように
					Vector3 setPos;

					if (nearPlayer == 0) {
						setPos = magnetBlocks[i].GetPos();
					}
					else {
						setPos = magnetBlocks[j].GetPos();
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

					//違う極であればくっつく処理を
					if (magnetBlocks[i].GetIsNorth() != magnetBlocks[j].GetIsNorth()) {

						//自機近いほうのブロックに隣り合うように

						//計算した座標とくっついた情報をセット
						if (nearPlayer == 0) {

							magnetBlocks[i].SetPos(setPos);
							//magnetBlocks[i].SetIsStick(true);
							//magnetBlocks[i].SetIsStickBlockNum(j);
							//magnetBlocks[i].SetIsMagMove(0, j);
							//magnetBlocks[i].SetIsStickContact(j,contact);

							////どの面が触れたか
							//magnetBlocks[i].SetStickContact(j, contact1);
						}
						else {
							magnetBlocks[j].SetPos(setPos);
							//magnetBlocks[j].SetIsStick(true);
							//magnetBlocks[j].SetIsStickBlockNum(i);
							//magnetBlocks[j].SetIsMagMove(0, i);
							//magnetBlocks[j].SetIsStickContact(i,contact);

							////どの面が触れたか
							//magnetBlocks[j].SetStickContact(i, contact2);
						}


						//磁石がくっついているに変更
						magnetBlocks[i].SetIsStick(j, true);
						magnetBlocks[j].SetIsStick(i, true);
						//くっついた磁石は何か
						magnetBlocks[i].SetStickBlockNum(contact1, j);
						magnetBlocks[j].SetStickBlockNum(contact2, i);
						//どの面が触れたか
						magnetBlocks[i].SetStickContact(j, contact1);
						magnetBlocks[j].SetStickContact(i, contact2);
						//くっついた面を記録
						magnetBlocks[i].SetIsStickContact(contact1, true);
						magnetBlocks[j].SetIsStickContact(contact2, true);

						//磁石同士の反応をしないように
						magnetBlocks[i].SetIsMagMove(0, j);
						magnetBlocks[j].SetIsMagMove(0, i);

					}
					else {//同じ極であればくっつかない
						if (nearPlayer == 0) {
							magnetBlocks[i].SetPos(setPos);
							//magnetBlocks[i].SetIsStick(false);
							magnetBlocks[i].SetIsMagMove(true, j);
						}
						else {
							magnetBlocks[j].SetPos(setPos);
							//magnetBlocks[j].SetIsStick(false);
							magnetBlocks[j].SetIsMagMove(true, i);
						}

						////磁石がくっついていないに変更
						//magnetBlocks[i].SetIsStick(false);
						//magnetBlocks[j].SetIsStick(false);

						//magnetBlocks[i].SetIsMagMove(true, j);
						//magnetBlocks[j].SetIsMagMove(true, i);

					}
				}
			}
		}
	}

	//あったっているブロックすべてのお互いの磁力をきる

	//くっついているブロックをtrueに
	for (int i = 0; i < magnetBlocks.size(); i++) {

		//4面調べる
		for (int j = 0; j < 5; j++) {



			//もしくっついているなら
			if (magnetBlocks[i].GetIsStick(j)) {

				stickBlockMass[i] = true;

				stickBlockMass[magnetBlocks[i].GetStickBlockNum(j)] = true;

				mostNear = NearPlayerJudge(bPos[mostNearPlayerBlock], bPos[i], pPos);

				if (mostNear != 0) {
					mostNearPlayerBlock = i;
				}

			}


			debugText_->SetPos(0 + (j * 60), 120);
			debugText_->Printf("%d = %d", j,stickBlockMass[j]);

		}

		//debugText_->SetPos((100 * i),0);
		//debugText_->Printf(" %d = %d", i,stickBlockMass[i]);

	}

	debugText_->SetPos(0, 100);
	debugText_->Printf("%d", mostNearPlayerBlock);

	//磁力を切る

	for (int i = 0; i < magnetBlocks.size(); i++) {


		for (int j = 0; j < magnetBlocks.size(); j++) {


			//くっついている磁石同士

			if (stickBlockMass[i] && stickBlockMass[j] && i != j) {

				magnetBlocks[j].SetIsMagMove(false, i);

			}

			debugText_->SetPos((100 * j), (20 * i));
			debugText_->Printf("%d", magnetBlocks[j].GetIsMagMove(i));

			//自機

			//ブロックが他のブロックとくっついている場合 一番近いブロックの磁力が自機と影響しあう
			if (magnetBlocks[i].GetIsStick(j) && i != j) {
				if (mostNearPlayerBlock == i) {
					magnetBlocks[i].SetIsMove(true);
				}
				else {
					magnetBlocks[i].SetIsMove(false);
				}
			}
		}
	}

	//くっついたらそのブロックから離れないようにする処理

	for (int i = 0; i < magnetBlocks.size(); i++) {

		for (int j = 0; j < magnetBlocks.size(); j++) {

			//ほかの磁石にくっついているとき
			if (magnetBlocks[i].GetIsStick(j)) {

				Vector3 setPos = magnetBlocks[i].GetPos();
				int stickBlockNum = magnetBlocks[i].GetStickBlockNum(j);

				contact = magnetBlocks[i].GetStickContact(stickBlockNum);
				Vector3 stickPos = magnetBlocks[stickBlockNum].GetPos();


				//1番プレイヤーに近い場合、その座標が基準になるため自身はほかの磁石の座標を参照しない
				if (mostNearPlayerBlock == i) {

					break;

				}
				else {

					if (i == 0 && magnetBlocks[0].GetIsStick(3)) {
						a++;
					}

					int isFirstTime = true;
					//int mostNearPlayerBlockFour = magnetBlocks[i].GetStickBlockNum(1);


					//4辺の中で1番プレイヤーに近いブロックを基準に
					for (int k = 1; k < 5; k++) {

						if (magnetBlocks[i].GetIsStickContact(k) == true) {

							if (isFirstTime == 1) {
								mostNearPlayerBlockFour = k;
								isFirstTime = false;
							}
							else {
								mostNearFour = NearPlayerJudge(bPos[magnetBlocks[i].GetStickBlockNum(mostNearPlayerBlockFour)], bPos[magnetBlocks[i].GetStickBlockNum(k)], pPos);
							}

							if (mostNearFour != 0) {
								mostNearPlayerBlockFour = k;
							}

						}

					}

					setPos = magnetBlocks[i].GetPos();
					stickBlockNum = magnetBlocks[i].GetStickBlockNum(mostNearPlayerBlockFour);

					contact = magnetBlocks[i].GetStickContact(stickBlockNum);
					stickPos = magnetBlocks[stickBlockNum].GetPos();

				}

				if (i == 0) {
					debugText_->SetPos(0, 140);
					debugText_->Printf("Pos %f,%f", setPos.x, setPos.z);

					debugText_->SetPos(0, 160);
					debugText_->Printf("stickBlockNum %d", stickBlockNum);

					debugText_->SetPos(0, 180);
					debugText_->Printf("SPos %f,%f", stickPos.x, stickPos.z);

					debugText_->SetPos(0, 200);
					debugText_->Printf(" %d", mostNearPlayerBlockFour);
				}

				int blockSize = 2;

				//当たった時の処理
				if (contact == 1) {
					setPos.z = (stickPos.z - blockSize);
					setPos.x = stickPos.x;
				}
				else if (contact == 2) {
					setPos.z = (stickPos.z + blockSize);
					setPos.x = stickPos.x;
				}
				else if (contact == 3) {
					setPos.x = (stickPos.x + blockSize);
					setPos.z = stickPos.z;
				}
				else if (contact == 4) {
					setPos.x = (stickPos.x - blockSize);
					setPos.z = stickPos.z;
				}

				if (stickBlockNum != i) {

					////計算した座標をセット

					magnetBlocks[i].SetPos(setPos);

				}

			}
			else {

			}
		}
	}

	for (int i = 0; i < magnetBlocks.size(); i++) {

		for (int j = 0; j < magnetBlocks.size(); j++) {

			if (j >= i) {
				break;
			}

			if (magnetBlocks[j].GetIsMagMove(i) == 1) {


			}


		}
	}

	//debugText_->SetPos(0, 0);
	//debugText_->Printf("S = 0 N = 1  = %d", nearPlayer);

	//debugText_->SetPos(0, 20);
	//debugText_->Printf("n  = %f", nearPlayerSize0);

	//debugText_->SetPos(0, 40);
	//debugText_->Printf("j  = %f", nearPlayerSize1);

	//Vector3 magnetMoveVec;
	//magnetMoveVec = magnetBlocks[1].GetMovePos();
	//debugText_->SetPos(0, 60);
	//debugText_->Printf("_______ = %f,%f,%f", magnetMoveVec.x, magnetMoveVec.y, magnetMoveVec.z);


	player->SetColX(ColX);
	player->SetColY(ColY);
	player->SetColZ(ColZ);

}

void GameScene::MagnetsUpdate() {

	//配列の最大数-1回for文を回す
	for (int i = 0; i < magnetBlocks.size() - 1; i++) {
		//i個目の磁石に対して、i+1 ~ 配列末尾までのブロックと磁石の判定を行う
		for (int j = i + 1; j < magnetBlocks.size(); j++) {

			//引き寄せ処理がどちらもONの場合引き寄せるように

			bool isMagMove = magnetBlocks[i].GetIsMagMove(j);

			if (isMagMove) {
				isMagMove = magnetBlocks[j].GetIsMagMove(i);
			}

			if (isMagMove) {

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

}

int GameScene::GetContact(Vector3 mainPos, Vector3 subPos)
{
	//0なし 1上　2下　3左　4右
	int contact = 0;

	//X軸 Z軸判断用
	float contactNumX = 0;
	float contactNumZ = 0;

	if (mainPos.x > subPos.x) {
		contact = 3;
		contactNumX = mainPos.x - subPos.x;
	}
	else {
		contact = 4;
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
			contact = 2;
		}
		else {
			contact = 1;
		}

	}

	return contact;
}

bool GameScene::NearPlayerJudge(Vector3 bPos1, Vector3 bPos2, Vector3 pPos)
{

	int nearPlayer;
	float nearPlayerSize1 = 0.0f;
	float nearPlayerSize2 = 0.0f;

	//どちらのほうが自機に近いかベクトルを使って計算

	float A1 = bPos1.x;
	float A2 = bPos1.z;

	float B1 = bPos2.x;
	float B2 = bPos2.z;

	float P1 = pPos.x;
	float P2 = pPos.z;

	//bPos1
	nearPlayerSize1 = sqrt(((P1 - A1) * (P1 - A1)) + ((P2 - A2) * (P2 - A2)));
	//bPos2
	nearPlayerSize2 = sqrt(((P1 - B1) * (P1 - B1)) + ((P2 - B2) * (P2 - B2)));

	if (nearPlayerSize1 < nearPlayerSize2) {
		//bPos1のほうが近い
		nearPlayer = 0;
	}
	else {
		//bPos2のほうが近い
		nearPlayer = 1;
	}

	return nearPlayer;
}
