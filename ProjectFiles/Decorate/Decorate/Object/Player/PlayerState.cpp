#include "PlayerState.h"

#include "Player.h"

#include "../../Utility/Pad.h"

PlayerState::PlayerState(Player* player):
	m_isAction(false),
	m_isDodge(false),
	m_nowState(StateKind::Idle),
	m_pPlayer(player)
{
	/*処理無し*/
}

PlayerState::~PlayerState()
{
	/*処理無し*/
}

void PlayerState::Update()
{
	// 状態遷移
	StateTransition();

	// 走り、歩き状態でないときはサウンドを止める
}

void PlayerState::OnDamage()
{
	// 死亡状態のときは何もしない
	
	m_isAction = true;
	// ステイトの種類をダメージ状態にする
	ChangeState(StateKind::Damage);
}

void PlayerState::SetState(StateKind stateKind)
{
	for (int i = 0; i < static_cast<int>(StateKind::StateNum); i++)
	{
		// ステイトの種類が指定のステイトの場合
		if (static_cast<int>(stateKind) == i)
		{
			// 状態を変更する
			m_nowState = static_cast<StateKind>(i);
			return;
		}
	}
}

void PlayerState::EndState()
{
	// アクション状態の終了
	m_isAction = false;
	// 回避状態の終了
	m_isDodge = false;
}

void PlayerState::StateTransitionIdle()
{
	// アクション中の場合は何もしない
	if (m_isAction) return;

	// 移動ボタンが何も押されていなかった場合
	if (!Pad::IsPress(PAD_INPUT_LEFT) && !Pad::IsPress(PAD_INPUT_RIGHT) &&
		!Pad::IsPress(PAD_INPUT_UP) && !Pad::IsPress(PAD_INPUT_DOWN))
	{
		// ステイトを待機状態にする
		ChangeState(StateKind::Idle);
	}
}

void PlayerState::StateTransitionWalk()
{
	// アクション中の場合は何もしない
	if (m_isAction) return;

	// 移動ボタンが何かしら押されていた場合
	if (Pad::IsPress(PAD_INPUT_LEFT) || Pad::IsPress(PAD_INPUT_RIGHT) ||
		Pad::IsPress(PAD_INPUT_UP) || Pad::IsPress(PAD_INPUT_DOWN))
	{
		// ステイトを歩き状態にする
		ChangeState(StateKind::Walk);
	}
}

void PlayerState::StateTransitionDash()
{
	// アクション中の場合は何もしない
	if (m_isAction) return;

	//移動ボタンが押されていなかったら何もしない
	if (!Pad::IsPress(PAD_INPUT_LEFT) && !Pad::IsPress(PAD_INPUT_RIGHT) &&
		!Pad::IsPress(PAD_INPUT_UP) && !Pad::IsPress(PAD_INPUT_DOWN)) return;

	// RBが押されていた場合
	if (Pad::IsPress(PAD_INPUT_6))
	{
		// ステイトをダッシュ状態にする
		ChangeState(StateKind::Dash);
	}
}

void PlayerState::StateTransitionDodge()
{
	// 回避中の場合は何もしない
	if (m_isDodge) return;

	// Aボタンが押された場合
	if (Pad::IsTrigger(PAD_INPUT_1))
	{
		// アクションフラグを立てる
		m_isAction = true;
		// 回避フラグを立てる
		m_isDodge = true;
		// ステイトを回避状態にする
		ChangeState(StateKind::Dodge);
	}
}

void PlayerState::StateTransitionAttack()
{
	// アクション中の場合は何もしない
	if (m_isAction) return;

	// Xボタンが押された場合
	if (Pad::IsTrigger(PAD_INPUT_3))
	{
		// アクションフラグを立てる
		m_isAction = true;
		// ステイトを攻撃状態にする
		ChangeState(StateKind::Attack);
	}
}

void PlayerState::StateTransitionHardAttack()
{
	// アクション中の場合は何もしない
	if (m_isAction) return;

	// Yボタンが押された場合
	if (Pad::IsTrigger(PAD_INPUT_4))
	{
		// アクションフラグを立てる
		m_isAction = true;
		// ステイトを強攻撃状態にする
		ChangeState(StateKind::HardAttack);
	}
}

void PlayerState::StateTransition()
{
	// 状態遷移
	StateTransitionIdle();
	StateTransitionWalk();
	StateTransitionDash();
	StateTransitionDodge();
	StateTransitionAttack();
	StateTransitionHardAttack();
}

void PlayerState::ChangeState(StateKind stateKind)
{
	for (int i = 0; i < static_cast<int>(StateKind::StateNum); i++)
	{
		// ステイトの種類が指定のステイトの場合
		if (static_cast<int>(stateKind) == i)
		{
			// 現在のステイトを指定のステイトに変更
			m_nowState = static_cast<StateKind>(i);
			// ステイトの初期化
			m_pPlayer->InitState();
			return;
		}
	}
}