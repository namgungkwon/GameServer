// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterListWidget.h"

#include "Client/Client.h"

void UCharacterListWidget::SetCharacterList(const Protocol::GC_AUTH_TOKEN& pkt)
{
	CharacterList.Empty();
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

void UCharacterListWidget::SetCharacterList(const Protocol::GC_CREATE_CHARACTER& pkt)
{
	CharacterList.Empty();
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

void UCharacterListWidget::SendCreateCharacterPkt(FString name, FString templateId)
{	
	Protocol::CG_CREATE_CHARACTER Pkt;
	
	Pkt.set_templateid(FCString::Atoi(*templateId));
	Pkt.set_name(TCHAR_TO_UTF8(*name));
	SEND_PACKET(Pkt);
}
