#include "src/draw.h"
#include "src/board.h"
#include <string>
#include <iostream>

using namespace board7x7;

int main()
{
	Board board;

	for (Si i = 0; i < 100; )
	{
		std::cout << board.search() << std::endl;
		board.playBestMove();
		drawBoard(board);
		std::cout << std::endl << std::string(75, '-') << std::endl;

		if (!board.verify())
			throw "Error, whu8fewb huvfe uho !!!";
	}

	return 0;
}