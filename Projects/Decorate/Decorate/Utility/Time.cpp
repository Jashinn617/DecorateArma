#include "Time.h"
#include "DxLib.h"

namespace
{
	constexpr int kDebugTimePosX = 1000;	// デバッグ用時間の座標X
	constexpr int kDebugTimePosY = 60;		// デバッグ用時間の座標Y
}

Time::Time(int rimitTime):
	m_time(0),
	m_rimitTime(rimitTime)
{
	/*処理無し*/
}

Time::~Time()
{
	/*処理無し*/
}

void Time::Reset()
{
	m_time = 0;
}

bool Time::Update()
{
	// リミット時間を超えていた場合はtrue
	if (m_time >= m_rimitTime)return true;
	m_time++;

	return false;
}

void Time::DebugDraw()
{
	// デバッグ用
#ifdef _DEBUG
	DrawFormatString(kDebugTimePosX, kDebugTimePosY, 0xffffff, "%d", m_time);
#endif // _DEBUG
}
