#include "HpBarBossEnemy.h"

#include "../../Utility/Time.h"

#include <cassert>

namespace
{
	const char* const kHpFileName = "Data/Image/HpBar/Enemy/Boss/Normal.png";				// 通常時HPバー画像ファイルパス
	const char* const kHpDecreaseFileName = "Data/Image/HpBar/Enemy/Boss/Decrease.png";	// 減少時HPバー画像ファイルパス
	const char* const kHpEmptyFileName = "Data/Image/HpBar/Enemy/Boss/Empty.png";			// 減少後HPバー画像ファイルパス
	const char* const kTextFileName = "Data/Image/HpBar/Enemy/Boss/Text.png";				// テキスト画像ファイルパス
	const char* const kFrameFileName = "Data/Image/HpBar/Enemy/Boss/Frame.png";			// フレーム画像ファイルパス

	constexpr int kHpBarDecrease = 1;			// HPバーの減少量
	constexpr int kHpBarDecreaseTime = 30;		// HPバーが減るまでにかかる時間
	constexpr int kHpBarPosX = 80;				// HPバー座標X
	constexpr int kHpBarPosY = 950;				// HPバー座標Y
	constexpr int kTextPosX = 80;				// テキスト座標X
	constexpr int kTextPosY = 870;				// テキスト座標Y
	constexpr int kFramePosX = 63;				// フレーム座標X
	constexpr int kFramePosY = 935;				// フレーム座標Y
}

HpBarBossEnemy::HpBarBossEnemy(const int& hp)
{
	// 体力設定
	m_hp = &hp;
	m_maxHp = *m_hp;
	m_decreaseHp = *m_hp;
	
	// 画像ロード
	m_hpH = LoadGraph(kHpFileName);
	// ロードに失敗したら止める
	assert(m_hpH != -1);
	// 以下同じ処理
	m_hpDecreaseH = LoadGraph(kHpDecreaseFileName);
	assert(m_hpDecreaseH != -1);
	m_hpEmptyH = LoadGraph(kHpEmptyFileName);
	assert(m_hpEmptyH != -1);
	m_textH = LoadGraph(kTextFileName);
	assert(m_textH != -1);
	m_frameH = LoadGraph(kFrameFileName);
	assert(m_frameH != -1);

	// 画像サイズの取得
	GetGraphSize(m_hpH, &m_graphSizeX, &m_graphSizeY);

	// １体力分画像の長さの計算
	m_oneHpLenght = static_cast<float>(m_graphSizeX) / static_cast<float>(m_maxHp);

	// 体力減少までにかかる時間の設定
	m_pDecreaseTime = std::make_shared<Time>(kHpBarDecreaseTime);
}

HpBarBossEnemy::~HpBarBossEnemy()
{
	// 画像のデリート
	DeleteGraph(m_hpH);
	DeleteGraph(m_hpDecreaseH);
	DeleteGraph(m_hpEmptyH);
	DeleteGraph(m_textH);
	DeleteGraph(m_frameH);
}

void HpBarBossEnemy::Update()
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

void HpBarBossEnemy::Draw()
{
	

	// 減少後画像描画
	DrawRectGraph(kHpBarPosX, kHpBarPosY,	// 座標
		0, 0,								// 描画する画像の左上座標
		m_graphSizeX, m_graphSizeY,			// 画像サイズ
		m_hpEmptyH,							// 画像ハンドル
		true,								// 透過するかどうか
		false);								// 反転するかどうか

	// 減少時画像描画
	DrawRectGraph(kHpBarPosX, kHpBarPosY,								// 座標
		0, 0,															// 描画する画像の左上座標
		static_cast<int>(m_oneHpLenght * m_decreaseHp), m_graphSizeY,	// 画像サイズ
		m_hpDecreaseH,													// 画像ハンドル
		true,															// 透過するかどうか
		false);															// 反転するかどうか

	// 通常時画像描画
	DrawRectGraph(kHpBarPosX, kHpBarPosY,						// 座標
		0, 0,													// 描画する画像の左上座標
		static_cast<int>(m_oneHpLenght * *m_hp), m_graphSizeY,	// 画像サイズ
		m_hpH,													// 画像ハンドル
		true,													// 透過するかどうか
		false);

	// フレーム描画
	DrawGraph(kFramePosX, kFramePosY, m_frameH, true);

	// テキスト描画
	DrawGraph(kTextPosX, kTextPosY, m_textH, true);
}
