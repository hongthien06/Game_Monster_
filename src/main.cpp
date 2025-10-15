#include<SDL3/SDL.h>
#include<SDL3/SDL_main.h>

using namespace std;
void cleanup(SDL_Window* win);
void running();
int main(int argc, char* argv[]) {
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ERROR", "ERROR", nullptr);
		return 1;
	}
	//Create window
	int width = 1280;
	int height = 720;
	SDL_Window* win = SDL_CreateWindow("Game", width, height, 0);
	if (!win) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ERROR", "ERROR", nullptr);
		cleanup(win);
		return 1;
	}
	//Start game loop
	running();

	cleanup(win);
	return 0;
}
void cleanup(SDL_Window* win) {
	SDL_DestroyWindow(win);
	SDL_Quit();
}
void running() {
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
	}
}