#pragma once
#include "DxLib.h"

/// <summary>
/// 移動方向ベクトルを取得するクラス
/// </summary>
class MoveDirectionVec
{
public:		// 関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	MoveDirectionVec();

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~MoveDirectionVec();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 移動方向の取得
	/// </summary>
	/// <returns>移動方向</returns>
	VECTOR GetDirectionVec()const { return m_direction; }

private:	// 変数
	VECTOR m_direction;		// 方向

};

