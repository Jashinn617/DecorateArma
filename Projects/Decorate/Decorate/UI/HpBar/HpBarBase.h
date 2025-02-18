#pragma once
#include "DxLib.h"

#include <memory>

class Time;

/// <summary>
/// HPバーの基底クラス
/// </summary>
class HpBarBase
{
public:	// 関数
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~HpBarBase() {/*処理無し*/ };

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// 体力回復時の処理
	/// </summary>
	/// <param name="recoveryNum">回復値</param>
	virtual void OnRecoveryHp(int recoveryNum);

protected:	// 変数
	const int* m_hp = 0;					// 体力
	int m_maxHp = 0;						// 最大体力
	int m_hpH;								// 通常時HPバー画像ハンドル
	int m_hpDecreaseH;						// 減少時HPバー画像ハンドル
	int m_hpEmptyH;							// 減少後HPバー画像ハンドル
	int m_graphSizeX;						// 画像Xサイズ
	int m_graphSizeY;						// 画像Yサイズ
	int m_decreaseHp;						// 体力減少値
	float m_oneHpLenght;					// １体力分の画像の長さ
	std::shared_ptr<Time> m_pDecreaseTime;	// 体力減少にかかる時間
};

