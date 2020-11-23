#pragma once

#include <stdint.h>
#include <stdbool.h>

#if defined(_WIN32)
#define TUNKIO_EXPORT __declspec(dllexport)
#define TUNKIO_CALLING_CONVENTION __cdecl
#else
#define TUNKIO_EXPORT __attribute__((visibility("default")))
#define TUNKIO_CALLING_CONVENTION
#endif

#if defined(__cplusplus)
extern "C"
{
#endif
	typedef enum
	{
		FileWipe = 'f',
		DirectoryWipe = 'd',
		DriveWipe = 'D'
	} TunkioTargetType;

	typedef enum
	{
		ZeroFill = '0',
		OneFill = '1',
		CharacterFill = 'c',
		SentenceFill = 's',
		FileFill = 'f',
		RandomFill = 'r',
	} TunkioFillType;

	typedef enum
	{
		Open = 'O',
		Size = 'S',
		Write = 'W',
		Verify = 'V',
		Remove = 'R'
	} TunkioStage;

	typedef void(TunkioWipeStartedCallback)(
		void* context,
		uint16_t passes,
		uint64_t bytesToWritePerPass);

	typedef void(TunkioPassStartedCallback)(
		void* context,
		uint16_t pass);

	typedef bool(TunkioProgressCallback)(
		void* context,
		uint16_t pass,
		uint64_t bytesWritten); // Return false to cancel

	typedef void(TunkioPassCompletedCallback)(
		void* context,
		uint16_t pass);

	//typedef void(TunkioVerificationStarted)(
	//	uint64_t bytesToVerify);

	//typedef void(TunkioVerificationProgress)(
	//	uint64_t bytesVerified);

	//typedef void(TunkioVerificationFinished)(
	//	bool verificationSucceeded,
	//	uint64_t bytesVerified);

	typedef void(TunkioCompletedCallback)(
		void* context,
		uint16_t passes,
		uint64_t totalBytesWritten);

	typedef void(TunkioErrorCallback)(
		void* context,
		TunkioStage stage,
		uint16_t pass,
		uint64_t bytesWritten,
		uint32_t errorCode);

	struct TunkioHandle
	{
		int32_t Unused;
	};

	TUNKIO_EXPORT struct TunkioHandle* TUNKIO_CALLING_CONVENTION TunkioInitialize(
		void* context,
		const char* path,
		TunkioTargetType type);

	TUNKIO_EXPORT bool TUNKIO_CALLING_CONVENTION TunkioAddWipeRound(
		struct TunkioHandle*,
		TunkioFillType,
		bool verify,
		const char* optional);

	TUNKIO_EXPORT bool TUNKIO_CALLING_CONVENTION TunkioSetWipeStartedCallback(
		struct TunkioHandle*,
		TunkioWipeStartedCallback*);

	TUNKIO_EXPORT bool TUNKIO_CALLING_CONVENTION TunkioSetPassStartedCallback(
		struct TunkioHandle*,
		TunkioPassStartedCallback*);

	TUNKIO_EXPORT bool TUNKIO_CALLING_CONVENTION TunkioSetProgressCallback(
		struct TunkioHandle*,
		TunkioProgressCallback*);

	TUNKIO_EXPORT bool TUNKIO_CALLING_CONVENTION TunkioSetPassCompletedCallback(
		struct TunkioHandle*,
		TunkioPassCompletedCallback*);

	TUNKIO_EXPORT bool TUNKIO_CALLING_CONVENTION TunkioSetWipeCompletedCallback(
		struct TunkioHandle*,
		TunkioCompletedCallback*);

	TUNKIO_EXPORT bool TUNKIO_CALLING_CONVENTION TunkioSetErrorCallback(
		struct TunkioHandle*,
		TunkioErrorCallback*);

	TUNKIO_EXPORT bool TUNKIO_CALLING_CONVENTION TunkioSetRemoveAfterFill(
		struct TunkioHandle*,
		bool remove);

	TUNKIO_EXPORT bool TUNKIO_CALLING_CONVENTION TunkioRun(
		struct TunkioHandle*);

	TUNKIO_EXPORT void TUNKIO_CALLING_CONVENTION TunkioFree(
		struct TunkioHandle*);

#if defined(__cplusplus)
}
#endif