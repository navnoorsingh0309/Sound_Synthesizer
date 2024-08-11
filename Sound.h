#pragma once
#pragma comment(lib, "winmm.lib")
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include <condition_variable>
#include "Sound.h"
#include <functional>
#include "Sound.h"
#include <wx\wx.h>
#include <exception>

#define M_PI 3.14159265358979323846

template<typename T>
class Sound
{
public:
	Sound()
	{

	}

	// Destroy
	bool Destroy()
	{
		return false;
	}

	// Gettting Number of sound devices
	int getNumberOfSoundDevices()
	{
		return nSoundDevices;
	}
	// Getting all devices vector
	std::vector<std::wstring> getSoundDevices()
	{
		// Getting number of devices
		nSoundDevices = waveOutGetNumDevs();

		// Loading all devices in a vector
		WAVEOUTCAPS woc;
		for (int i = 0; i < nSoundDevices; i++)
		{
			if (waveOutGetDevCaps(i, &woc, sizeof(WAVEOUTCAPS)) == S_OK)
				allSoundDevices.push_back(woc.szPname);
		}
		return allSoundDevices;
	}

	// Initializing Device to use
	bool initializeDevice(std::wstring device, unsigned int nChannelsP, unsigned int nSamplesPerSec, unsigned int nBlocksP, unsigned int nBlockSamplesP)
	{
		audioBitDepthInBytes = sizeof(T);
		nChannels = nChannelsP;
		samplingRate = nSamplesPerSec;
		nBlocks = nBlocksP;
		nBlockSamples = nBlockSamplesP;
		nFreeBlocks = nBlocks;
		waveHeader = nullptr;
		memoryBlocks = nullptr;

		// Looking for device
		auto d = std::find(allSoundDevices.begin(), allSoundDevices.end(), device);
		// If found
		if (d != allSoundDevices.end())
		{
			//Finding Device id
			int deviceID = distance(allSoundDevices.begin(), d);
			// Wave format we will use
			WAVEFORMATEX format;
			// Pulse code emodulation
			format.wFormatTag = WAVE_FORMAT_PCM;
			format.nChannels = nChannels;
			format.nSamplesPerSec = nSamplesPerSec;
			format.wBitsPerSample = sizeof(T) * 8;
			format.cbSize = 0;
			format.nBlockAlign = format.nChannels * format.wBitsPerSample / 8;
			format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;
			// If failed to open device
			if (waveOutOpen(&wave_out, deviceID, &format, NULL, (DWORD_PTR)this, CALLBACK_FUNCTION) != MMSYSERR_NOERROR)
			{
				return false;
			}

			// Allocating Memory
			// Allocating and zeroing header memory blocks
			waveHeader = new WAVEHDR[nBlocks];
			std::memset(waveHeader, 0, sizeof(WAVEHDR) * nBlocks);
			// Allocating and zeroing memory blocks for audio
			memoryBlocks = new T[nBlocks * nBlockSamples];
			std::memset(memoryBlocks, 0, sizeof(T) * nBlocks * nBlockSamples);

			// Use wave headers to hold ptr to chunk data and link to chunk in bytes
			for (int i = 0; i < nBlocks; i++)
			{
				waveHeader[i].dwBufferLength = nBlockSamples * sizeof(T);
				waveHeader[i].lpData = (LPSTR)(memoryBlocks + (i * nBlockSamples));
			};			
		}
	}
	// Initializing Volume
	bool initializeVolume(DWORD volume)
	{
		// Setting up VOlume if opening success
		if (waveOutSetVolume(wave_out, volume) != MMSYSERR_NOERROR)
			return false;
	}

	//Converts frequency (Hz) to angular velocity
	static double Hz2W(double dHertz)
	{
		return dHertz * 2.0 * M_PI;
	}
	// Setting Wave function
	void setWaveFunc(double(*func)(double))
	{
		waveFunction = func;
	}
	// Start Playing
	void Play()
	{
		play = true;
		nBlocks--;
		audioThread = std::thread(&Sound::MainThread, this);
		// Set the thread priority to a higher level
		SetThreadPriority(audioThread.native_handle(), THREAD_PRIORITY_TIME_CRITICAL);
		audioThread.detach();
	}
	void Stop()
	{
		play = false;
	}

	// Setting wave function
	void setWaveFunction(double(*func)(double))
	{
		waveFunction = func;
	}
	unsigned int nBlocks = 8;
private:
	double(*waveFunction)(double);

	int nSoundDevices = 0;
	std::vector<std::wstring> allSoundDevices;
	std::wstring currentDevice;
	HWAVEOUT wave_out;
	WAVEHDR* waveHeader;
	// Memory blocks
	T* memoryBlocks;
	// Blocks
	std::atomic<unsigned int> nFreeBlocks;
	unsigned int currentBlock = 0;
	unsigned int samplingRate  = 44100;
	unsigned int nChannels = 1;
	unsigned int nBlockSamples = 512;

	std::thread audioThread;

	unsigned int audioBitDepthInBytes;
	std::atomic<bool> play = 0;
	std::atomic<double> dTime = 0.0;

	double clip(double dSample, double dMax)
	{
		if (dSample >= 0.0)
			return fmin(dSample, dMax);
		else
			return fmax(dSample, -dMax);
	}
	// Main Audio Thread
	void MainThread()
	{
		dTime = 0.0;
		double dTimeStep = 1.0 / (double)this->samplingRate;
		T nMaxSample = (T)pow(2, (sizeof(T) * 8) - 1) - 1;
		double dMaxSample = (double)nMaxSample;
		while (play)
		{
			/* if (nFreeBlocks <= 0)
			{
				std::unique_lock<std::mutex> ul(audioMutex);
				isBlockFree.wait(ul);
			} */
			nFreeBlocks--;
			// Prepare block for processing
			if (waveHeader[currentBlock].dwFlags & WHDR_PREPARED)
				waveOutUnprepareHeader(wave_out, &waveHeader[currentBlock], sizeof(WAVEHDR));

			T newSample;
			for (unsigned int n = 0; n < nBlockSamples; n++)
			{
				newSample = (T)(waveFunction(dTime) * dMaxSample);
				
				memoryBlocks[currentBlock * nBlockSamples + n] = newSample;
				dTime = dTime + dTimeStep;
			}

			waveOutPrepareHeader(wave_out, &waveHeader[currentBlock],
				sizeof(waveHeader[currentBlock]));
			// Write to wave outlet
			waveOutWrite(wave_out, &waveHeader[currentBlock], sizeof(waveHeader[currentBlock]));

			currentBlock++;
			currentBlock %= nBlocks;
		}
	}
};