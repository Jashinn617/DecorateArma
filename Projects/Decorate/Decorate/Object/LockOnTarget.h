#pragma once
#include "DxLib.h"

/// <summary>
/// 敵のロックオン処理をするクラス
/// </summary>
class LockOnTarget
{
public:	// 関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	LockOnTarget();

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~LockOnTarget();

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="targetPos">ロックオンしたターゲット座標</param>
	void Update(VECTOR targetPos);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private:	// 変数
	int m_H;				// ロックオンしたときに出る画像のハンドル
	int m_graphWidth;		// 画像の横サイズ
	int m_graphHeight;		// 画像の縦サイズ
	VECTOR m_targetPos;		// ロックオンした敵の座標
};

