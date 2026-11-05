// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterListWidget.h"

#include "Client/Client.h"

void UCharacterListWidget::SetCharacterList(const Protocol::GC_AUTH_TOKEN& pkt)
{
	for(auto& item : pkt.summaries())
	{
		FCharacterListWidgetItem CharacterListItem;
		
		CharacterListItem.Name = FString(item.name().c_str());
		CharacterListItem.Level = item.level();
		CharacterListItem.ObjectId = item.objectid();
		
		CharacterList.Add(CharacterListItem);
	}

	UpdateCharacterList();
}

void UCharacterListWidget::SendEnterGamePkt(int32 listIdx)
{
	Protocol::CG_ENTER_GAME Pkt;
	Pkt.set_playerid(CharacterList[listIdx].ObjectId);
	SEND_PACKET(Pkt);
}
