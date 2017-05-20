/*
//  GameOverState.cpp
//  SDL Game Programming Book
//
//  Created by shaun mitchell on 17/02/2013.
//  Copyright (c) 2013 shaun mitchell. All rights reserved.
*/

#include "GameOverState.h"
#include "MainMenuState.h"
#include "PlayState.h"
#include "TextureManager.h"
#include "AnimatedGraphic.h"
#include "Game.h"
#include "MenuButton.h"
#include "InputHandler.h"
#include "StateParser.h"

const std::string GameOverState::s_gameOverID = "GAMEOVER";

void GameOverState::s_gameOverToMain() {
    TheGame::Instance()->getStateMachine()->changeState(new MainMenuState());
}

void GameOverState::s_restartPlay() {
    TheGame::Instance()->getStateMachine()->changeState(new PlayState());
}

void GameOverState::update() {
	// Handle button presses
	if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_RETURN) || InputHandler::Instance()->getButtonState(0, 0)) {
		if (currentBtn == 1) s_gameOverToMain();							// 1. Return to Main Menu
		else if (currentBtn == 2) s_restartPlay();							// 2. Restart The Game
	}
	else if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_ESCAPE) ||	// Press Esc key to
		InputHandler::Instance()->isKeyDown(SDL_SCANCODE_BACKSPACE) ||		// 2017/04/23 or backspace
		InputHandler::Instance()->getButtonState(0, 1)) {					// 2017/04/22 OR Gamepad button B
		s_gameOverToMain();													// Return to Main Menu
	}

	// If up key, or gamepad up pressed
	if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_UP) || InputHandler::Instance()->getAxisY(0, 1) < 0) {
		if (!pressed) setCurrentBtn(BUTTON_UP);
		pressed = true;
		std::cout << "currentButton " << currentBtn << std::endl;
	}
	//else pressed = false;

	// If down key, or gamepad down pressed
	if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_DOWN) || InputHandler::Instance()->getAxisY(0, 1) > 0) {
		if (!pressed) setCurrentBtn(BUTTON_DOWN);
		pressed = true;
		std::cout << "currentButton " << currentBtn << std::endl;
	}

	selectCurrentButton();

	// 2017/04/22 Leave 1/4 of a second before the button selector moves again
	if (SDL_GetTicks() > btnTimer + 250) {
		btnTimer = SDL_GetTicks();			// Reset time between button presses
		pressed = false;					// Reset ability to press button
	}

    if(m_loadingComplete && !m_gameObjects.empty()) {
        for(int i = 0; i < m_gameObjects.size(); i++) {
            m_gameObjects[i]->update();
        }
    }
}

void GameOverState::render(){
    if(m_loadingComplete && !m_gameObjects.empty()) {
        for(int i = 0; i < m_gameObjects.size(); i++) {
            m_gameObjects[i]->draw();
        }
    }

	Texture::Instance()->loadReadyText(Game::Instance()->getPlayerName() + " scored " + std::to_string(Game::Instance()->getTime()));
	Texture::Instance()->draw("readyID", 230, 450, 340, 20, Game::Instance()->getRenderer());	// 2017/04/22 Indicate to player that B button returns to game play	
}

bool GameOverState::onEnter() {
	numButtons = 2;								// 2017/04/24 There are 2 buttons in the state
	currentBtn = 1;								// Set the current button

    // parse the state
    StateParser stateParser;
    stateParser.parseState("assets/attack.xml", s_gameOverID, &m_gameObjects, &m_textureIDList);
    
    m_callbacks.push_back(0);
    m_callbacks.push_back(s_gameOverToMain);
    m_callbacks.push_back(s_restartPlay);
    
    // set the callbacks for menu items
    setCallbacks(m_callbacks);
    
    m_loadingComplete = true;
    
    std::cout << "entering GameOverState\n";
    return true;
}

bool GameOverState::onExit() {
    if(m_loadingComplete && !m_gameObjects.empty()) {
        for(int i = 0; i < m_gameObjects.size(); i++) {
            m_gameObjects[i]->clean();
            delete m_gameObjects[i];
        }
        
        m_gameObjects.clear();
    }
    
    std::cout << m_gameObjects.size();
    
    // clear the texture manager
    for(int i = 0; i < m_textureIDList.size(); i++) {
        TheTextureManager::Instance()->clearFromTextureMap(m_textureIDList[i]);
    }
    
    TheInputHandler::Instance()->reset();
    
    std::cout << "exiting GameOverState\n";
    return true;
}

void GameOverState::setCallbacks(const std::vector<Callback>& callbacks) {
    // go through the game objects
    for(int i = 0; i < m_gameObjects.size(); i++) {
        // if they are of type MenuButton then assign a callback based on the id passed in from the file
        if(dynamic_cast<MenuButton*>(m_gameObjects[i])) {
            MenuButton* pButton = dynamic_cast<MenuButton*>(m_gameObjects[i]);
            pButton->setCallback(callbacks[pButton->getCallbackID()]);
        }
    }
}

void GameOverState::selectCurrentButton() {
	if (!m_gameObjects.empty()) {													// If its not empty
		for (int i = 0; i < m_gameObjects.size(); i++) {							// Go through the game objects list
			if (dynamic_cast<MenuButton*>(m_gameObjects[i])) {						// if they are of type MenuButton then assign a callback based on the id passed in from the file
				MenuButton* pButton = dynamic_cast<MenuButton*>(m_gameObjects[i]);

				if (pButton->getCallbackID() == currentBtn) pButton->selected = true;
				else pButton->selected = false;
			}
		}
	}
}

