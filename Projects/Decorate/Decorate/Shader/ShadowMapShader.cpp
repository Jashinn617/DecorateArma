#include "ShadowMapShader.h"

#include "../Utility/Game.h"

#include <cassert>

namespace
{
	constexpr int kShadowMapSizeX = 15360;		// シャドウマップのサイズX
	constexpr int kShadowMapSizeY = 8640;		// シャドウマップのサイズY

	// カメラの視野角
	constexpr float kShadowMapFov = 45.0f * DX_PI_F / 180.0f;	// シャドウマップのカメラの視野角
	constexpr float kNormalFov = 60.0f * DX_PI_F / 180.0f;		// 通常時のカメラの視野角
}

ShadowMapShader::ShadowMapShader()
{
	// シェーダのロード
	// ロードに失敗したら止める
	m_pixelShaderH = LoadPixelShader("Data/Shader/PixelShader/PixelShaderShadow.pso");
	assert(m_pixelShaderH != -1);
	m_pixelShaderFieldH = LoadPixelShader("Data/Shader/PixelShader/ToonFieldPixelShader.pso");
	assert(m_pixelShaderFieldH != -1);
	// 頂点シェーダ
	m_vertexShader1FrameH = LoadVertexShader("Data/Shader/VertexShader/VertexShader3D.vso");
	assert(m_vertexShader1FrameH != -1);
	m_vertexShader4FrameH = LoadVertexShader("Data/Shader/VertexShader/VertexShader3D4Frame.vso");
	assert(m_vertexShader4FrameH != -1);
	m_vertexShader8FrameH = LoadVertexShader("Data/Shader/VertexShader/VertexShader3D8Frame.vso");
	assert(m_vertexShader8FrameH != -1);
	m_vertexShaderNormal4FrameH = LoadVertexShader("Data/Shader/VertexShader/VertexShaderNormal4Frame.vso");
	assert(m_vertexShaderNormal4FrameH != -1);
	m_vertexShaderNormal8FrameH = LoadVertexShader("Data/Shader/VertexShader/VertexShaderNormal8Frame.vso");
	assert(m_vertexShaderNormal8FrameH != -1);
	m_vertexShaderShadowMapFieldH = LoadVertexShader("Data/Shader/VertexShader/VertexShaderShadowMap.vso");
	assert(m_vertexShaderShadowMapFieldH != -1);

	/*ライトの定数バッファの作成*/
	m_cbufferLightDirH = CreateShaderConstantBuffer(sizeof(float) * 4);
	assert(m_cbufferLightDirH != -1);
	// アドレスの取得
	m_pCbufferLightDir = static_cast<VECTOR*>(GetBufferShaderConstantBuffer(m_cbufferLightDirH));

	/*カメラのターゲットの位置の定数バッファの作成*/
	m_cbufferCameraTargetPosH = CreateShaderConstantBuffer(sizeof(float) * 4);
	assert(m_cbufferCameraTargetPosH != -1);
	// アドレスの取得
	m_pCbuffferCameraTargetPos = static_cast<VECTOR*>(GetBufferShaderConstantBuffer(m_cbufferCameraTargetPosH));

	/*カメラの位置の定数バッファの作成*/
	m_cbufferCameraPosH = CreateShaderConstantBuffer(sizeof(float) * 4);
	assert(m_cbufferCameraPosH != -1);
	m_pCbufferCameraPos = static_cast<VECTOR*>(GetBufferShaderConstantBuffer(m_cbufferCameraPosH));

	// ビュープロジェクション行列の定数バッファの作成
	m_cbufferViewProjectionMatH = CreateShaderConstantBuffer(sizeof(MATRIX));
	assert(m_cbufferViewProjectionMatH != -1);
	// アドレスの取得
	m_viewProjectionMat = static_cast<MATRIX*>(GetBufferShaderConstantBuffer(m_cbufferViewProjectionMatH));

	// シャドウマップの作成
	m_shadowMap = MakeScreen(kShadowMapSizeX, kShadowMapSizeY);
	assert(m_shadowMap != -1);
}

