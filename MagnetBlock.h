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

	//自機との磁力ON,OFF
	void SetIsMove(bool isMove) { this->isMove = isMove; }
	bool GetIsMove() { return isMove; }

	//磁石との磁力 第一引数に磁力のON,OFF 第二引数に変更するブロックの番号
	void SetIsMagMove(int isMagMove,int i) { this->isMagMove[i] = isMagMove; }
	bool GetIsMagMove(int i) { return isMagMove[i]; }

	//N極かS極か
	bool GetIsNorth() const { return isNorth; }

	//移動するスピード
	float GetMoveSpd()const { return moveSpd; }


	//磁石とくっついているか 引数は何番とか
	bool GetIsStick(int num) { return isStick[num]; }
	//磁石とくっついているか 第一引数に何番とか 第二引数にくっついているか
	void SetIsStick(int num,bool isStick) { this->isStick[num] = isStick; }


	//どの面とくっついているか　引数のnumはくっついている磁石の番号
	int GetStickContact(int num) { return stickContact[num]; }
	//どの面とくっついているか  第一引数に何番とか 第二引数にどの面
	void SetStickContact(int num,int stickContact) { this->stickContact[num] = stickContact; }


	//どのブロックとくっついているか 引数はどの面か
	int GetStickBlockNum(int contact) { return stickBlockNum[contact]; }
	//どのブロックとくっついているか  第一引数にどの面 第二引数に何番とか
	void SetStickBlockNum(int contact,int stickBlockNum) { this->stickBlockNum[contact] = stickBlockNum; }


	//面がくっついているか　trueがくっついているfalseがくっついていない
	int GetIsStickContact(int num) { return isStickContact[num]; }
	//面がくっついているか  第一引数にどの面か 第二引数にくっついているか
	void SetIsStickContact(int num, bool isStickContact) { this->isStickContact[num] = isStickContact; }


	//プレイヤーとくっついているか
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
	bool isStick[MagMax] = {};

	//どの面がくっついたか
	int stickContact[MagMax];

	//何番とくっついたか	//0なし 1上　2下　3左　4右
	int stickBlockNum[5];

	//くっついたかくっついていないか　(4面)
	int isStickContact[5];


};