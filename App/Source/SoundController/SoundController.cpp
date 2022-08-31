/**
 CSoundController
 @brief A class which manages the sound objects
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "SoundController.h"

#include <iostream>
using namespace std;

/**
 @brief Constructor
 */
CSoundController::CSoundController(void)
	: cSoundEngine(NULL)
	, vec3dfListenerPos(vec3df(0, 0, 0))
	, vec3dfListenerDir(vec3df(0, 0, 1))
	,musicsfx(NULL)
	, musicVol( 0.001f) 
	//, currentlyPlaying(0)
{
}

/**
 @brief Destructor
 */
CSoundController::~CSoundController(void)
{
	// Iterate through the entityMap
	for (std::map<int, CSoundInfo*>::iterator it = soundMap.begin(); it != soundMap.end(); ++it)
	{
		// If the value/second was not deleted elsewhere, then delete it here
		if (it->second != NULL)
		{
			delete it->second;
			it->second = NULL;
		}
	}

	// Remove all elements in entityMap
	soundMap.clear();

	// Since we have already dropped the ISoundSource, then we don't need to delete the cSoundEngine
	//// Clear the sound engine
	//if (cSoundEngine)
	//{
	//	cSoundEngine->drop();
	//	cSoundEngine = NULL;
	//}
}

/**
 @brief Initialise this class instance
 @return A bool value. true is this class instance was initialised, else false
 */
bool CSoundController::Init(void)
{
	currentSongDuration = 0;
	// Initialise the sound engine with default parameters
	cSoundEngine = createIrrKlangDevice(ESOD_WIN_MM, ESEO_MULTI_THREADED);
	if (cSoundEngine == NULL)
	{
		cout << "Unable to initialise the IrrKlang sound engine" << endl;
		return false;
	}
	return true;

	//musicVol = 0.001f;
	//musicVol = 1.0f;
	musicVol = 0.001f;


	//currentlyPlaying = 0;


}

/**
 @brief Load a sound
 @param filename A string variable storing the name of the file to read from
 @param ID A const int variable which will be the ID of the iSoundSource in the map
 @param bPreload A const bool variable which indicates if this iSoundSource will be pre-loaded into memory now.
 @param bIsLooped A const bool variable which indicates if this iSoundSource will have loop playback.
 @param eSoundType A SOUNDTYPE enum variable which states the type of sound
 @param vec3dfSoundPos A vec3df variable which contains the 3D position of the sound
 @return A bool value. True if the sound was loaded, else false.
 */
bool CSoundController::LoadSound(	string filename,
									const int ID,
									const bool bPreload,
									const bool bIsLooped,
									CSoundInfo::SOUNDTYPE eSoundType,
									vec3df vec3dfSoundPos)
{
	// Load the sound from the file
	ISoundSource* pSoundSource = cSoundEngine->addSoundSourceFromFile(filename.c_str(),
																	E_STREAM_MODE::ESM_NO_STREAMING, 
																	bPreload);


	// Trivial Rejection : Invalid pointer provided
	if (pSoundSource == nullptr)
	{
		cout << "Unable to load sound " << filename.c_str() << endl;
		return false;
	}

	// Force the sound source not to have any streaming
	pSoundSource->setForcedStreamingThreshold(-1);

	// Clean up first if there is an existing Entity with the same name
	RemoveSound(ID);

	// Add the entity now
	CSoundInfo* cSoundInfo = new CSoundInfo();
	if (eSoundType == CSoundInfo::SOUNDTYPE::_2D)
		cSoundInfo->Init(ID, pSoundSource, bIsLooped);
	else
		cSoundInfo->Init(ID, pSoundSource, bIsLooped, eSoundType, vec3dfSoundPos);

	// Set to soundMap
	soundMap[ID] = cSoundInfo;



	//ISound* SoundInfo = new ISound();
	///*if (eSoundType == ISound::type)
	//	cSoundInfo->Init(ID, pSoundSource, bIsLooped);
	//else
	//	cSoundInfo->Init(ID, pSoundSource, bIsLooped, eSoundType, vec3dfSoundPos);*/
	//soundMap_2[ID] = cSoundInfo;


	

	return true;
}

