#pragma once
#include "../astd.hpp"
#include "math.hpp"
#include "renderer3d.hpp"

inline std::vector<SolidTri> loadFromObj(sf::String file_name) {
	bool succ = false;
	std::vector<U8> file = astd::readFromFile(file_name, &succ);
	if (!succ)
		return std::vector<SolidTri>();

	std::string str(file.begin(), file.end());
	std::vector<sf::String> lines = astd::split(str, '\n');

	std::vector<Point3> vertices;
	std::vector<SolidTri> ret;

	for (sf::String& line : lines) {
		std::vector<sf::String> piece = astd::split(line, ' ');
		if (piece.size() < 4)
			continue;

		if (piece[0] == 'v')
			vertices.push_back(Point3{ astd::strToPoint(piece[1]), astd::strToPoint(piece[2]), astd::strToPoint(piece[3]) });
		else if (piece[0] == 'f')
			ret.push_back(SolidTri{ vertices[astd::strToInt(piece[1]) - 1], vertices[astd::strToInt(piece[2]) - 1], vertices[astd::strToInt(piece[3]) - 1],
				0xFF000000 | (astd::getMicros() % (1 << 24)) });
	}

	return ret;
}
