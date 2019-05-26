#include <iostream>
#include <SFML/Graphics.hpp>
#include "dstudio.hpp"
#include "dsampler.hpp"
#include <ctime>


DSAMPLER_GFX::DSAMPLER_GFX() {
	error = 0;
	blurPass[0] = 0;
	blurPass[1] = 9;
	blurPass[2] = 9;
	blurState[0] = 0;
	blurState[1] = 0;
	blurState[2] = 0;

	this->SpriteButtons = new sf::Sprite[SPRITES_COUNT];
	this->Potentiometers = new DSTUDIO_POTENTIOMETER[4];
	this->PlansRenderTexture = new sf::RenderTexture[LAYERS_COUNT];
	this->PlansBlurRenderTexture = new sf::RenderTexture[LAYERS_COUNT];
	this->PlansRenderSprite = new sf::Sprite[LAYERS_COUNT];
	this->PlansBlurRenderSprite = new sf::Sprite[LAYERS_COUNT];

	if(!TemporaryTexture.create(320,198)) {
		std::cout << "Cannot create buffer render texture  lol :'(\n";
		return;
	}
	if(!MergeRenderTexture.create(320,198)) {
		std::cout << "Cannot create buffer render texture  lol :'(\n";
		return;
	}
	if(!NoiseRenderTexture.create(320,198)) {
		std::cout << "Cannot create buffer render texture  lol :'(\n";
		return;
	}
	for(int i=0; i<LAYERS_COUNT; i++) {
		if(!PlansRenderTexture[i].create(320,198)) {
			std::cout << "Cannot create buffer render texture  lol :'(\n";
			return;
		}
		if(!PlansBlurRenderTexture[i].create(320,198)) {
			std::cout << "Cannot create buffer render texture  lol :'(\n";
			return;
		}
	}

	if (!TextureTitle.loadFromFile("gfx/dsamplerTitle.png")) {
		std::cout << "Cannot load gfx/dsampleTitle.png :'(\n";
		return;
	}
	if (!TexturePotentiometerTitle.loadFromFile("gfx/dsamplerPotentiometerTitle0x00.png")) {
		std::cout << "Cannot load gfx/dsamplerPotentiometerTitle0x00.png :'(\n";
		return;
	}
	if (!TextureButtonOpen.loadFromFile("gfx/dsamplerButtonOpen.png")) {
		std::cout << "Cannot load gfx/dsamplerButtonOpen.png :'(\n";
		return;
	}
	if (!TextureButtonSave.loadFromFile("gfx/dsamplerButtonSave.png")) {
		std::cout << "Cannot load gfx/dsamplerButtonSave.png :'(\n";
		return;
	}
	if (!TextureButtonHold.loadFromFile("gfx/dsamplerButtonHold.png")) {
		std::cout << "Cannot load gfx/dsamplerButtonHold.png :'(\n";
		return;
	}
	if (!TextureButtonReset.loadFromFile("gfx/dsamplerButtonReset.png")) {
		std::cout << "Cannot load gfx/dsamplerButtonReset.png :'(\n";
		return;
	}
	if (!TextureButtonMidiCapture.loadFromFile("gfx/dsamplerButtonMidiCapture.png")) {
		std::cout << "Cannot load gfx/dsamplerButtonMidiCapture.png :'(\n";
		return;
	}
	if (!TextureButtonReverse.loadFromFile("gfx/dsamplerButtonReverse.png")) {
		std::cout << "Cannot load gfx/dsamplerButtonReverse.png :'(\n";
		return;
	}
	if (!TextureSampleName.loadFromFile("gfx/dsamplerSampleName.png")) {
		std::cout << "Cannot load gfx/dsamplerSampleName.png :'(\n";
		return;
	}

	if (!TextureClose.loadFromFile("gfx/dstudioClose.png")) {
		std::cout << "Cannot load gfx/dstudioClose.png :'(\n";
		return;
	}
	if (!TextureArrowUp.loadFromFile("gfx/dstudioArrowUp.png")) {
		std::cout << "Cannot load gfx/dstudioArrowUp.png :'(\n";
		return;
	}
	if (!TextureArrowDown.loadFromFile("gfx/dstudioArrowDown.png")) {
		std::cout << "Cannot load gfx/dstudioArrowDown.png :'(\n";
		return;
	}
	if (!TextureScrollTop.loadFromFile("gfx/dsamplerScrollTop.png")) {
		std::cout << "Cannot load gfx/dsamplerScrollTop.png :'(\n";
		return;
	}
	if (!TextureScrollBottom.loadFromFile("gfx/dsamplerScrollBottom.png")) {
		std::cout << "Cannot load gfx/dsamplerScrollBottom.png :'(\n";
		return;
	}
	if (!TextureLine.loadFromFile("gfx/dstudioLine.png")) {
		std::cout << "Cannot load gfx/dstudioLine.png :'(\n";
		return;
	}
	if (!TextureOpen.loadFromFile("gfx/dstudioOpen.png")) {
		std::cout << "Cannot load gfx/dstudioOpen.png :'(\n";
		return;
	}
	if (!TextureDStudioLogo.loadFromFile("gfx/dstudioLogo.png")) {
		std::cout << "Cannot load gfx/dstudioLogo.png :'(\n";
		return;
	}
	if (!TextureGoodbye.loadFromFile("gfx/dstudioGoodbye.png")) {
		std::cout << "Cannot load gfx/dstudioGoodbye.png :'(\n";
		return;
	}
	if (!TextureSure.loadFromFile("gfx/dstudioSure.png")) {
		std::cout << "Cannot load gfx/dstudioSure.png :'(\n";
		return;
	}
	if (!TextureHellNo.loadFromFile("gfx/dstudioHellNo.png")) {
		std::cout << "Cannot load gfx/dstudioHellNo.png :'(\n";
		return;
	}
	if (!TextureParentDirectory.loadFromFile("gfx/dstudioParentDirectory.png")) {
		std::cout << "Cannot load gfx/dstudioParentDirectory.png :'(\n";
		return;
	}

	if (!shaderNoise.loadFromFile("gfx/vertex.shader", "gfx/fragmentNoise.shader"))
	{
		std::cout << "Cannot load shaders... :'(\n";
		return;
	}
	if (!shaderChromaticAberration.loadFromFile("gfx/vertex.shader", "gfx/fragmentChromaticAberration.shader"))
	{
		std::cout << "Cannot load shaders... :'(\n";
		return;
	}
	if (!shaderGaussianBlur.loadFromFile("gfx/vertex.shader", "gfx/fragmentGaussianBlur.shader"))
	{
		std::cout << "Cannot load shaders... :'(\n";
		return;
	}

	SpriteTitle.setTexture(TextureTitle);

	Potentiometers[ _DSAMPLER_POTENTIOMETER_AMP_ ].sprite.setPosition(sf::Vector2f(27, 58.5));
	Potentiometers[ _DSAMPLER_POTENTIOMETER_START_ ].sprite.setPosition(sf::Vector2f(76, 58.5));
	Potentiometers[ _DSAMPLER_POTENTIOMETER_END_ ].sprite.setPosition(sf::Vector2f(125, 58.5));
	Potentiometers[ _DSAMPLER_POTENTIOMETER_PITCH_ ].sprite.setPosition(sf::Vector2f(174, 58.5));

	SpritePotentiometerTitle.setTexture(TexturePotentiometerTitle);

	SpriteButtons[ _DSAMPLER_BUTTON_OPEN_ - 4 ].setTexture(TextureButtonOpen);
	SpriteButtons[ _DSAMPLER_BUTTON_SAVE_ - 4 ].setTexture(TextureButtonSave);
	SpriteButtons[ _DSAMPLER_BUTTON_HOLD_ - 4 ].setTexture(TextureButtonHold);
	SpriteButtons[ _DSAMPLER_BUTTON_MORE_ - 4 ].setTexture(TextureButtonReset);
	SpriteButtons[ _DSAMPLER_BUTTON_MIDI_CAPTURE_ - 4 ].setTexture(TextureButtonMidiCapture);
	SpriteButtons[ _DSAMPLER_BUTTON_REVERSE_ -4 ].setTexture(TextureButtonReverse);
	SpriteButtons[ _DSAMPLER_BUTTON_SCROLL_TOP_ -4 ].setTexture(TextureScrollTop);
	SpriteButtons[ _DSAMPLER_BUTTON_SCROLL_BOTTOM_ - 4 ].setTexture(TextureScrollBottom);
	SpriteSampleName.setTexture(TextureSampleName);

	SpritePotentiometerTitle.setPosition(sf::Vector2f(0, 83));
	SpriteButtons[ _DSAMPLER_BUTTON_OPEN_ - 4 ].setPosition(sf::Vector2f(195, 45));
	SpriteButtons[ _DSAMPLER_BUTTON_SAVE_ - 4 ].setPosition(sf::Vector2f(253, 45));
	SpriteButtons[ _DSAMPLER_BUTTON_HOLD_ - 4 ].setPosition(sf::Vector2f(195, 57));
	SpriteButtons[ _DSAMPLER_BUTTON_MORE_ - 4 ].setPosition(sf::Vector2f(253, 57));
	SpriteButtons[ _DSAMPLER_BUTTON_MIDI_CAPTURE_ - 4 ].setPosition(sf::Vector2f(195, 69));
	SpriteButtons[ _DSAMPLER_BUTTON_REVERSE_ -4 ].setPosition(sf::Vector2f(253, 69));
	SpriteButtons[ _DSAMPLER_BUTTON_SCROLL_TOP_ -4 ].setPosition(sf::Vector2f(195, 33));
	SpriteButtons[ _DSAMPLER_BUTTON_SCROLL_BOTTOM_ - 4 ].setPosition(sf::Vector2f(195, 81));
	SpriteSampleName.setPosition(sf::Vector2f(2, 96));

	SpriteButtons[ _DSAMPLER_BUTTON_CLOSE_ - 4 ].setTexture(TextureClose);
	SpriteOpen.setTexture(TextureOpen);
	SpriteArrowUp.setTexture(TextureArrowUp);
	SpriteArrowDown.setTexture(TextureArrowDown);
	SpriteParentDirectory.setTexture(TextureParentDirectory);
	SpriteLine.setTexture(TextureLine);

	SpriteButtons[ _DSAMPLER_BUTTON_CLOSE_ - 4 ].setPosition(sf::Vector2f(293,2));
	SpriteArrowUp.setPosition(sf::Vector2f(258,2));
	SpriteArrowDown.setPosition(sf::Vector2f(223,2));
	SpriteOpen.setPosition(sf::Vector2f(2,2));
	SpriteParentDirectory.setPosition(sf::Vector2f(40,2));

	SpriteDStudioLogo.setTexture(TextureDStudioLogo);
	SpriteGoodbye.setTexture(TextureGoodbye);
	SpriteButtons[ _DSAMPLER_BUTTON_SURE_ - 4].setTexture(TextureSure);
	SpriteButtons[ _DSAMPLER_BUTTON_HELL_NO_ - 4].setTexture(TextureHellNo);
	SpriteDStudioLogo.setPosition(sf::Vector2f(99,40));
	SpriteGoodbye.setPosition(sf::Vector2f(64,12));
	SpriteButtons[ _DSAMPLER_BUTTON_SURE_ - 4].setPosition(sf::Vector2f(107,160));
	SpriteButtons[ _DSAMPLER_BUTTON_HELL_NO_ - 4].setPosition(sf::Vector2f(160,160));

	refreshPlan[0] = true;
	refreshPlan[1] = false;
	refreshPlan[2] = false;
}

