#include "MoveDirectionVec.h"

#include "Pad.h"
#include "CharacterData.h"

namespace
{
	constexpr float kUp = -1.0f;		// 上方向移動ベクトル
	constexpr float kDown = 1.0f;		// 下方向移動ベクトル
	constexpr float kRight = 1.0f;		// 右方向移動ベクトル
	constexpr float kLeft = -1.0f;		// 左方向移動ベクトル
}

MoveDirectionVec::MoveDirectionVec() :
	m_direction{ 0.0f,0.0f,0.0f }
{
	/*処理無し*/
}

MoveDirectionVec::~MoveDirectionVec()
{
	/*処理無し*/
}

void MoveDirectionVec::Update()
{
	// パッドアナログ情報取得
	DINPUT_JOYSTATE input;

	input.X = 0;
	input.Y = 0;

	// 移動方向
	float moveAngle = 0.0f;

	// 入力状態の取得
	GetJoypadDirectInputState(DX_INPUT_PAD1, &input);

	// 移動ベクトルの初期化
	m_direction = VGet(0.0f, 0.0f, 0.0f);

	// スティックでのアナログ入力状態が反応しているときはそっちを優先する
	if (input.X != 0 || input.Y != 0)
	{
		m_direction.x = 0.001f * input.X;
		m_direction.z = 0.001f * input.Y;
	}
	else
	{
		// 左ボタンを押されたらプレイヤーの向きを左に
		if (Pad::IsPress(PAD_INPUT_LEFT))
		{
			m_direction.x = kLeft;
		}
		// 右ボタンを押されたらプレイヤーの向き右に
		else if (Pad::IsPress(PAD_INPUT_RIGHT))
		{
			m_direction.x = kRight;
		}
		else
		{
			m_direction.x = 0.0f;
		}

		// 上ボタンを押されたらプレイヤーの向きを上に
		if (Pad::IsPress(PAD_INPUT_UP))
		{
			m_direction.z = kUp;
		}
		// 下ボタンを押されたらプレイヤーの向き下に
		else if (Pad::IsPress(PAD_INPUT_DOWN))
		{
			m_direction.z = kDown;
		}
		else
		{
			m_direction.z = 0.0f;
		}
	}

	// 移動方向ベクトルの正規化
	if (VSize(m_direction) >= 1.0f)
	{
		m_direction = VNorm(m_direction);
	}
}
