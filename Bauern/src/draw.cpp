#include "board.h"
#include "draw.h"
#include <string>
#include <iostream>

using namespace board7x7;

char boardRaw[] = 
	"   |   |   |   |   |   |   \n"
	"   |   |   |   |   |   |   \n"
	"   |   |   |   |   |   |   \n"
	"---|---|---|---|---|---|---\n"
	"   |   |   |   |   |   |   \n"
	"   |   |   |   |   |   |   \n"
	"   |   |   |   |   |   |   \n"
	"---|---|---|---|---|---|---\n"
	"   |   |   |   |   |   |   \n"
	"   |   |   |   |   |   |   \n"
	"   |   |   |   |   |   |   \n"
	"---|---|---|---|---|---|---\n"
	"   |   |   |###|   |   |   \n"
	"   |   |   |###|   |   |   \n"
	"   |   |   |###|   |   |   \n"
	"---|---|---|---|---|---|---\n"
	"   |   |   |   |   |   |   \n"
	"   |   |   |   |   |   |   \n"
	"   |   |   |   |   |   |   \n"
	"---|---|---|---|---|---|---\n"
	"   |   |   |   |   |   |   \n"
	"   |   |   |   |   |   |   \n"
	"   |   |   |   |   |   |   \n"
	"---|---|---|---|---|---|---\n"
	"   |   |   |   |   |   |   \n"
	"   |   |   |   |   |   |   \n"
	"   |   |   |   |   |   |   \n";

size_t constexpr index(size_t i)
{
	i = 48 - i;

	const size_t index = (4 * i) + (i / 7) * (28 * 3) + 28 + 1;

	return index;
}

void Board::draw(void) const
{
	BB black = getBlack();
	BB white = getWhite();
	
	for (int32_t i = 0; i < 49; i++, black >>= 1, white >>= 1)
	{
		if (i == 24)
			continue;

		if (black & 1)
			boardRaw[index(i)] = PLAYER_BLACK;
		else if (white & 1)
			boardRaw[index(i)] = PLAYER_WHITE;
		else
			boardRaw[index(i)] = ' ';

	}
	std::cout << boardRaw;
	return;
}

std::ostream& operator<<(std::ostream& os, const Move& mv)
{
	return os << (uint32_t)mv.src << ',' << (uint32_t)mv.dst;
}