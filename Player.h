#pragma once
#include"ViewProjection.h"
#include"WorldTransform.h"
#include"Model.h"
#include"DebugText.h"
#include"MathUtility.h"
#include"Input.h"

enum stateIndex {
	UnMagnet,
	NorthPole,
	SouthPole,
};

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


	//�����蔻��p 2�̓}�C�i�X
	void OnMapCollision();
	void OnMapCollisionX();
	void OnMapCollisionY();
	void OnMapCollisionZ();
	void OnMapCollisionX2();
	void OnMapCollisionY2();
	void OnMapCollisionZ2();

	//�Q�b�^�[
	Vector3 GetTranslation() { return worldTransform.translation_; }
	Vector3 GetPosition() { return pos; }

	float GetSize() { return size; }
	Vector3 GetMove() { return move; }
	float GetSpeed() { return playerSpd; }

	//�Z�b�^�[
	void SetColX(Vector2 colX) { this->colX = colX; }
	void SetColY(Vector2 colY) { this->colY = colY; }
	void SetColZ(Vector2 colZ) { this->colZ = colZ; }

	
/// <summary>
/// �����o�ϐ�
/// </summary>
public:

	WorldTransform worldTransform;

	Model* model = nullptr;

	Vector3 pos = Vector3(0, 2, 0);
	Vector3 move = { 0,0,0 };

	float playerSpd = 0.05f;

	int size = 2;

	Vector2 colX = {};
	Vector2 colY = {};
	Vector2 colZ = {};

/// <summary>
/// �����o�֐��Ŏg���悤�̊֐�
/// </summary>
private:
	void Move();

	void ChangeState();


	~Player();

/// <summary>
/// �O���ŎQ�Ƃł��Ȃ������o�ϐ�
/// </summary>
private:
	Input* input_ = nullptr;
	DebugText* debugText_ = nullptr;
	uint32_t whiteTexture = 0;

	uint32_t redTexture = 0;
	uint32_t blueTexture = 0;
	uint32_t playerTexture = 0;

	int state;
};

