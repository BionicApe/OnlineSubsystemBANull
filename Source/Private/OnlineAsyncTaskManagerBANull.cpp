// Copyright Epic Games, Inc. All Rights Reserved.

#include "OnlineAsyncTaskManagerBANull.h"

void FOnlineAsyncTaskManagerBANull::OnlineTick()
{
	check(BANullSubsystem);
	check(FPlatformTLS::GetCurrentThreadId() == OnlineThreadId || !FPlatformProcess::SupportsMultithreading());
}

