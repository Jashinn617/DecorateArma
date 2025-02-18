#pragma once
#include "Effekseer3DManager.h"

/// <summary>
/// 通常再生のエフェクト
/// </summary>
class EffectNormal : public Effekseer3DManager::EffectBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="fileName">ファイルパス</param>
	/// <param name="pos">座標</param>
	/// <param name="rot">角度</param>
	EffectNormal(const char* fileName, VECTOR pos, VECTOR rot = { 0.0f,0.0f,0.0f });

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~EffectNormal();

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override final;

	/// <summary>
	/// 再生
	/// </summary>
	void Play() override final;

private:	// 変数
	VECTOR m_pos;	// 座標
	VECTOR m_rot;	// 回転
};

