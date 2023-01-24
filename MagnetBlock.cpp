#include "MagnetBlock.h"
#include"Matrix.h"
#include"Player.h"
using namespace MathUtility;

MagnetBlock::~MagnetBlock()
{
}

void MagnetBlock::Initialize(MagnetData magnetData)
{
	//デバッグテキストなど、汎用機能のインスタンス取得
	debugText = DebugText::GetInstance();

	model = Model::Create();

	worldTransform.Initialize();

	this->pos = magnetData.pos;

	//引数で受け取った座標を反映
	worldTransform.translation_ = pos;
	worldTransform.scale_ = Vector3(0.99f, 0.99f, 0.99f);
	worldTransformUpdate(&worldTransform);

	isNorth = magnetData.isNorth_;

	for (int i = 0; i < 20; i++) {
		isMagMove[i] = true;

	}

	for (int j = 0; j < 5; j++) {
		contactNum[j] = 100;
	}
}

void MagnetBlock::Update()
{

	//座標を反映
	worldTransform.translation_ = pos;
	worldTransformUpdate(&worldTransform);
	//moveVec = { 0.0f,0.0f,0.0f };z
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

//void MagnetBlock::AddTenPos(Vector3 pos)
//{
//	this->tentativePos.x += pos.x; 
//	this->tentativePos.y += pos.y;
//	this->tentativePos.z += pos.z;
//}
//
//void MagnetBlock::SubTenPos(Vector3 pos)
//{
//	this->tentativePos.x -= pos.x;
//	this->tentativePos.y -= pos.y;
//	this->tentativePos.z -= pos.z;
//}

bool MagnetBlock::Colision(Vector3 pos1, float pos1Size, Vector3 pos2, float pos2Size)
{
	float pos1X1 = pos1.x - (pos1Size / 2);
	float pos1X2 = pos1.x + (pos1Size / 2);

	float pos1Z1 = pos1.z - (pos1Size / 2);
	float pos1Z2 = pos1.z + (pos1Size / 2);

	float pos2X1 = pos2.x - (pos2Size / 2);
	float pos2X2 = pos2.x + (pos2Size / 2);

	float pos2Z1 = pos2.z - (pos2Size / 2);
	float pos2Z2 = pos2.z + (pos2Size / 2);

	if (pos1X1 < pos2X2 && pos2X1 < pos1X2) {

		if (pos1Z1 < pos2Z2 && pos2Z1 < pos1Z2) {

			return true;

		}
	}

	return false;
}

void MagnetBlock::OnMapCollision()
{
}

void MagnetBlock::OnMapCollisionX()
{
	pos.x -= adjustPixcelSpeed;

}

void MagnetBlock::OnMapCollisionY()
{
	pos.y -= adjustPixcelSpeed;

}

void MagnetBlock::OnMapCollisionZ()
{
	pos.z -= adjustPixcelSpeed;

}

void MagnetBlock::OnMapCollisionX2()
{
	pos.x += adjustPixcelSpeed;
}

void MagnetBlock::OnMapCollisionY2()
{
	pos.y += adjustPixcelSpeed;

}

void MagnetBlock::OnMapCollisionZ2()
{
	pos.z += adjustPixcelSpeed;

}