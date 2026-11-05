// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginWidget.h"

#include "../Client.h"
#include "../NetWork/Protocol.pb.h"

void ULoginWidget::CreateAccount(FString id, FString password)
{
	Protocol::CL_CREATE_ACCOUNT pkt;
	pkt.set_id(TCHAR_TO_UTF8(*id));
	pkt.set_password(TCHAR_TO_UTF8(*password));
	SEND_L_PACKET(pkt);
}

void ULoginWidget::Login(FString id, FString password)
{
	Protocol::CL_LOGIN pkt;
	pkt.set_id(TCHAR_TO_UTF8(*id));
	pkt.set_password(TCHAR_TO_UTF8(*password));
	SEND_L_PACKET(pkt);
}
