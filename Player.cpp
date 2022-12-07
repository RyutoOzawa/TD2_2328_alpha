#include "Player.h"
#include"matrix.h"
#include"TextureManager.h"



Player::~Player()
{
	delete model;
}

void Player::Initialize()
{
	input_ = Input::GetInstance();
	debugText_ = DebugText::GetInstance();


	worldTransform.Initialize();

	model = Model::Create();

	whiteTexture = TextureManager::Load("white1x1.png");
	redTexture = TextureManager::Load("red1x1.png");
	blueTexture = TextureManager::Load("blue1x1.png");

	playerTexture = whiteTexture;

}

void Player::Update()
{
	//移動
	Move();

	//自機の状態を変える
	ChangeState();

	//自機の状態によって使う画像を変える
	if (state == UnMagnet) {
		playerTexture = whiteTexture;
	}
	else if (state == NorthPole) {
		playerTexture = redTexture;
	}
	else if (state == SouthPole) {
		playerTexture = blueTexture;
	}

	//行列の更新
	worldTransformUpdate(&worldTransform);

	
}

void Player::Draw(const ViewProjection& viewProjection_)
{
	model->Draw(worldTransform, viewProjection_,playerTexture);
}

void Player::Move()
{
	float playerSpd = 0.05f;
	
	if (input_->PushKey(DIK_A)) {
		pos.x -= playerSpd;
	}
	if (input_->PushKey(DIK_D)) {
		pos.x += playerSpd;
	}
	if (input_->PushKey(DIK_S)) {
		pos.z -= playerSpd;
	}
	if (input_->PushKey(DIK_W)) {
		pos.z += playerSpd;
	}

	//座標を反映
	worldTransform.translation_ = pos;
}

void Player::ChangeState()
{
	if (input_->TriggerKey(DIK_SPACE)) {
		state++;
		if (state > SouthPole) {
			state = UnMagnet;
		}
	}
}


