#include "window.h"

#include <limits>
#include <cstdio>
#include <cassert>

#include <string>

static void DrawColoredRect(SDL_Renderer* renderer, const SDL_Rect& r, const SDL_Color& c);
static GridIndex SnakeHelper(SDL_Scancode code, const Game& g);

Window::Window(int windowWidth, int windowHeight) :
mWindowWidth(windowWidth),
mWindowHeight(windowHeight)
{    
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) < 0)
    {
        printf("SDL could not initialize. SDL_Error %s", SDL_GetError());
        exit(1);
    }
    else
    {
		//Set texture filtering to linear
		if ( not SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}
        
        // Create window
        mWindow = SDL_CreateWindow("Snake", 
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			windowWidth,
			windowHeight,
                                   SDL_WINDOW_SHOWN);
        
        if(mWindow == nullptr)
        {
            printf("Window could not be created. SDL_Error %s", SDL_GetError());
            exit(1);
        }
        else
        {
			// Check for joystick
			if( SDL_NumJoysticks() < 1 )
			{
				printf("No JoyStick Connected!\n");
			}
			else
			{
				// Load joystick
				mController = SDL_GameControllerOpen(0);
				if ( mController == nullptr )
					printf("Unable to open game controller. SDL Error: %s\n", SDL_GetError());
			}
			
            mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if (mRenderer == nullptr)
            {
                printf("Renderer failed");
                exit(1);
            }

			//Initialize SDL_ttf
			if (TTF_Init() > 0)
			{
				printf("SDL_ttf did not initialize. SDL_ttf Error: %s\n", TTF_GetError());
				exit(1);
			}

			//Open the font
			const int fontSize{ 64 };
			mFont.mFontP = TTF_OpenFont("major-mono-display-v2-latin-regular.ttf", fontSize);
			if (mFont.mFontP == nullptr)
			{
				printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
				exit(1);
			}

			// Initialize SDL_mixer
			int stereo{ 2 };
			if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, stereo, 2048) < 0)
			{
				printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
				exit(1);
			}

			// Load Music
			mMusic = Mix_LoadMUS("Audio/MySong3(bass).mp3");
			if(mMusic == nullptr)
			{
				printf("Music failed to load. SDL_mixer Error: %s\n", Mix_GetError());
				exit(1);
			}

			//Load sound effects
			mEatingSound = Mix_LoadWAV("Audio/eatingFX.wav");
			if (mEatingSound == nullptr)
			{
				printf("Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError());
				exit(1);
			}

			mSnareSound = Mix_LoadWAV("Audio/snare.wav");
			if (mEatingSound == nullptr)
			{
				printf("Failed to load snare sound effect! SDL_mixer Error: %s\n", Mix_GetError());
				exit(1);
			}

			mKickSound = Mix_LoadWAV("Audio/kick01.wav");
			if (mEatingSound == nullptr)
			{
				printf("Failed to load kick sound effect! SDL_mixer Error: %s\n", Mix_GetError());
				exit(1);
			}

			mHHSound = Mix_LoadWAV("Audio/hihat02.wav");
			if (mEatingSound == nullptr)
			{
				printf("Failed to load hihat sound effect! SDL_mixer Error: %s\n", Mix_GetError());
				exit(1);
			}

			//Initialize renderer color
			SDL_SetRenderDrawColor(mRenderer, 0x00, 0x00, 0x00, 0xFF);
        }
     
		UpdateUpdateTimer();
    }
}

Window::~Window()
{
	// Free controller
	SDL_GameControllerClose(mController);
	mController = nullptr;

	// Free Texture
	FreeTexture(mTexture);
	mTexture = nullptr;

	// Free sound effects
	Mix_FreeChunk(mHHSound);
	mHHSound = nullptr;

	// Free sound effects
	Mix_FreeChunk(mKickSound);
	mKickSound = nullptr;

	// Free sound effects
	Mix_FreeChunk(mSnareSound);
	mSnareSound = nullptr;

	// Free sound effects
	Mix_FreeChunk(mEatingSound);
	mEatingSound = nullptr;

	// Free Music
	Mix_FreeMusic(mMusic);
	mMusic = nullptr;

	//Free font
	TTF_CloseFont(mFont.mFontP);
	mFont.mFontP = nullptr;

    // Deallocate renderer
    SDL_DestroyRenderer(mRenderer);
	mRenderer = nullptr;
    
    //Destroy window
    SDL_DestroyWindow(mWindow);
    mWindow = nullptr;
    
	Mix_Quit();
	TTF_Quit();
    SDL_Quit();
}