bool DSAMPLER_GFX::READY() {
	return this->error;
}

void DSAMPLER_GFX::OPACITY(int opacity, int element) {
	if (element < 4 ) {
		this->Potentiometers[ element ].sprite.setColor(sf::Color(255, 255, 255, opacity));
	}
	else { 
		this->SpriteButtons[ element - 4 ].setColor(sf::Color(255, 255, 255, opacity));
	}
}

void DSAMPLER_GFX::ROTATION(int angle, int element) {
	this->Potentiometers[ element ] .sprite.setRotation(angle);
	this->refreshPlan[0]= true;
}

void DSAMPLER_GFX::GAUSSIAN_BLUR(int PLAN) {
	this->TemporaryTexture = this->PlansRenderTexture[ PLAN ].getTexture();
	this->PlansBlurRenderSprite[ PLAN ].setTexture( this->TemporaryTexture );
	this->PlansBlurRenderTexture[ PLAN ].clear(sf::Color(255,255,255,255));
	this->PlansBlurRenderTexture[ PLAN ].draw(this->PlansBlurRenderSprite[ PLAN ]);
	this->PlansBlurRenderTexture[ PLAN ].display();

	for (int i=0; i< this->blurPass[ PLAN ]; i++) {
		// Applying Alpha
		this->PlansBlurRenderSprite[ PLAN ].setColor( sf::Color(255, 255, 255, (int) (((float) 255) * ( 1.0 - ((float)i)/9.0) )));
		// Vertical Blur
		this->TemporaryTexture = this->PlansBlurRenderTexture[ PLAN ].getTexture();
		this->PlansBlurRenderSprite[ PLAN ].setTexture( this->TemporaryTexture );
		this->shaderGaussianBlur.setParameter("direction",0.0,1.0);
		this->shaderGaussianBlur.setParameter("blurRadius", (float) 0.00505);
		this->PlansBlurRenderTexture[ PLAN ].clear(sf::Color(255,255,255,255));
		this->PlansBlurRenderTexture[ PLAN ].draw(this->PlansBlurRenderSprite[ PLAN ], &(this->shaderGaussianBlur));
		this->PlansBlurRenderTexture[ PLAN ].display();

		// Horizontal Blur
		this->TemporaryTexture = this->PlansBlurRenderTexture[ PLAN ].getTexture();
		this->PlansBlurRenderSprite[ PLAN ].setTexture( this->TemporaryTexture );
		this->shaderGaussianBlur.setParameter("direction",1.0,0.0);
		this->shaderGaussianBlur.setParameter("blurRadius", (float) 0.003125);
		this->PlansBlurRenderTexture[ PLAN ].clear(sf::Color(255,255,255,255));
		this->PlansBlurRenderTexture[ PLAN ].draw(this->PlansBlurRenderSprite[ PLAN ], &(this->shaderGaussianBlur));
		this->PlansBlurRenderTexture[ PLAN ].display();
	}
}

