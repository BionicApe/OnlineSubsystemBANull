// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemBANullTypes.h"
#include "NboSerializer.h"

/**
 * Serializes data in network byte order form into a buffer
 */
class FNboSerializeToBufferBANull : public FNboSerializeToBuffer
{
public:
	/** Default constructor zeros num bytes*/
	FNboSerializeToBufferBANull() :
		FNboSerializeToBuffer(512)
	{
	}

	/** Constructor specifying the size to use */
	FNboSerializeToBufferBANull(uint32 Size) :
		FNboSerializeToBuffer(Size)
	{
	}

	/**
	 * Adds BANull session info to the buffer
	 */
 	friend inline FNboSerializeToBufferBANull& operator<<(FNboSerializeToBufferBANull& Ar, const FOnlineSessionInfoBANull& SessionInfo)
 	{
		check(SessionInfo.HostAddr.IsValid());
		// Skip SessionType (assigned at creation)
		Ar << *SessionInfo.SessionId;
		Ar << *SessionInfo.HostAddr;
		return Ar;
 	}

	/**
	 * Adds BANull Unique Id to the buffer
	 */
	friend inline FNboSerializeToBufferBANull& operator<<(FNboSerializeToBufferBANull& Ar, const FUniqueNetIdBANull& UniqueId)
	{
		Ar << UniqueId.UniqueNetIdStr;
		return Ar;
	}
};

/**
 * Class used to write data into packets for sending via system link
 */
class FNboSerializeFromBufferBANull : public FNboSerializeFromBuffer
{
public:
	/**
	 * Initializes the buffer, size, and zeros the read offset
	 */
	FNboSerializeFromBufferBANull(uint8* Packet,int32 Length) :
		FNboSerializeFromBuffer(Packet,Length)
	{
	}

	/**
	 * Reads BANull session info from the buffer
	 */
 	friend inline FNboSerializeFromBufferBANull& operator>>(FNboSerializeFromBufferBANull& Ar, FOnlineSessionInfoBANull& SessionInfo)
 	{
		check(SessionInfo.HostAddr.IsValid());
		// Skip SessionType (assigned at creation)
		SessionInfo.SessionId = FUniqueNetIdBANull::Create();
		Ar >> const_cast<FUniqueNetIdBANull&>(*SessionInfo.SessionId);
		Ar >> *SessionInfo.HostAddr;
		return Ar;
 	}

	/**
	 * Reads BANull Unique Id from the buffer
	 */
	friend inline FNboSerializeFromBufferBANull& operator>>(FNboSerializeFromBufferBANull& Ar, FUniqueNetIdBANull& UniqueId)
	{
		Ar >> UniqueId.UniqueNetIdStr;
		return Ar;
	}
};
