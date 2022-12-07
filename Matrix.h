#pragma once
#include "Vector3.h"
#include "Matrix4.h"
#include <d3d12.h>
#include "WorldTransform.h"

//WorldTransformの更新
void worldTransformUpdate(WorldTransform* worldTransform_);
//WorldTransformのとベクトルの掛け算
void worldTransformScale(Vector3* vector_, WorldTransform* worldTransform_);
void worldTransformMove(Vector3* vector_, WorldTransform* worldTransform_);
void worldTransformRoll(Vector3* vector_, WorldTransform* worldTransform_);
void vecWorldTransform(Vector3* vector_, WorldTransform* worldTransform_);

//ベクトルの長さを返す関数
float vector3Length(Vector3 vector_);
//正規化した値を返す関数
Vector3 vector3Normalize(Vector3 vector_);
