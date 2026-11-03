#pragma once

#include "Protocol.pb.h"

///////////////////////////////////////////////////////
//	<	ClientSession	>
///////////////////////////////////////////////////////
class GameSession : public PacketSession
{
public:
	GameSession() = default;

protected:
	virtual void			OnConnected() override;
	virtual void			OnDisconnected() override;
	virtual void			OnRecvPacket(BYTE* buffer, int32 len);
	virtual void			OnSend(int32 len) override;

private:
};