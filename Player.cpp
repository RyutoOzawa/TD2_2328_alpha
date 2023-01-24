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
	//�T�C�Y�̒���
	worldTransform.scale_ = Vector3(0.99, 0.99, 0.99);
}

void Player::Update()
{
	//�ړ�
	Move();

	//���@�̏�Ԃ�ς���
	ChangeState();

	//���@�̏�Ԃɂ���Ďg���摜��ς���
	if (state == UnMagnet) {
		playerTexture = whiteTexture;
	}
	else if (state == NorthPole) {
		playerTexture = redTexture;
	}
	else if (state == SouthPole) {
		playerTexture = blueTexture;
	}

	//�s��̍X�V
	//���W�𔽉f
	worldTransform.translation_ = pos;
	worldTransformUpdate(&worldTransform);

}

void Player::Draw(const ViewProjection& viewProjection_)
{
	model->Draw(worldTransform, viewProjection_, playerTexture);
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

}

void Player::ChangeState()
{
	if (input_->TriggerKey(DIK_SPACE)) {
		state++;
		//state = SouthPole;
		if (state > SouthPole) {
			state = UnMagnet;
		}
	}
}

void Player::OnMapCollision()
{
}

void Player::OnMapCollisionX()
{
	pos.x -= adjustPixcelSpeed;

}

void Player::OnMapCollisionY()
{
	pos.y -= adjustPixcelSpeed;

}

void Player::OnMapCollisionZ()
{
	pos.z -= adjustPixcelSpeed;

}

void Player::OnMapCollisionX2()
{
	pos.x += adjustPixcelSpeed;
}

void Player::OnMapCollisionY2()
{
	pos.y += adjustPixcelSpeed;

}

void Player::OnMapCollisionZ2()
{
	pos.z += adjustPixcelSpeed;

}