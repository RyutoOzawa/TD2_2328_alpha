#pragma once
#include"WorldTransform.h"
#include"Model.h"
#include"DebugText.h"
#include"MathUtility.h"
#include"ViewProjection.h"

class MagnetBlock
{
public:
	~MagnetBlock();

	/// <summary>
	/// èâä˙âª
	/// </summary>
	/// <param name="pos"></param>
	void Initialize(const Vector3& pos,bool isNorth_);

	void Update(const Vector3& playerPos,float moveDistance);

	void Draw(const ViewProjection& viewProjection, const uint32_t& nPoleTexture, const uint32_t& sPoleTexture);

public:


private:
	Model* model = nullptr;
	DebugText* debugText = nullptr;
	WorldTransform worldTransform;
	Vector3 pos{};
	uint32_t textureHandle = 0;

	bool isNorth = false;
	float moveSpd = 0.025f;
};

