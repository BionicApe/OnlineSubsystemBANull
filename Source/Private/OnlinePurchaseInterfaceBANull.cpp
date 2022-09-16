// Copyright Epic Games, Inc. All Rights Reserved.

#include "OnlinePurchaseInterfaceBANull.h"
#include "OnlineStoreV2InterfaceBANull.h"
#include "OnlineSubsystemBANull.h"
#include "OnlineError.h"
#include "Misc/ConfigCacheIni.h"

namespace
{
	static FPurchaseReceipt::FReceiptOfferEntry MakeReceiptOfferEntry(const FUniqueNetIdBANull& BANullUserId, const FString& Id, const FString& Name)
	{
		FPurchaseReceipt::FReceiptOfferEntry OfferEntry(FString(), Id, 1);
		{
			FPurchaseReceipt::FLineItemInfo LineItem;
			LineItem.ItemName = Name;
			LineItem.UniqueId = Id;
			OfferEntry.LineItems.Emplace(MoveTemp(LineItem));
		}

		return OfferEntry;
	}
}

FOnlinePurchaseBANull::FOnlinePurchaseBANull(FOnlineSubsystemBANull& InBANullSubsystem)
	: BANullSubsystem(InBANullSubsystem)
{
}

FOnlinePurchaseBANull::~FOnlinePurchaseBANull()
{
}

void FOnlinePurchaseBANull::Tick()
{
	if (PendingPurchaseFailTime.IsSet() && PendingPurchaseDelegate.IsSet())
	{
		if (FPlatformTime::Seconds() > PendingPurchaseFailTime.GetValue())
		{
			FOnPurchaseCheckoutComplete Delegate = MoveTemp(PendingPurchaseDelegate.GetValue());
			PendingPurchaseDelegate.Reset();
			PendingPurchaseFailTime.Reset();

			Delegate.ExecuteIfBound(FOnlineError(TEXT("Checkout was cancelled or timed out")), MakeShared<FPurchaseReceipt>());
		}
	}
}

bool FOnlinePurchaseBANull::IsAllowedToPurchase(const FUniqueNetId& UserId)
{
	return true;
}

void FOnlinePurchaseBANull::Checkout(const FUniqueNetId& UserId, const FPurchaseCheckoutRequest& CheckoutRequest, const FOnPurchaseCheckoutComplete& Delegate)
{
	// Lambda to wrap calling our delegate with an error and logging the message
	auto CallDelegateError = [this, &Delegate](FString&& ErrorMessage)
	{
		BANullSubsystem.ExecuteNextTick([Delegate, MovedErrorMessage = MoveTemp(ErrorMessage)]() mutable
		{
			UE_LOG_ONLINE(Error, TEXT("%s"), *MovedErrorMessage);

			const TSharedRef<FPurchaseReceipt> PurchaseReceipt = MakeShared<FPurchaseReceipt>();
			PurchaseReceipt->TransactionState = EPurchaseTransactionState::Failed;

			Delegate.ExecuteIfBound(FOnlineError(MoveTemp(MovedErrorMessage)), PurchaseReceipt);
		});
	};

	if (CheckoutRequest.PurchaseOffers.Num() == 0)
	{
		CallDelegateError(TEXT("FOnlinePurchaseBANull::Checkout failed, there were no entries passed to purchase"));
		return;
	}
	else if (CheckoutRequest.PurchaseOffers.Num() != 1)
	{
		CallDelegateError(TEXT("FOnlinePurchaseBANull::Checkout failed, there were more than one entry passed to purchase. We currently only support one."));
		return;
	}

	check(CheckoutRequest.PurchaseOffers.IsValidIndex(0));
	const FPurchaseCheckoutRequest::FPurchaseOfferEntry& Entry = CheckoutRequest.PurchaseOffers[0];

	if (Entry.Quantity != 1)
	{
		CallDelegateError(TEXT("FOnlinePurchaseBANull::Checkout failed, purchase quantity not set to one. We currently only support one."));
		return;
	}

	if (Entry.OfferId.IsEmpty())
	{
		CallDelegateError(TEXT("FOnlinePurchaseBANull::Checkout failed, OfferId is blank."));
		return;
	}

	const IOnlineStoreV2Ptr BANullStoreInt = BANullSubsystem.GetStoreV2Interface();

	TSharedPtr<FOnlineStoreOffer> BANullOffer = BANullStoreInt->GetOffer(Entry.OfferId);
	if (!BANullOffer.IsValid())
	{
		CallDelegateError(TEXT("FOnlinePurchaseBANull::Checkout failed, Could not find corresponding offer."));
		return;
	}

	if (PendingPurchaseDelegate.IsSet())
	{
		CallDelegateError(TEXT("FOnlinePurchaseBANull::Checkout failed, there was another purchase in progress."));
		return;
	}

	PendingPurchaseDelegate = Delegate;

	TWeakPtr<FOnlinePurchaseBANull, ESPMode::ThreadSafe> WeakMe = AsShared();
	const FUniqueNetIdBANull& BANullUserId = static_cast<const FUniqueNetIdBANull&>(UserId);

	BANullSubsystem.ExecuteNextTick([BANullUserId, BANullOffer, WeakMe]
	{
		FOnlinePurchaseBANullPtr StrongThis = WeakMe.Pin();
		if (StrongThis.IsValid())
		{
			StrongThis->CheckoutSuccessfully(BANullUserId, BANullOffer);
		}
	});
}

