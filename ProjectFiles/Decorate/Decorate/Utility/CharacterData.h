#pragma once
#include "DxLib.h"

/// <summary>
/// キャラクターの持っている情報
/// </summary>
namespace CharacterData
{
	/// <summary>
	/// キャラクター情報
	/// </summary>
	struct CharacterInfo
	{
		VECTOR pos;			// 座標
		VECTOR topPos;		// 頂点座標
		VECTOR bottomPos;	// 足元座標
		VECTOR vec;			// 移動ベクトル
		VECTOR rot;			// 回転
		int modelH;			// モデルハンドル
		int isExist;		// 存在しているかどうか
	};
}