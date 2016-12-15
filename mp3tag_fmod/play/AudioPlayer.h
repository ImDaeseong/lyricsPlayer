#pragma once

#include <Mmsystem.h>
#pragma comment(lib, "Winmm.lib")
#include <digitalv.h>

class CAudioPlayer
{
public:
	void SetPosition(DWORD dwPosition);
	DWORD GetPosition();
	DWORD GetMode();
	DWORD GetRunningTime();
	BOOL Open(char *pFile);
	BOOL Open(CString strFile);
	BOOL Pause();
	BOOL Stop();
	BOOL Play();

	void OpenDrive();
	void CloseDrive();

	void SkipForward (int nSeconds);
	void SkipBack (int nSeconds);


protected:
	MCI_OPEN_PARMS m_mciOpenParms;
	MCI_PLAY_PARMS m_mciPlayParms;
	MCI_RECORD_PARMS m_mciRecordParms;
	MCI_SAVE_PARMS m_mciSaveParms;
	DWORD m_dwDeviceID;
	DWORD m_nTotalTime;
	int m_nTimer;

public:
	int GetVolume();
	void SetVolume(DWORD dwVolume);
	CAudioPlayer();
	virtual ~CAudioPlayer();
};