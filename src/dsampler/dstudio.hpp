#include <SFML/Graphics.hpp>
#define _dstudio_ 
#define FRAMES_RATE 20
class DSTUDIO_POTENTIOMETER  {
        public:
		static sf::Texture texture;
		DSTUDIO_POTENTIOMETER();
                sf::Sprite sprite;
		sf::Vector2f position;
		int rotation;
		int opacity;
};

