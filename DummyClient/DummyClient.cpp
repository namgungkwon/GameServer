#include "pch.h"
#include "DummyClient.h"

#include "LoginServerSession.h"
#include "GameServerSession.h"

#include "LoginServerPacketHandler.h"
#include "GameServerPacketHandler.h"

#include <random>

int64 GetRandom(int64 min, int64 max)
{
	if (min > max)
		return 0;

	std::random_device rd;  // CSPRNG 소스 (구현 의존 but 대부분 안전)
	std::uniform_int_distribution<std::size_t> dist;

	int64 range = max - min + 1;

	int64 rnd = dist(rd) % range;
	return rnd + min;
}

void DummyClient::Init(ClientServiceRef game, ClientServiceRef login, string id, string password)
{
	_loginServerService = login;
	_gameServerService = game;

	this->id = id;
	this->password = password;

	this->characterId = id;

	_loginServerSession = static_pointer_cast<LoginServerSession>(_loginServerService->AddSession());
	_loginServerSession->_owner = shared_from_this();

	_gameServerSession = static_pointer_cast<GameServerSession>(_gameServerService->AddSession());
	_gameServerSession->_owner = shared_from_this();
}

void DummyClient::Clear()
{
	_isReady = false;
	DisconnectGameServer();
	DisconnectLoginServer();
}

bool DummyClient::ConnectLoginServer()
{
	if (_loginServerService == nullptr)
		return false;

	if (_loginServerSession)
		return true;

	return false;
}

bool DummyClient::ConnectGameServer()
{
	if (_gameServerService == nullptr)
		return false;

	if (_gameServerSession)
		return true;

	return false;
}

void DummyClient::DisconnectLoginServer()
{
	if (_loginServerSession)
		return; 

	_loginServerSession->Disconnect();

	_loginServerSession = nullptr;
}

void DummyClient::DisconnectGameServer()
{
	if (_gameServerSession)
		return;

	_gameServerSession->Disconnect();

	_gameServerSession = nullptr;
}

void DummyClient::Process()
{
	if (_isReady)
	{
		uint64 tick = GetTickCount64();
		deltaTick = tick - currentTick;
		currentTick = tick;
		//RequestAttack();
		RequestMove();
		//RequestChat();
		//RequestWorldChat();
	}
}

void DummyClient::Login()
{
	if (ConnectLoginServer() == false)
		return;

	// 로그인
	{
		Protocol::CL_LOGIN pkt;
		pkt.set_id(id.c_str());
		pkt.set_password(password.c_str());
		SendBufferRef sendBuffer = LoginServerPacketHandler::MakeSendBuffer(pkt);
		_loginServerSession->Send(sendBuffer);
		//LOG_INFO(L"Send LoginPkt");
	}
	// 이후 처리는 LoginServerPacketHandler에서 진행
}

void DummyClient::CreateAccount()
{
	if (ConnectLoginServer() == false)
		return;

	Protocol::CL_CREATE_ACCOUNT pkt;
	pkt.set_id(id.c_str());
	pkt.set_password(password.c_str());
	SendBufferRef sendBuffer = LoginServerPacketHandler::MakeSendBuffer(pkt);
	_loginServerSession->Send(sendBuffer);
	//LOG_INFO(L"Send CreateAccountPkt");
}

void DummyClient::RequestEnterGameServer(string token)
{
	if (ConnectGameServer() == false)
		return;

	this->token = token;

	{
		Protocol::CG_AUTH_TOKEN pkt;
		pkt.set_token(this->token);
		SendBufferRef sendBuffer = GameServerPacketHandler::MakeSendBuffer(pkt);
		_gameServerSession->Send(sendBuffer);
		//LOG_INFO(L"Send RequestEnterGameServerPkt");
	}
}

void DummyClient::RequestCreateCharacter()
{
	if (ConnectGameServer() == false)
		return;

	Protocol::CG_CREATE_CHARACTER pkt;
	pkt.set_name(characterId.c_str());
	pkt.set_templateid(templateId);
	SendBufferRef sendBuffer = GameServerPacketHandler::MakeSendBuffer(pkt);
	_gameServerSession->Send(sendBuffer);
}

void DummyClient::RequestEnterGame(uint64 playerId)
{
	if (ConnectGameServer() == false)
		return;

	this->playerId = playerId;

	Protocol::CG_ENTER_GAME pkt;
	pkt.set_playerid(playerId);
	SendBufferRef sendBuffer = GameServerPacketHandler::MakeSendBuffer(pkt);
	_gameServerSession->Send(sendBuffer);
	//LOG_INFO(L"Send RequestEnterGamePkt");
}

