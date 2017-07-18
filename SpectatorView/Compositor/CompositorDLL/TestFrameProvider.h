#pragma once
#include "IFrameProvider.h"
#include "HologramQueue.h"

class TestFrameProvider :
	public IFrameProvider
{
	IFrameProvider * const _impl;
	bool _isInitialized = false;

	bool TryInitialize() 
	{
		if (!_initializeCalled)
			return false;
		if (_isInitialized)
			return true;
		return _isInitialized = _impl == nullptr || SUCCEEDED(_impl->Initialize(_colorSRV, _outputTexture));
	}

	bool _initializeCalled = false;
	ID3D11ShaderResourceView *_colorSRV = nullptr;
	ID3D11Texture2D *_outputTexture = nullptr;

public:
	explicit TestFrameProvider(IFrameProvider* frame_provider)
		: _impl(frame_provider)
	{
	}

	HRESULT Initialize(ID3D11ShaderResourceView* colorSRV, ID3D11Texture2D* outputTexture) override 
	{ 
		_initializeCalled = true;
		_colorSRV = colorSRV;
		_outputTexture = outputTexture;
		TryInitialize();
		return S_OK;
	}

	LONGLONG GetTimestamp() override 
	{
		if (TryInitialize())
			return _impl->GetTimestamp();
		LARGE_INTEGER result;
		if (!QueryPerformanceCounter(&result))
			return INVALID_TIMESTAMP;
		return result.QuadPart;
	}

	LONGLONG GetDurationHNS() override 
	{
		if (TryInitialize())
			return _impl->GetDurationHNS();
		return (LONGLONG)((1.0f / 30.0f) * QPC_MULTIPLIER);
	}

	void Update() override 
	{
		if (TryInitialize())
			_impl->Update();
	}

	bool IsEnabled() override { return _initializeCalled; }

	bool SupportsOutput() override 
	{ 
		if (TryInitialize())
			return _impl->SupportsOutput();
		return false; 
	}

	void Dispose() override 
	{
		if (_isInitialized)
			_impl->Dispose();
	}

	bool OutputYUV() override 
	{ 
		if (TryInitialize())
			return _impl->OutputYUV();
		return false; 
	}

	bool IsVideoFrameReady() override 
	{ 
		if (TryInitialize())
			return _impl->IsVideoFrameReady();
		return true; 
	}

	virtual ~TestFrameProvider() 
	{
		delete _impl;
	}
};