// Copyright Epic Games, Inc. All Rights Reserved.

#include "OnlineSubsystemBANull.h"
#include "HAL/RunnableThread.h"
#include "OnlineAsyncTaskManagerBANull.h"

#include "OnlineSessionInterfaceBANull.h"
#include "OnlineLeaderboardInterfaceBANull.h"
#include "OnlineIdentityBANull.h"
#include "OnlineAchievementsInterfaceBANull.h"
#include "OnlineStoreV2InterfaceBANull.h"
#include "OnlinePurchaseInterfaceBANull.h"
#include "OnlineMessageSanitizerBANull.h"
#include "Stats/Stats.h"
#include "Misc/ConfigCacheIni.h"

#if WITH_ENGINE
#include "VoiceInterfaceBANull.h"
#endif //WITH_ENGINE

FThreadSafeCounter FOnlineSubsystemBANull::TaskCounter;

IOnlineSessionPtr FOnlineSubsystemBANull::GetSessionInterface() const
{
	return SessionInterface;
}

IOnlineFriendsPtr FOnlineSubsystemBANull::GetFriendsInterface() const
{
	return nullptr;
}

IOnlinePartyPtr FOnlineSubsystemBANull::GetPartyInterface() const
{
	return nullptr;
}

IOnlineGroupsPtr FOnlineSubsystemBANull::GetGroupsInterface() const
{
	return nullptr;
}

IOnlineSharedCloudPtr FOnlineSubsystemBANull::GetSharedCloudInterface() const
{
	return nullptr;
}

IOnlineUserCloudPtr FOnlineSubsystemBANull::GetUserCloudInterface() const
{
	return nullptr;
}

IOnlineEntitlementsPtr FOnlineSubsystemBANull::GetEntitlementsInterface() const
{
	return nullptr;
};

IOnlineLeaderboardsPtr FOnlineSubsystemBANull::GetLeaderboardsInterface() const
{
	return LeaderboardsInterface;
}

IOnlineVoicePtr FOnlineSubsystemBANull::GetVoiceInterface() const
{
#if WITH_ENGINE
	if (VoiceInterface.IsValid() && !bVoiceInterfaceInitialized)
	{	
		if (!VoiceInterface->Init())
		{
			VoiceInterface = nullptr;
		}

		bVoiceInterfaceInitialized = true;
	}

	return VoiceInterface;
#else //WITH_ENGINE
	return nullptr;
#endif //WITH_ENGINE
}

IOnlineExternalUIPtr FOnlineSubsystemBANull::GetExternalUIInterface() const
{
	return nullptr;
}

IOnlineTimePtr FOnlineSubsystemBANull::GetTimeInterface() const
{
	return nullptr;
}

IOnlineIdentityPtr FOnlineSubsystemBANull::GetIdentityInterface() const
{
	return IdentityInterface;
}

IOnlineTitleFilePtr FOnlineSubsystemBANull::GetTitleFileInterface() const
{
	return nullptr;
}

IOnlineStoreV2Ptr FOnlineSubsystemBANull::GetStoreV2Interface() const
{
	return StoreV2Interface;
}

IOnlinePurchasePtr FOnlineSubsystemBANull::GetPurchaseInterface() const
{
	return PurchaseInterface;
}

IOnlineEventsPtr FOnlineSubsystemBANull::GetEventsInterface() const
{
	return nullptr;
}

IOnlineAchievementsPtr FOnlineSubsystemBANull::GetAchievementsInterface() const
{
	return AchievementsInterface;
}

IOnlineSharingPtr FOnlineSubsystemBANull::GetSharingInterface() const
{
	return nullptr;
}

IOnlineUserPtr FOnlineSubsystemBANull::GetUserInterface() const
{
	return nullptr;
}

IOnlineMessagePtr FOnlineSubsystemBANull::GetMessageInterface() const
{
	return nullptr;
}

IOnlinePresencePtr FOnlineSubsystemBANull::GetPresenceInterface() const
{
	return nullptr;
}

IOnlineChatPtr FOnlineSubsystemBANull::GetChatInterface() const
{
	return nullptr;
}

IOnlineStatsPtr FOnlineSubsystemBANull::GetStatsInterface() const
{
	return nullptr;
}

IOnlineTurnBasedPtr FOnlineSubsystemBANull::GetTurnBasedInterface() const
{
	return nullptr;
}

IOnlineTournamentPtr FOnlineSubsystemBANull::GetTournamentInterface() const
{
	return nullptr;
}

IMessageSanitizerPtr FOnlineSubsystemBANull::GetMessageSanitizer(int32 LocalUserNum, FString& OutAuthTypeToExclude) const
{
	return MessageSanitizerInterface;
}

