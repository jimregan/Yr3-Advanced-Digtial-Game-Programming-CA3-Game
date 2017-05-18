/*
//  PlayState.cpp
//  SDL Game Programming Book
//
//  Created by shaun mitchell on 09/02/2013.
//  Copyright (c) 2013 shaun mitchell. All rights reserved.

	Modified by Joe O'Regan
		2017/02/16 Added Timer
*/

#include <iostream>
#include "PlayState.h"
#include "GameOverState.h"
#include "PauseState.h"
#include "Game.h"
#include "InputHandler.h"
#include "LevelParser.h"
#include "Level.h"
#include "BulletHandler.h"

const std::string PlayState::s_playID = "PLAY";
std::stringstream timeText;

unsigned int lastTime = 0, currentTime, countdownTimer = 0;

void PlayState::update() {
	if (m_loadingComplete && !m_exiting) {
		if (TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_ESCAPE)) {
			TheGame::Instance()->getStateMachine()->pushState(new PauseState());
		}

		/*        if(TheInputHandler::Instance()->getButtonState(0, 8))
		//        {
		//            TheGame::Instance()->getStateMachine()->pushState(new PauseState());
		/        }
		*/
		TheBulletHandler::Instance()->updateBullets();

		if (TheGame::Instance()->getPlayerLives() == 0) {
			TheGame::Instance()->getStateMachine()->changeState(new GameOverState());
		}

		if (pLevel != 0) {
			pLevel->update();
		}
	}
}

void PlayState::render() {

	if (m_loadingComplete) {
		if (pLevel != 0) {
			pLevel->render();
		}

		gameTimer();	// Calculate and display the game timer

		for (int i = 0; i < TheGame::Instance()->getPlayerLives(); i++) {
			TheTextureManager::Instance()->drawFrame("lives", i * 30, 0, 32, 30, 0, 0, TheGame::Instance()->getRenderer(), 0.0, 255);
		}

		// id, x, y, width, height, currentRow, currentFrame, pRenderer, angle, alpha, flip
		TheTextureManager::Instance()->drawFrame("test", (SCREEN_WIDTH - 120) / 2, 0, 120, 30, 0, 0, TheGame::Instance()->getRenderer(), 0.0, 255);
		TheTextureManager::Instance()->drawFrame("timer", 400, 0, 150, 30, 0, 0, TheGame::Instance()->getRenderer(), 0.0, 255);

		TheBulletHandler::Instance()->drawBullets();
	}
}

bool PlayState::onEnter() {
	TheGame::Instance()->setPlayerLives(3);


	LevelParser levelParser;
	pLevel = levelParser.parseLevel(TheGame::Instance()->getLevelFiles()[TheGame::Instance()->getCurrentLevel() - 1].c_str());

	TheTextureManager::Instance()->load("assets/bullet1.png", "bullet1", TheGame::Instance()->getRenderer());
	TheTextureManager::Instance()->load("assets/bullet2.png", "bullet2", TheGame::Instance()->getRenderer());
	TheTextureManager::Instance()->load("assets/bullet3.png", "bullet3", TheGame::Instance()->getRenderer());
	TheTextureManager::Instance()->load("assets/lives.png", "lives", TheGame::Instance()->getRenderer());		// Lives in top left corner
																												//bool loadFromRenderedText(std::string textureText, SDL_Color textColor, TTF_Font* font, SDL_Renderer* rend, bool textWrapped = false);
																												//TheTextureManager::Instance()->load("assets/lives.png", "test", TheGame::Instance()->getRenderer());		// Lives in top left corner
	TheTextureManager::Instance()->loadFromRenderedText("Level 1", "test", { 255, 255, 255, 255 }, TTF_OpenFont("Fonts/Retro.ttf", 20), TheGame::Instance()->getRenderer());		// Lives in top left corner

	if (pLevel != 0) {
		m_loadingComplete = true;
	}

	std::cout << "entering PlayState\n";
	return true;
}

bool PlayState::onExit() {
	m_exiting = true;

	TheInputHandler::Instance()->reset();
	TheBulletHandler::Instance()->clearBullets();

	countdownTimer = 0;

	std::cout << "exiting PlayState\n";
	return true;
}


void PlayState::gameTimer() {
	timeText.str("");

	currentTime = SDL_GetTicks();

	if (currentTime > lastTime + 1000) {
		lastTime = currentTime;

		countdownTimer++;	// Increment countdown timer

		std::cout << "Timer: " << countdownTimer << std::endl;
	}

	timeText << "Time: " << countdownTimer;						// Set the game timer

	TheTextureManager::Instance()->loadFromRenderedText(timeText.str().c_str(), "timer", { 255, 255, 255, 255 }, TTF_OpenFont("Fonts/Retro.ttf", 12), TheGame::Instance()->getRenderer());		// Lives in top left corner
}