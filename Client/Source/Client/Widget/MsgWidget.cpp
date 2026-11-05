#include "MsgWidget.h"

#include "Components/TextBlock.h"

void UMsgWidget::SetMessage(const FString& Message)
{
	if (MessageText)
	{
		MessageText->SetText(FText::FromString(*Message));
	}
}