/**
 @brief Play a sound by its ID
 @param ID A const int variable which will be the ID of the iSoundSource in the map
 */
void CSoundController::PlaySoundByID(const int ID)
{
	CSoundInfo* pSoundInfo = GetSound(ID);
	if (!pSoundInfo)
	{
		cout << "Sound #" << ID << " is not playable." << endl;
		return;
	}
	else if (cSoundEngine->isCurrentlyPlaying(pSoundInfo->GetSound()))
	{
		cout << "Sound #" << ID << " is currently being played." << endl;
		return;
	}

	if (pSoundInfo->GetSoundType() == CSoundInfo::SOUNDTYPE::_2D)
	{
		cSoundEngine->play2D(	pSoundInfo->GetSound(), 
								pSoundInfo->GetLoopStatus());
	}
	else if (pSoundInfo->GetSoundType() == CSoundInfo::SOUNDTYPE::_3D)
	{
		cSoundEngine->setListenerPosition(vec3dfListenerPos, vec3dfListenerDir);
		cSoundEngine->play3D(	pSoundInfo->GetSound(), 
								pSoundInfo->GetPosition(), 
								pSoundInfo->GetLoopStatus());
	}
}

ISound* CSoundController::PlaySoundByID_2(const int ID)
{
	ISound* pSound = nullptr;
	//this is set in a way to return null when sound is playing, hence volumedecrease() register it as null
	CSoundInfo* pSoundInfo = GetSound(ID);
	if (!pSoundInfo)
	{
		//cout << "Sound #" << ID << " is not playable." << endl;
		return pSound;
	}
	else if (cSoundEngine->isCurrentlyPlaying(pSoundInfo->GetSound()))
	{
		//cout << "Sound #" << ID << " is currently being played." << endl;
		return pSound;
	}

	if (pSoundInfo->GetSoundType() == CSoundInfo::SOUNDTYPE::_2D)
	{
		pSound = cSoundEngine->play2D(pSoundInfo->GetSound(),
			pSoundInfo->GetLoopStatus(), false, true, true);
	}
	else if (pSoundInfo->GetSoundType() == CSoundInfo::SOUNDTYPE::_3D)
	{
		cSoundEngine->setListenerPosition(vec3dfListenerPos, vec3dfListenerDir);
		pSound = cSoundEngine->play3D(pSoundInfo->GetSound(),
			pSoundInfo->GetPosition(),
			pSoundInfo->GetLoopStatus());
	}

	return pSound;

}

/**
 @brief Stop playing a sound by its ID
 @param ID A const int variable which will be the ID of the iSoundSource in the map
 */
void CSoundController::StopPlayByID(const int ID)
{
	CSoundInfo* pSoundInfo = GetSound(ID);
	if (!pSoundInfo)
	{
		//cout << "Sound #" << ID << " is not playable." << endl;
		return;
	}
	else if (cSoundEngine->isCurrentlyPlaying(pSoundInfo->GetSound()))
	{
		// Stop a sound playback
		cSoundEngine->stopAllSoundsOfSoundSource(pSoundInfo->GetSound());
	}
}

void CSoundController::StopAllSound()
{
	cSoundEngine->stopAllSounds();
}

/**
 @brief Increase Master volume
 @return true if successfully increased volume, else false
 */
bool CSoundController::MasterVolumeIncrease(void)
{
	// Get the current volume
	float fCurrentVolume = cSoundEngine->getSoundVolume() + 0.01f;
	// Check if the maximum volume has been reached
	if (fCurrentVolume > 1.0f)
		fCurrentVolume = 1.0f;

	// Update the Mastervolume
	cSoundEngine->setSoundVolume(fCurrentVolume);
	cout << "MasterVolumeIncrease: fCurrentVolume = " << fCurrentVolume << endl;

	return true;
}

/**
 @brief Decrease Master volume
 @return true if successfully decreased volume, else false
 */
