#pragma once
#include "Effekseer3DManager.h"

/// <summary>
/// ループをするエフェクト
/// </summary>
class EffectLoop : public Effekseer3DManager::EffectBase
{
public:		// 関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="fileName">ファイルパス</param>
	/// <param name="pos">座標</param>
	/// <param name="rot">角度</param>
	EffectLoop(const char* fileName, VECTOR pos, VECTOR rot = { 0.0f,0.0f,0.0f });

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~EffectLoop();

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

