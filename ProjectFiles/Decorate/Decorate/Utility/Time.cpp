#include "Time.h"

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