void FOnlinePurchaseBANull::CheckoutSuccessfully(const FUniqueNetIdBANull& UserId, TSharedPtr<FOnlineStoreOffer> Offer)
{
	// Cache this receipt
	TArray<FPurchaseReceipt>& UserReceipts = UserFakeReceipts.FindOrAdd(UserId);
	FPurchaseReceipt& PurchaseReceipt = UserReceipts.Emplace_GetRef();
	PurchaseReceipt.AddReceiptOffer(MakeReceiptOfferEntry(UserId, Offer->OfferId, Offer->Title.ToString()));

	check(PendingPurchaseDelegate.IsSet());

	// Have a pending purchase
	FOnPurchaseCheckoutComplete Delegate = MoveTemp(PendingPurchaseDelegate.GetValue());
	PendingPurchaseDelegate.Reset();
	PendingPurchaseFailTime.Reset();

	// Finish pending purchase
	Delegate.ExecuteIfBound(FOnlineError(true), MakeShared<FPurchaseReceipt>(PurchaseReceipt));
}

void FOnlinePurchaseBANull::FinalizePurchase(const FUniqueNetId& UserId, const FString& ReceiptId)
{
	const FUniqueNetIdBANull& BANullUserId = static_cast<const FUniqueNetIdBANull&>(UserId);
	TArray<FPurchaseReceipt>* UserReceipts = UserFakeReceipts.Find(BANullUserId);
	if (UserReceipts)
	{
		for (const FPurchaseReceipt& UserReceipt : *UserReceipts)
		{
			for (const FPurchaseReceipt::FReceiptOfferEntry& ReceiptOffer : UserReceipt.ReceiptOffers)
			{
				if (ReceiptOffer.OfferId == ReceiptId)
				{
					UE_LOG_ONLINE(Log, TEXT("Consumption of Entitlement %s completed was successful"), *ReceiptId);
					return;
				}
			}
		}
	}

	UE_LOG_ONLINE(Error, TEXT("Didn't find receipt with id %s"), *ReceiptId);
}

void FOnlinePurchaseBANull::RedeemCode(const FUniqueNetId& UserId, const FRedeemCodeRequest& RedeemCodeRequest, const FOnPurchaseRedeemCodeComplete& Delegate)
{
	TWeakPtr<FOnlinePurchaseBANull, ESPMode::ThreadSafe> WeakMe = AsShared();
	const FUniqueNetIdBANull& BANullUserId = static_cast<const FUniqueNetIdBANull&>(UserId);

	BANullSubsystem.ExecuteNextTick([BANullUserId, WeakMe, RedeemCodeRequest, Delegate]
	{
		FOnlinePurchaseBANullPtr StrongThis = WeakMe.Pin();
		if (StrongThis.IsValid())
		{
			UE_LOG_ONLINE(Log, TEXT("FOnlinePurchaseBANull::RedeemCode redeemed successfully"));

			// Cache this receipt
			TArray<FPurchaseReceipt>& UserReceipts = StrongThis->UserFakeReceipts.FindOrAdd(BANullUserId);
			FPurchaseReceipt& PurchaseReceipt = UserReceipts.Emplace_GetRef();
			PurchaseReceipt.AddReceiptOffer(MakeReceiptOfferEntry(BANullUserId, RedeemCodeRequest.Code, RedeemCodeRequest.Code));

			Delegate.ExecuteIfBound(FOnlineError(true), MakeShared<FPurchaseReceipt>(PurchaseReceipt));
		}
	});
}

void FOnlinePurchaseBANull::QueryReceipts(const FUniqueNetId& UserId, bool bRestoreReceipts, const FOnQueryReceiptsComplete& Delegate)
{
	const FUniqueNetIdBANull& BANullUserId = static_cast<const FUniqueNetIdBANull&>(UserId);
	if (!BANullUserId.IsValid())
	{
		BANullSubsystem.ExecuteNextTick([Delegate]
		{
			UE_LOG_ONLINE(Error, TEXT("FOnlinePurchaseBANull::QueryReceipts user is invalid"));

			Delegate.ExecuteIfBound(FOnlineError(TEXT("User is invalid")));
		});
		return;
	}

	BANullSubsystem.ExecuteNextTick([Delegate]
	{
		Delegate.ExecuteIfBound(FOnlineError(true));
	});
}

void FOnlinePurchaseBANull::GetReceipts(const FUniqueNetId& UserId, TArray<FPurchaseReceipt>& OutReceipts) const
{
	const FUniqueNetIdBANull& BANullUserId = static_cast<const FUniqueNetIdBANull&>(UserId);

	const TArray<FPurchaseReceipt>* FoundReceipts = UserFakeReceipts.Find(BANullUserId);
	if (FoundReceipts == nullptr)
	{
		OutReceipts.Empty();
	}
	else
	{
		OutReceipts = *FoundReceipts;
	}
}

void FOnlinePurchaseBANull::FinalizeReceiptValidationInfo(const FUniqueNetId& UserId, FString& InReceiptValidationInfo, const FOnFinalizeReceiptValidationInfoComplete& Delegate)
{

}
