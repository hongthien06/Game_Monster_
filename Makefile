all:
	g++ -I src/include -I src/Core -I src/Systems -I src/Entities -I src/Components -I src/Environment -I src/Config -L src/lib -o main.exe src/main.cpp src/Core/Game.cpp src/Environment/Map.cpp -lmingw32 -lSDL3 -lSDL3_image -static-libgcc -static-libstdc++
