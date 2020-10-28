#pragma once
#include <iostream>
#include <fcntl.h>
#include <io.h>
#include <clocale>
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <cmath>
#include <complex>
#include <filesystem>
#include <fstream>
#include <cctype>
#include <chrono>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>

const double PI = 3.14159265358979323846;
const double INF = std::numeric_limits<double>::infinity();
namespace fs = std::filesystem;

typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;

typedef int8_t I8;
typedef int16_t I16;
typedef int32_t I32;
typedef int64_t I64;

// Abstrac standard functions
namespace astd {
	// Splits string from a character
	inline std::vector<sf::String> split(sf::String str, const U32 chr = ' ') {
		str += chr;
		std::vector<sf::String> ret;
		sf::String retstr;
		for (U32 chrc : str) {
			if (chrc == chr) {
				ret.push_back(retstr);
				retstr = "";
			}
			else retstr += chrc;
		}
		return ret;
	}

	// Uppercase each character of the string
	inline sf::String upper(sf::String str) {
		std::wstring wstr = str.toWideString();
		sf::String res;
		for (wchar_t wchr : wstr)
			res += (wchar_t)std::toupper(wchr);
		return res;
	}

	// Lowercase each character of the string
	inline sf::String lower(sf::String str) {
		std::wstring wstr = str.toWideString();
		sf::String res;
		for (wchar_t wchr : wstr)
			res += (wchar_t)std::tolower(wchr);
		return res;
	}

	// Slicing string error free, returns an empty string if the starting index is out of range
	inline sf::String safeStr(sf::String str, const U32 index, const U32 length) {
		if (index >= str.getSize()) return "";
		return str.substring(index, length);
	}

	// Get character by index from a string error free, returns '\0' NULL ending if the index is out of range
	inline U32 safeStr(sf::String str, const U32 index) {
		if (index >= str.getSize()) return 0;
		return str[index];
	}

	// Convert string to int, returns 0 on error
	inline I64 strToInt(const sf::String str) {
		try {
			return std::stoll(str.toWideString());
		}
		catch (...) {
			return 0;
		}
	}

	// Convert string to floating point (is double), returns 0 on error
	inline double strToPoint(const sf::String str) {
		try {
			return std::stod(str.toWideString());
		}
		catch (...) {
			return 0;
		}
	}

	// Get the time since 1970 in seconds as a floating point (double)
	inline double getTime() {
		return (double)std::chrono::system_clock::now().time_since_epoch().count() / 10000000;
	}

	// Get the time as milliseconds as integer
	inline U64 getMillis() {
		return std::chrono::system_clock::now().time_since_epoch().count() / 10000;
	}

	// Get the time as microseconds as integer
	inline U64 getMicros() {
		return std::chrono::system_clock::now().time_since_epoch().count() / 10;
	}

	// Sleeps the program in seconds
	inline void sleep(const double seconds) {
		double finish = getTime() + seconds;
		while (finish > getTime());
	}

	// Sleeps in milliseconds
	inline void sleepMilli(const U64 millis) {
		U64 finish = getMillis() + millis;
		while (finish > getMillis());
	}

	// Sleeps in microseconds
	inline void sleepMicros(const U64 micros) {
		U64 finish = getMicros() + micros;
		while (finish > getMicros());
	}

	// Writes a binary string to a file, returns if it went successful
	inline bool writeToFile(const sf::String file_name, std::vector<U8> data) {
		std::ofstream fout(file_name.toWideString(), std::ios::out | std::ios::binary);
		if (fout.is_open()) {
			fout.write((char*)(&data[0]), data.size());
			fout.close();
			return fout.good();
		}
		return false;
	}

	// Reads the binary content of a file and returns it, it'll return an empty vector if it didn't went well 
	inline std::vector<U8> readFromFile(const sf::String file_name, bool* success = NULL) {
		std::ifstream fin(file_name.toWideString(), std::ios::in | std::ios::binary);
		std::vector<U8> content;
		if (fin.is_open()) {
			U32 it = 0;
			while (1) {
				char byte;
				fin.read(&byte, 1);
				if (fin.eof()) break;
				content.push_back(byte);
				it++;
			}
			if (success != NULL)
				*success = true;
			return content;
		}
		if (success != NULL)
			*success = false;
		return content;
	}

	inline std::vector<U8> createByteVectorFromPointer(void* data, size_t size) {
		std::vector<U8> vec;
		for (size_t i = 0; i < size; i++)
			vec.push_back(((U8*)data)[i]);
		return vec;
	}
}
