// g++ *.cpp -lsfml-graphics -lsfml-window -lsfml-system 

#include <iostream>
#include <SFML/Graphics.hpp>
#include "dsampler.hpp"

int main() {
	sf::RenderWindow window(sf::VideoMode(320, 198), "DSAMPLER", sf::Style::Close | sf::Style::Titlebar);
	if (!sf::Shader::isAvailable()) {
		std::cout << "Post GFX aren't supported... :(\n";
	}

	if (!DSTUDIO_POTENTIOMETER::texture.loadFromFile("gfx/potentiometer.png")) {
        	std::cout << "Cannot load gfx/potentiometer.png :'(\n";
        	return -1;
	}

	DSAMPLER_GFX gfx;
	if (gfx.READY()) {
                window.close();
		return -1;
	}
	DSAMPLER_ACTION action(&window, &gfx);

	while (window.isOpen()) {
		action.LISTEN();
		gfx.DRAW(&window); // and render :)
		sf::sleep( sf::milliseconds(FRAMES_RATE));

	}

	return 0;
}
