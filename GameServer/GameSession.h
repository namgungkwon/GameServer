#pragma once
class GameSession : public PacketSession
{
public:
	virtual void OnConnected() override;
	virtual void OnDisconnected() override;
	virtual void OnRecvPacket(BYTE* buffer, int32 len) override;

public:
	bool IsAuthenticated() { return _accountDBId != 0; }
	bool TryEnterGame() { return EnterRoomProcess.exchange(true) == false; }
	void ReleaseEnterGame() { EnterRoomProcess.store(false); }

private:
	GETSET(uint64, _accountDBId, AccountDBId);
	GETSET(uint64, _playerDBId, PlayerDBId);
	GETSET_WEAK(Player, _player, Player);

private:
	Atomic<bool> EnterRoomProcess = false;
};