bool CSoundController::MasterVolumeDecrease(void)
{
	// Get the current volume
	float fCurrentVolume = cSoundEngine->getSoundVolume() - 0.01f;
	// Check if the minimum volume has been reached
	if (fCurrentVolume < 0.0f)
		fCurrentVolume = 0.0f;

	// Update the Mastervolume
	cSoundEngine->setSoundVolume(fCurrentVolume);
	cout << "MasterVolumeDecrease: fCurrentVolume = " << fCurrentVolume << endl;

	return true;
}

bool CSoundController::SetMasterVolume(float volume)
{
	// Get the current volume
	float fNewVolume = volume;
	// Check if the minimum volume has been reached
	if (fNewVolume < 0.0f)
		fNewVolume = 0.0f;

	if (fNewVolume > 1.0f)
		fNewVolume = 1.0f;

	// Update the Mastervolume
	cSoundEngine->setSoundVolume(fNewVolume);
	cout << "MasterVolumeDecrease: fNewVolume = " << fNewVolume << endl;

	return true;
}


/**
 @brief Increase volume of a ISoundSource
 @param ID A const int variable which contains the ID of the iSoundSource in the map
 @return true if successfully decreased volume, else false
 */
bool CSoundController::VolumeIncrease(const int ID)
{
	// Get the ISoundSource
	ISoundSource* pISoundSource = GetSound(ID)->GetSound();
	if (pISoundSource == nullptr)
	{
		return false;
	}

	// Get the current volume
	float fCurrentVolume = pISoundSource->getDefaultVolume();

	// Check if the maximum volume has been reached
	if (fCurrentVolume >= 1.0f)
	{
		pISoundSource->setDefaultVolume(1.0f);
		return false;
	}

	// Increase the volume by 10%
	pISoundSource->setDefaultVolume(fCurrentVolume + 0.01f);

	return true;
}

/**
 @brief Decrease volume of a ISoundSource
 @param ID A const int variable which contains the ID of the iSoundSource in the map
 @return true if successfully decreased volume, else false
 */
bool CSoundController::VolumeDecrease(const int ID)
{
	// Get the ISoundSource
	ISoundSource* pISoundSource = GetSound(ID)->GetSound();
	if (pISoundSource == nullptr)
	{
		return false;
	}

	// Get the current volume
	float fCurrentVolume = pISoundSource->getDefaultVolume();

	// Check if the minimum volume has been reached
	if (fCurrentVolume <= 0.0f)
	{
		pISoundSource->setDefaultVolume(0.0f);
		return false;
	}

	// Decrease the volume by 10%
	pISoundSource->setDefaultVolume(fCurrentVolume - 0.01f);

	return true;
}

bool CSoundController::SetVolume(const int ID, float volume)
{
	// Get the ISoundSource
	ISoundSource* pISoundSource = GetSound(ID)->GetSound();
	if (pISoundSource == nullptr)
	{
		return false;
	}

	// Get the current volume
	float fNewVolume = volume;

	if (fNewVolume >= 1.0f)
	{
		pISoundSource->setDefaultVolume(1.0f);
		return false;
	}
	// Check if the minimum volume has been reached
	if (fNewVolume <= 0.0f)
	{
		pISoundSource->setDefaultVolume(0.0f);
		return false;
	}

	// Decrease the volume by 10%
	pISoundSource->setDefaultVolume(fNewVolume);

	return true;
}

bool CSoundController::AddToPlaylist(const int ID)
{
	/*if (soundMap[ID]!=nullptr)
	{*/
		musicPlaylist.push_back(ID);
		return true;
	/*}
	else
	{
		cout << "Track not in soundMap" << endl;
		return false;
	}*/
}




