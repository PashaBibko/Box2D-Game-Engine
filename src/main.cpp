#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>

int main()
{
	sf::RenderWindow window (sf::VideoMode(800, 600), "SFML works!");
	b2World* world = new b2World(b2Vec2(0.0f, 9.8f));

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();

		window.display();
	}

	return 0;
}