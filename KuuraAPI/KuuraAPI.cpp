#include "KuuraAPI-PCH.hpp"
#include "KuuraAPI.h"
#include "KuuraComposer.hpp"

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

	instance->Callbacks.*field = value;
}

KuuraHandle* KUURA_CALLING_CONVENTION KuuraInitialize(void* context)
{
	return reinterpret_cast<KuuraHandle*>(new Kuura::Composer(context));
}

bool KUURA_CALLING_CONVENTION KuuraAddTarget(
	struct KuuraHandle* handle,
	const char* path,
	enum KuuraTargetType type,
	bool removeAfterWipe)
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

	return instance->AddWorkload(path, type, removeAfterWipe);
}

bool KUURA_CALLING_CONVENTION KuuraAddWipeRound(
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

	return instance->AddFiller(type, verify, optional);
}

void KUURA_CALLING_CONVENTION KuuraSetWipeStartedCallback(
	KuuraHandle* handle,
	KuuraWipeStartedCallback* callback)
{
	Set(handle, &Kuura::CallbackContainer::WipeStartedCallback, callback);
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

void KUURA_CALLING_CONVENTION KuuraSetWipeCompletedCallback(
	KuuraHandle* handle,
	KuuraWipeCompletedCallback* callback)
{
	Set(handle, &Kuura::CallbackContainer::WipeCompletedCallback, callback);
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