ShadowMapShader::~ShadowMapShader()
{
	/*頂点シェーダのデリート*/
	DeleteShader(m_vertexShader1FrameH);
	DeleteShader(m_vertexShader4FrameH);
	DeleteShader(m_vertexShader8FrameH);
	DeleteShader(m_vertexShaderNormal4FrameH);
	DeleteShader(m_vertexShaderNormal8FrameH);
	DeleteShader(m_vertexShaderShadowMapFieldH);
	/*ピクセルシェーダのデリート*/
	DeleteShader(m_pixelShaderH);
	DeleteShader(m_pixelShaderFieldH);
	DeleteShader(m_shadowMap);
	/*定数バッファのデリート*/
	DeleteShaderConstantBuffer(m_cbufferLightDirH);
	DeleteShaderConstantBuffer(m_cbufferCameraTargetPosH);
	DeleteShaderConstantBuffer(m_cbufferCameraPosH);
	DeleteShaderConstantBuffer(m_cbufferViewProjectionMatH);
}

void ShadowMapShader::WriteStart(VECTOR cameraTargetPos)
{
	// シャドウマップへの書き込みの開始
	SetDrawScreen(m_shadowMap);
	ClearDrawScreen();
	DrawBox(0, 0, kShadowMapSizeX, kShadowMapSizeY, 0xffffff, true);

	// シャドウマップのカメラの位置と向きを設定する
	SetCameraPositionAndTargetAndUpVec(GetLightPosition(), cameraTargetPos, VGet(1, 0, 0));

	// シャドウマップ書き込み時のカメラの視野角
	// ClearDrawScreenをしたときにクリアしたものを再設定している
	SetupCamera_Perspective(kShadowMapFov);
}

void ShadowMapShader::SetShadowMapCameraTarget(VECTOR cameraTargetPos)
{
	// カメラ座標の取得
	m_pCbufferCameraPos[0] = cameraTargetPos;
}

void ShadowMapShader::SetShader(int shaderType)
{
	// ライトの向きの取得
	m_pCbufferLightDir[0] = GetLightDirection();
	// カメラのターゲットの位置の取得
	m_pCbuffferCameraTargetPos[0] = GetCameraTarget();

	/*書き込んだ値をGPUに反映する*/
	// ライトの向き
	UpdateShaderConstantBuffer(m_cbufferLightDirH);
	SetShaderConstantBuffer(m_cbufferLightDirH, DX_SHADERTYPE_PIXEL, 0);
	// カメラのターゲットの位置
	UpdateShaderConstantBuffer(m_cbufferCameraTargetPosH);
	SetShaderConstantBuffer(m_cbufferCameraTargetPosH, DX_SHADERTYPE_PIXEL, 1);
	// カメラの位置
	UpdateShaderConstantBuffer(m_cbufferCameraPosH);
	SetShaderConstantBuffer(m_cbufferCameraPosH, DX_SHADERTYPE_PIXEL, 2);

	// シェーダを適用する
	MV1SetUseOrigShader(true);

	//モデルのタイプによって適応させるシェーダーを変える
	if (shaderType == DX_MV1_VERTEX_TYPE_1FRAME)
	{
		SetUseVertexShader(m_vertexShader1FrameH);
	}
	else if (shaderType == DX_MV1_VERTEX_TYPE_4FRAME)
	{
		SetUseVertexShader(m_vertexShader4FrameH);
	}
	else if (shaderType == DX_MV1_VERTEX_TYPE_8FRAME)
	{
		SetUseVertexShader(m_vertexShader8FrameH);
	}
	else if (shaderType == DX_MV1_VERTEX_TYPE_NMAP_1FRAME)
	{
		SetUseVertexShader(m_vertexShader1FrameH);
	}
	else if (shaderType == DX_MV1_VERTEX_TYPE_NMAP_4FRAME)
	{
		SetUseVertexShader(m_vertexShaderNormal4FrameH);
	}
	else if (shaderType == DX_MV1_VERTEX_TYPE_NMAP_8FRAME)
	{
		SetUseVertexShader(m_vertexShaderNormal8FrameH);
	}
	else if (shaderType == DX_MV1_VERTEX_TYPE_NMAP_FREE_FRAME)
	{
		SetUseVertexShader(m_vertexShader1FrameH);
	}
	else
	{
		MV1SetUseOrigShader(false);
	}

	// ピクセルシェーダのセット
	SetUsePixelShader(m_pixelShaderH);
}

