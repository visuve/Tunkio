#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <uchar.h>

#if defined(_WIN32)
#define KUURA_EXPORT __declspec(dllexport)
#define KUURA_CALLING_CONVENTION __cdecl
#else
#define KUURA_EXPORT __attribute__((visibility("default")))
#define KUURA_CALLING_CONVENTION
#endif

#if defined(__cplusplus)
extern "C"
{
#endif
	struct KuuraHandle
	{
		int32_t Unused;
	};

	enum KuuraTargetType
	{
		FileWipe = 'f',
		DirectoryWipe = 'd',
		DriveWipe = 'D'
	};

	enum KuuraFillType
	{
		ZeroFill = '0',
		OneFill = '1',
		ByteFill = 'b',
		SequenceFill = 's',
		FileFill = 'f',
		RandomFill = 'r',
	};

	enum KuuraStage
	{
		Open = 'O',
		Unmount = 'U',
		Size = 'S',
		Write = 'W',
		Verify = 'V',
		Delete = 'D'
	};

	typedef void(KuuraWipeStartedCallback)(
		void* context,
		uint16_t passes,
		uint64_t bytesToWritePerPass);

	typedef void(KuuraPassStartedCallback)(
		void* context,
		uint16_t pass);

	typedef bool(KuuraProgressCallback)(
		void* context,
		uint16_t pass,
		uint64_t bytesWritten); // Return false to cancel

	typedef void(KuuraPassCompletedCallback)(
		void* context,
		uint16_t pass);

	typedef void(KuuraWipeCompletedCallback)(
		void* context,
		uint16_t passes,
		uint64_t totalBytesWritten);

	typedef void(KuuraErrorCallback)(
		void* context,
		enum KuuraStage stage,
		uint16_t pass,
		uint64_t bytesWritten,
		uint32_t errorCode);

	KUURA_EXPORT struct KuuraHandle* KUURA_CALLING_CONVENTION KuuraInitialize(
		const char* path,
		enum KuuraTargetType type,
		bool removeAfterWipe,
		void* context);

	KUURA_EXPORT bool KUURA_CALLING_CONVENTION KuuraAddWipeRound(
		struct KuuraHandle*,
		enum KuuraFillType,
		bool verify,
		const char* optional);

	KUURA_EXPORT void KUURA_CALLING_CONVENTION KuuraSetWipeStartedCallback(
		struct KuuraHandle*,
		KuuraWipeStartedCallback*);

	KUURA_EXPORT void KUURA_CALLING_CONVENTION KuuraSetPassStartedCallback(
		struct KuuraHandle*,
		KuuraPassStartedCallback*);

	KUURA_EXPORT void KUURA_CALLING_CONVENTION KuuraSetProgressCallback(
		struct KuuraHandle*,
		KuuraProgressCallback*);

	KUURA_EXPORT void KUURA_CALLING_CONVENTION KuuraSetPassCompletedCallback(
		struct KuuraHandle*,
		KuuraPassCompletedCallback*);

	KUURA_EXPORT void KUURA_CALLING_CONVENTION KuuraSetWipeCompletedCallback(
		struct KuuraHandle*,
		KuuraWipeCompletedCallback*);

	KUURA_EXPORT void KUURA_CALLING_CONVENTION KuuraSetErrorCallback(
		struct KuuraHandle*,
		KuuraErrorCallback*);

	KUURA_EXPORT bool KUURA_CALLING_CONVENTION KuuraRun(
		struct KuuraHandle*);

	KUURA_EXPORT void KUURA_CALLING_CONVENTION KuuraFree(
		struct KuuraHandle*);

#if defined(__cplusplus)
}
#endif