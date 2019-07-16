#include "file.h"

#define __STDC_WANT_LIB_EXT1__ 1 // this is need for ctime_s

#include <cassert>
#include <ctime> 
#include <tuple>

#include <iostream>

#include "game.h"

DataFile::DataFile(const std::string& path) :
	fs(path, std::fstream::in | std::fstream::out | std::fstream::app)
{
	if (not fs.is_open())
	{
		std::cout << "Operation failed to performed\n";
		fs.close();
		exit(1);
	}
}
void 
DataFile::WriteHeader(const std::string& playerName, int gridWidth, int gridHeight)
{
		time_t my_time{ time(nullptr) };
		// char timeStr[26];
		std::string timeStr{ ctime(&my_time) };
		if (fs.gcount() == 0 or fs.peek() == '}')
			fs << '\n';

		fs << "{\n\tplayer: " << playerName << "\n\tdate: " << timeStr << "\tgridWidth: " << gridWidth << "\n\tgridHeight: " << gridHeight << "\n\tgameInfo: ";

		mWriteFooter = true;
}

void 
DataFile::WriteSnakePos(int p)
{
	fs << "s" << p;
}

void
DataFile::WriteFoodPos(int p, int /*FoodType*/ fT)
{
	char foodType{'-'};
	switch (fT)
	{
	case ORANGE:
		foodType = 'o';
		break;
	case COCONUT:
		foodType = 'c';
		break;
	case BANNANA:
		foodType = 'b';
		break;
	case WATERMELLON:
		foodType = 'w';
		break;
	};

	fs << "f" << foodType << p;
}

void 
DataFile::WriteScore(const unsigned score, const unsigned gameNum)
{
	if (mWriteFooter)
	{
		fs << "e\n\tscore: " << score << "\n\tgameNumber: " << gameNum << "\n}";
		mWriteFooter = false;
	}
}

void 
DataFile::InitReplayFromFile(ReplayMode* game)
{
	std::string trash; // used to discarded tags

	fs >> trash; // removes the {

	fs >> trash;
	fs >> game->mPlayer;

	fs >> trash;
	for (int i = 0; i < 5; ++i) { // 5 elements in the date
		fs >> trash;
		game->mDate += " " + trash;
	}

	fs >> trash;
	fs >> game->mWidthOfGrid;

	fs >> trash;
	fs >> game->mHeightOfGrid;

	fs >> trash;
	fs >> game->mPlayerInput;

	fs >> trash;
	fs >> game->mFinalScore;

	fs >> trash;
	fs >> trash;

	fs >> trash; // removes the }
}

