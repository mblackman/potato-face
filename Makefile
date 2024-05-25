build:
	mkdir -p bin
	g++ -Wall -std=c++17 -I "./libs" src/*.cpp -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -llua5.3 -o bin/gameengine

run:
	./bin/gameengine

clean:
	rm -rf bin/