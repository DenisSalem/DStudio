#ifndef _dstudio_
#include "dstudio.hpp"
#endif

#define _DSAMPLER_POTENTIOMETER_AMP_	0
#define _DSAMPLER_POTENTIOMETER_START_	1
#define _DSAMPLER_POTENTIOMETER_END_	2
#define _DSAMPLER_POTENTIOMETER_PITCH_	3

#define _DSAMPLER_BUTTON_OPEN_		4
#define _DSAMPLER_BUTTON_SAVE_		5
#define _DSAMPLER_BUTTON_HOLD_		6
#define _DSAMPLER_BUTTON_MORE_		7
#define _DSAMPLER_BUTTON_MIDI_CAPTURE_	8
#define _DSAMPLER_BUTTON_REVERSE_	9
#define _DSAMPLER_BUTTON_SCROLL_TOP_	10
#define _DSAMPLER_BUTTON_SCROLL_BOTTOM_ 11
#define _DSAMPLER_BUTTON_CLOSE_		12
#define _DSAMPLER_BUTTON_SURE_		13
#define _DSAMPLER_BUTTON_HELL_NO_	14

#define SPRITES_COUNT			15
#define LAYERS_COUNT			3
#define SPRITE_OPACITY			128

class DSAMPLER_GFX{
	public:
		DSAMPLER_GFX();
		bool READY();
		void GAUSSIAN_BLUR(int PLAN);
		void OPACITY(int opacity, int element); 
		void ROTATION(int opacity, int element); 
		void DRAW(sf::RenderWindow * window);

		int blurPass[LAYERS_COUNT];
		int blurState[LAYERS_COUNT];

		bool refreshPlan[ LAYERS_COUNT ];
		int			* plan;
	private: 
		void DRAW_PLAN_0();
		void DRAW_PLAN_1();
		void DRAW_PLAN_2();

		sf::Shader 		shaderNoise;
		sf::Shader 		shaderChromaticAberration;
		sf::Shader 		shaderGaussianBlur;

		sf::RenderTexture 	* PlansRenderTexture;
		sf::RenderTexture 	* PlansBlurRenderTexture;
		sf::RenderTexture	NoiseRenderTexture;
		sf::RenderTexture	MergeRenderTexture;
		sf::RenderTexture	ChromaticAberrationRenderTexture;

		sf::Texture		TemporaryTexture;
		sf::Texture 		TexturePotentiometer;
		sf::Texture 		TexturePotentiometerTitle;
		sf::Texture 		TextureScrollTop;
		sf::Texture 		TextureScrollBottom;
		sf::Texture 		TextureButtonOpen;
		sf::Texture 		TextureButtonSave;
		sf::Texture 		TextureButtonHold;
		sf::Texture 		TextureButtonReset;
		sf::Texture 		TextureButtonMidiCapture;
		sf::Texture 		TextureButtonReverse;
		sf::Texture 		TextureSampleName;
		sf::Texture 		TextureTitle;
		sf::Texture 		TextureClose;
		sf::Texture 		TextureArrowUp;
		sf::Texture 		TextureArrowDown;
		sf::Texture 		TextureLine;
		sf::Texture 		TextureOpen;
		sf::Texture 		TextureParentDirectory;
		sf::Texture 		TextureSure;
		sf::Texture 		TextureHellNo;
		sf::Texture 		TextureDStudioLogo;
		sf::Texture 		TextureGoodbye;

		DSTUDIO_POTENTIOMETER	* Potentiometers;

		sf::Sprite	* PlansRenderSprite;
		sf::Sprite	* PlansBlurRenderSprite;

		sf::Sprite	NoiseRenderSprite;
		sf::Sprite	ChromaticAberrationRenderSprite;

		sf::Sprite 	SpritePotentiometerTitle;
		sf::Sprite 	SpriteTitle;

		sf::Sprite *	SpriteButtons;

		sf::Sprite 	SpriteSampleName;
		sf::Sprite	SpriteOpen;
		sf::Sprite	SpriteParentDirectory;
		sf::Sprite	SpriteArrowUp;
		sf::Sprite	SpriteArrowDown;
		sf::Sprite	SpriteLine;
		sf::Sprite	MergeRenderSprite;
		sf::Sprite	SpriteDStudioLogo;
		sf::Sprite	SpriteGoodbye;

		bool error;

};

class DSAMPLER_ACTION {
	public:
		DSAMPLER_ACTION(sf::RenderWindow * windowPointer, DSAMPLER_GFX * gfxPointer);
		void LISTEN();
		int plan;
	private:
		void UPDATE_OPACITY(bool restore, int element);
		void UPDATE_BLUR(bool restore, int plan);
		void UPDATE_ROTATION(int x, int y, int potentiometer);
		DSAMPLER_GFX * gfx;
		char mouseState;
		char currentActivePotentiometer;
		sf::RenderWindow * window;
		sf::Vector2i localPosition;
};
