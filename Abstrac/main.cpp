#include "astd.hpp"
#include "Renderer/math.hpp"
#include "Renderer/renderer3d.hpp"

sf::String debug_text;

int entry(std::vector<sf::String> args) {
#ifdef _WIN32
	// (void) so that visual studio won't complain
	(void)_setmode(_fileno(stdin), _O_U16TEXT);
	(void)_setmode(_fileno(stdout), _O_U16TEXT);
#endif
	std::setlocale(LC_ALL, "en_US.utf8");

	sf::Font font;
	font.loadFromFile("tahoma.ttf");
	sf::Text text;
	text.setFont(font);
	text.setFillColor(sf::Color(255, 0, 0, 255));
	text.setCharacterSize(16);

	sf::RenderWindow window(sf::VideoMode(800, 600), "Abstrac 3D test");
	double prev = astd::getTime();

	Render3DLayer aa = Render3DLayer(800, 600, 90, 500, 0.1);
	Mesh mesh;
	mesh.solid_tris.push_back(SolidTri{ Point3{0, 0, 1}, Point3{1, 0, 1}, Point3{0, 1, 1}, 3 });
	aa.meshes.push_back(mesh);

	double speed = 0.2;

	while (window.isOpen()) {
		double delta = astd::getTime() - prev;
		prev = astd::getTime();
		window.setTitle(sf::String("FPS: ") + (sf::String)std::to_string(1 / delta));

		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
			aa.position.y += speed;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			aa.position.x -= speed;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			aa.position.y -= speed;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
			aa.position.x += speed;

		std::wcout << "X: " << aa.position.x << " Y: " << aa.position.y << " Z: " << aa.position.z << '\n';

		aa.clear(0xFFFFFFFF);
		aa.render();

		sf::Image buf;
		buf.create(800, 600, (U8*)aa.render_buffer);
		sf::Texture tex;
		tex.loadFromImage(buf);
		sf::Sprite sprite(tex);

		text.setString(debug_text);
		
		window.draw(sprite);
		window.draw(text);
		window.display();
	}

	return 0;
}


#ifdef _WIN32 || _WIN64
int wmain(const int argc, wchar_t* argv[]) {
	std::vector<sf::String> args;
	for (int arg = 0; arg < argc; arg++)
		args.push_back(argv[arg]);
	return entry(args);
}
#else
int main(const int argc, char* argv[]) {
	std::vector<sf::String> args;
	for (int arg = 0; arg < argc; arg++) {
		std::string str = argv[arg];
		args.push_back(sf::String::fromUtf8(str.begin(), str.end()));
	}
	return entry(args);
}
#endif
