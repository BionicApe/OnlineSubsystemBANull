// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineAsyncTaskManager.h"

/**
 *	BANull version of the async task manager to register the various BANull callbacks with the engine
 */
class FOnlineAsyncTaskManagerBANull : public FOnlineAsyncTaskManager
{
protected:

	/** Cached reference to the main online subsystem */
	class FOnlineSubsystemBANull* BANullSubsystem;

public:

	FOnlineAsyncTaskManagerBANull(class FOnlineSubsystemBANull* InOnlineSubsystem)
		: BANullSubsystem(InOnlineSubsystem)
	{
	}

	~FOnlineAsyncTaskManagerBANull() 
	{
	}

	// FOnlineAsyncTaskManager
	virtual void OnlineTick() override;
};
