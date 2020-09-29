#pragma once

#include <stdint.h>
#include <stdbool.h>

#if defined(_WIN32) || defined(_WIN64)
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
		Random = 'R'
	} TunkioFillMode;

	typedef enum
	{
		Open = 'o',
		Size = 's',
		Write = 'w',
		Remove = 'r',
	} TunkioStage;

	typedef void(TunkioStartedCallback)(uint64_t bytesToWrite);
	typedef bool(TunkioProgressCallback)(uint64_t bytesWritten); // Return false to cancel
	typedef void(TunkioErrorCallback)(TunkioStage stage, uint64_t bytesWritten, uint32_t errorCode);
	typedef void(TunkioCompletedCallback)(uint64_t bytesWritten);

	struct TunkioHandle
	{
		int32_t Unused;
	};

	TUNKIO_EXPORT struct TunkioHandle* TUNKIO_CALLING_CONVENTION TunkioInitialize(const char* path, TunkioTargetType type);

	TUNKIO_EXPORT bool TUNKIO_CALLING_CONVENTION TunkioSetFillMode(struct TunkioHandle*, TunkioFillMode mode);
	TUNKIO_EXPORT bool TUNKIO_CALLING_CONVENTION TunkioSetStartedCallback(struct TunkioHandle*, TunkioStartedCallback*);
	TUNKIO_EXPORT bool TUNKIO_CALLING_CONVENTION TunkioSetProgressCallback(struct TunkioHandle*, TunkioProgressCallback*);
	TUNKIO_EXPORT bool TUNKIO_CALLING_CONVENTION TunkioSetErrorCallback(struct TunkioHandle*, TunkioErrorCallback*);
	TUNKIO_EXPORT bool TUNKIO_CALLING_CONVENTION TunkioSetCompletedCallback(struct TunkioHandle*, TunkioCompletedCallback*);
	TUNKIO_EXPORT bool TUNKIO_CALLING_CONVENTION TunkioSetRemoveAfterFill(struct TunkioHandle*, bool remove);

	TUNKIO_EXPORT bool TUNKIO_CALLING_CONVENTION TunkioRun(struct TunkioHandle*);
	TUNKIO_EXPORT void TUNKIO_CALLING_CONVENTION TunkioFree(struct TunkioHandle*);

#if defined(__cplusplus)
};
#endif