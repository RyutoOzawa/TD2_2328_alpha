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

	void Update();

	void Draw(const ViewProjection& viewProjection, const uint32_t& nPoleTexture, const uint32_t& sPoleTexture);

	Vector3 GetPos() { return pos; };
	void SetPos(Vector3 pos) { this->pos = pos; };

	//Vector3 GetTenPos() { return tentativePos; };
	//void SetTenPos(Vector3 tentativePos) { this->tentativePos = tentativePos; };
	//void AddTenPos(Vector3 pos);
	//void SubTenPos(Vector3 pos);


	//自機との磁力ON,OFF
	void SetIsMove(bool isMove) { this->isMove = isMove; }
	bool GetIsMove() { return isMove; }

	//磁石との磁力ON,OFF
	void SetIsMagMove(int num,bool isMagMove) { this->isMagMove[num] = isMagMove; }
	bool GetIsMagMove(int num) { return isMagMove[num]; }

	//N極かS極か
	bool GetIsNorth() const { return isNorth; }

	//移動するスピード
	float GetMoveSpd()const { return moveSpd; }

	//当たっていたらtrueを返す
	bool Colision(Vector3 pos1 ,float pos1Size,Vector3 pos2,float pos2Size);

	float GetSize()const { return size; }

	//numにどの面か　contactNumにあったったブロック
	void SetContactNum(int num, int contactNum) { this->contactNum[num] = contactNum; }
	int GetContactNum(int num) { return  contactNum[num]; }
	void ReSetContactNum(int num) { this->contactNum[num] = 100; }


public:


private:
	Model* model = nullptr;
	DebugText* debugText = nullptr;
	WorldTransform worldTransform;
	Vector3 pos{};
	////仮座標
	//Vector3 tentativePos{};
	uint32_t textureHandle = 0;
	Vector3 moveVec;

	bool isNorth = false;
	float moveSpd = 0.025f;

	//引き寄せ処理ON OFF
	//プレイヤーとの
	bool isMove = true;
	//磁石との
	bool isMagMove[20] = {};

	float size = 2;

	int contactNum[5] = {};

};