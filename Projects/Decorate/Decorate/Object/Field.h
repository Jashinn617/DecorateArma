#pragma once
#include "ObjectBase.h"
#include "../Utility/Game.h"

class Model;

class Field : public ObjectBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Field();

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="stage">ステージの種類</param>
	Field(Game::StageKind stageKind);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Field();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init() override final {/*処理無し*/ };

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override final;

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="pToonShader">トゥーンシェーダ</param>
	virtual void Draw(std::shared_ptr<ToonShader> pToonShader) override final;

	/// <summary>
	/// シャドウマップ描画
	/// </summary>
	/// <param name="pShadoeMapShader">シャドウマップシェーダ</param>
	virtual void ShadowMapDraw(std::shared_ptr<ShadowMapShader> pShadoeMapShader) override final;

	/// <summary>
	/// 2D描画
	/// </summary>
	virtual void Draw2D() override final {/*処理無し*/ };

	/// <summary>
	/// ステージクリア時処理
	/// </summary>
	virtual void StageClear() override final {/*処理無し*/ };

	/// <summary>
	/// 自身の当たり判定情報
	/// </summary>
	/// <returns>当たり判定の種類</returns>
	virtual ColType GetColType() const override final { return ColType::Field; }
};

