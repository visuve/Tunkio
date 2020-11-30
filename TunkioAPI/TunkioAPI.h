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
	struct TunkioHandle
	{
		int32_t Unused;
	};

	enum TunkioTargetType
	{
		FileWipe = 'f',
		DirectoryWipe = 'd',
		DriveWipe = 'D'
	};

	enum TunkioFillType
	{
		ZeroFill = '0',
		OneFill = '1',
		CharacterFill = 'c',
		SentenceFill = 's',
		FileFill = 'f',
		RandomFill = 'r',
	};

	enum TunkioStage
	{
		Open = 'O',
		Unmount = 'U',
		Size = 'S',
		Write = 'W',
		Rewind = 'R',
		Verify = 'V',
		Delete = 'D'
	};

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

	typedef void(TunkioWipeCompletedCallback)(
		void* context,
		uint16_t passes,
		uint64_t totalBytesWritten);

	typedef void(TunkioErrorCallback)(
		void* context,
		enum TunkioStage stage,
		uint16_t pass,
		uint64_t bytesWritten,
		uint32_t errorCode);

	TUNKIO_EXPORT struct TunkioHandle* TUNKIO_CALLING_CONVENTION TunkioInitialize(
		const char* path,
		enum TunkioTargetType type,
		bool removeAfterWipe,
		void* context);

	TUNKIO_EXPORT bool TUNKIO_CALLING_CONVENTION TunkioAddWipeRound(
		struct TunkioHandle*,
		enum TunkioFillType,
		bool verify,
		const char* optional);

	TUNKIO_EXPORT void TUNKIO_CALLING_CONVENTION TunkioSetWipeStartedCallback(
		struct TunkioHandle*,
		TunkioWipeStartedCallback*);

	TUNKIO_EXPORT void TUNKIO_CALLING_CONVENTION TunkioSetPassStartedCallback(
		struct TunkioHandle*,
		TunkioPassStartedCallback*);

	TUNKIO_EXPORT void TUNKIO_CALLING_CONVENTION TunkioSetProgressCallback(
		struct TunkioHandle*,
		TunkioProgressCallback*);

	TUNKIO_EXPORT void TUNKIO_CALLING_CONVENTION TunkioSetPassCompletedCallback(
		struct TunkioHandle*,
		TunkioPassCompletedCallback*);

	TUNKIO_EXPORT void TUNKIO_CALLING_CONVENTION TunkioSetWipeCompletedCallback(
		struct TunkioHandle*,
		TunkioWipeCompletedCallback*);

	TUNKIO_EXPORT void TUNKIO_CALLING_CONVENTION TunkioSetErrorCallback(
		struct TunkioHandle*,
		TunkioErrorCallback*);

	TUNKIO_EXPORT bool TUNKIO_CALLING_CONVENTION TunkioRun(
		struct TunkioHandle*);

	TUNKIO_EXPORT void TUNKIO_CALLING_CONVENTION TunkioFree(
		struct TunkioHandle*);

#if defined(__cplusplus)
}
#endif
