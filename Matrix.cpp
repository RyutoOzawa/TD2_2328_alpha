#include "Matrix.h"
#include <cassert>
#include "math.h"
#include <cmath>

void worldTransformScale(Vector3* vector_, WorldTransform* worldTransform_)
{
	Vector3 vec(vector_->x, vector_->y, vector_->z);
	Matrix4 matScale(
		worldTransform_->scale_.x, 0.0f, 0.0f, 0.0f,
		0.0f, worldTransform_->scale_.y, 0.0f, 0.0f,
		0.0f, 0.0f, worldTransform_->scale_.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	vector_->x = vec.x * matScale.m[0][0] + vec.y * matScale.m[1][0] + vec.z * matScale.m[2][0];
	vector_->y = vec.x * matScale.m[0][1] + vec.y * matScale.m[1][1] + vec.z * matScale.m[2][1];
	vector_->z = vec.x * matScale.m[0][2] + vec.y * matScale.m[1][2] + vec.z * matScale.m[2][2];
}

void worldTransformMove(Vector3* vector_, WorldTransform* worldTransform_)
{
	Vector3 vec(vector_->x, vector_->y, vector_->z);
	Matrix4 matTrans(
		1.0f, 0.0f, 0.0f, 0,
		0.0f, 1.0f, 0.0f, 0,
		0.0f, 0.0f, 1.0f, 0,
		worldTransform_->translation_.x, worldTransform_->translation_.y, worldTransform_->translation_.z, 1.0f
	);
	vector_->x = vec.x * matTrans.m[0][0] + vec.y * matTrans.m[1][0] + vec.z * matTrans.m[2][0] + matTrans.m[3][0];
	vector_->y = vec.x * matTrans.m[0][1] + vec.y * matTrans.m[1][1] + vec.z * matTrans.m[2][1] + matTrans.m[3][1];
	vector_->z = vec.x * matTrans.m[0][2] + vec.y * matTrans.m[1][2] + vec.z * matTrans.m[2][2] + matTrans.m[3][2];
}

void worldTransformRoll(Vector3* vector_, WorldTransform* worldTransform_)
{
	Vector3 vec(vector_->x, vector_->y, vector_->z);
	Matrix4 matRotX(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, cos(worldTransform_->rotation_.x), sin(worldTransform_->rotation_.x), 0.0f,
		0.0f, -sin(worldTransform_->rotation_.x), cos(worldTransform_->rotation_.x), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
	Matrix4 matRotY(
		cos(worldTransform_->rotation_.y), 0.0f, -sin(worldTransform_->rotation_.y), 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		sin(worldTransform_->rotation_.y), 0.0f, cos(worldTransform_->rotation_.y), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
	Matrix4 matRotZ(
		cos(worldTransform_->rotation_.z), sin(worldTransform_->rotation_.z), 0.0f, 0.0f,
		-sin(worldTransform_->rotation_.z), cos(worldTransform_->rotation_.z), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	matRotZ *= matRotX;
	matRotZ *= matRotY;
	vector_->x = vec.x * matRotZ.m[0][0] + vec.y * matRotZ.m[1][0] + vec.z * matRotZ.m[2][0];
	vector_->y = vec.x * matRotZ.m[0][1] + vec.y * matRotZ.m[1][1] + vec.z * matRotZ.m[2][1];
	vector_->z = vec.x * matRotZ.m[0][2] + vec.y * matRotZ.m[1][2] + vec.z * matRotZ.m[2][2];
}

void worldTransformUpdate(WorldTransform* worldTransform_)
{
	worldTransform_->matWorld_ = {
		worldTransform_->scale_.x, 0.0f, 0.0f, 0.0f,
		0.0f, worldTransform_->scale_.y, 0.0f, 0.0f,
		0.0f, 0.0f, worldTransform_->scale_.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	Matrix4 matTrans(
		1.0f, 0.0f, 0.0f, 0,
		0.0f, 1.0f, 0.0f, 0,
		0.0f, 0.0f, 1.0f, 0,
		worldTransform_->translation_.x, worldTransform_->translation_.y, worldTransform_->translation_.z, 1.0f
	);
	Matrix4 matRotX(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, cos(worldTransform_->rotation_.x), sin(worldTransform_->rotation_.x), 0.0f,
		0.0f, -sin(worldTransform_->rotation_.x), cos(worldTransform_->rotation_.x), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
	Matrix4 matRotY(
		cos(worldTransform_->rotation_.y), 0.0f, -sin(worldTransform_->rotation_.y), 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		sin(worldTransform_->rotation_.y), 0.0f, cos(worldTransform_->rotation_.y), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
	Matrix4 matRotZ(
		cos(worldTransform_->rotation_.z), sin(worldTransform_->rotation_.z), 0.0f, 0.0f,
		-sin(worldTransform_->rotation_.z), cos(worldTransform_->rotation_.z), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
	worldTransform_->matWorld_ *= matRotZ;
	worldTransform_->matWorld_ *= matRotX;
	worldTransform_->matWorld_ *= matRotY;
	worldTransform_->matWorld_ *= matTrans;

	worldTransform_->TransferMatrix();
}

void vecWorldTransform(Vector3* vector_, WorldTransform* worldTransform_)
{
	worldTransformScale(vector_, worldTransform_);
	worldTransformRoll(vector_, worldTransform_);
	worldTransformMove(vector_, worldTransform_);
}

float vector3Length(Vector3 vector_)
{
	float x2 = vector_.x * vector_.x;
	float y2 = vector_.y * vector_.y;
	float z2 = vector_.z * vector_.z;
	return sqrt(x2 + y2 + z2);
}

Vector3 vector3Normalize(Vector3 vector_)
{
	Vector3 v;
	v.x = vector_.x / vector3Length(vector_);
	v.y = vector_.y / vector3Length(vector_);
	v.z = vector_.z / vector3Length(vector_);

	return v;
}
