#pragma once
#include"ViewProjection.h"
#include"WorldTransform.h"
#include"Model.h"
#include"DebugText.h"
#include"MathUtility.h"
#include"Input.h"

class Player
{
	/// <summary>
	/// メンバ関数
	/// </summary>

	public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(const ViewProjection& viewProjection_);

	
/// <summary>
/// メンバ変数
/// </summary>
public:

	WorldTransform worldTransform;

	Model* model = nullptr;

	Vector3 pos;
	
/// <summary>
/// メンバ関数で使うようの関数
/// </summary>
private:
	void Move();
	~Player();

/// <summary>
/// 外部で参照できないメンバ変数
/// </summary>
private:
	Input* input_ = nullptr;
	DebugText* debugText_ = nullptr;
};

