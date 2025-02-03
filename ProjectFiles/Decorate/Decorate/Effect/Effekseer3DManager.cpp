#include "Effekseer3DManager.h"

#include "EffekseerForDXLib.h"

#include "EffectLoopFollow.h"
#include "EffectLoop.h"
#include "EffectFollow.h"
#include "EffectNormal.h"

#include "../Object/ObjectBase.h"

Effekseer3DManager::Effekseer3DManager()
{
	CsvLoad::GetInstance().EffectLoad(m_data);
}

Effekseer3DManager::~Effekseer3DManager()
{
	/*処理無し*/
}

Effekseer3DManager& Effekseer3DManager::GetInstance()
{
	// 唯一の実態
	static Effekseer3DManager instance;

	return instance;
}

void Effekseer3DManager::Update()
{
	// 更新
	UpdateEffekseer3D();
}

void Effekseer3DManager::Draw()
{
	Effekseer_Sync3DSetting();

	// 再生中のエフェクトの描画
	int result = 0;
	result = DrawEffekseer3D();
}

void Effekseer3DManager::Add(const char* fileName, PlayType type, ObjectBase* pObject, VECTOR pos, VECTOR rot)
{
	switch (type)
	{
	case Effekseer3DManager::PlayType::LoopFollow:
		m_pEffect.push_back(std::make_shared<EffectLoopFollow>(fileName, pObject));
		break;
	case Effekseer3DManager::PlayType::Loop:
		m_pEffect.push_back(std::make_shared<EffectLoop>(fileName, pos, rot));
		break;
	case Effekseer3DManager::PlayType::Flollw:
		m_pEffect.push_back(std::make_shared<EffectFollow>(fileName, pObject));
		break;
	case Effekseer3DManager::PlayType::Normal:
		m_pEffect.push_back(std::make_shared<EffectNormal>(fileName, pos, rot));
		break;
	default:
		break;
	}
}

void Effekseer3DManager::Delete()
{
	// 再生中のエフェクトを全て削除する
	m_pEffect.clear();
}