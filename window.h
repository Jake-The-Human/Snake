#ifndef WINDOW_H
#define WINDOW_H

#include "game.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_mixer.h"

#include <string>
#include <vector>

enum class Screen{ SELECT, PLAY, REPLAY, DRUM, SEQ, EXIT };

struct Window
{
    Window(int windowWidth, int windowHeight);
    
    Window(const Window&) = delete;
    Window(Window&&) = delete;

    ~Window();

	Screen SelectScreen();

	void UpdateUpdateTimer(unsigned score = 1);

	void PollEvents(Game& game);
    GridIndex HandleInput(Game& game);
	void TryToPlayMusic();
	void TryToPlaySoundFX();
	void TryToPlayDrumSound(const FoodType& f);

    void Draw(const Game& game);
	void DrawReplaySelect(const std::vector<ReplayMode>&, int);

	bool LoadTextureFromText(const std::string& text, const  SDL_Color& color);
	void FreeTexture(SDL_Texture*);

	struct Font
	{
		TTF_Font*	  mFontP{ nullptr };
		int mWidth;
		int mHeight;
	};

    SDL_Window*   mWindow   {nullptr};

	SDL_GameController* mController{nullptr};

    SDL_Renderer* mRenderer {nullptr};
    SDL_Texture*  mTexture  {nullptr};

	Mix_Music* mMusic{ nullptr };
	Mix_Chunk* mEatingSound{ nullptr };
	Mix_Chunk* mSnareSound{ nullptr };
	Mix_Chunk* mKickSound{ nullptr };
	Mix_Chunk* mHHSound{ nullptr };

	Font		  mFont		{nullptr, 0, 0};

	unsigned int mNextUpdate{0};

	SDL_Scancode mPrevScanCode{ SDL_Scancode::SDL_SCANCODE_UP };

    int mWindowWidth;
    int mWindowHeight;

private:

	void Reset(Game& g);

	void DrawStartMenu(const Game& game);

	void DrawGameHelper(const Game& game, const std::string& score);
	void DrawPauseHelper(const Game& game);
	void DrawStopHelper(const Game& game, const std::string& score);

	void RunEventLoop(Game& g);
	void StartEventLoop(Game& g);
	void StopEventLoop(Game& g);

	bool MovedUp(const Uint8* currentKeyStates) const;
	bool MovedDown(const Uint8* currentKeyStates) const;
	bool MovedLeft(const Uint8* currentKeyStates) const;
	bool MovedRight(const Uint8* currentKeyStates) const;

	void StringDrawHelper(std::string&, const SDL_Rect&);
	
};

#endif