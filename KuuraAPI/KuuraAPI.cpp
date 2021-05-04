#include "KuuraAPI-PCH.hpp"
#include "KuuraAPI.h"
#include "KuuraComposer.hpp"
#include "../KuuraResources/KuuraVersion.h"

template <typename T>
void Set(KuuraHandle* handle, T(Kuura::CallbackContainer::* field), T value)
{
	assert(handle);
	assert(field);
	assert(value);

	auto instance = reinterpret_cast<Kuura::Composer*>(handle);

	if (!instance)
	{
		return;
	}

	instance->Set(field, value);
}

KuuraHandle* KUURA_CALLING_CONVENTION KuuraInitialize(void* context)
{
	return reinterpret_cast<KuuraHandle*>(new Kuura::Composer(context));
}

bool KUURA_CALLING_CONVENTION KuuraAddTarget(
	struct KuuraHandle* handle,
	enum KuuraTargetType type,
	const KuuraChar* path,
	bool removeAfterOverwrite)
{
	if (!path)
	{
		return false;
	}

	auto instance = reinterpret_cast<Kuura::Composer*>(handle);

	if (!instance)
	{
		return false;
	}

	return instance->AddTarget(type, path, removeAfterOverwrite);
}

bool KUURA_CALLING_CONVENTION KuuraAddPass(
	struct KuuraHandle* handle,
	KuuraFillType type,
	bool verify,
	const char* optional)
{
	const auto instance = reinterpret_cast<Kuura::Composer*>(handle);

	if (!instance)
	{
		return false;
	}

	return instance->AddPass(type, verify, optional);
}

void KUURA_CALLING_CONVENTION KuuraSetOverwriteStartedCallback(
	KuuraHandle* handle,
	KuuraOverwriteStartedCallback* callback)
{
	Set(handle, &Kuura::CallbackContainer::OverwriteStartedCallback, callback);
}

void KUURA_CALLING_CONVENTION KuuraSetPassStartedCallback(
	KuuraHandle* handle,
	KuuraPassStartedCallback* callback)
{
	Set(handle, &Kuura::CallbackContainer::PassStartedCallback, callback);
}

void KUURA_CALLING_CONVENTION KuuraSetProgressCallback(
	KuuraHandle* handle,
	KuuraProgressCallback* callback)
{
	Set(handle, &Kuura::CallbackContainer::ProgressCallback, callback);
}

void KUURA_CALLING_CONVENTION KuuraSetPassCompletedCallback(
	KuuraHandle* handle,
	KuuraPassCompletedCallback* callback)
{
	Set(handle, &Kuura::CallbackContainer::PassCompletedCallback, callback);
}

void KUURA_CALLING_CONVENTION KuuraSetOverwriteCompletedCallback(
	KuuraHandle* handle,
	KuuraOverwriteCompletedCallback* callback)
{
	Set(handle, &Kuura::CallbackContainer::OverwriteCompletedCallback, callback);
}

void KUURA_CALLING_CONVENTION KuuraSetErrorCallback(
	KuuraHandle* handle,
	KuuraErrorCallback* callback)
{
	Set(handle, &Kuura::CallbackContainer::ErrorCallback, callback);
}

bool KUURA_CALLING_CONVENTION KuuraRun(KuuraHandle* handle)
{
	const auto instance = reinterpret_cast<Kuura::Composer*>(handle);

	if (!instance)
	{
		return false;
	}

	return instance->Run();
}

void KUURA_CALLING_CONVENTION KuuraFree(KuuraHandle* handle)
{
	if (handle)
	{
		delete reinterpret_cast<Kuura::Composer*>(handle);
	}
}

const char* KUURA_CALLING_CONVENTION KuuraVersion()
{
	return KuuraVersionString;
}
