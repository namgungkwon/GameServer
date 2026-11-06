pushd %~dp0
protoc.exe --proto_path=./ --cpp_out=./ ./Enum.proto
protoc.exe --proto_path=./ --cpp_out=./ ./Struct.proto
protoc.exe --proto_path=./ --cpp_out=./ ./Protocol.proto

XCOPY /Y Enum.pb.h "../../GameServer"
XCOPY /Y Enum.pb.cc "../../GameServer"
XCOPY /Y Struct.pb.h "../../GameServer"
XCOPY /Y Struct.pb.cc "../../GameServer"
XCOPY /Y Protocol.pb.h "../../GameServer"
XCOPY /Y Protocol.pb.cc "../../GameServer"

XCOPY /Y Enum.pb.h "../../LoginServer"
XCOPY /Y Enum.pb.cc "../../LoginServer"
XCOPY /Y Struct.pb.h "../../LoginServer"
XCOPY /Y Struct.pb.cc "../../LoginServer"
XCOPY /Y Protocol.pb.h "../../LoginServer"
XCOPY /Y Protocol.pb.cc "../../LoginServer"

XCOPY /Y Enum.pb.h "../../DummyClient"
XCOPY /Y Enum.pb.cc "../../DummyClient"
XCOPY /Y Struct.pb.h "../../DummyClient"
XCOPY /Y Struct.pb.cc "../../DummyClient"
XCOPY /Y Protocol.pb.h "../../DummyClient"
XCOPY /Y Protocol.pb.cc "../../DummyClient"

XCOPY /Y Enum.pb.h "../../Client/Source/Client/Network"
XCOPY /Y Enum.pb.cc "../../Client/Source/Client/Network"
XCOPY /Y Struct.pb.h "../../Client/Source/Client/Network"
XCOPY /Y Struct.pb.cc "../../Client/Source/Client/Network"
XCOPY /Y Protocol.pb.h "../../Client/Source/Client/Network"
XCOPY /Y Protocol.pb.cc "../../Client/Source/Client/Network"

DEL /Q /F *.pb.h
DEL /Q /F *.pb.cc


GenPackets.exe --path=./Protocol.proto --output=ClientPacketHandler --output3=LoginServerPacketHandler --recv=CL_ --send=LC_
GenPackets.exe --path=./Protocol.proto --output=LoginServerPacketHandler --output3=ClientPacketHandler --recv=LC_ --send=CL_
XCOPY /Y ClientPacketHandler.h "../../LoginServer"
XCOPY /Y LoginServerPacketHandler.h "../../Client/Source/Client/Network"
XCOPY /Y LoginServerPacketHandler.h "../../DummyClient"
DEL /Q /F *.h

GenPackets.exe --path=./Protocol.proto --output=ClientPacketHandler --output3=GameServerPacketHandler --recv=CG_ --send=GC_
GenPackets.exe --path=./Protocol.proto --output=GameServerPacketHandler --output3=ClientPacketHandler --recv=GC_ --send=CG_
XCOPY /Y ClientPacketHandler.h "../../GameServer"
XCOPY /Y GameServerPacketHandler.h "../../Client/Source/Client/Network"
XCOPY /Y GameServerPacketHandler.h "../../DummyClient"


XCOPY /Y Packet.h "../../GameServer"
if not exist "../../GameServer/Packet.cpp" (
    xcopy /Y "Packet.cpp" "../../GameServer"  
)

XCOPY /Y Packet.h "../../Client/Source/Client/Network"
if not exist "../../Client/Source/Client/Network/Packet.cpp" (
    xcopy /Y "Packet.cpp" "../../Client/Source/Client/Network"  
)

XCOPY /Y Packet.h "../../DummyClient"
if not exist "../../DummyClient/Packet.cpp" (
    xcopy /Y "Packet.cpp"  "../../DummyClient" 
)

XCOPY /Y Packet.h "../../LoginServer"
if not exist "../../LoginServer/Packet.cpp" (
    xcopy /Y "Packet.cpp"  "../../LoginServer" 
)

DEL /Q /F *.h
IF ERRORLEVEL 1 PAUSE


PAUSE
