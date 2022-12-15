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

	void SetMove(int i) { this->move = i; }
	Vector3 GetMoveVec() { return moveVec; }

	bool GetIsNorth() const { return isNorth; }

	float GetMoveSpd()const { return moveSpd; }


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

	bool move = 1;

};