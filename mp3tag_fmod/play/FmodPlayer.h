#pragma once

#include <fmod.h>
#include <fmod.hpp>
#include <fmod_common.h>
#include <fmod_errors.h>
#pragma comment (lib, "fmod_vc.lib")

class CFmodPlayer
{
public:
	CFmodPlayer();
	virtual ~CFmodPlayer();

	BOOL Open(CString strFile);
	BOOL Pause();
	BOOL Stop();
	BOOL Play();
	BOOL IsPlaying();
	void SetMute(bool bMute);
    void SetVolume(float fVolume); 
	float GetPosition();
	float GetRunningTime();
	void SetPosition(int pos);
	void SkipBack();
	void SkipForward();
	CString GetCurrentTimeDisplay();
	CString GetTotalTimeDisplay();

private:
	FMOD::System *system;
    FMOD::Sound *sound;
    FMOD::Channel *channel;
	FMOD_RESULT result;
};