all:
	g++ -I src/include -L src/lib -o main.exe main.cpp -lmingw32 -lSDL3 -lSDL3_image -static-libgcc -static-libstdc++