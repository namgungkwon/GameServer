#pragma once

#include "SendBuffer.h"

struct PacketHeader
{
	uint16 size;
	uint16 id;
};

#define useRef(name) using name##Ref = TSharedPtr<class name>;
useRef(PacketSession);
useRef(SendBuffer);


