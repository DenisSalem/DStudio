#include <iostream>
#include <SFML/Graphics.hpp>
#include "dsampler.hpp"
#include <cmath>

DSAMPLER_ACTION::DSAMPLER_ACTION(sf::RenderWindow * windowPointer, DSAMPLER_GFX * gfxPointer) {
	gfx = gfxPointer;
	gfx->plan = &(this->plan);
	window = windowPointer;
	mouseState = 0;
	plan = 0;
	currentActivePotentiometer = -1;	
}

void DSAMPLER_ACTION::LISTEN() {

	//Listen blur states
	for (int i = 0; i< LAYERS_COUNT; i++) {
		if (this->gfx->blurState[i] == 1 && this->gfx->blurPass[i] < 9) {
			this->gfx->blurPass[i]++;
		}
		if (this->gfx->blurState[i] == -1 && this->gfx->blurPass[i] > 0) {
			this->gfx->blurPass[i]--;
		}
	}

	//Listen for mouse or keyboard events
	sf::Event event;
	sf::Vector2u size = this->window->getSize();

	this->localPosition = sf::Mouse::getPosition( *(this->window));
	while( this->window->pollEvent(event)) {
		if ( event.type == event.MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
			// Once the left mouse button is pressed, we're looking for its position.
			if (
				( (float) this->localPosition.x / (float) size.x) >= 0.01875 && ( (float) this->localPosition.x / (float) size.x) <= 0.15625 &&
				( (float) this->localPosition.y / (float) size.y) >= 0.18686 && ( (float) this->localPosition.y / (float) size.y) <= 0.40404 &&
				this->plan == 0
			) {
				this->UPDATE_OPACITY(false, _DSAMPLER_POTENTIOMETER_AMP_);
				this->mouseState = 1;
				this->currentActivePotentiometer = _DSAMPLER_POTENTIOMETER_AMP_;
			}
			else if (
				( (float) this->localPosition.x / (float) size.x) >= 0.171875 && ( (float) this->localPosition.x / (float) size.x) <= 0.3 &&
				( (float) this->localPosition.y / (float) size.y) >= 0.18686 && ( (float) this->localPosition.y / (float) size.y) <= 0.40404 &&
				this->plan == 0
			) {
				this->UPDATE_OPACITY(false, _DSAMPLER_POTENTIOMETER_START_);
				this->mouseState = 1;
				this->currentActivePotentiometer = _DSAMPLER_POTENTIOMETER_START_;
			}
			else if (
				( (float) this->localPosition.x / (float) size.x) >= 0.33125 && ( (float) this->localPosition.x / (float) size.x) <= 0.453125 &&
				( (float) this->localPosition.y / (float) size.y) >= 0.18686 && ( (float) this->localPosition.y / (float) size.y) <= 0.40404 &&
				this->plan == 0
			) {
				this->UPDATE_OPACITY(false, _DSAMPLER_POTENTIOMETER_END_);
				this->mouseState = 1;
				this->currentActivePotentiometer = _DSAMPLER_POTENTIOMETER_END_;
			}
			else if (
				( (float) this->localPosition.x / (float) size.x) >= 0.484375 && ( (float) this->localPosition.x / (float) size.x) <= 0.60625 &&
				( (float) this->localPosition.y / (float) size.y) >= 0.18686 && ( (float) this->localPosition.y / (float) size.y) <= 0.40404 &&
				this->plan == 0
			) {
				this->UPDATE_OPACITY(false, _DSAMPLER_POTENTIOMETER_PITCH_);
				this->mouseState = 1;
				this->currentActivePotentiometer = _DSAMPLER_POTENTIOMETER_PITCH_;
			}
			else if (
				( (float) this->localPosition.x / (float) size.x) >= 0.625 && ( (float) this->localPosition.x / (float) size.x) <= 0.796875 &&
				( (float) this->localPosition.y / (float) size.y) >= 0.252525 && ( (float) this->localPosition.y / (float) size.y) <= 0.297979 &&
				this->plan == 0
			) {
				this->UPDATE_OPACITY(false, _DSAMPLER_BUTTON_OPEN_);
				this->mouseState = 1;
			}
			else if (
				( (float) this->localPosition.x / (float) size.x) >= 0.93125 && ( (float) this->localPosition.x / (float) size.x) <= 0.978125 &&
				( (float) this->localPosition.y / (float) size.y) >= 0.03535 && ( (float) this->localPosition.y / (float) size.y) <= 0.11111 &&
				this->plan == 1
			) {
				this->UPDATE_OPACITY(false, _DSAMPLER_BUTTON_CLOSE_);
				this->mouseState = 1;
			}
			else if (
				( (float) this->localPosition.x / (float) size.x) >= 0.35 && ( (float) this->localPosition.x / (float) size.x) <= 0.478125 &&
				( (float) this->localPosition.y / (float) size.y) >= 0.83333 && ( (float) this->localPosition.y / (float) size.y) <= 0.929292 &&
				this->plan == 2
			) {
				this->UPDATE_OPACITY(false, _DSAMPLER_BUTTON_SURE_);
				this->mouseState = 1;
			}
			else if (
				( (float) this->localPosition.x / (float) size.x) >= 0.525 && ( (float) this->localPosition.x / (float) size.x) <= 0.715625 &&
				( (float) this->localPosition.y / (float) size.y) >= 0.83333 && ( (float) this->localPosition.y / (float) size.y) <= 0.929292 &&
				this->plan == 2
			) {
				this->UPDATE_OPACITY(false, _DSAMPLER_BUTTON_HELL_NO_);
				this->mouseState = 1;
			}
		}
		// same here, depending of the plan and the position action will be different. 
		else if (event.type == event.MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
			if (
				( (float) this->localPosition.x / (float) size.x) >= 0.93125 && ( (float) this->localPosition.x / (float) size.x) <= 0.978125 &&
				( (float) this->localPosition.y / (float) size.y) >= 0.03535 && ( (float) this->localPosition.y / (float) size.y) <= 0.11111 &&
				this->plan == 1
			) {
				this->UPDATE_BLUR(true,1);
			}
			else if (
				( (float) this->localPosition.x / (float) size.x) >= 0.62813 && ( (float) this->localPosition.x / (float) size.x) <= 0.79063 &&
				( (float) this->localPosition.y / (float) size.y) >= 0.25253 && ( (float) this->localPosition.y / (float) size.y) <= 0.29798 &&
				this->plan == 0
			) {
				this->UPDATE_BLUR(false, 1);
			}
			else if (
				( (float) this->localPosition.x / (float) size.x) >= 0.35 && ( (float) this->localPosition.x / (float) size.x) <= 0.478125 &&
				( (float) this->localPosition.y / (float) size.y) >= 0.83333 && ( (float) this->localPosition.y / (float) size.y) <= 0.929292 &&
				this->plan == 2
			) {
				std::cout << "Goodbye friendly user!\n";
				this->window->close();
			}
			else if (
				( (float) this->localPosition.x / (float) size.x) >= 0.525 && ( (float) this->localPosition.x / (float) size.x) <= 0.715625 &&
				( (float) this->localPosition.y / (float) size.y) >= 0.83333 && ( (float) this->localPosition.y / (float) size.y) <= 0.929292 &&
				this->plan == 2
			) {
				this->UPDATE_BLUR(true, 1);
				this->UPDATE_BLUR(true, 2);
			}
			this->UPDATE_OPACITY(true, -1);
			this->mouseState = 0;
			this->currentActivePotentiometer = -1;
		}
		else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
			if (this->plan > 0) {
				this->UPDATE_BLUR(true, 1);
				this->UPDATE_BLUR(true, 2);
			}
			else {
				this->UPDATE_BLUR(false, 2);
			}
		}	
		else if (event.type == sf::Event::Closed ) {
			this->window->close();
		}
	}
	if (this->mouseState == 1) {
		this->UPDATE_ROTATION(this->localPosition.x , this->localPosition.y, this->currentActivePotentiometer);
	}	
	
}

