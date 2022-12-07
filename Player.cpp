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

	//���W�𔽉f
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


