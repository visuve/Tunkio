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
		File = 'f',
		Directory = 'd',
		Drive = 'D'
	} TunkioTargetType;

	typedef enum
	{
		Zeroes = '0',
		Ones = '1',
		Character = 'c',
		String = 's',
		Random = 'r'
	} TunkioFillType;

	typedef enum
	{
		Open = 'O',
		Size = 'S',
		Write = 'W',
		Verify = 'V',
		Remove = 'R'
	} TunkioStage;

	typedef void(TunkioStartedCallback)(
		uint16_t totalIterations,
		uint64_t bytesToWritePerIteration);

	typedef void(TunkioIterationStartedCallback)(
		uint16_t currentIteration);

	typedef bool(TunkioProgressCallback)(
		uint16_t currentIteration,
		uint64_t bytesWritten); // Return false to cancel

	typedef void(TunkioErrorCallback)(
		TunkioStage stage,
		uint16_t currentIteration,
		uint64_t bytesWritten,
		uint32_t errorCode);

	typedef void(TunkioIterationCompleteCallback)(
		uint16_t currentIteration);

	//typedef void(TunkioVerificationStarted)(
	//	uint64_t bytesToVerify);

	//typedef void(TunkioVerificationProgress)(
	//	uint64_t bytesVerified);

	//typedef void(TunkioVerificationFinished)(
	//	bool verificationSucceeded,
	//	uint64_t bytesVerified);

	typedef void(TunkioCompletedCallback)(
		uint16_t totalIterations,
		uint64_t totalBytesWritten);

	struct TunkioHandle
	{
		int32_t Unused;
	};

	TUNKIO_EXPORT struct TunkioHandle* TUNKIO_CALLING_CONVENTION TunkioInitialize(
		const char* path,
		TunkioTargetType type);

	TUNKIO_EXPORT bool TUNKIO_CALLING_CONVENTION TunkioAddWipeRound(
		struct TunkioHandle*,
		TunkioFillType,
		bool verify,
		const char* optional);

	TUNKIO_EXPORT bool TUNKIO_CALLING_CONVENTION TunkioSetStartedCallback(
		struct TunkioHandle*,
		TunkioStartedCallback*);

	TUNKIO_EXPORT bool TUNKIO_CALLING_CONVENTION TunkioSetIterationStartedCallback(
		struct TunkioHandle*,
		TunkioIterationStartedCallback*);

	TUNKIO_EXPORT bool TUNKIO_CALLING_CONVENTION TunkioSetProgressCallback(
		struct TunkioHandle*,
		TunkioProgressCallback*);

	TUNKIO_EXPORT bool TUNKIO_CALLING_CONVENTION TunkioSetErrorCallback(
		struct TunkioHandle*,
		TunkioErrorCallback*);

	TUNKIO_EXPORT bool TUNKIO_CALLING_CONVENTION TunkioSetIterationCompletedCallback(
		struct TunkioHandle*,
		TunkioIterationCompleteCallback*);

	TUNKIO_EXPORT bool TUNKIO_CALLING_CONVENTION TunkioSetCompletedCallback(
		struct TunkioHandle*,
		TunkioCompletedCallback*);

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