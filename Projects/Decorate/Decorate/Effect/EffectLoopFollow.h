#pragma once
#include "Effekseer3DManager.h"

/// <summary>
/// 追従とループをするエフェクト
/// </summary>
class EffectLoopFollow : public Effekseer3DManager::EffectBase
{
public:	// 関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="FileName">ファイルパス</param>
	/// <param name="pObject">オブジェクト</param>
	EffectLoopFollow(const char* fileName, ObjectBase* pObj);

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~EffectLoopFollow();

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override final;

	/// <summary>
	/// 再生
	/// </summary>
	void Play() override final;

private:	// 変数
	ObjectBase* m_pObj;
};

