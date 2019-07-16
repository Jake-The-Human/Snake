#include "game.h"

#include <cassert>
#include <cstdlib>     /* srand, rand */
#include <ctime>       /* time */

Game::Game(int widthOfGrid, int  heightOfGrid, DataFile& file) :
mWidthOfGrid(widthOfGrid),
mHeightOfGrid(heightOfGrid),
mFile(file)
{
    // You must have at least a 2x2 grid
    assert(mWidthOfGrid  >= 2);
    assert(mHeightOfGrid >= 2);

	/* initialize random seed: */
	srand(static_cast<unsigned int>(time(nullptr)));
    
    // grid will alway be a square for now
	int totalSize{ mWidthOfGrid * mHeightOfGrid };
    // Head of the snake will be in the center of the grid
    mSnake.push_back(totalSize / 2);
}


void 
Game::Reset()
{
	mScore = 0;
	mFood.clear();

	int totalSize{ mWidthOfGrid * mHeightOfGrid };

	mSnake.delete_snake();
	mSnake.push_back(totalSize / 2);

	mGameState = GameState::RUN;
	mNumOfGamesPlayed++;

	if (not IsReplayMode())
	{
		mFile.WriteHeader(mPlayer, mWidthOfGrid, mHeightOfGrid);
		//mFile.WriteSnakePos(GetSnakeHead());
	}
}


bool 
Game::Collision(int newIndexOfHead, bool foodCaller)
{
	bool collided{ mSnake.collision(newIndexOfHead) };
	if(not foodCaller and collided)
		mGameState = GameState::STOP; //breaks game when unpausing

	return collided;
}

std::optional<FoodType>
Game::UpdateSnake(int newIndexOfHead)
{
	std::optional<FoodType> ateFood{};
    // This need to be a fail condition
    if(Collision(newIndexOfHead))
        return ateFood;

	// this moves snake forward
	mSnake.push_front(newIndexOfHead);

	// Snake eats food
    for(auto& f : mFood)
	{
		if (newIndexOfHead == f.mIndex)
		{
			ateFood = std::optional<FoodType>{ f.mType };
			// if the snake ate some food then increase
			// increase points
			mScore += f.GetPoints();
			
			// update the food that was eaten to what ever the last food
			// in mFood then pop the back
			auto lastElement = --(mFood.end());
			f.mIndex = lastElement->mIndex;
			f.mType  = lastElement->mType;
			
			mFood.pop_back();

			break;
		}
	}

	if(not ateFood)
		mSnake.pop_back();

	if(not IsReplayMode())
		mFile.WriteSnakePos(GetSnakeHead());

	return ateFood;
}

bool
Game::UpdateFood(int newFoodIndex, FoodType foodtype)
{
	if (newFoodIndex == -1)
	{
		// only let there be at most 4 pieces of food on
		// the screen
		if(mFood.size() > 4)
			return false;

		// need better solution but temp fix to stop the fruit on slaut
		if (rand() % mWidthOfGrid != 0)
			return false;

		int pos{ rand() % (mWidthOfGrid * mHeightOfGrid) };

		if ( Collision(pos, true) )
			return false;

		for (auto f : mFood)
		{
			if (f.mIndex == pos)
				return false;
		}

		FoodType fT = static_cast<FoodType>(rand() % FoodType::NUMOFFOOD);
		mFood.push_back(Food{ pos, fT });

		if (not IsReplayMode()) 
			mFile.WriteFoodPos(pos, fT);

	}
	else
	{
		if ( Collision(newFoodIndex, true) )
			return false;

		for (auto f : mFood)
		{
			if (f.mIndex == newFoodIndex)
				return false;
		}

		// TODO: encode food type
		mFood.push_back( Food{ newFoodIndex, foodtype } );
	}

	return true;
}

ReplayMode::ReplayMode(DataFile& file) :
	Game(10,10,file) // dummy constructor 
{
	mGameState = GameState::RUN;
}


GameInput
ReplayMode::GetNextInput()
{
	std::string move{};
	char type, foodType{'-'};

	if(mPlayerInputIndex >= mPlayerInput.size())
		return { 'e', '-', -1 };
	else if (mPlayerInput[mPlayerInputIndex] == 's')
	{
		type = 's';
		mPlayerInputIndex++;
	}
	else if (mPlayerInput[mPlayerInputIndex] == 'f')
	{
		type = 'f';
		mPlayerInputIndex++;

		foodType = mPlayerInput[mPlayerInputIndex];
		mPlayerInputIndex++;
	}
	else
	{
		return { 'e', '-', -1 };
	}


	while ( mPlayerInputIndex < mPlayerInput.size() and
			mPlayerInput[mPlayerInputIndex] != 's'  and
			mPlayerInput[mPlayerInputIndex] != 'f'  and
			mPlayerInput[mPlayerInputIndex] != 'e' )
	{
		move += mPlayerInput[mPlayerInputIndex];
		mPlayerInputIndex++;
	}

	return { type, foodType, std::stoi(move) };
}