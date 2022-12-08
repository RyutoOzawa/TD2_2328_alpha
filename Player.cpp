#include "Player.h"
#include "matrix.h"

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

}

void Player::Update()
{
	//移動
	Move();

	//行列の更新
	worldTransformUpdate(&worldTransform);

}

void Player::Draw(const ViewProjection& viewProjection_)
{
	model->Draw(worldTransform, viewProjection_);
}

void Player::Move()
{

	if (input_->PushKey(DIK_A)) {
		if (colX.y == 0) {
			pos.x -= playerSpd;
			move.x = -playerSpd;
		}
		
	}
	else if (input_->PushKey(DIK_D)) {
		if (colX.x == 0) {
			pos.x += playerSpd;
			move.x = playerSpd;
		}
	}
	else {
		move.x = 0;
	}


	if (input_->PushKey(DIK_S)) {
		if (colZ.y == 0) {
			pos.z -= playerSpd;
			move.z = -playerSpd;
		}
	}
	else if (input_->PushKey(DIK_W)) {
		if (colZ.x == 0) {
			pos.z += playerSpd;
			move.z = playerSpd;
		}
	}
	else {
		move.z = 0;
	}

	//座標を反映
	worldTransform.translation_ = pos;
}


void Player::OnMapCollision()
{
}

void Player::OnMapCollisionX()
{
	pos.x -= 0.01;

}

void Player::OnMapCollisionY()
{
	pos.y -= 0.01;

}

void Player::OnMapCollisionZ()
{
	pos.z -= 0.01;

}

void Player::OnMapCollisionX2()
{
	pos.x += 0.01;
}

void Player::OnMapCollisionY2()
{
	pos.y += 0.01;

}

void Player::OnMapCollisionZ2()
{
	pos.z += 0.01;

}
