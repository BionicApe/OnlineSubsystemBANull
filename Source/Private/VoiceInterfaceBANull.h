// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "VoiceInterfaceImpl.h"
#include "Online/CoreOnline.h"
#include "OnlineSubsystemTypes.h"
#include "Interfaces/VoiceInterface.h"
#include "Net/VoiceDataCommon.h"
#include "OnlineSubsystemUtilsPackage.h"

class ONLINESUBSYSTEMBANULL_API FOnlineVoiceImplBANull : public FOnlineVoiceImpl {
PACKAGE_SCOPE:
	FOnlineVoiceImplBANull() : FOnlineVoiceImpl()
	{};

public:
	/** Constructor */
	FOnlineVoiceImplBANull(class IOnlineSubsystem* InOnlineSubsystem) :
		FOnlineVoiceImpl(InOnlineSubsystem)
	{
	};

	virtual ~FOnlineVoiceImplBANull() override {}
};

typedef TSharedPtr<FOnlineVoiceImpl, ESPMode::ThreadSafe> FOnlineVoiceImplPtr;
