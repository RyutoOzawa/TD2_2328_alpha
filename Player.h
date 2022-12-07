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
	/// �����o�֐�
	/// </summary>

	public:

	/// <summary>
	/// ������
	/// </summary>
	void Initialize();

	/// <summary>
	/// �X�V
	/// </summary>
	void Update();

	/// <summary>
	/// �`��
	/// </summary>
	void Draw(const ViewProjection& viewProjection_);

	
/// <summary>
/// �����o�ϐ�
/// </summary>
public:

	WorldTransform worldTransform;

	Model* model = nullptr;

	Vector3 pos;
	
/// <summary>
/// �����o�֐��Ŏg���悤�̊֐�
/// </summary>
private:
	void Move();
	~Player();

/// <summary>
/// �O���ŎQ�Ƃł��Ȃ������o�ϐ�
/// </summary>
private:
	Input* input_ = nullptr;
	DebugText* debugText_ = nullptr;
};

