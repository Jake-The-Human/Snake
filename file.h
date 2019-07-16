#ifndef FILE_H
#define FILE_H

//#include "game.h"

#include <fstream>
#include <string>
#include <vector>

struct ReplayMode;

struct DataFile
{
	DataFile(const std::string& path);
	~DataFile() { fs.close(); }

	// Write
	void WriteHeader(const std::string& playerName, int gridWidth, int gridHeight);
	void WriteSnakePos(int);
	void WriteFoodPos(int, int/*FoodType*/);
	void WriteScore(unsigned, unsigned);

	// Read
	void InitReplayFromFile(ReplayMode* game);
	bool eof() { return fs.eof(); }
	bool bad() { return fs.bad(); }
//private:
	std::fstream fs;
	// TODO: these are temp need to find a better way of doing this
	bool mWriteFooter{ true };
};

#endif