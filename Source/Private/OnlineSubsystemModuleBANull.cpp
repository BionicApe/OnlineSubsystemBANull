// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "OnlineSubsystemBANullModule.h"
#include "OnlineSubsystemModule.h"
#include "OnlineSubsystemNames.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemBANull.h"

IMPLEMENT_MODULE(FOnlineSubsystemBANullModule, OnlineSubsystemBANull);

/**
 * Class responsible for creating instance(s) of the subsystem
 */
class FOnlineFactoryBANull : public IOnlineFactory
{
public:

	FOnlineFactoryBANull() {}
	virtual ~FOnlineFactoryBANull() {}

	virtual IOnlineSubsystemPtr CreateSubsystem(FName InstanceName)
	{
		FOnlineSubsystemBANullPtr OnlineSub = MakeShared<FOnlineSubsystemBANull, ESPMode::ThreadSafe>(InstanceName);
		if (OnlineSub->IsEnabled())
		{
			if (!OnlineSub->Init())
			{
				UE_LOG_ONLINE(Warning, TEXT("BANull API failed to initialize!"));
				OnlineSub->Shutdown();
				OnlineSub = NULL;
			}
		}
		else
		{
			UE_LOG_ONLINE(Warning, TEXT("BANull API disabled!"));
			OnlineSub->Shutdown();
			OnlineSub = NULL;
		}

		return OnlineSub;
	}
};

void FOnlineSubsystemBANullModule::StartupModule()
{
	BANullFactory = new FOnlineFactoryBANull();

	// Create and register our singleton factory with the main online subsystem for easy access
	FOnlineSubsystemModule& OSS = FModuleManager::GetModuleChecked<FOnlineSubsystemModule>("OnlineSubsystem");
	OSS.RegisterPlatformService(TEXT("BANull"), BANullFactory);
}

void FOnlineSubsystemBANullModule::ShutdownModule()
{
	FOnlineSubsystemModule& OSS = FModuleManager::GetModuleChecked<FOnlineSubsystemModule>("OnlineSubsystem");
	OSS.UnregisterPlatformService(TEXT("BANull"));

	delete BANullFactory;
	BANullFactory = NULL;
}
