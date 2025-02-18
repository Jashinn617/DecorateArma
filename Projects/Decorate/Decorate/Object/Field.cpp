#include "Field.h"
#include "Model.h"
#include "ObjectManager.h"
#include "Player/Player.h"

#include "../Shader/ToonShader.h"

#include <string>
#include <cassert>

namespace
{
	constexpr int kStageNum = 2;

	const std::string kFieldModelPath = "Data/Model/Field/";		// フィールドモデルまでのパス
	// ステージによってフィールドモデルを変える
	const std::string kStageModelPath[kStageNum] =
	{
		"Test/Test.mv1",
		"Stage1/Stage1.mv1",
	};
}

Field::Field()
{
}

Field::Field(Game::StageKind stageKind)
{
	// モデルのパス指定
	std::string modelPath = kFieldModelPath + kStageModelPath[static_cast<int>(stageKind)];
	// モデル生成
	m_pModel = std::make_shared<Model>(modelPath.c_str());

	m_pModel->SetScale(VGet(0.4f, 0.4f, 0.4f));
	m_pModel->SetPos(VGet(0.0f, -300.0f, 0.0f));

	// モデルの頂点タイプの取得
	for (int i = 0; i < MV1GetTriangleListNum(m_pModel->GetModelHandle()); i++)
	{
		// 頂点タイプの取得
		m_vertexShaderType.push_back(MV1GetTriangleListVertexType(m_pModel->GetModelHandle(), i));
	}
}

Field::~Field()
{
}

void Field::Update()
{
	m_pModel->Update();
}

void Field::Draw(std::shared_ptr<ToonShader> pToonShader)
{
	// モデルをフレームごとに描画する
	for (int i = 0; i < MV1GetTriangleListNum(m_pModel->GetModelHandle()); i++)
	{
		// シェーダの設定
		pToonShader->SetShaderField(m_vertexShaderType[i]);

		// 描画
		MV1DrawTriangleList(m_pModel->GetModelHandle(), i);
	}
	// シェーダを使わない設定にする
	pToonShader->ShaderEnd();
}

void Field::ShadowMapDraw(std::shared_ptr<ShadowMapShader> pShadoeMapShader)
{
	// 何もしない描画
	m_pModel->Draw();
}