void ShadowMapShader::SetShaderField(VECTOR cameraTargetPos)
{
	// ビュープロジェクション行列の取得
	*m_viewProjectionMat = ViewProjectionMatrix(cameraTargetPos);

	// ライトの向きの取得
	m_pCbufferLightDir[0] = GetLightDirection();
	// カメラのターゲットの位置の取得
	m_pCbuffferCameraTargetPos[0] = GetCameraTarget();
	// カメラの位置の取得
	m_pCbufferCameraPos[0] = GetCameraPosition();

	// シェーダ使用時のグラフィックハンドルの設定
	SetUseTextureToShader(1, m_shadowMap);

	/*書き込んだ値をGPUに反映する*/
	// ライトの向き
	UpdateShaderConstantBuffer(m_cbufferLightDirH);
	SetShaderConstantBuffer(m_cbufferLightDirH, DX_SHADERTYPE_PIXEL, 0);
	// カメラのターゲットの位置
	UpdateShaderConstantBuffer(m_cbufferCameraTargetPosH);
	SetShaderConstantBuffer(m_cbufferCameraTargetPosH, DX_SHADERTYPE_PIXEL, 1);
	// カメラの位置
	UpdateShaderConstantBuffer(m_cbufferCameraPosH);
	SetShaderConstantBuffer(m_cbufferCameraPosH, DX_SHADERTYPE_PIXEL, 2);
	// ビュープロジェクション行列
	UpdateShaderConstantBuffer(m_cbufferViewProjectionMatH);
	SetShaderConstantBuffer(m_cbufferViewProjectionMatH, DX_SHADERTYPE_VERTEX, 4);

	// シェーダの適用
	MV1SetUseOrigShader(true);
	// 頂点シェーダの設定
	SetUseVertexShader(m_vertexShaderShadowMapFieldH);
	// ピクセルシェーダの設定
	SetUsePixelShader(m_pixelShaderFieldH);
}

void ShadowMapShader::WriteEnd()
{
	// シャドウマップへの書き込みを終わる
	SetDrawScreen(DX_SCREEN_BACK);
	MV1SetUseOrigShader(false);
	// 通常時のカメラの視野角
	SetupCamera_Perspective(kNormalFov);
}

void ShadowMapShader::ShaderEnd()
{
	/*シェーダを使わない*/
	MV1SetUseOrigShader(false);
}

MATRIX ShadowMapShader::ViewProjectionMatrix(VECTOR targetPos)
{
	// ビュー行列
	MATRIX viewMat = GetCameraViewportMatrix();

	VECTOR cameraPos = GetLightPosition();
	VECTOR cameraTargetPos = targetPos;
	VECTOR UpVec = VGet(1, 0, 0);

	// ビュー行列の計算
	CreateLookAtMatrix(&viewMat, &cameraPos, &cameraTargetPos, &UpVec);
	// 射影行列の作成
	MATRIX mat;

	// 射影行列の計算
	CreatePerspectiveFovMatrix(&mat, kShadowMapFov, GetCameraNear(), GetCameraFar(), 9.0f / 16.0f);

	// ビュープロジェクション行列の作成
	MATRIX viewProjectionMat = MMult(viewMat, mat);

	return viewProjectionMat;
}
