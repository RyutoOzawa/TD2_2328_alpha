#pragma once
#include"WorldTransform.h"
#include"Model.h"
#include"DebugText.h"
#include"MathUtility.h"
#include"ViewProjection.h"

struct MagnetData
{
	Vector3 pos;
	bool isNorth_;
};

class MagnetBlock
{
public:
	~MagnetBlock();

	/// <summary>
	/// ‰Šú‰»
	/// </summary>
	/// <param name="pos"></param>
	void Initialize(MagnetData magnetData);

	void Update(const Vector3& playerPos, int playerState, float moveDistance);

	void Draw(const ViewProjection& viewProjection, const uint32_t& nPoleTexture, const uint32_t& sPoleTexture);

	Vector3 GetPos() { return pos; };
	void SetPos(Vector3 pos) { this->pos = pos; };

	void SetIsMove(bool isMove) { this->isMove = isMove; }
	bool GetIsMove() { return isMove; }

	//第一引数に磁力のON,OFF 第二引数に変更するブロックの番号
	void SetIsMagMove(int isMagMove,int i) { this->isMagMove[i] = isMagMove; }

	bool GetIsMagMove(int i) { return isMagMove[i]; }

	bool GetIsNorth() const { return isNorth; }

	float GetMoveSpd()const { return moveSpd; }

	bool GetIsStick() { return isStick; }
	void SetIsStick(bool isStick) { this->isStick = isStick; }

	int GetIsStickContact() { return stickContact; }
	void SetIsStickContact(int stickContact) { this->stickContact = stickContact; }

	int GetIsStickBlockNum() { return stickBlockNum; }
	void SetIsStickBlockNum(int stickBlockNum) { this->stickBlockNum = stickBlockNum; }

	bool GetIsStickPlayer() { return isStickPlayer; }
	void SetIsStickPlayer(bool isStickPlayer) { this->isStickPlayer = isStickPlayer; }

public:


private:
	Model* model = nullptr;
	DebugText* debugText = nullptr;
	WorldTransform worldTransform;
	Vector3 pos{};
	uint32_t textureHandle = 0;
	Vector3 moveVec;

	bool isNorth = false;
	float moveSpd = 0.025f;

	//引き寄せ処理ON OFF
	//プレイヤーとの
	bool isMove = true;
	//磁石との
	static const int MagMax = 50;
	int isMagMove[MagMax];

	//プレイヤーとくっついたか
	bool isStickPlayer = false;

	//他の磁石とくっついたか
	bool isStick = false;	
	//どの面がくっついたか
	int stickContact;
	//何番とくっついたか
	int stickBlockNum;

};