Screen 
Window::SelectScreen()
{	
	//Clear screen
	SDL_SetRenderDrawColor(mRenderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderClear(mRenderer);

	Screen state{ Screen::SELECT };
	puts("test");
	// std::string options{ "1. Play Normal Snake\n2. Replay Mode\n3. Nothing\n4. Nothing\n5. Exit" };
	// Draw start screen
	std::string optionOne{ "1. Normal Snake" };
	std::string optionTwo{ "2. Replay Mode" };
	std::string optionThree{ "3. Drum Mach" };
	std::string optionFour{ "4. Nothing" };
	std::string optionFive{ "5. Exit" };

	//Set rendering space and render to screen
	SDL_Rect QuadOne{ 10, 0, mWindowWidth / 2, 100 };
	SDL_Rect QuadTwo{ 10, 110, mWindowWidth / 2, 100 };
	SDL_Rect QuadThree{ 10, 220, mWindowWidth / 2, 100 };
	SDL_Rect QuadFour{ 10, 330, mWindowWidth / 2, 100 };
	SDL_Rect QuadFive{ 10, 440, mWindowWidth / 2, 100 };

	//Render to screen
	StringDrawHelper(optionOne, QuadOne);
	StringDrawHelper(optionTwo, QuadTwo);
	StringDrawHelper(optionThree, QuadThree);
	StringDrawHelper(optionFour, QuadFour);
	StringDrawHelper(optionFive, QuadFive);

	//Render to screen
	SDL_RenderPresent(mRenderer);

	while (state == Screen::SELECT)
	{
		SDL_Event e;
		//Handle events on queue
		while (SDL_PollEvent(&e) != 0)
		{
			//User requests quit
			if (e.type == SDL_QUIT)
			{
				state = Screen::EXIT;
			}
			else if (e.type == SDL_CONTROLLERBUTTONDOWN)
			{
				switch (e.cbutton.button)
				{
					// case SDL_CONTROLLER_BUTTON_BACK:
						// Reset(g);
						// g.mGameState = GameState::RUN;
						// break;
					// case SDL_CONTROLLER_BUTTON_START:
						// g.mGameState = GameState::QUIT;
						// break;
				};
			}
			else if (e.type == SDL_KEYDOWN)
			{
				switch (e.key.keysym.sym)
				{
				case SDLK_1:
					state = Screen::PLAY;
					break;
				case SDLK_2:
					state = Screen::REPLAY;
					break;
				case SDLK_3:
					state = Screen::DRUM;
					break;
				case SDLK_4:
					state = Screen::SELECT; // SEQ;
					break;
				case SDLK_ESCAPE:
				case SDLK_5:
					state = Screen::EXIT;
					break;
				};
			}
		}
	}

	return state;
}

void 
Window::UpdateUpdateTimer(unsigned score)
{ 
	score = { score > 0 ? score : 1 };
	mNextUpdate = SDL_GetTicks() + (200 / score); 
}

void 
Window::PollEvents(Game& g)
{
	if (g.mGameState == GameState::RUN or g.mGameState == GameState::PAUSE)
	{
		RunEventLoop(g);
	}
	else if (g.mGameState == GameState::START)
	{
		StartEventLoop(g);
	}
	else if (g.mGameState == GameState::STOP)
	{
		StopEventLoop(g);
	}
}

static GridIndex
SnakeHelper(SDL_Scancode code, const Game& g) 
{
	int newHeadIndex{ g.GetSnakeHead() };

	int x{ g.GetSnakeHead() % g.mWidthOfGrid };
	int y{ g.GetSnakeHead() / g.mWidthOfGrid };

	if (code == SDL_SCANCODE_UP and code != SDL_SCANCODE_DOWN)
	{
		if ((y - 1) >= 0)
		{
			newHeadIndex -= g.mWidthOfGrid;
		}
	}
	else if (code == SDL_SCANCODE_DOWN and code != SDL_SCANCODE_UP)
	{
		if ((y + 1) < g.mHeightOfGrid)
			newHeadIndex += g.mWidthOfGrid;
	}
	else if (code == SDL_SCANCODE_LEFT and code != SDL_SCANCODE_RIGHT)
	{
		if ((x - 1) >= 0)
			newHeadIndex -= 1;
	}
	else if (code == SDL_SCANCODE_RIGHT and code != SDL_SCANCODE_LEFT)
	{
		if ((x + 1) < g.mWidthOfGrid)
			newHeadIndex += 1;
	}

	return newHeadIndex;
}


GridIndex
Window::HandleInput(Game& g)// might want to to this async
{
    int newHeadIndex {g.GetSnakeHead()};
	
    //int entityWidth  { mWindowWidth  / g.mWidthOfGrid };
    //int entityHeight { mWindowHeight / g.mHeightOfGrid};

	PollEvents(g); //TODO:: need to refactored to just return event not change the game state
    
	int x{ g.GetSnakeHead() % g.mWidthOfGrid };
	int y{ g.GetSnakeHead() / g.mWidthOfGrid };
    
    const Uint8* currentKeyStates = SDL_GetKeyboardState( nullptr );

    if( MovedUp(currentKeyStates) )
    {
        if( (y - 1) >= 0)
        {
            newHeadIndex -= g.mWidthOfGrid;
			mPrevScanCode = SDL_SCANCODE_UP;
        }
    } 
    else if( MovedDown(currentKeyStates) )
    {
		if ((y + 1) < g.mHeightOfGrid)
		{
			newHeadIndex += g.mWidthOfGrid;
			mPrevScanCode = SDL_SCANCODE_DOWN;
		}
    }
    else if( MovedLeft(currentKeyStates) )
    {
		if ((x - 1) >= 0)
		{
			newHeadIndex -= 1;
			mPrevScanCode = SDL_SCANCODE_LEFT;
		}
    }
    else if( MovedRight(currentKeyStates) )
    {
		if ((x + 1) < g.mWidthOfGrid)
		{
			newHeadIndex += 1;
			mPrevScanCode = SDL_SCANCODE_RIGHT;
		}
    }
    else
    {
		newHeadIndex = SnakeHelper(mPrevScanCode, g);
    }

	return newHeadIndex;
}

void 
Window::TryToPlayMusic()
{
	if (Mix_PlayingMusic() == 0 and mMusic != nullptr)
		Mix_PlayMusic(mMusic, -1);
}

void
Window::TryToPlaySoundFX()
{
	if(mEatingSound != nullptr)
		Mix_PlayChannel(-1, mEatingSound, 0);
}

void
Window::TryToPlayDrumSound(const FoodType& f)
{
	switch (f)
	{
		case FoodType::BANNANA:
		{
			if(mSnareSound != nullptr)
				Mix_PlayChannel(-1, mSnareSound, 0);
			break;
		}
		case FoodType::COCONUT:
		{
			if(mKickSound != nullptr)
				Mix_PlayChannel(-1, mKickSound, 0);
			break;
		}
		case FoodType::ORANGE:
		{
			if(mHHSound != nullptr)
				Mix_PlayChannel(-1, mHHSound, 0);
			break;
		}
		case FoodType::WATERMELLON:
		{
			// skip this is for the seq so you can build
			// seq with rests
			break;
		}
		case FoodType::NUMOFFOOD:
		default:
		{}// don't care about this one
	};
}

void 
Window::Draw(const Game& game)
{
	//Clear screen
	SDL_SetRenderDrawColor(mRenderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderClear(mRenderer);

	// Make score string
	std::string score{ "000000" };
	std::string tempScore{ std::to_string(game.mScore) };

	if (tempScore.size() <= score.size())
		score.replace(score.size() - tempScore.size(), tempScore.size(), tempScore);
	else
		score = "PRO!!";

	// Draw game
	if (game.mGameState == GameState::RUN)
	{
		DrawGameHelper(game, score);
	}
	// Draw pause menu
	else if (game.mGameState == GameState::PAUSE)
	{
		DrawPauseHelper(game);
	}
	else if (game.mGameState == GameState::START)
	{
		DrawStartMenu(game);
	}
	// Draw Game over
	else // (game.mGameState == GameState::STOP)
	{
		DrawStopHelper(game, score);
	}

	SDL_RenderPresent(mRenderer);
}

bool 
Window::LoadTextureFromText(const std::string& text, const  SDL_Color& color)
{
	//Get rid of preexisting texture
	FreeTexture(mTexture);

	//Render text surface
	SDL_Surface* textSurface{ TTF_RenderText_Solid(mFont.mFontP, text.c_str(), color) };
	if (textSurface == NULL)
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}
	else
	{
		//Create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface(mRenderer, textSurface);
		if (mTexture == NULL)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mFont.mWidth  = textSurface->w;
			mFont.mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface(textSurface);
	}

	//Return success
	return mTexture != nullptr;
}

void 
Window::FreeTexture(SDL_Texture* texture)
{
	if (texture != nullptr)
	{
		SDL_DestroyTexture(texture);
		texture = nullptr;
	}
}

static void
DrawColoredRect(SDL_Renderer* renderer, const SDL_Rect& rect, const SDL_Color& c)
{
	SDL_SetRenderDrawColor(renderer, c.r, c.g, c.g, c.a);
	SDL_RenderFillRect(renderer, &rect);
}


void Window::Reset(Game& g)
{
	g.Reset();
	mPrevScanCode = SDL_Scancode::SDL_SCANCODE_UP;
}

void
Window::DrawStartMenu(const Game& game)
{
	// Draw start screen
	SDL_Color textColor = { 0xFF, 0xFF, 0xFF, 0xFF };
	if (not LoadTextureFromText("SNAKE", textColor))
	{
		printf("Failed to render text texture!\n");
	}

	//Set rendering space and render to screen
	SDL_Rect renderQuad = { mWindowWidth / 4, mWindowHeight/4, mFont.mWidth, mFont.mHeight };

	//Render to screen
	SDL_RenderCopy(mRenderer, mTexture, nullptr, &renderQuad);

	std::string name = game.mPlayer;
	// Draw user name
	if (game.mPlayer != "")
	{

		if (not LoadTextureFromText(name.c_str(), textColor))
		{
			printf("Failed to render text texture!\n");
		}

		//Set rendering space and render to screen
		renderQuad = { (mWindowWidth / 4), (mWindowHeight / 4) + mFont.mHeight, mFont.mWidth, mFont.mHeight };

		//Render to screen
		SDL_RenderCopy(mRenderer, mTexture, nullptr, &renderQuad);
	}
}


void 
Window::DrawGameHelper(const Game& game, const std::string& score)
{
	//Top left corner viewport
	SDL_Rect topViewport{ 0, 0, mWindowWidth, mWindowHeight / 8 };
	SDL_RenderSetViewport(mRenderer, &topViewport);

	// Draw user name and score
	SDL_Color textColor = { 0xFF, 0xFF, 0xFF, 0xFF };
	if (not LoadTextureFromText("PLAYER: "+ game.mPlayer +"    SCORE: " + score + " ", textColor))
	{
		printf("Failed to render text texture!\n");
	}

	//Set rendering space and render to screen
	SDL_Rect renderQuad = { 0, 0, topViewport.w, topViewport.h };

	//Render to screen
	SDL_RenderCopy(mRenderer, mTexture, nullptr, &renderQuad);

	//Bottom viewport
	SDL_Rect bottomViewport{ 0, mWindowHeight / 8, mWindowWidth,  mWindowHeight - (mWindowHeight / 8) };
	SDL_RenderSetViewport(mRenderer, &bottomViewport);

	int entityWidth{ bottomViewport.w / game.mWidthOfGrid };
	int entityHeight{ bottomViewport.h / game.mHeightOfGrid };

	// Draw fruit
	SDL_Rect fruitRect{ 0, 0, entityWidth, entityHeight };
	// Orange, White, Yellow, Green
	SDL_Color color[] = { { 255, 165, 0,  255 }, {255, 255, 255, 255}, {255, 215, 0, 255}, {10, 255, 10, 255} };

	for (auto f : game.mFood)
	{
		fruitRect.x = (f.mIndex % game.mWidthOfGrid)  * entityWidth;
		fruitRect.y = (f.mIndex / game.mHeightOfGrid) * entityHeight;

		DrawColoredRect(mRenderer, fruitRect, color[f.mType]);
	}


	// Rect reminder {x, y, w, h}
	SDL_Rect snakeRect{ 0, 0, entityWidth, entityHeight };
	//Draw RED snake
	SDL_SetRenderDrawColor(mRenderer, 0xFF, 0x00, 0x00, 0xFF);
	
	game.mSnake.foreach(
		[&](int snakePart)	{
		snakeRect.x = (snakePart % game.mWidthOfGrid)  * entityWidth;
		snakeRect.y = (snakePart / game.mHeightOfGrid) * entityHeight;

		SDL_RenderFillRect(mRenderer, &snakeRect);
	 }
	);
	


	// ReDraw the snake head green for debugging
	SDL_SetRenderDrawColor(mRenderer, 0x00, 0xFF, 0x00, 0xFF);
	snakeRect.x = (game.GetSnakeHead() % game.mWidthOfGrid)  * entityWidth;
	snakeRect.y = (game.GetSnakeHead() / game.mHeightOfGrid) * entityHeight;

	SDL_RenderFillRect(mRenderer, &snakeRect);

	//Draw Dividing line Top
	SDL_SetRenderDrawColor(mRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderDrawLine(mRenderer, 0, 0, bottomViewport.w, 0);
	SDL_RenderDrawLine(mRenderer, 0, 0 + 1, bottomViewport.w, 0 + 1);

	//Draw Dividing line Bottom
	//SDL_SetRenderDrawColor(mRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderDrawLine(mRenderer, 0, (game.mHeightOfGrid * entityHeight), bottomViewport.w,
						 (game.mHeightOfGrid * entityHeight));
	SDL_RenderDrawLine(mRenderer, 0, (game.mHeightOfGrid * entityHeight) - 1, bottomViewport.w,
						 (game.mHeightOfGrid * entityHeight) - 1);

	//Draw Dividing line Right
	//SDL_SetRenderDrawColor(mRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderDrawLine(mRenderer, 0, (game.mHeightOfGrid * entityHeight), 
						(game.mWidthOfGrid * entityWidth) - 1, (game.mHeightOfGrid * entityHeight));
	SDL_RenderDrawLine(mRenderer, 0, (game.mHeightOfGrid * entityHeight), 
						(game.mWidthOfGrid * entityWidth) - 1, (game.mHeightOfGrid * entityHeight));

	//Draw Dividing line Left
	//SDL_SetRenderDrawColor(mRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderDrawLine(mRenderer, 0, 0, 0, (game.mHeightOfGrid * entityHeight));
	SDL_RenderDrawLine(mRenderer, (game.mWidthOfGrid * entityWidth) - 1, 0,
						 (game.mWidthOfGrid * entityWidth) - 1, (game.mHeightOfGrid * entityHeight));
	
	//Debug stuff
	bool DEBUG{ false };
	if ( DEBUG )
	{
		SDL_SetRenderDrawColor(mRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		for (int i = 0; i < game.mWidthOfGrid; ++i)
		{
			SDL_RenderDrawLine(mRenderer, i * entityWidth, 0, i * entityWidth, bottomViewport.h);
		}

		for (int i = 1; i < game.mHeightOfGrid; ++i)
		{
			SDL_RenderDrawLine(mRenderer, 0, i * entityHeight, bottomViewport.w, i * entityHeight);
		}

		printf("%d ", game.GetSnakeHead());
	}
}


void
Window::DrawPauseHelper(const Game& game)
{
	SDL_Color textColor = { 0xFF, 0xFF, 0xFF, 0xFF };
	if (not LoadTextureFromText("PAUSED ", textColor))
	{
		printf("Failed to render text texture!\n");
	}
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { mWindowWidth / 2,  mWindowHeight / 2, mFont.mWidth,  mFont.mHeight };
	//Render to screen
	SDL_RenderCopy(mRenderer, mTexture, nullptr, &renderQuad);
}


void
Window::DrawStopHelper(const Game& game, const std::string& score)
{
	SDL_Color textColor = { 0xFF, 0xFF, 0xFF, 0xFF };
	if (not LoadTextureFromText("GAME OVER!", textColor))
	{
		printf("Failed to render text texture!\n");
	}

	//Set rendering space and render to screen
	SDL_Rect renderQuad = { mWindowWidth / 8,  mWindowHeight / 2, mFont.mWidth,  mFont.mHeight };
	//Render to screen
	SDL_RenderCopy(mRenderer, mTexture, nullptr, &renderQuad);

	if (not LoadTextureFromText("Score: " + score, textColor))
	{
		printf("Failed to render text texture!\n");
	}
	//Set rendering space and render to screen
	renderQuad = { mWindowWidth / 8,  (mWindowHeight / 2) + mFont.mHeight, mFont.mWidth,  mFont.mHeight };
	SDL_RenderCopy(mRenderer, mTexture, nullptr, &renderQuad);

	// TODO:: need to add high score display
}

void 
Window::RunEventLoop(Game& g)
{
	SDL_Event e;
	//Handle events on queue
	while (SDL_PollEvent(&e) != 0)
	{
		//User requests quit
		if (e.type == SDL_QUIT)
		{
			g.mGameState = GameState::QUIT;
		}
		else if (e.type == SDL_CONTROLLERBUTTONDOWN)
		{
			switch (e.cbutton.button)
			{
				case SDL_CONTROLLER_BUTTON_BACK:
					g.mGameState = GameState::QUIT;
					break;
				case SDL_CONTROLLER_BUTTON_START:
								// should change to pause
				if (g.mGameState != GameState::PAUSE)
					g.mGameState = GameState::PAUSE;
				else
					g.mGameState = GameState::RUN;
					break;
			};
		}
		else if (e.type == SDL_KEYDOWN)
		{
			switch (e.key.keysym.sym)
			{
			case SDLK_0:
				if (g.mWidthOfGrid <= 500 and g.mHeightOfGrid <= 500)
				{
					g.mWidthOfGrid *= 2;
					g.mHeightOfGrid *= 2;
					g.UpdateSnake(g.GetSnakeHead());
				}
				break;
			case SDLK_MINUS:
				if (g.mWidthOfGrid > 2 and g.mHeightOfGrid > 2)
				{
					g.mWidthOfGrid /= 2;
					g.mHeightOfGrid /= 2;
					g.UpdateSnake(g.GetSnakeHead() / 2);
				}
				break;
			case SDLK_SPACE:
				// should change to pause
				if (g.mGameState != GameState::PAUSE)
					g.mGameState = GameState::PAUSE;
				else
					g.mGameState = GameState::RUN;

				break;
			case SDLK_ESCAPE:
				g.mGameState = GameState::STOP;
				break;
			};
		}
	}
}


void
Window::StartEventLoop(Game& g)
{
	SDL_Event e;
	//Handle events on queue
	while (SDL_PollEvent(&e) != 0)
	{
		if (e.type == SDL_TEXTINPUT)
		{
			//Not copy or pasting
			if (g.mPlayer.size() <= 3 and not ((e.text.text[0] == 'c' or
				 e.text.text[0] == 'C') and (e.text.text[0] == 'v' or 
				 e.text.text[0] == 'V') && SDL_GetModState() & KMOD_CTRL))
			{
				//Append character
				g.mPlayer += e.text.text;
				//renderText = true;
			}
		}

		//User requests quit
		if (e.type == SDL_QUIT)
		{
			g.mGameState = GameState::QUIT;
		}
		else if (e.type == SDL_KEYDOWN)
		{
		//Special text input event
		
			// Special key input
		if (e.type == SDL_KEYDOWN)
			{
				//Handle backspace
				if (e.key.keysym.sym == SDLK_BACKSPACE and g.mPlayer.length() > 0)
				{
					//lop off character
					g.mPlayer.pop_back();
				}
				//Handle copy
				else if (e.key.keysym.sym == SDLK_c and SDL_GetModState() & KMOD_CTRL)
				{
					SDL_SetClipboardText(g.mPlayer.c_str());
				}
				//Handle paste
				else if (e.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL)
				{
					g.mPlayer = SDL_GetClipboardText();
				}
				else if (e.key.keysym.sym == SDLK_ESCAPE)
				{
					g.mGameState = GameState::QUIT;
				}
				else if (e.key.keysym.sym == SDLK_RETURN)
				{
					g.mGameState = GameState::RUN;
				}
			}
		}
	}
}

void 
Window::StopEventLoop(Game& g)
{
	SDL_Event e;
	//Handle events on queue
	while (SDL_PollEvent(&e) != 0)
	{
		//User requests quit
		if (e.type == SDL_QUIT)
		{
			g.mGameState = GameState::QUIT;
		}
		else if (e.type == SDL_CONTROLLERBUTTONDOWN)
		{
			switch (e.cbutton.button)
			{
				case SDL_CONTROLLER_BUTTON_BACK:
					Reset(g);
					g.mGameState = GameState::RUN;
					break;
				case SDL_CONTROLLER_BUTTON_START:
					g.mGameState = GameState::QUIT;
					break;
			};
		}
		else if (e.type == SDL_KEYDOWN)
		{
			switch (e.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				g.mGameState = GameState::QUIT;
				break;
			case SDLK_r:
				Reset(g);
				g.mGameState = GameState::RUN;
				break;
			};
		}
	}
}

void 
Window::DrawReplaySelect(const std::vector<ReplayMode>& games, int index)
{
	index += 1; // this is need to make the graphics draw the selector in the right spot 0 * height = 0
	//Clear screen
	SDL_SetRenderDrawColor(mRenderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderClear(mRenderer);

	int selectionIndicator{ 50 };
	int i{ 1 };
	for (const ReplayMode& game : games)
	{
		// Draw user name and score
		SDL_Color textColor = { 0xFF, 0xFF, 0xFF, 0xFF };

		if (not LoadTextureFromText(game.mPlayer + " " + game.mDate + " " + std::to_string(game.mFinalScore), textColor))
		{
			printf("Failed to render text texture!\n");
		}

		//Set rendering space and render to screen
		SDL_Rect renderQuad{ selectionIndicator, selectionIndicator * i, mWindowWidth - selectionIndicator, mFont.mHeight };

		//Render to screen
		SDL_RenderCopy(mRenderer, mTexture, nullptr, &renderQuad);

		i += 1;
	}

	// Draw indicator
	SDL_Rect renderQuad{ 0, selectionIndicator * index, selectionIndicator, selectionIndicator };
	DrawColoredRect(mRenderer, renderQuad, { 0xFF, 0xFF, 0xFF, 0xFF });

	SDL_RenderPresent(mRenderer);
}


bool 
Window::MovedUp(const Uint8* currentKeyStates) const
{
	const Uint8 currentJoyStates{ SDL_GameControllerGetButton(mController, SDL_CONTROLLER_BUTTON_DPAD_UP) };
	//printf("%d, ", currentJoyStates); // this worked!!!!!!!!!!!!!!!!!!!!!!!!
	return (currentKeyStates[ SDL_SCANCODE_UP ] or currentJoyStates) and mPrevScanCode != SDL_SCANCODE_DOWN;
}


bool 
Window::MovedDown(const Uint8* currentKeyStates) const
{
	const Uint8 currentJoyStates{ SDL_GameControllerGetButton(mController, SDL_CONTROLLER_BUTTON_DPAD_DOWN) };

	return (currentKeyStates[ SDL_SCANCODE_DOWN ] or currentJoyStates) and mPrevScanCode != SDL_SCANCODE_UP;
}


bool
Window::MovedLeft(const Uint8* currentKeyStates) const
{
	const Uint8 currentJoyStates{ SDL_GameControllerGetButton(mController, SDL_CONTROLLER_BUTTON_DPAD_LEFT) };

	return (currentKeyStates[ SDL_SCANCODE_LEFT ] or currentJoyStates) and mPrevScanCode != SDL_SCANCODE_RIGHT;
}


bool
Window::MovedRight(const Uint8* currentKeyStates) const
{
	const Uint8 currentJoyStates{ SDL_GameControllerGetButton(mController, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) };

	return (currentKeyStates[ SDL_SCANCODE_RIGHT ] or currentJoyStates) and mPrevScanCode != SDL_SCANCODE_LEFT;
}


void 
Window::StringDrawHelper(std::string& s, const SDL_Rect& rect)
{
	SDL_Color textColor{ 0xff, 0xFF, 0xFF, 0xFF };
	if (not LoadTextureFromText(s.c_str(), textColor))
	{
		printf("Failed to render text texture!\n");
	}

	//Render to screen
	SDL_RenderCopy(mRenderer, mTexture, nullptr, &rect);
}
