#include "LockOnTarget.h"

#include <string>
#include <cassert>

namespace 
{
	const char* const kFileName = "Data/Image/GameScene/TargetLogo.png";	// 画像ファイルパス
	constexpr float kScale = 0.3f;								// 画像スケール
	constexpr float kHeight = 100.0f;							// 高さ調整

}

LockOnTarget::LockOnTarget():
	m_H(-1),
	m_graphWidth(0),
	m_graphHeight(0),
	m_targetPos{0.0f,0.0f,0.0f}
{
	// 画像のロード
	m_H = LoadGraph(kFileName);
	// 画像が入ってなかったら止める
	assert(m_H != -1);
	// 画像サイズの取得
	GetGraphSize(m_H, &m_graphWidth, &m_graphHeight);
}

LockOnTarget::~LockOnTarget()
{
	// 画像デリート
	DeleteGraph(m_H);
}

void LockOnTarget::Update(VECTOR targetPos)
{
	// ターゲット座標取得
	m_targetPos = targetPos;
}

void LockOnTarget::Draw()
{
	VECTOR pos = VGet(m_targetPos.x, m_targetPos.y + kHeight, m_targetPos.z);

	// サイズの設定
	int graphWidth = static_cast<int>(m_graphWidth * kScale);
	int graphHeight = static_cast<int>(m_graphHeight * kScale);

	// 画像描画
	DrawModiBillboard3D(pos,						// 画像座標
		-graphWidth * 0.5f, graphHeight * 0.5f,		// 画像左上座標
		graphWidth * 0.5f, graphHeight * 0.5f,		// 画像右上座標
		graphWidth * 0.5f, -graphHeight * 0.5f,		// 画像右下座標
		-graphWidth * 0.5f, -graphHeight * 0.5f,	// 画像左下座標
		m_H,											// 画像ハンドル
		true);											// 反転するかどうか	
}