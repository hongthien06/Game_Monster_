#include<SDL3/SDL.h>
#include<SDL3/SDL_main.h>
#include<SDL3_image/SDL_image.h>

using namespace std;

struct SDLState {
	SDL_Window* window;
	SDL_Renderer* renderer;
};

void cleanup(SDLState &state);
int main(int argc, char* argv[]) {
	SDLState state;
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ERROR", "ERROR", nullptr);
		return 1;
	}
	//Create window
	int width = 1280;
	int height = 720;

	state.window= SDL_CreateWindow("Game", width, height, 0);
	if (!state.window) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ERROR", "ERROR", nullptr);
		cleanup(state);
		return 1;
	}
	state.renderer = SDL_CreateRenderer(state.window, nullptr);
	if(!state.renderer){
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ERROR", "ERROR", state.window);
		cleanup(state);
		return 1;
	}
	//load game assets
	SDL_Texture* idleTex = IMG_LoadTexture(state.renderer, "assets/images/idle.png");
	SDL_SetTextureScaleMode(idleTex, SDL_SCALEMODE_NEAREST);
	//Start game loop
	bool running = true;
	while (running) {
		SDL_Event event{ 0 };
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				//Click quit out window
			case SDL_EVENT_QUIT: {
				running = false;
				break;
			}
			}
		}
		SDL_SetRenderDrawColor(state.renderer, 20, 10, 30, 255);
		SDL_RenderClear(state.renderer);
		SDL_FRect src{
			.x = 0,
			.y = 0,
			.w = 32,
			.h = 32
		};

		SDL_FRect dst{
			.x = 0,
			.y = 0,
			.w = 96,
			.h = 96
		};

		SDL_RenderTexture(state.renderer, idleTex, &src, &dst);

		SDL_RenderPresent(state.renderer);
	}
	SDL_DestroyTexture(idleTex);
	cleanup(state);
	return 0;
}
void cleanup(SDLState& state) {
	SDL_DestroyRenderer(state.renderer);
	SDL_DestroyWindow(state.window);
	SDL_Quit();
}