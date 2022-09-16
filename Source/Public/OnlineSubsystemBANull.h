// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemImpl.h"
#include "OnlineSubsystemBANullPackage.h"
#include "HAL/ThreadSafeCounter.h"

class FOnlineAchievementsBANull;
class FOnlineIdentityBANull;
class FOnlineLeaderboardsBANull;
class FOnlineSessionBANull;
class FOnlineVoiceImpl;

/** Forward declarations of all interface classes */
typedef TSharedPtr<class FOnlineSessionBANull, ESPMode::ThreadSafe> FOnlineSessionBANullPtr;
typedef TSharedPtr<class FOnlineProfileBANull, ESPMode::ThreadSafe> FOnlineProfileBANullPtr;
typedef TSharedPtr<class FOnlineFriendsBANull, ESPMode::ThreadSafe> FOnlineFriendsBANullPtr;
typedef TSharedPtr<class FOnlineUserCloudBANull, ESPMode::ThreadSafe> FOnlineUserCloudBANullPtr;
typedef TSharedPtr<class FOnlineLeaderboardsBANull, ESPMode::ThreadSafe> FOnlineLeaderboardsBANullPtr;
typedef TSharedPtr<class FOnlineExternalUIBANull, ESPMode::ThreadSafe> FOnlineExternalUIBANullPtr;
typedef TSharedPtr<class FOnlineIdentityBANull, ESPMode::ThreadSafe> FOnlineIdentityBANullPtr;
typedef TSharedPtr<class FOnlineAchievementsBANull, ESPMode::ThreadSafe> FOnlineAchievementsBANullPtr;
typedef TSharedPtr<class FOnlineStoreV2BANull, ESPMode::ThreadSafe> FOnlineStoreV2BANullPtr;
typedef TSharedPtr<class FOnlinePurchaseBANull, ESPMode::ThreadSafe> FOnlinePurchaseBANullPtr;
typedef TSharedPtr<class FMessageSanitizerBANull, ESPMode::ThreadSafe> FMessageSanitizerBANullPtr;
#if WITH_ENGINE
typedef TSharedPtr<class FOnlineVoiceImpl, ESPMode::ThreadSafe> FOnlineVoiceImplPtr;
#endif //WITH_ENGINE

/**
 *	OnlineSubsystemBANull - Implementation of the online subsystem for BANull services
 */
class ONLINESUBSYSTEMBANULL_API FOnlineSubsystemBANull : 
	public FOnlineSubsystemImpl
{

public:

	virtual ~FOnlineSubsystemBANull() = default;

	// IOnlineSubsystem

	virtual IOnlineSessionPtr GetSessionInterface() const override;
	virtual IOnlineFriendsPtr GetFriendsInterface() const override;
	virtual IOnlinePartyPtr GetPartyInterface() const override;
	virtual IOnlineGroupsPtr GetGroupsInterface() const override;
	virtual IOnlineSharedCloudPtr GetSharedCloudInterface() const override;
	virtual IOnlineUserCloudPtr GetUserCloudInterface() const override;
	virtual IOnlineEntitlementsPtr GetEntitlementsInterface() const override;
	virtual IOnlineLeaderboardsPtr GetLeaderboardsInterface() const override;
	virtual IOnlineVoicePtr GetVoiceInterface() const override;
	virtual IOnlineExternalUIPtr GetExternalUIInterface() const override;	
	virtual IOnlineTimePtr GetTimeInterface() const override;
	virtual IOnlineIdentityPtr GetIdentityInterface() const override;
	virtual IOnlineTitleFilePtr GetTitleFileInterface() const override;
	virtual IOnlineStoreV2Ptr GetStoreV2Interface() const override;
	virtual IOnlinePurchasePtr GetPurchaseInterface() const override;
	virtual IOnlineEventsPtr GetEventsInterface() const override;
	virtual IOnlineAchievementsPtr GetAchievementsInterface() const override;
	virtual IOnlineSharingPtr GetSharingInterface() const override;
	virtual IOnlineUserPtr GetUserInterface() const override;
	virtual IOnlineMessagePtr GetMessageInterface() const override;
	virtual IOnlinePresencePtr GetPresenceInterface() const override;
	virtual IOnlineChatPtr GetChatInterface() const override;
	virtual IOnlineStatsPtr GetStatsInterface() const override;
	virtual IOnlineTurnBasedPtr GetTurnBasedInterface() const override;
	virtual IOnlineTournamentPtr GetTournamentInterface() const override;
	virtual IMessageSanitizerPtr GetMessageSanitizer(int32 LocalUserNum, FString& OutAuthTypeToExclude) const override;

	virtual bool Init() override;
	virtual bool Shutdown() override;
	virtual FString GetAppId() const override;
	virtual bool Exec(class UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override;
	virtual FText GetOnlineServiceName() const override;

	// FTSTickerObjectBase
	
	virtual bool Tick(float DeltaTime) override;

	// FOnlineSubsystemBANull

PACKAGE_SCOPE:

	/** Only the factory makes instances */
	FOnlineSubsystemBANull() = delete;
	explicit FOnlineSubsystemBANull(FName InInstanceName) :
		FOnlineSubsystemImpl(TEXT("BANull"), InInstanceName),
		SessionInterface(nullptr),
		VoiceInterface(nullptr),
		bVoiceInterfaceInitialized(false),
		LeaderboardsInterface(nullptr),
		IdentityInterface(nullptr),
		AchievementsInterface(nullptr),
		StoreV2Interface(nullptr),
		MessageSanitizerInterface(nullptr),
		OnlineAsyncTaskThreadRunnable(nullptr),
		OnlineAsyncTaskThread(nullptr)
	{}

private:

	/** Interface to the session services */
	FOnlineSessionBANullPtr SessionInterface;

	/** Interface for voice communication */
	mutable IOnlineVoicePtr VoiceInterface;

	/** Interface for voice communication */
	mutable bool bVoiceInterfaceInitialized;

	/** Interface to the leaderboard services */
	FOnlineLeaderboardsBANullPtr LeaderboardsInterface;

	/** Interface to the identity registration/auth services */
	FOnlineIdentityBANullPtr IdentityInterface;

	/** Interface for achievements */
	FOnlineAchievementsBANullPtr AchievementsInterface;

	/** Interface for store */
	FOnlineStoreV2BANullPtr StoreV2Interface;

	/** Interface for purchases */
	FOnlinePurchaseBANullPtr PurchaseInterface;

	/** Interface for message sanitizing */
	FMessageSanitizerBANullPtr MessageSanitizerInterface;

	/** Online async task runnable */
	class FOnlineAsyncTaskManagerBANull* OnlineAsyncTaskThreadRunnable;

	/** Online async task thread */
	class FRunnableThread* OnlineAsyncTaskThread;

	// task counter, used to generate unique thread names for each task
	static FThreadSafeCounter TaskCounter;
};

typedef TSharedPtr<FOnlineSubsystemBANull, ESPMode::ThreadSafe> FOnlineSubsystemBANullPtr;

