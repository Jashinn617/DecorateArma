#pragma once
#include "DxLib.h"

#include <memory>

class Player;

/// <summary>
/// プレイヤーのステイトを管理するクラス
/// </summary>
class PlayerState
{
public:	// 列挙型

	/// <summary>
	/// ステイトの種類
	/// </summary>
	enum class StateKind
	{
		Idle,			// 待機
		Walk,			// 歩き
		Dash,			// ダッシュ
		Attack,			// 攻撃
		HardAttack,		// 強攻撃
		Dodge,			// 回避
		Damage,			// ダメージ
		StateNum,		// ステイト数
	};

public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="pPlayer">プレイヤーポインタ</param>
	PlayerState(Player* pPlayer);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~PlayerState();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// ダメージ処理
	/// </summary>
	void OnDamage();

	/// <summary>
	/// ステイトの設定
	/// </summary>
	/// <param name="stateKind"></param>
	void SetState(StateKind stateKind);

	/// <summary>
	/// ステイトの終了処理
	/// </summary>
	void EndState();

	/// <summary>
	/// 現在のステイトの取得
	/// </summary>
	/// <returns>現在の状態</returns>
	StateKind GetState()const { return m_nowState; }

private:	// 関数
	/// <summary>
	/// 待機状態への変更
	/// </summary>
	void StateTransitionIdle();

	/// <summary>
	/// 歩き状態への変更
	/// </summary>
	void StateTransitionWalk();

	/// <summary>
	/// ダッシュ状態への変更
	/// </summary>
	void StateTransitionDash();

	/// <summary>
	/// 回避状態への変更
	/// </summary>
	void StateTransitionDodge();

	/// <summary>
	/// 攻撃状態への変更
	/// </summary>
	void StateTransitionAttack();

	/// <summary>
	/// 強攻撃状態への変更
	/// </summary>
	void StateTransitionHardAttack();

	/// <summary>
	/// 各ステイトの遷移
	/// </summary>
	void StateTransition();

	/// <summary>
	/// ステイト変更
	/// </summary>
	/// <param name="state">ステイト</param>
	void ChangeState(StateKind stateKind);

private:	// 変数
	bool m_isAction;		// アクション行動中か
	bool m_isDodge;			// 回避状態か
	StateKind m_nowState;	// 現在のステイト
	Player* m_pPlayer;		// プレイヤーポインタ
};