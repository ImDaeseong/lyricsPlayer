#include "stdafx.h"
#include "FmodPlayer.h"

CFmodPlayer::CFmodPlayer()
{
	system = 0;
    sound = 0;
    channel = 0;

	unsigned int version;
    void  *extradriverdata = 0;

	result = FMOD::System_Create(&system);
	result = system->getVersion(&version);
	result = system->init(32, FMOD_INIT_NORMAL, extradriverdata);   
}

CFmodPlayer::~CFmodPlayer()
{
	result = sound->release();
	result = system->close(); 
    result = system->release();
}

BOOL CFmodPlayer::Open(CString strFile)
{
	if( channel != NULL )
		channel->stop();
	
	if( sound != NULL )
		sound->release();

	result = system->createStream(strFile, FMOD_DEFAULT, 0, &sound);
	if (result != FMOD_OK) return FALSE;
	return TRUE;
}

BOOL CFmodPlayer::Play()
{
	result = system->playSound(sound, 0, false, &channel);
	if (result != FMOD_OK) return FALSE;
	return TRUE;
}

BOOL CFmodPlayer::Stop()
{
	bool isPlaying = false;
	if (channel)
		result = channel->isPlaying(&isPlaying);
		
	if (isPlaying) 
		result = channel->stop();
	else 
		result = system->playSound(sound, 0, false, &channel);

	if (result != FMOD_OK) return FALSE;
	return TRUE;
}

BOOL CFmodPlayer::Pause()
{
	if( channel != NULL )
	{
		bool paused;
		result = channel->getPaused(&paused);
		result = channel->setPaused(!paused);
	}
	if (result != FMOD_OK) return FALSE;
	return TRUE;
}

BOOL CFmodPlayer::IsPlaying() 
{
	bool isPlaying = false;
	if(channel)
	{
		result = channel->isPlaying(&isPlaying);
		if (result != FMOD_OK) return FALSE;
	}
	return isPlaying;
}

void CFmodPlayer::SetMute(bool bMute)
{
	if (channel)
		result = channel->setMute(bMute);
}

void CFmodPlayer::SetVolume(float fVolume)
{
	if (channel)
		result = channel->setVolume(fVolume);
}

float CFmodPlayer::GetPosition()
{
	unsigned int pos;
	if( channel != NULL )
	{	
		result = channel->getPosition(&pos, FMOD_TIMEUNIT_MS);
		if (result != FMOD_OK) return 0;
	}
	return pos;//return pos / 1000.f;
}

float CFmodPlayer::GetRunningTime()
{
	unsigned int len = 0;
	if( sound != NULL )
	{
		result = sound->getLength(&len, FMOD_TIMEUNIT_MS);
	    if (result != FMOD_OK) return 0;
	}
	return len;//return float(len) / 1000.0f;
}

void CFmodPlayer::SetPosition(int pos) 
{
	if( channel != NULL )
	{
		result = channel->setPosition(pos, FMOD_TIMEUNIT_MS);//result = channel->setPosition( ((unsigned int)pos * 1000), FMOD_TIMEUNIT_MS );
	}
}

void CFmodPlayer::SkipBack()
{
	unsigned int pos;
	if( channel != NULL )
	{	
		result = channel->getPosition(&pos, FMOD_TIMEUNIT_MS);
		if (result == FMOD_OK)
		{
			pos -= 1000;//pos -= 10000;
			result = channel->setPosition(pos, FMOD_TIMEUNIT_MS);
		}
	}
}

void CFmodPlayer::SkipForward()
{
	unsigned int pos;
	if( channel != NULL )
	{	
		result = channel->getPosition(&pos, FMOD_TIMEUNIT_MS);
		if (result == FMOD_OK)
		{
			pos += 1000;//pos += 10000;
			result = channel->setPosition(pos, FMOD_TIMEUNIT_MS);
		}
	}
}

CString CFmodPlayer::GetCurrentTimeDisplay()
{
	CString strCurTime = "";

	if( channel != NULL )
	{	
		unsigned int pos;
		result = channel->getPosition(&pos, FMOD_TIMEUNIT_MS);  
		strCurTime.Format("%02d:%02d", pos / 1000 / 60, pos / 1000 % 60);
	}
	return strCurTime;
}

CString CFmodPlayer::GetTotalTimeDisplay()
{
	CString strTotalTime = "";
	
	if( sound != NULL )
	{
		unsigned int len = 0;
		result = sound->getLength(&len, FMOD_TIMEUNIT_MS);
		strTotalTime.Format("%02d:%02d", len / 1000 / 60, len / 1000 % 60);
	}
	return strTotalTime;
}