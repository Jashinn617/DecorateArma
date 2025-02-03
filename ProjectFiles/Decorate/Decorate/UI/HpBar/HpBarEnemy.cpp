#include "HpBarEnemy.h"

#include "../../Utility/Time.h"

#include <cassert>

namespace
{
	const char* const kHpFileName = "Data/Image/HpBar/Enemy/UI-05.png";			// 通常時HPバー画像ファイルパス
	const char* const kHpDecreaseFineName = "Data/Image/HpBar/Enemy/UI-7.png";	// 減少時HPバー画像ファイルパス
	const char* const kHpEmptyFineName = "Data/Image/HpBar/Enemy/UI-9.png";		// 減少後HPバー画像ファイルパス

	constexpr int kHpBarDecrease = 1;			// HPバーの減少量
	constexpr int kHpBarDecreaseTime = 30;		// HPバーが減るまでにかかる時間
	constexpr int kHpBarAdjustmentSizeX = -700;	// HPバーのサイズ調整X
	constexpr int kHpBarAdjustmentSizeY = -200;	// HPバーのサイズ調整Y
	constexpr float kHpBarHeight = 175.0f;		// HPバーを表示する高さ
}

HpBarEnemy::HpBarEnemy(const int& hp, const VECTOR& pos):
	m_pos(&pos)
{
	// 体力設定
	m_hp = &hp;
	m_maxHp = *m_hp;
	m_decreaseHp = *m_hp;

	// 画像ロード
	m_hpH = LoadGraph(kHpFileName);
	// ロードに失敗したら止める
	assert(m_hpH != -1);
	// 以下同文
	m_hpDecreaseH = LoadGraph(kHpDecreaseFineName);
	assert(m_hpDecreaseH != -1);
	m_hpEmptyH = LoadGraph(kHpEmptyFineName);
	assert(m_hpEmptyH != -1);

	// 画像サイズの取得
	GetGraphSize(m_hpH, &m_graphSizeX, &m_graphSizeY);
	// 画像サイズの調整
	m_graphSizeX += kHpBarAdjustmentSizeX;
	m_graphSizeY += kHpBarAdjustmentSizeY;

	// １体力分画像の長さの計算
	m_oneHpLenght = static_cast<float>(m_graphSizeX) / static_cast<float>(m_maxHp);

	// 体力減少までにかかる時間の設定
	m_pDecreaseTime = std::make_shared<Time>(kHpBarDecreaseTime);
}

HpBarEnemy::~HpBarEnemy()
{
	// 画像のデリート
	DeleteGraph(m_hpH);
	DeleteGraph(m_hpDecreaseH);
	DeleteGraph(m_hpEmptyH);
}

void HpBarEnemy::Update()
{
	// 体力が減少値以上だったら何もしない
	if (*m_hp >= m_decreaseHp) return;
	// 体力減少時間が経過していなかったら何もしない
	if (!m_pDecreaseTime->Update()) return;

	// 体力減少
	// 少しずつHPバーの値を減らしていく
	m_decreaseHp = max(m_decreaseHp - kHpBarDecrease, *m_hp);

	// 体力が減少値以上だったら体力と同じ値にする
	if (*m_hp <= m_decreaseHp)
	{
		m_decreaseHp = *m_hp;
		// 体力減少時間のリセット
		m_pDecreaseTime->Reset();
	}
}

void HpBarEnemy::Draw()
{
	// 座標計算
	VECTOR pos = VGet(m_pos->x, m_pos->y + kHpBarHeight, m_pos->z);

	// 減少後画像描画
	DrawModiBillboard3D(pos,									// 描画座標
		-m_graphSizeX * 0.5f, 0.0f,								// 画像左上座標
		m_graphSizeX * 0.5f, 0.0f,								// 画像右上座標
		m_graphSizeX * 0.5f, static_cast<float>(m_graphSizeY),	// 画像右下座標
		-m_graphSizeX * 0.5f, static_cast<float>(m_graphSizeY),	// 画像左下座標
		m_hpEmptyH,												// 画像ハンドル
		true);													// 透過するかどうか

	// 減少時画像描画
	DrawModiBillboard3D(pos,																		// 描画座標
		-m_graphSizeX * 0.5f, 0.0f,																	// 画像左上座標
		(m_oneHpLenght * m_decreaseHp) - (m_graphSizeX * 0.5f), 0.0f,								// 画像右上座標
		(m_oneHpLenght * m_decreaseHp) - (m_graphSizeX * 0.5f), static_cast<float>(m_graphSizeY),	// 画像右下座標
		-m_graphSizeX * 0.5f, static_cast<float>(m_graphSizeY),										// 画像左下座標
		m_hpDecreaseH,																				// 画像ハンドル
		true);																						// 透過するかどうか

	// 通常時画像描画
	DrawModiBillboard3D(pos,																// 描画座標
		-m_graphSizeX * 0.5f, 0.0f,															// 画像左上座標
		(m_oneHpLenght * *m_hp) - (m_graphSizeX * 0.5f), 0.0f,								// 画像右上座標
		(m_oneHpLenght * *m_hp) - (m_graphSizeX * 0.5f), static_cast<float>(m_graphSizeY),	// 画像右下座標
		-m_graphSizeX * 0.5f, static_cast<float>(m_graphSizeY),								// 画像左下座標
		m_hpH,																				// 画像ハンドル
		true);																				// 透過するかどうか
}