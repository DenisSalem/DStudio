#include <SFML/Graphics.hpp>
#ifndef _dstudio_
#include "dstudio.hpp"
#endif

sf::Texture DSTUDIO_POTENTIOMETER::texture;

DSTUDIO_POTENTIOMETER::DSTUDIO_POTENTIOMETER() {
	DSTUDIO_POTENTIOMETER::texture.setSmooth(true);
	sprite.setTexture(DSTUDIO_POTENTIOMETER::texture);
	sprite.setOrigin(30,30);
}