void DSAMPLER_GFX::DRAW_PLAN_0() {
	this->PlansRenderTexture[0].clear(sf::Color(255,255,255,255));
	this->PlansRenderTexture[0].draw(this->SpriteTitle);
	this->PlansRenderTexture[0].draw(this->Potentiometers[ _DSAMPLER_POTENTIOMETER_AMP_ ].sprite);
	this->PlansRenderTexture[0].draw(this->Potentiometers[ _DSAMPLER_POTENTIOMETER_START_ ].sprite);
	this->PlansRenderTexture[0].draw(this->Potentiometers[ _DSAMPLER_POTENTIOMETER_END_ ].sprite);
	this->PlansRenderTexture[0].draw(this->Potentiometers[ _DSAMPLER_POTENTIOMETER_PITCH_ ].sprite);
	this->PlansRenderTexture[0].draw(this->SpritePotentiometerTitle);
	this->PlansRenderTexture[0].draw(this->SpriteButtons[ _DSAMPLER_BUTTON_SCROLL_TOP_ - 4 ]);
	this->PlansRenderTexture[0].draw(this->SpriteButtons[ _DSAMPLER_BUTTON_SCROLL_BOTTOM_ - 4 ]);
	this->PlansRenderTexture[0].draw(this->SpriteButtons[ _DSAMPLER_BUTTON_OPEN_ - 4 ]);
	this->PlansRenderTexture[0].draw(this->SpriteButtons[ _DSAMPLER_BUTTON_SAVE_ - 4 ]);
	this->PlansRenderTexture[0].draw(this->SpriteButtons[ _DSAMPLER_BUTTON_HOLD_ - 4 ]);
	this->PlansRenderTexture[0].draw(this->SpriteButtons[ _DSAMPLER_BUTTON_MORE_ - 4 ]);
	this->PlansRenderTexture[0].draw(this->SpriteButtons[ _DSAMPLER_BUTTON_MIDI_CAPTURE_ - 4 ]);
	this->PlansRenderTexture[0].draw(this->SpriteButtons[ _DSAMPLER_BUTTON_REVERSE_ - 4 ]);
	this->PlansRenderTexture[0].draw(this->SpriteSampleName);
	this->PlansRenderTexture[0].display();
}

