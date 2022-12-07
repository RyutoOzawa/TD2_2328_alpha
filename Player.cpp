#include "Player.h"

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
	//�ړ�
	Move();

	//�s��̍X�V
	worldTransform.TransferMatrix();

}

void Player::Draw(const ViewProjection& viewProjection_)
{
	model->Draw(worldTransform, viewProjection_);
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

	//���W�𔽉f
	worldTransform.translation_ = pos;
}


