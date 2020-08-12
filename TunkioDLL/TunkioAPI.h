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
		Device = 'D'
	} TunkioTarget;

	typedef enum
	{
		Zeroes = '0',
		Ones = '1',
		Random = 'r'
	} TunkioMode;

	typedef void(*TunkioStartedCallback)(uint64_t bytesToWrite);
	typedef bool(*TunkioProgressCallback)(uint64_t bytesWritten); // Return false to cancel
	typedef void(*TunkioErrorCallback)(uint32_t error, uint64_t bytesWritten);
	typedef void(*TunkioCompletedCallback)(uint64_t bytesWritten);

	struct TunkioHandle
	{
		int32_t Unused;
	};

	struct TunkioString
	{
		uint32_t Length;
		const char* Data;
	};

	struct TunkioCallbacks
	{
		TunkioStartedCallback StartedCallback;
		TunkioProgressCallback ProgressCallback;
		TunkioErrorCallback ErrorCallback;
		TunkioCompletedCallback CompletedCallback;
	};

	struct TunkioOptions
	{
		TunkioTarget Target;
		TunkioMode Mode;
		bool Remove;
		struct TunkioCallbacks Callbacks;
		struct TunkioString Path;
	};

	TUNKIO_EXPORT struct TunkioHandle* TUNKIO_CALLING_CONVENTION TunkioCreate(const struct TunkioOptions*);
	TUNKIO_EXPORT bool TUNKIO_CALLING_CONVENTION TunkioRun(struct TunkioHandle*);
	TUNKIO_EXPORT void TUNKIO_CALLING_CONVENTION TunkioFree(struct TunkioHandle*);

#if defined(__cplusplus)
};
#endif