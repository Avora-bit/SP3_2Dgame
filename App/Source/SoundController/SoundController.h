/**
 CSoundController
 @brief A class which manages the sound objects
 By: Toh Da Jun
 Date: Mar 2020
 */
#pragma once

// Include SingletonTemplate
#include <DesignPatterns\SingletonTemplate.h>

// Include GLEW
#include <includes/irrKlang.h>
using namespace irrklang;
#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

// Include string
#include <string>
// Include map storage
#include <map>
#include <vector>
using namespace std;

// Include SoundInfo class; it stores the sound and other information
#include "SoundInfo.h"



class CSoundController : public CSingletonTemplate<CSoundController>
{
	friend CSingletonTemplate<CSoundController>;
public:
	// Initialise this class instance
	bool Init(void);

	// Load a sound
	bool LoadSound(	string filename, 
					const int ID,
					const bool bPreload = true,
					const bool bIsLooped = false,
					CSoundInfo::SOUNDTYPE eSoundType = CSoundInfo::SOUNDTYPE::_2D,
					vec3df vec3dfSoundPos = vec3df(0.0f, 0.0f, 0.0f));

	// Play a sound by its ID
	void PlaySoundByID(const int ID);


	ISound* PlaySoundByID_2(const int ID);

	// Stop a sound by its ID
	void StopPlayByID(const int ID);

	void StopAllSound();

	// Increase Master volume
	bool MasterVolumeIncrease(void);
	// Decrease Master volume
	bool MasterVolumeDecrease(void);
	// Set Master volume
	bool SetMasterVolume(float volume);

	// Increase volume of a ISoundSource
	bool VolumeIncrease(const int ID);
	// Decrease volume of a ISoundSource
	bool VolumeDecrease(const int ID);

	bool SetVolume(const int ID, float volume);

	bool AddToPlaylist(const int ID);



	void Update(const double dElapsedTime, float distance);

	// For 3D sounds only
	// Set Listener position
	void SetListenerPosition(const float x, const float y, const float z);
	// Set Listener direction
	void SetListenerDirection(const float x, const float y, const float z);

	
	void setVolume_2(int soundID);

	void setsoundvol(float vol);
	void setmusicvol(float vol);
	float returnmusicvol();

	

protected:
	// Constructor
	CSoundController(void);

	// Destructor
	virtual ~CSoundController(void);


	ISound* musicsfx;
	float soundVol;
	float musicVol;
	// Get an sound from this map
	CSoundInfo* GetSound(const int ID);


	// Remove an sound from this map
	bool RemoveSound(const int ID);
	// Get the number of sounds in this map
	int GetNumOfSounds(void) const;

	// The handler to the irrklang Sound Engine
	ISoundEngine* cSoundEngine;

	std::vector<int> musicPlaylist;
	//std::vector<ISound> musicPlaylist;

	// The map of all the entity created
	std::map<int, CSoundInfo*> soundMap;

	std::map<int, ISound*> soundMap_2;


	double currentSongDuration;

	// For 3D sound only: Listener position
	vec3df vec3dfListenerPos;
	// For 3D sound only: Listender view direction
	vec3df vec3dfListenerDir;
};

