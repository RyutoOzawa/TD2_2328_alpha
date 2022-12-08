#include "MagnetBlock.h"
#include"Matrix.h"

MagnetBlock::~MagnetBlock()
{
}

void MagnetBlock::Initialize(const Vector3& pos, bool isNorth_)
{
	//デバッグテキストなど、汎用機能のインスタンス取得
	debugText = DebugText::GetInstance();

	model = Model::Create();

	worldTransform.Initialize();

	this->pos = pos;

	//引数で受け取った座標を反映
	worldTransform.translation_ = pos;
	worldTransformUpdate(&worldTransform);

	isNorth = isNorth_;
}

void MagnetBlock::Update()
{


	//座標を反映
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