void DSAMPLER_ACTION::UPDATE_BLUR(bool restore, int plan) {
	sf::Vector2u size = this->window->getSize();
	if (!restore) {
		this->plan = plan;
		this->gfx->refreshPlan[plan] = 1;
		this->gfx->blurState[0]=1;
		this->gfx->blurState[plan]=-1;
	}
	else {
		this->plan = 0;
		this->gfx->blurState[0]=-1;
		this->gfx->blurState[plan]=1;
	}
}

void DSAMPLER_ACTION::UPDATE_ROTATION(int x, int y, int potentiometer) {
	sf::Vector2u size = this->window->getSize();
	float adj,opp;

	if (potentiometer == _DSAMPLER_POTENTIOMETER_AMP_ ) {
		adj = x -  (0.0875 * size.x);
		opp = -1 * (y - (0.29545 * size.y));
	}
	else if (potentiometer == _DSAMPLER_POTENTIOMETER_START_) {
		adj = x -  (0.2375 * size.x);
		opp = -1 * (y - (0.29545 * size.y));
	}
	else if (potentiometer == _DSAMPLER_POTENTIOMETER_END_) {
		adj = x -  (0.3875 * size.x);
		opp = -1 * (y - (0.29545 * size.y));
	}
	else if (potentiometer == _DSAMPLER_POTENTIOMETER_PITCH_) {
		adj = x -  (0.54375 * size.x);
		opp = -1 * (y - (0.29545 * size.y));
	}
	float hyp = sqrt( adj*adj + opp*opp );
	float angle;
	if (adj < 0) 
		angle = asin(opp/hyp) * 57.296 + 40;
	else 
		angle = 90 - asin(opp/hyp) * 57.296 + 130;

	if (angle < 0)
		angle = 0;
	else if (angle >= 280)
		angle = 280;

	this->gfx->ROTATION(angle, potentiometer);
}

void DSAMPLER_ACTION::UPDATE_OPACITY(bool restore, int element) {
	if (restore) {
		for (int i = 0; i < SPRITES_COUNT; i++ ) {
			this->gfx->OPACITY(255, i);
		}
	}
	else {
		this->gfx->OPACITY(SPRITE_OPACITY, element);
	}
	this->gfx->refreshPlan[0] = true;
}
