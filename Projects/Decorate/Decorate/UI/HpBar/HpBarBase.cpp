#include "HpBarBase.h"

void HpBarBase::OnRecoveryHp(int recoveryNum)
{
	m_decreaseHp += recoveryNum;
}
