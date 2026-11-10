#pragma once

class LoginServerSession : public PacketSession
{
public:
	void Init(DummyClientRef owner) { _owner = owner; }

public:
	virtual void OnRecvPacket(BYTE* buffer, int32 len) override;

public:
	DummyClientRef GetOwner() { return _owner.lock(); }

public:
	std::weak_ptr<DummyClient> _owner;
};