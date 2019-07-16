#include "game.h"
#include "window.h"
#include "file.h"

#include <cstdio>
#include <tuple>
#include <vector>

#include "SDL2/SDL.h"

const int GRIDWIDTH{ 10 };
const int GRIDHEIGHT{ 10 };

const int WINDOWWIDTH  { 1000 };
const int WINDOWHEIGHT { 1000 };

void GamePlayMode(DataFile& dataFile, Window& window, Game& game);
void GameReplayMode(DataFile& dataFile, Window& window);
void GameDrumMode(DataFile& dataFile, Window& window, Game& game);

std::vector<ReplayMode> GetAllReplays(DataFile& dataFile);

FoodType DecodeFoodTypeFromFile(char fT);

int main(int argv, char* argc[])
{
	DataFile dataFile("data.txt");
	Window window(WINDOWWIDTH, WINDOWHEIGHT);
	Screen currentState{ window.SelectScreen() };

	if (currentState == Screen::PLAY)
	{
		Game game(GRIDWIDTH, GRIDHEIGHT, dataFile);
		GamePlayMode(dataFile, window, game);
	}
	else if (currentState == Screen::REPLAY) // REPLAY
	{
		GameReplayMode(dataFile, window);
	}
	else if (currentState == Screen::DRUM)
	{
		Game game(GRIDWIDTH, GRIDHEIGHT, dataFile);
		GameDrumMode(dataFile, window, game);
	}

    return 0;
}

void
GamePlayMode(DataFile& dataFile, Window& window, Game& game)
{
	{ // start screen
		//Enable text input
		SDL_StartTextInput();

		while (game.mGameState == GameState::START)
		{
			window.PollEvents(game);
			window.Draw(game);
		}

		//Disable text input
		SDL_StopTextInput();

		dataFile.WriteHeader(game.mPlayer, GRIDWIDTH, GRIDHEIGHT);
		//dataFile.WriteSnakePos(game.GetSnakeHead());
	}

	// Game play loop
	while (game.mGameState != GameState::QUIT)
	{
		// HandleInput/Update Snake
		GridIndex index = window.HandleInput(game);
		window.TryToPlayMusic();

		if (game.mGameState == GameState::RUN)
		{
			if (SDL_TICKS_PASSED(SDL_GetTicks(), window.mNextUpdate))
			{
				if (game.UpdateSnake(index))
					window.TryToPlaySoundFX();

				game.UpdateFood();
				window.UpdateUpdateTimer();
			}
		}
		else if (game.mGameState == GameState::STOP)
		{
			window.Draw(game);
			dataFile.WriteScore(game.mScore, game.mNumOfGamesPlayed);
			window.PollEvents(game);
		}

		window.Draw(game);
	}
}

void
GameReplayMode(DataFile& dataFile, Window& window)
{
	std::string move{};
	std::vector<ReplayMode> replays{ GetAllReplays(dataFile)  };

	for (ReplayMode& replay : replays)
	{
		// Game Replay loop
		while (replay.mGameState != GameState::QUIT)
		{
			// Get input
			GameInput gInput = replay.GetNextInput();

			if (std::get<0>(gInput) == 'e')
				replay.mGameState = GameState::STOP;

			window.TryToPlayMusic();

			if (replay.mGameState == GameState::RUN)
			{
				if ( std::get<0>(gInput) == 's' and replay.UpdateSnake(std::get<2>(gInput)) )
					window.TryToPlaySoundFX();

				if (std::get<0>(gInput) == 'f')
					replay.UpdateFood( std::get<2>(gInput), DecodeFoodTypeFromFile(std::get<1>(gInput)) );

			}
			else if (replay.mGameState == GameState::STOP)
			{
				window.Draw(replay);
				dataFile.WriteScore(replay.mScore, replay.mNumOfGamesPlayed);
				//window.PollEvents(replay);
				SDL_Delay(2000);
				replay.mGameState = GameState::QUIT;
			}

			window.Draw(replay);
		}
	}
}


void 
GameDrumMode(DataFile& dataFile, Window& window, Game& game)
{
	{ // start screen
		//Enable text input
		SDL_StartTextInput();

		while (game.mGameState == GameState::START)
		{
			window.PollEvents(game);
			window.Draw(game);
		}

		//Disable text input
		SDL_StopTextInput();

		dataFile.WriteHeader(game.mPlayer, GRIDWIDTH, GRIDHEIGHT);
		//dataFile.WriteSnakePos(game.GetSnakeHead());
	}

	// Game play loop
	while (game.mGameState != GameState::QUIT)
	{
		// HandleInput/Update Snake
		GridIndex index = window.HandleInput(game);
		//window.TryToPlayMusic();

		if (game.mGameState == GameState::RUN)
		{
			if (SDL_TICKS_PASSED(SDL_GetTicks(), window.mNextUpdate))
			{
				if (auto food = game.UpdateSnake(index))
					window.TryToPlayDrumSound(*food);

				game.UpdateFood();
				window.UpdateUpdateTimer();
			}
		}
		else if (game.mGameState == GameState::STOP)
		{
			window.Draw(game);
			dataFile.WriteScore(game.mScore, game.mNumOfGamesPlayed);
			window.PollEvents(game);
		}

		window.Draw(game);
	}
}

FoodType DecodeFoodTypeFromFile(char fT)
{
	switch (fT)
	{
	case 'o':
		return ORANGE;
	case 'c':
		return COCONUT;
	case 'b':
		return BANNANA;
	case 'w':
		return WATERMELLON;
	};

	return WATERMELLON;
}


std::vector<ReplayMode> 
GetAllReplays(DataFile& dataFile)
{
	std::vector<ReplayMode> replays;
	while (not dataFile.eof() and not dataFile.bad())
	{
		ReplayMode game(dataFile);
		dataFile.InitReplayFromFile(&game);
		replays.push_back(game);
	}

	return replays;
}