//REAGAN'S UPDATE
void CSoundController::Update(const double dElapsedTime, float distance)
{

	//cSoundController->LoadSound(FileSystem::getPath("Sounds\\Adventure.ogg"), 1, true);
	//cSoundController->LoadSound(FileSystem::getPath("Sounds\\Advance.ogg"), 2, true);
	//cSoundController->LoadSound(FileSystem::getPath("Sounds\\The Bullet Bill Express.ogg"), 3, true);

	//
	//cSoundController->AddToPlaylist(3);
	//cSoundController->AddToPlaylist(2);
	//cSoundController->AddToPlaylist(1);

	static int currentlyPlaying = 0;

	if (musicPlaylist.size() > 0)
	{
		currentSongDuration += (dElapsedTime * 1000);
		ISound* currentSong = PlaySoundByID_2(musicPlaylist[currentlyPlaying]);
		if (!cSoundEngine->isCurrentlyPlaying(GetSound(musicPlaylist[currentlyPlaying])->GetSound()))
		{
			PlaySoundByID(musicPlaylist[currentlyPlaying]);
			//currentSong = PlaySoundByID_2(musicPlaylist[currentlyPlaying]);

		}

		//SWITCH SOUNDTRACK DEPENDING ON DISTANCE
		if (distance < 5.f)
		{
			if (currentSong != nullptr)
			{
				currentSong->setVolume(0.0f);
				currentSong = nullptr;
				currentlyPlaying = 1;
				currentSongDuration = 0;
				currentSong = PlaySoundByID_2(musicPlaylist[currentlyPlaying]);
			}
		}
		else
		{
			if (currentSong != nullptr)
			{
				currentSong->setVolume(0.0f);
				currentSong = nullptr;
				currentlyPlaying = 0;
				currentSongDuration = 0;
				currentSong = PlaySoundByID_2(musicPlaylist[currentlyPlaying]);
			}
		}

		//IF SONG REACHES THE LENGTH
		/*if (currentSongDuration >= GetSound(musicPlaylist[currentlyPlaying])->GetSound()->getPlayLength())
		{
			StopPlayByID(musicPlaylist[currentlyPlaying]);
			if (currentlyPlaying <= musicPlaylist.size() - 1)
				currentlyPlaying++;
			else
				currentlyPlaying = 0;

			currentSongDuration = 0;
		}*/


		

		if (currentSong != nullptr)
		{
			musicsfx = currentSong;
		}
		if (musicsfx != nullptr)
		{
			musicsfx->setVolume(musicVol);
		}
	}
}
//


//int CSoundController::return_currentMusic()
//{
//	return musicPlaylist[currentmusic];
//}

void CSoundController::setsoundvol(float vol)
{
}

void CSoundController::setmusicvol(float vol)
{
	musicVol = vol;
}

float CSoundController::returnmusicvol()
{
	return musicVol;
}


// For 3D sounds only
/**
 @brief Set Listener position
 @param x A const float variable containing the x-component of a position
 @param y A const float variable containing the y-component of a position
 @param z A const float variable containing the z-component of a position
 */
void CSoundController::SetListenerPosition(const float x, const float y, const float z)
{
	vec3dfListenerPos.set(x, y, z);
}

/**
 @brief Set Listener direction
 @param x A const float variable containing the x-component of a direction
 @param y A const float variable containing the y-component of a direction
 @param z A const float variable containing the z-component of a direction
 */
void CSoundController::SetListenerDirection(const float x, const float y, const float z)
{
	vec3dfListenerDir.set(x, y, z);
}


//MY OWN VERSION OF SETVOLUME - REAGAN
void CSoundController::setVolume_2(int soundID)
{
	ISound* snd = PlaySoundByID_2(soundID);
	if (snd != nullptr)
	{
		musicsfx = snd;
	}
		
	if (musicsfx != nullptr)
	{
		musicsfx->setVolume(musicVol);
		//vol = max(0, vol);
	}
}
//



/**
 @brief Get an sound from this map
 @param ID A const int variable which will be the ID of the iSoundSource in the map
 @return A CSoundInfo* variable
 */
CSoundInfo* CSoundController::GetSound(const int ID)
{
	if (soundMap.count(ID) != 0)
		return soundMap[ID];

	return nullptr;
}



/**
 @brief Remove an sound from this map
 @param ID A const int variable which will be the ID of the iSoundSource in the map
 @return true if the sound was successfully removed, else false
 */
bool CSoundController::RemoveSound(const int ID)
{
	CSoundInfo* pSoundInfo = GetSound(ID);
	if (pSoundInfo != nullptr)
	{
		delete pSoundInfo;
		soundMap.erase(ID);
		return true;
	}
	return false;
}

/**
@brief Get the number of sounds in this map
@return The number of sounds currently stored in the Map
*/
int CSoundController::GetNumOfSounds(void) const
{
	return soundMap.size();
}