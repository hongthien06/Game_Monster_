#include<SDL3/SDL.h>
#include<SDL3/SDL_main.h>
#include<SDL3_image/SDL_image.h>
#include <algorithm>

using namespace std;

struct SDLState {
	SDL_Window* window;
	SDL_Renderer* renderer;
	int width, height, logH, logW;
};

bool initalize(SDLState& state);
void cleanup(SDLState &state);
int main(int argc, char* argv[]) {
	SDLState state;
	state.height = 900;
	state.width = 1600;
	state.logH = 320;
	state.logW = 640;
	if (!initalize(state)) {
		return 1;
	}
	//setup game data
	const bool* keys = SDL_GetKeyboardState(nullptr);
	float playerX = 150.0f;
	float playerVelocityX = 0.0f; 
	const float acceleration = 500.0f;  //Toc do tang toc
	const float deceleration = 600.0f;  //Toc do dung lai
	const float maxSpeed = 200.0f;      
	const float floor = state.logH;

	//load game assets
	SDL_Texture* idleTex = IMG_LoadTexture(state.renderer, "assets/images/idle.png");
	if (!idleTex) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ERROR", "ERROR", state.window);
		cleanup(state);
		return 1;
	}
	SDL_SetTextureScaleMode(idleTex, SDL_SCALEMODE_NEAREST);
	uint64_t prevTime = SDL_GetTicks();
	//Start game loop
	bool running = true;
	while (running) {
		uint64_t nowTime = SDL_GetTicks();
		float deltaTime = (nowTime - prevTime) / 1000.0f;
		SDL_Event event{ 0 };
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				//Click quit out window
			case SDL_EVENT_QUIT: {
				running = false;
				break;
			}
			case SDL_EVENT_WINDOW_RESIZED: {
				state.width = event.window.data1;
				state.height = event.window.data2;
				break;
			}
			}
		}
		//handle movement
		int moveDirection = 0;
		if (keys[SDL_SCANCODE_A]) {
			moveDirection = -1;;
		}
		if (keys[SDL_SCANCODE_D]) {
			moveDirection = 1;
		}
		if (moveDirection != 0) {
			playerVelocityX += acceleration * moveDirection * deltaTime;
		}
		else {
			if (playerVelocityX > 0) {
				playerVelocityX -= deceleration * deltaTime;
				if (playerVelocityX < 0) playerVelocityX = 0; 
			}
			else if (playerVelocityX < 0) {
				playerVelocityX += deceleration * deltaTime;
				if (playerVelocityX > 0) playerVelocityX = 0; 
			}
		}
		playerVelocityX = std::max(-maxSpeed, std::min(playerVelocityX, maxSpeed));
		playerX += playerVelocityX * deltaTime;


		SDL_SetRenderDrawColor(state.renderer, 255, 255, 255, 255);
			SDL_RenderClear(state.renderer);
			const float spriteSize = 48;
			SDL_FRect src{
				.x = 0,
				.y = 0,
				.w = 120,
				.h = 130
			};

			SDL_FRect dst{
				.x = playerX,
				.y = floor - spriteSize,
				.w = spriteSize,
				.h = spriteSize 
			};

			SDL_RenderTexture(state.renderer, idleTex, &src,&dst);

		SDL_RenderPresent(state.renderer);
		prevTime = nowTime;
	}
	SDL_DestroyTexture(idleTex);
	cleanup(state);
	return 0;
}
bool initalize(SDLState& state) {
	bool initSucces = true;
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ERROR", "ERROR", nullptr);
		initSucces = false;
	}
	//Create window

	state.window = SDL_CreateWindow("Game", state.width, state.height, SDL_WINDOW_RESIZABLE);
	if (!state.window) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ERROR", "ERROR", nullptr);
		cleanup(state);
		initSucces = false;
	}
	state.renderer = SDL_CreateRenderer(state.window, nullptr);
	if (!state.renderer) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ERROR", "ERROR", state.window);
		cleanup(state);
		initSucces = false;
	}
	//configure presentation
	SDL_SetRenderLogicalPresentation(state.renderer, state.logW, state.logH, SDL_LOGICAL_PRESENTATION_LETTERBOX);

	return initSucces;
}
void cleanup(SDLState& state) {
	SDL_DestroyRenderer(state.renderer);
	SDL_DestroyWindow(state.window);
	SDL_Quit();
}