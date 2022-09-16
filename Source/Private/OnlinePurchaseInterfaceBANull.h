// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Interfaces/OnlinePurchaseInterface.h"
#include "OnlineSubsystemBANullTypes.h"

class FOnlineSubsystemBANull;

class FOnlinePurchaseBANull
	: public IOnlinePurchase
	, public TSharedFromThis<FOnlinePurchaseBANull, ESPMode::ThreadSafe>
{
public:
	FOnlinePurchaseBANull(FOnlineSubsystemBANull& InBANullSubsystem);
	virtual ~FOnlinePurchaseBANull();

	void Tick();

public:
	//~ Begin IOnlinePurchase Interface
	virtual bool IsAllowedToPurchase(const FUniqueNetId& UserId) override;
	virtual void Checkout(const FUniqueNetId& UserId, const FPurchaseCheckoutRequest& CheckoutRequest, const FOnPurchaseCheckoutComplete& Delegate) override;
	virtual void FinalizePurchase(const FUniqueNetId& UserId, const FString& ReceiptId) override;
	virtual void RedeemCode(const FUniqueNetId& UserId, const FRedeemCodeRequest& RedeemCodeRequest, const FOnPurchaseRedeemCodeComplete& Delegate) override;
	virtual void QueryReceipts(const FUniqueNetId& UserId, bool bRestoreReceipts, const FOnQueryReceiptsComplete& Delegate) override;
	virtual void GetReceipts(const FUniqueNetId& UserId, TArray<FPurchaseReceipt>& OutReceipts) const override;
	virtual void FinalizeReceiptValidationInfo(const FUniqueNetId& UserId, FString& InReceiptValidationInfo, const FOnFinalizeReceiptValidationInfoComplete& Delegate) override;
	//~ End IOnlinePurchase Interface

PACKAGE_SCOPE:
	void CheckoutSuccessfully(const FUniqueNetIdBANull& UserId, TSharedPtr<FOnlineStoreOffer> Offer);

PACKAGE_SCOPE:
	/** Pointer back to our parent subsystem */
	FOnlineSubsystemBANull& BANullSubsystem;

	/** Cached receipts information per user */
	TMap<FUniqueNetIdBANull, TArray<FPurchaseReceipt> > UserFakeReceipts;

	/** Do we have a purchase currently in progress? */
	TOptional<FOnPurchaseCheckoutComplete> PendingPurchaseDelegate;

	TOptional<double> PendingPurchaseFailTime;
};

using FOnlinePurchaseBANullPtr = TSharedPtr<FOnlinePurchaseBANull, ESPMode::ThreadSafe>;
using FOnlinePurchaseBANullRef = TSharedRef<FOnlinePurchaseBANull, ESPMode::ThreadSafe>;
