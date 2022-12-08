#include "MagnetBlock.h"
#include"Matrix.h"
#include"Player.h"
using namespace MathUtility;

MagnetBlock::~MagnetBlock()
{
}

void MagnetBlock::Initialize(const Vector3& pos, bool isNorth_)
{
	//�f�o�b�O�e�L�X�g�ȂǁA�ėp�@�\�̃C���X�^���X�擾
	debugText = DebugText::GetInstance();

	model = Model::Create();

	worldTransform.Initialize();

	this->pos = pos;

	//�����Ŏ󂯎�������W�𔽉f
	worldTransform.translation_ = pos;
	worldTransformUpdate(&worldTransform);

	isNorth = isNorth_;
}

void MagnetBlock::Update(const Vector3& playerPos, int playerState, float moveDistance)
{
	if (move == 1) {
		//���@�̏�Ԃ����΂Ȃ�����񂹓��̏������s��
		if (playerState != UnMagnet) {
			bool isPlayerNorth = false;
			if (playerState == NorthPole) {
				isPlayerNorth = true;
			}
			else if (playerState == SouthPole) {
				isPlayerNorth = false;
			}
			bool isPull;
			if (isPlayerNorth == isNorth) {

				isPull = false;
			}
			else {
				isPull = true;
			}

			//���@���W���Q�Ƃ��A���@�Ǝ��΂̋������v�Z
			Vector3 vecPlayerToblock;
			vecPlayerToblock.x = playerPos.x - pos.x;
			vecPlayerToblock.y = playerPos.y - pos.y;
			vecPlayerToblock.z = playerPos.z - pos.z;
			//�x�N�g���̒����͈ړ��J�n�����ȉ��Ȃ玩�@�A���΂̎��͂��g���Ĉ����񂹓��̏���
			float vecLength = vector3Length(vecPlayerToblock);
			if (isPull) {

				if (vecLength <= moveDistance) {
					Vector3 moveVec;
					moveVec = Vector3Normalize(vecPlayerToblock);
					moveVec *= moveSpd;
					pos.x += moveVec.x;
					pos.y += moveVec.y;
					pos.z += moveVec.z;
				}
			}
			else {

				if (vecLength <= moveDistance) {
					Vector3 moveVec;
					moveVec = Vector3Normalize(vecPlayerToblock);
					moveVec *= moveSpd;
					pos.x -= moveVec.x;
					pos.y -= moveVec.y;
					pos.z -= moveVec.z;
				}
			}
		}
	}

	//���W�𔽉f
	worldTransform.translation_ = pos;
	worldTransformUpdate(&worldTransform);
}

void MagnetBlock::Draw(const ViewProjection& viewProjection, const uint32_t& nPoleTexture, const uint32_t& sPoleTexture)
{
	uint32_t magnetTex = 0;
	magnetTex = sPoleTexture;
	if (isNorth) {
		magnetTex = nPoleTexture;
	}
	model->Draw(worldTransform, viewProjection, magnetTex);
}