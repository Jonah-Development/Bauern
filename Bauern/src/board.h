#pragma once
#include <stdint.h>

namespace board7x7
{
	typedef uint64_t BB;
	typedef uint32_t Si;

	constexpr BB mask = (1 << 23) | (0xffffffffffffffff << 49);

	constexpr BB invalidWinningPosition = 1ull << 24;
	constexpr BB centerVertical = 0b0001000000100000010000001000000100000010000001000ull;
	constexpr BB centerHorizontal = 0x7full << 21;
	constexpr BB diagionals = 0b1000001010001000101000001000001010001000101000001;

	constexpr BB maskSouth = 0x1fffffull;
	constexpr BB maskNorth = maskSouth << 28;
	constexpr BB maskEast = 0b0000111000011100001110000111000011100001110000111;
	constexpr BB maskWest = maskEast << 4;
	constexpr BB maskNorthWest = maskNorth | maskWest;
	constexpr BB maskSouthWest = maskSouth | maskWest;
	constexpr BB maskNorthEast = maskNorth | maskEast;
	constexpr BB maskSouthEast = maskSouth | maskEast;

	constexpr BB rayNorth = 0b1000000100000010000001;
	constexpr BB rayWest = 0b1111;
	constexpr BB rayNorthWest = 0b1000000010000000100000001;
	constexpr BB rayNorthEast = 0b1000001000001000001;

	constexpr BB maskBorderSouth = 0x7f;
	constexpr BB maskBorderNorth = maskBorderSouth << 42;
	constexpr BB maskBorderEast = 0b0000001000000100000010000001000000100000010000001;
	constexpr BB maskBorderWest = maskBorderEast << 6;
	constexpr BB maskBorder = maskBorderSouth | maskBorderNorth | maskBorderEast | maskBorderWest;
	constexpr BB maskNeighbour = 9727775195120271810;

	constexpr int32_t Infinity = 0x3fffffff;

	constexpr BB winningPosNord(const Si index)
	{
		if ((1ull << index) & (centerVertical | maskNorth))
			return invalidWinningPosition;
		return rayNorth << index;
	}

	constexpr BB winningPosSouth(const Si index)
	{
		if ((1ull << index) & (centerVertical | maskSouth))
			return invalidWinningPosition;
		return rayNorth << (index - 21);
	}

	constexpr BB winningPosWest(const Si index)
	{
		if ((1ull << index) & (centerHorizontal | maskWest))
			return invalidWinningPosition;
		return rayWest << index;
	}

	constexpr BB winningPosEast(const Si index)
	{
		if ((1ull << index) & (centerHorizontal | maskEast))
			return invalidWinningPosition;
		return rayWest << (index - 3);
	}

	constexpr BB winningPosNorthWest(const Si index)
	{
		if ((1ull << index) & (diagionals | maskNorthWest))
			return invalidWinningPosition;
		return rayNorthWest << index;
	}

	constexpr BB winningPosNorthEast(const Si index)
	{
		if ((1ull << index) & (diagionals | maskNorthEast))
			return invalidWinningPosition;
		return rayNorthEast << index;
	}

	constexpr BB winningPosSouthEast(const Si index)
	{
		if ((1ull << index) & (diagionals | maskSouthEast))
			return invalidWinningPosition;
		return rayNorthWest << (index - 24);
	}

	constexpr BB winningPosSouthWest(const Si index)
	{
		if ((1ull << index) & (diagionals | maskSouthWest))
			return invalidWinningPosition;
		return rayNorthEast << (index - 18);
	}

	constexpr size_t MAX_MOVES = 5 * (49 - 10 - 1);
	constexpr char PLAYER_BLACK = 'X';
	constexpr char PLAYER_WHITE = 'O';
	enum class Player : uint8_t
	{
		white = PLAYER_WHITE,
		black = PLAYER_BLACK,
	};


	struct Move
	{
		Move(void) { }
		Move(uint8_t _src, uint8_t _dst) :
			src(_src), dst(_dst) { }
		uint8_t src, dst;
	};

	struct Board
	{
		Board(void) :
			m_White(0),
			m_Black(0),
			m_FreeWhite(5),
			m_FreeBlack(5),
			m_SideToMove(Player::white)
		{ }

		void draw(void) const;

		inline BB getWhite(void) const { return m_White; }
		inline BB getBlack(void) const { return m_Black; }

		template <bool _IsRoot = true>
		int32_t search(int32_t alpha = -Infinity, int32_t beta = Infinity, uint32_t depth = 6);
		int32_t eval(void);
		int playBestMove(uint32_t depth = 6)
		{
			int32_t res = search(-Infinity, Infinity, depth);
			makeMove(m_BestMove);
			return res;
		}

		Move getBestMove(void) const
		{
			return m_BestMove;
		}

		bool verify(void) const
		{
			return (m_White & m_Black) == 0;
		}

		Player getSideToMove(void) const { return m_SideToMove; }

	public:
		bool isWinningMove(const Si index, const Player sideToMove) const;

		size_t moveGen(Move moves[MAX_MOVES]);
		void makeMove(const Move& move);
		void unmakeMove(const Move& move);

		size_t getFreePieces(void) const
		{
			return (m_SideToMove == Player::white) ?
				m_FreeWhite :
				m_FreeBlack;
		}

		void setWhite(BB b) { m_White = b; }

	private:
		BB m_White;
		BB m_Black;

		uint8_t m_FreeWhite;
		uint8_t m_FreeBlack;
		Player m_SideToMove;
		Move m_BestMove;
	};

}