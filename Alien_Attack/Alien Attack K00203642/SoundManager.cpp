/*
    SoundManager.cpp
    SDL Game Programming Book
  
    Created by shaun mitchell on 26/03/2013.
    Copyright (c) 2013 shaun mitchell. All rights reserved.
	
	Modified by:	Joe O'Regan
	Student Number:	K00203642

	Done:
		2017/03/16	Added pausePlayMusic() function to pause the original music in the game
					by pressing the m key. Music can also be turned on / off in the settings menu
					Added playMusic() function to play the music selecting a random song from a list of songs
*/

#include "SoundManager.h"

SoundManager* SoundManager::s_pInstance;

SoundManager::SoundManager() {
    Mix_OpenAudio(22050, AUDIO_S16, 2, (4096 / 2));

	//currentSong = rand() % NUMBER_OF_SONGS + 1;		// 2017/03/16 choose random song
}

SoundManager::~SoundManager() {
    Mix_CloseAudio();
}

bool SoundManager::load(std::string fileName, std::string id, sound_type type) {
    if(type == SOUND_MUSIC) {
        Mix_Music* pMusic = Mix_LoadMUS(fileName.c_str());
        if(pMusic == 0) {
            std::cout << "Could not load music: ERROR - " << Mix_GetError() << std::endl;
            return false;
        }
        
        m_music[id] = pMusic;
        return true;
    }
    else if(type == SOUND_SFX) {
        Mix_Chunk* pChunk = Mix_LoadWAV(fileName.c_str());
        if(pChunk == 0) {
            std::cout << "Could not load SFX: ERROR - " << Mix_GetError() << std::endl;
            return false;
        }
        
        m_sfxs[id] = pChunk;
        return true;
    }
    return false;
}

void SoundManager::playMusic(std::string id, int loop) {
	Mix_PlayMusic(m_music[id], loop);
}
void SoundManager::playMusic(int song) {
	//if (song == 0) song = rand() % NUMBER_OF_SONGS + 1;	// random number between 1 and 3
	if (song == 0) currentSong = rand() % 3 + 1;			// random number between 1 and 3
	else currentSong = song;

	//currentSong = 1;

	// 2017/04/23 Check song 
	//if (currentSong > NUMBER_OF_SONGS) currentSong = 1;	// giving error
	if (currentSong > 3) currentSong = 1;
	else if (currentSong < 1) currentSong = NUMBER_OF_SONGS;

	std::string id;
	//if (song == 1) {
	if (currentSong == 1) {
		id = "music1"; 
		//currentSong = 1;
	}
	else if (currentSong == 2) {
		id = "music2";
		//currentSong = 2;
	}
	else if (currentSong == 3) {
		id = "music3";
		//currentSong = 3;
	}

	Mix_PlayMusic(m_music[id], -1); ////////////////////////////COMMENT OUT MUSIC PLAYING
	//Mix_PlayMusic(m_music["music3"], -1);
}

/*
	2017/04/23	Skip the audion tracks forwards and backwards
*/
void SoundManager::trackForwards() {
	currentSong++;
	//if (currentSong > NUMBER_OF_SONGS) currentSong = 1;
	//else if (currentSong < 1) currentSong = NUMBER_OF_SONGS;
	playMusic(currentSong);
}
void SoundManager::trackBackwards() {
	currentSong--;
	//if (currentSong > NUMBER_OF_SONGS) currentSong = 1;
	//else if (currentSong < 1) currentSong = NUMBER_OF_SONGS;
	playMusic(currentSong);

}
/* 2017/03/16 Function to stop the music playing */
void SoundManager::pausePlayMusic() {
	if (Mix_PlayingMusic() == 0) {							// If there is no music playing
		playMusic(currentSong);
		//playMusic("music1", -1);
		//int song = playMusic();
		//Audio::Instance()->identifyTrack(song);
		//infoMessage("Music Play");
		std::cout << "Music Play" << std::endl;
	}
	else {													// If music is being played
		if (Mix_PausedMusic() == 1) {						// Check if the music is paused
			Mix_ResumeMusic();								// Resume music
			//infoMessage("Music Play");
			std::cout << "Music Play" << std::endl;
		}
		else {												// If the music is playing
			Mix_PauseMusic();								// Pause the music
			//infoMessage("Music Paused");
			std::cout << "Music Paused" << std::endl;
		}
	}
	//Mix_HaltMusic();
	//std::cout << "Music stopped" << std::endl;
}

void SoundManager::playSound(std::string id, int loop) {
    Mix_PlayChannel(-1, m_sfxs[id], loop);
}

void SoundManager::volumeDownMusic() {
	volumeMusic -= 8;
	if (volumeMusic <= 0) volumeMusic = 0;
	Mix_VolumeMusic(volumeMusic);											// Set music volume down MIX_MAX_VOLUME = 128 BALANCES OUT THE VOLUMES A BIT BETTER
}
void SoundManager::volumeUpMusic() {
	volumeMusic += 8;
	if (volumeMusic >= MIX_MAX_VOLUME) volumeMusic = MIX_MAX_VOLUME;		// Volume goes up to 128
	Mix_VolumeMusic(volumeMusic);											// Set music volume up
}
void SoundManager::volumeDownEffects() {
	volumeEffects -= 8;
	if (volumeEffects <= 0) volumeEffects = 0;
	
	setFXvolumes(volumeEffects);											// Volume has to be set for individual chunks
}
void SoundManager::volumeUpEffects() {
	volumeEffects += 8;
	if (volumeEffects >= MIX_MAX_VOLUME) volumeEffects = MIX_MAX_VOLUME;	// Volume goes up to 128
	
	setFXvolumes(volumeEffects);
}

// Volume has to be set for individual chunks
void SoundManager::setFXvolumes(int v) {
	Mix_VolumeChunk(m_sfxs["explode"], v);									// Set effects volume up
	Mix_VolumeChunk(m_sfxs["shoot"], v);									// Set effects volume up
	Mix_VolumeChunk(m_sfxs["fire"], v);										// Set effects volume up
	Mix_VolumeChunk(m_sfxs["puFX"], v);										// Set effects volume up
	Mix_VolumeChunk(m_sfxs["blastFX"], v);									// Set effects volume up
	Mix_VolumeChunk(m_sfxs["buttonFX"], v);									// 2017/04/28 Set effects volume up - Menu effects could have separate volume controls
}