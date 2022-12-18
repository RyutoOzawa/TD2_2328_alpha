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
	worldTransform.scale_ = Vector3(0.99f,0.99f,0.99f);
	worldTransformUpdate(&worldTransform);

	isNorth = magnetData.isNorth_;

	for (int i = 0; i < MagMax; i++) {
		isMagMove[i]  = true;
	}
}

void MagnetBlock::Update(const Vector3& playerPos, int playerState, float moveDistance)
{

	if (isMove) {

		//自機の状態が磁石なら引き寄せ等の処理を行う
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

			//自機座標を参照し、自機と磁石の距離を計算
			Vector3 vecPlayerToblock;
			vecPlayerToblock.x = playerPos.x - pos.x;
			vecPlayerToblock.y = playerPos.y - pos.y;
			vecPlayerToblock.z = playerPos.z - pos.z;
			//ベクトルの長さは移動開始距離以下なら自機、磁石の磁力を使って引き寄せ等の処理
			float vecLength = vector3Length(vecPlayerToblock);
			if (isPull) {

				if (vecLength <= moveDistance) {
					/*Vector3 moveVec;*/
					moveVec = Vector3Normalize(vecPlayerToblock);
					moveVec *= moveSpd;
					pos.x += moveVec.x;
					pos.y += moveVec.y;
					pos.z += moveVec.z;
				}
			}
			else {

				if (vecLength <= moveDistance) {
					/*Vector3 moveVec;*/
					moveVec = Vector3Normalize(vecPlayerToblock);
					moveVec *= moveSpd;
					pos.x -= moveVec.x;
					pos.y -= moveVec.y;
					pos.z -= moveVec.z;
				}
			}
		}

	}

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