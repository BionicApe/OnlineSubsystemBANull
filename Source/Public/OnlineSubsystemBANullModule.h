// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

/**
 * Online subsystem module class  (BANull Implementation)
 * Code related to the loading of the BANull module
 */
class FOnlineSubsystemBANullModule : public IModuleInterface
{
private:

	/** Class responsible for creating instance(s) of the subsystem */
	class FOnlineFactoryBANull* BANullFactory;

public:

	FOnlineSubsystemBANullModule() : 
		BANullFactory(NULL)
	{}

	virtual ~FOnlineSubsystemBANullModule() {}

	// IModuleInterface

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	virtual bool SupportsDynamicReloading() override
	{
		return false;
	}

	virtual bool SupportsAutomaticShutdown() override
	{
		return false;
	}
};