bool FOnlineSubsystemBANull::Tick(float DeltaTime)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_FOnlineSubsystemBANull_Tick);

	if (!FOnlineSubsystemImpl::Tick(DeltaTime))
	{
		return false;
	}

	if (OnlineAsyncTaskThreadRunnable)
	{
		OnlineAsyncTaskThreadRunnable->GameTick();
	}

	if (SessionInterface.IsValid())
	{
		SessionInterface->Tick(DeltaTime);
	}

#if WITH_ENGINE
	if (VoiceInterface.IsValid() && bVoiceInterfaceInitialized)
	{
		VoiceInterface->Tick(DeltaTime);
	}
#endif //WITH_ENGINE

	return true;
}

bool FOnlineSubsystemBANull::Init()
{
	const bool bBANullInit = true;
	
	if (bBANullInit)
	{
		// Create the online async task thread
		OnlineAsyncTaskThreadRunnable = new FOnlineAsyncTaskManagerBANull(this);
		check(OnlineAsyncTaskThreadRunnable);
		OnlineAsyncTaskThread = FRunnableThread::Create(OnlineAsyncTaskThreadRunnable, *FString::Printf(TEXT("OnlineAsyncTaskThreadBANull %s(%d)"), *InstanceName.ToString(), TaskCounter.Increment()), 128 * 1024, TPri_Normal);
		check(OnlineAsyncTaskThread);
		UE_LOG_ONLINE(Verbose, TEXT("Created thread (ID:%d)."), OnlineAsyncTaskThread->GetThreadID());

		SessionInterface = MakeShareable(new FOnlineSessionBANull(this));
		LeaderboardsInterface = MakeShareable(new FOnlineLeaderboardsBANull(this));
		IdentityInterface = MakeShareable(new FOnlineIdentityBANull(this));
		AchievementsInterface = MakeShareable(new FOnlineAchievementsBANull(this));
#if WITH_ENGINE
		VoiceInterface = MakeShareable(new FOnlineVoiceImpl(this));
#endif //WITH_ENGINE
		StoreV2Interface = MakeShareable(new FOnlineStoreV2BANull(*this));
		PurchaseInterface = MakeShareable(new FOnlinePurchaseBANull(*this));
		MessageSanitizerInterface = MakeShareable(new FMessageSanitizerBANull());
	}
	else
	{
		Shutdown();
	}

	return bBANullInit;
}

bool FOnlineSubsystemBANull::Shutdown()
{
	UE_LOG_ONLINE(VeryVerbose, TEXT("FOnlineSubsystemBANull::Shutdown()"));

	FOnlineSubsystemImpl::Shutdown();

	if (OnlineAsyncTaskThread)
	{
		// Destroy the online async task thread
		delete OnlineAsyncTaskThread;
		OnlineAsyncTaskThread = nullptr;
	}

	if (OnlineAsyncTaskThreadRunnable)
	{
		delete OnlineAsyncTaskThreadRunnable;
		OnlineAsyncTaskThreadRunnable = nullptr;
	}

#if WITH_ENGINE
	if (VoiceInterface.IsValid() && bVoiceInterfaceInitialized)
	{
		VoiceInterface->Shutdown();
	}
#endif //WITH_ENGINE
	
#define DESTRUCT_INTERFACE(Interface) \
	if (Interface.IsValid()) \
	{ \
		ensure(Interface.IsUnique()); \
		Interface = nullptr; \
	}
 
	// Destruct the interfaces
	DESTRUCT_INTERFACE(PurchaseInterface);
	DESTRUCT_INTERFACE(StoreV2Interface);
	DESTRUCT_INTERFACE(VoiceInterface);
	DESTRUCT_INTERFACE(AchievementsInterface);
	DESTRUCT_INTERFACE(IdentityInterface);
	DESTRUCT_INTERFACE(LeaderboardsInterface);
	DESTRUCT_INTERFACE(SessionInterface);
	DESTRUCT_INTERFACE(MessageSanitizerInterface);

#undef DESTRUCT_INTERFACE
	
	return true;
}

FString FOnlineSubsystemBANull::GetAppId() const
{
	return TEXT("");
}

bool FOnlineSubsystemBANull::Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar)
{
	if (FOnlineSubsystemImpl::Exec(InWorld, Cmd, Ar))
	{
		return true;
	}
	return false;
}
FText FOnlineSubsystemBANull::GetOnlineServiceName() const
{
	return NSLOCTEXT("OnlineSubsystemBANull", "OnlineServiceName", "BANull");
}