void DSAMPLER_GFX::DRAW_PLAN_1() {
	this->PlansRenderTexture[1].clear(sf::Color(255,255,255,255));
	this->PlansRenderTexture[1].draw(this->SpriteButtons[ _DSAMPLER_BUTTON_CLOSE_ - 4 ]);
	this->PlansRenderTexture[1].draw(this->SpriteArrowUp);
	this->PlansRenderTexture[1].draw(this->SpriteArrowDown);
	this->PlansRenderTexture[1].draw(this->SpriteOpen);
	this->PlansRenderTexture[1].draw(this->SpriteParentDirectory);
	this->SpriteLine.setPosition(2, 21);
	this->PlansRenderTexture[1].draw(this->SpriteLine);
	this->SpriteLine.setPosition(2, 42);
	this->PlansRenderTexture[1].draw(this->SpriteLine);
	this->SpriteLine.setPosition(2, 63);
	this->PlansRenderTexture[1].draw(this->SpriteLine);
	this->SpriteLine.setPosition(2, 84);
	this->PlansRenderTexture[1].draw(this->SpriteLine);
	this->SpriteLine.setPosition(2, 105);
	this->PlansRenderTexture[1].draw(this->SpriteLine);
	this->SpriteLine.setPosition(2, 126);
	this->PlansRenderTexture[1].draw(this->SpriteLine);
	this->SpriteLine.setPosition(2, 147);
	this->PlansRenderTexture[1].draw(this->SpriteLine);
	this->SpriteLine.setPosition(2, 168);
	this->PlansRenderTexture[1].draw(this->SpriteLine);
	this->PlansRenderTexture[1].display();
}
void DSAMPLER_GFX::DRAW_PLAN_2() {
	this->PlansRenderTexture[2].clear(sf::Color(255,255,255,255));
	this->PlansRenderTexture[2].draw(this->SpriteDStudioLogo);
	this->PlansRenderTexture[2].draw(this->SpriteGoodbye);
	this->PlansRenderTexture[2].draw(this->SpriteButtons[ _DSAMPLER_BUTTON_SURE_ - 4 ]);
	this->PlansRenderTexture[2].draw(this->SpriteButtons[ _DSAMPLER_BUTTON_HELL_NO_ - 4 ] );
	this->PlansRenderTexture[2].display();
}
void DSAMPLER_GFX::DRAW(sf::RenderWindow * window) {

	if (this->refreshPlan[0]) {
		this->DRAW_PLAN_0();
	}
	if (this->refreshPlan[1]) {
		this->DRAW_PLAN_1();
	}
	if (this->refreshPlan[2]) {
		this->DRAW_PLAN_2();
	}

	//Applying gaussian blur
	this->GAUSSIAN_BLUR(0);
	this->GAUSSIAN_BLUR(1);
	this->GAUSSIAN_BLUR(2);

	//Merge Layers
	this->MergeRenderSprite.setColor(sf::Color(255,255,255,255));	// May control Motion Blur
	this->TemporaryTexture = this->PlansBlurRenderTexture[ 0 ].getTexture();
	this->MergeRenderSprite.setTexture(this->TemporaryTexture);	
	this->MergeRenderTexture.draw(this->MergeRenderSprite);
	this->MergeRenderTexture.display();

	
	for(int i = 1; i < LAYERS_COUNT; i++) { 
		if(this->blurPass[ i ] < 9) {
			this->MergeRenderSprite.setColor(sf::Color(255,255,255, (int) (((float) 255) * ( 1.0 - ((float) this->blurPass[i])/9.0) )));
			this->TemporaryTexture = this->PlansBlurRenderTexture[ i ].getTexture();
			this->MergeRenderSprite.setTexture(this->TemporaryTexture);	
			this->MergeRenderTexture.draw(this->MergeRenderSprite);
			this->MergeRenderTexture.display();
		}
	}

	//Applying noise
	this->TemporaryTexture = this->MergeRenderTexture.getTexture();
	//this->TemporaryTexture = this->PlansBlurRenderTexture[ 0 ].getTexture();
	
	this->NoiseRenderSprite.setTexture( this->TemporaryTexture);
	this->NoiseRenderTexture.clear(sf::Color(255,255,255,255));
	this->shaderNoise.setParameter("seed",(float) (std::clock() % 43758));
	this->NoiseRenderTexture.draw(this->NoiseRenderSprite, &(this->shaderNoise));
	this->NoiseRenderTexture.display();

	//Applying chromatic aberration
	this->TemporaryTexture = this->NoiseRenderTexture.getTexture();
	this->ChromaticAberrationRenderSprite.setTexture( this->TemporaryTexture );
	window->clear(sf::Color(255,255,255,255));
	this->shaderChromaticAberration.setParameter("seed", ( (float) (std::clock() % 100) / 100 ) * 0.003 +0.002);
	window->draw(this->ChromaticAberrationRenderSprite,  &(this->shaderChromaticAberration));

	window->display();
	this->refreshPlan[0] = false;
}
