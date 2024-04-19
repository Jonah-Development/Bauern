#include "src/draw.h"
#include "src/board.h"
#include <string>
#include <iostream>

using namespace board7x7;

int main()
{
	Board board;

	for (Si i = 0; i < 100; i++)
	{
		std::cout << board.playBestMove((i & 1) ? 5 : 3) << std::endl;
		board.draw();
		std::cout << '[' << board.getBestMove() << "], ";
		std::cout << std::endl << std::string(75, '-') << std::endl;

		if (!board.verify())
			throw "Error, corrupted bitboards";
	}

	return 0;
}