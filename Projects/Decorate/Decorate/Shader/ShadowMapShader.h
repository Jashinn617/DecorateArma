#pragma once

#include "DxLib.h"

class ShadowMapShader
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	ShadowMapShader();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~ShadowMapShader();

	/// <summary>
	/// シャドウマップへの書き込み開始処理
	/// </summary>
	/// <param name="targetPos">カメラターゲット座標</param>
	void WriteStart(VECTOR cameraTargetPos);

	/// <summary>
	/// シャドウマップのカメラから見たターゲットを設定する
	/// </summary>
	/// <param name="targetPos">カメラターゲット座標</param>
	void SetShadowMapCameraTarget(VECTOR cameraTargetPos);

	/// <summary>
	/// シェーダの設定
	/// </summary>
	/// <param name="shaderType">頂点タイプ</param>
	void SetShader(int shaderType);

	/// <summary>
	/// フィールドのシェーダの設定
	/// </summary>
	/// <param name="targetPos">カメラターゲット座標</param>
	void SetShaderField(VECTOR cameraTargetPos);

	/// <summary>
	/// シャドウマップへの書き込み終了処理
	/// </summary>
	void WriteEnd();

	/// <summary>
	/// シェーダを使わない設定にする
	/// </summary>
	void ShaderEnd();

private:	// 関数
	/// <summary>
	/// ビュープロジェクション行列を作るための関数
	/// </summary>
	/// <param name="targetPos">カメラターゲット座標</param>
	/// <returns>ビュープロジェクション行列</returns>
	MATRIX ViewProjectionMatrix(VECTOR targetPos);

private:	// 変数
	/*頂点シェーダ*/
	int m_vertexShader1FrameH;				//頂点シェーダー1フレームハンドル
	int m_vertexShader4FrameH;				//頂点シェーダー4フレームハンドル
	int m_vertexShader8FrameH;				//頂点シェーダー8フレームハンドル
	int m_vertexShaderNormal4FrameH;		//頂点シェーダー4フレーム法線マップありハンドル
	int m_vertexShaderNormal8FrameH;		//頂点シェーダー8フレーム法線マップありハンドル
	int m_vertexShaderShadowMapFieldH;		// 頂点シェーダのシャドウマップをフィールドに貼り付ける為の頂点シェーダハンドル

	/*ピクセルシェーダ*/
	int m_pixelShaderH;				// ピクセルシェーダのハンドル
	int m_pixelShaderFieldH;		// ピクセルシェーダのフィールドに適用するハンドル

	/*定数バッファのハンドル*/
	int m_cbufferLightDirH;				// ライトの向き
	int m_cbufferCameraTargetPosH;		// カメラのターゲット座標
	int m_cbufferCameraPosH;			// カメラ座標
	int m_cbufferViewProjectionMatH;	// ビュープロジェクション行列

	/*定数バッファをいじるためのアドレスのコピー*/
	VECTOR* m_pCbufferLightDir;				// ライトの向き
	VECTOR* m_pCbuffferCameraTargetPos;		// カメラのターゲットの位置
	VECTOR* m_pCbufferCameraPos;			// カメラの位置

	/// <summary>
	/// ビュープロジェクション行列
	/// シャドウマップテクスチャをフィールドモデルに
	///	貼り付ける際に座標の変換を行うために必要な行列
	/// </summary>
	MATRIX* m_viewProjectionMat;
	int m_shadowMap;	// シャドウマップ用の変数
};