void DummyClient::EnterGame()
{
	currentTick = GetTickCount64();
	nextMoveTick = currentTick + GetRandom(MOVE_TIME_MIN, MOVE_TIME_MAX);
	nextAttackTick = currentTick + GetRandom(ATTACK_TIME_MIN, ATTACK_TIME_MAX);
	nextChatTick = currentTick + GetRandom(CHAT_TIME_MIN, CHAT_TIME_MAX);
	nextWorldChatTick = currentTick + GetRandom(CHAT_WORLD_TIME_MIN, CHAT_WORLD_TIME_MAX);

	//LOG_INFO(L"EnterGame");
	_isReady = true;
}

void DummyClient::RequestMove()
{
	Protocol::Vector3 normal = dir;
	if (std::abs(dir.x()) > 0.1f && std::abs(dir.y()) > 0.f)
	{
		dir.set_x(dir.x() * 0.707f);
		dir.set_y(dir.y() * 0.707f);
	}
	Protocol::Vector3 location = posData.location();
	location.set_x(std::clamp(location.x() + normal.x() * 300.f * deltaTick * 0.001f, 0.f, 100000.f));
	location.set_y(std::clamp(location.y() + normal.y() * 300.f * deltaTick * 0.001f, 0.f, 100000.f));

	*posData.mutable_location() = location;

	if (nextMoveTick < currentTick)
		return;

	nextMoveTick = currentTick + GetRandom(MOVE_TIME_MIN, MOVE_TIME_MAX);

	if (nextDirChange < currentTick)
	{
		nextDirChange = currentTick + GetRandom(MOVE_DIRECTION_CHANGE_MIN, MOVE_DIRECTION_CHANGE_MAX);
		dir = GetRndVelocity();
	}

	Protocol::CG_MOVE pkt;
	Protocol::ObjectMoveData* move = pkt.mutable_movedata();
	move->set_objectid(playerId);
	*move->mutable_location() = posData.location();
	*move->mutable_velocity() = dir;
	move->set_yaw(GetYaw(dir.x(), dir.y(), move->yaw()));
	SendBufferRef sendBuffer = GameServerPacketHandler::MakeSendBuffer(pkt);
	_gameServerSession->Send(sendBuffer);
}

void DummyClient::RequestAttack()
{
	if (nextAttackTick < currentTick)
		return;

	nextAttackTick = currentTick + GetRandom(ATTACK_TIME_MIN, ATTACK_TIME_MAX);

	Protocol::CG_ATTACK pkt;
	*pkt.mutable_posdata() = posData;
	pkt.set_skillid(0);
	SendBufferRef sendBuffer = GameServerPacketHandler::MakeSendBuffer(pkt);
	_gameServerSession->Send(sendBuffer);
}

void DummyClient::RequestChat()
{
	if (nextChatTick < currentTick)
		return;

	nextChatTick = currentTick + GetRandom(CHAT_TIME_MIN, CHAT_TIME_MAX);

	Protocol::CG_CHAT pkt;
	pkt.set_chat(localChat.c_str());
	pkt.set_type(Protocol::CHATTYPE_ADJACENT);
	SendBufferRef sendBuffer = GameServerPacketHandler::MakeSendBuffer(pkt);
	_gameServerSession->Send(sendBuffer);
}

void DummyClient::RequestWorldChat()
{
	if (nextWorldChatTick < currentTick)
		return;

	nextWorldChatTick = currentTick + GetRandom(CHAT_WORLD_TIME_MIN, CHAT_WORLD_TIME_MAX);

	Protocol::CG_CHAT pkt;
	pkt.set_chat(worldChat.c_str());
	pkt.set_type(Protocol::CHATTYPE_WORLD);
	SendBufferRef sendBuffer = GameServerPacketHandler::MakeSendBuffer(pkt);
	_gameServerSession->Send(sendBuffer);
}

Protocol::Vector3 DummyClient::GetRndVelocity()
{
	Protocol::Vector3 vec;

	vec.set_x(GetRandom(-1, 1));
	vec.set_y(GetRandom(-1, 1));
	vec.set_z(0);

	return vec;
}

float DummyClient::GetYaw(float x, float y, float currentYaw)
{
	uint64 xInt = 0;
	uint64 yInt = 0;

	if (std::abs(x) > 0.1f)
	{
		if (x > 0.5f)
			xInt = 1;
		else
			xInt = -1;
	}

	if (std::abs(y) > 0.1f)
	{
		if (y > 0.5f)
			yInt = 1;
		else
			yInt = -1;
	}

	if (xInt == 1)
	{
		if (yInt == 1)
		{
			return 45.f;
		}
		else if (yInt == 0)
		{
			return 0.f;
		}
		else
		{
			return 315.f;
		}
	}
	else if (xInt == 0)
	{
		if (yInt == 1)
		{
			return 90.f;
		}
		else if (yInt == 0)
		{
			return currentYaw;
		}
		else
		{
			return 270.f;
		}
	}
	else
	{
		if (yInt == 1)
		{
			return 135.f;
		}
		else if (yInt == 0)
		{
			return 180.f;
		}
		else
		{
			return 225.f;
		}
	}
}
