#ifndef GAME_H
#define GAME_H

#include "file.h"
#include "snake.h"

#include <tuple>
#include <vector> // maybe replace with your own list

using GridIndex = int;
//using Snake = std::vector<GridIndex>;
using GameInput = std::tuple<char, char, int>;

enum FoodType { ORANGE = 0, COCONUT = 1, BANNANA = 2, WATERMELLON = 3, NUMOFFOOD = 4};

struct Food
{
    GridIndex mIndex {0};
	enum FoodType mType { FoodType::ORANGE };
    // Note make this an int maybe later added rotting food to take points away
	inline int GetPoints() const { return (mType + 1) * 10; }
};

enum GameState { RUN, PAUSE, START, STOP, QUIT };

struct Game
{
	Game(int width, int height, DataFile& file);
	~Game() = default;

	void Reset();
    
    bool Collision(int newIndexOfHead, bool foodCaller = false);
    std::optional<FoodType> UpdateSnake(int newIndexOfHead); // returns type of food the snake ate else its empty
    bool UpdateFood(int newFoodIndex = -1, FoodType foodType = FoodType::BANNANA); // returns true if food was spawned
    
    inline GridIndex GetSnakeHead() const { return mSnake.get_head(); }
    inline int GetSizeOfGrid() const { return mWidthOfGrid * mHeightOfGrid; }
	virtual inline bool IsReplayMode() const { return false; }
    
    int mWidthOfGrid;
    int mHeightOfGrid;

	enum GameState mGameState { GameState::START };

    unsigned mScore {0};
	unsigned mNumOfGamesPlayed{ 0 }; // this count the number of games played in a row
	std::string mPlayer{"P1"};

    Snake mSnake;
    std::vector<Food> mFood;
    
	DataFile& mFile;
};

struct ReplayMode : public Game
{
	ReplayMode(DataFile& file);
	virtual ~ReplayMode() = default;
	virtual inline bool IsReplayMode() const override { return true; }
	GameInput GetNextInput();

	void LoadNextGame() 
	{
		mFinalScore = 0;
		mPlayerInputIndex = 0;
	}

	unsigned mFinalScore{ 0 };
	size_t mPlayerInputIndex{ 0 };
	std::string mDate{};
	std::string mPlayerInput{};
};

#endif