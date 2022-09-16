// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemTypes.h"
#include "IPAddress.h"

class FOnlineSubsystemBANull;

// from OnlineSubsystemTypes.h
TEMP_UNIQUENETIDSTRING_SUBCLASS(FUniqueNetIdBANull, TEXT("BANull"));

/** 
 * Implementation of session information
 */
class FOnlineSessionInfoBANull : public FOnlineSessionInfo
{
	/** Hidden on purpose */
	FOnlineSessionInfoBANull(const FOnlineSessionInfoBANull& Src) = delete;
	FOnlineSessionInfoBANull& operator=(const FOnlineSessionInfoBANull& Src) = delete;

PACKAGE_SCOPE:

	/** Constructor */
	FOnlineSessionInfoBANull();

	/** 
	 * Initialize a BANull session info with the address of this machine
	 * and an id for the session
	 */
	void Init(const FOnlineSubsystemBANull& Subsystem);

	/** The ip & port that the host is listening on (valid for LAN/GameServer) */
	TSharedPtr<class FInternetAddr> HostAddr;
	/** Unique Id for this session */
	FUniqueNetIdBANullRef SessionId;

public:

	virtual ~FOnlineSessionInfoBANull() {}

 	bool operator==(const FOnlineSessionInfoBANull& Other) const
 	{
 		return false;
 	}

	virtual const uint8* GetBytes() const override
	{
		return NULL;
	}

	virtual int32 GetSize() const override
	{
		return sizeof(uint64) + sizeof(TSharedPtr<class FInternetAddr>);
	}

	virtual bool IsValid() const override
	{
		// LAN case
		return HostAddr.IsValid() && HostAddr->IsValid();
	}

	virtual FString ToString() const override
	{
		return SessionId->ToString();
	}

	virtual FString ToDebugString() const override
	{
		return FString::Printf(TEXT("HostIP: %s SessionId: %s"), 
			HostAddr.IsValid() ? *HostAddr->ToString(true) : TEXT("INVALID"), 
			*SessionId->ToDebugString());
	}

	virtual const FUniqueNetId& GetSessionId() const override
	{
		return *SessionId;
	}
};
