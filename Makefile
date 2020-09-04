default:
	g++ src/main.cpp -o artly

debug:
	g++ src/main.cpp -DDEBUG -o artly