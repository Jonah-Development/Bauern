#include "board.h"
using namespace board7x7;

constexpr int32_t squareValueLookup[] =
{
    0,0,0,0,0,0,0,
    0,1,1,1,1,1,0,
    0,1,2,2,2,1,0,
    0,1,2,3,2,1,0,
    0,1,2,2,2,1,0,
    0,1,1,1,1,1,0,
    0,0,0,0,0,0,0,
};

template int32_t Board::search<false>(int32_t, int32_t, uint32_t);
template int32_t Board::search<true>(int32_t, int32_t, uint32_t);

template <bool _IsRoot>
int32_t Board::search(int32_t alpha, int32_t beta, uint32_t depth)
{
    if (depth == 0)
        return eval();

    // generate moves
    Move moves[MAX_MOVES];
    if (!verify())
        throw "Error, WTF???";
    const size_t numMoves = moveGen(moves);

    if (numMoves == 0)
        return m_SideToMove != Player::white ? Infinity : -Infinity;

    //if constexpr (_IsRoot)
    //    if (numMoves)
    //        m_BestMove = moves[0];

    const Player sideToMove = m_SideToMove;
    for (size_t i = 0; i < numMoves; i++)
    {
        const Move& move = moves[i];
        // we prefer early wins
        if (isWinningMove(move.dst, sideToMove))
            return sideToMove == Player::white ? (Infinity + depth) : 0-(-Infinity - depth);

        makeMove(move);
        const int32_t score = -search<false>(-beta, -alpha, depth - 1);
        unmakeMove(move);

        if (score >= beta)
            return beta;

        if (score > alpha)
        {
            alpha = score;
            if constexpr (_IsRoot)
                m_BestMove = move;
        }
    }

    return alpha;
}

int32_t Board::eval(void)
{
    // prefer squares at the center
    BB white = m_White;
    BB black = m_Black;
    int32_t result = 0;
    for (size_t i = 0; i < 49 && (white | black); i++, white >>= 1, black >>= 1)
    {
        if (white & 1)
            result += squareValueLookup[i];
        if (black & 1)
            result -= squareValueLookup[i];
    }

    return m_SideToMove == Player::white ? result : -result;
}

/// <summary>
/// Calculate whether a position is winning. 
/// Thereby, we only look at the bits around the index, 
/// because this is the only thing that will have changed
/// </summary>
/// <param name="index">position of last-moved piece</param>
/// <param name="sideToMove">player who did the last move</param>
/// <returns></returns>
bool Board::isWinningMove(const Si index, const Player sideToMove) const
{
    const BB pos = ((sideToMove == Player::white) ? m_White : m_Black) | (1ull << index);

    // if we are at the end of the line
    BB mask = winningPosNord(index);
    if ((pos & mask) == mask)
        return true;

    mask = winningPosSouth(index);
    if ((pos & mask) == mask)
        return true;

    mask = winningPosWest(index);
    if ((pos & mask) == mask)
        return true;

    mask = winningPosEast(index);
    if ((pos & mask) == mask)
        return true;

    mask = winningPosNorthEast(index);
    if ((pos & mask) == mask)
        return true;

    mask = winningPosNorthWest(index);
    if ((pos & mask) == mask)
        return true;

    mask = winningPosSouthEast(index);
    if ((pos & mask) == mask)
        return true;

    mask = winningPosSouthWest(index);
    if ((pos & mask) == mask)
        return true;

    // if we have placed the piece in the middle of the thingy
    mask = winningPosNord(index - 7);
    if (((1ull << index) & ~maskBorderSouth) && (pos & mask) == mask)
        return true;

    mask = winningPosSouth(index + 7);
    if (((1ull << index) & ~maskBorderNorth) && (pos & mask) == mask)
        return true;

    mask = winningPosWest(index - 1);
    if (((1ull << index) & ~maskBorderEast) && (pos & mask) == mask)
        return true;

    mask = winningPosEast(index + 1);
    if (((1ull << index) & ~maskBorderWest) && (pos & mask) == mask)
        return true;

    mask = winningPosNorthEast(index - 6);
    if (((1ull << index) & ~(maskBorderSouth | maskBorderWest)) && (pos & mask) == mask)
        return true;

    mask = winningPosNorthWest(index - 8);
    if (((1ull << index) & ~(maskBorderSouth | maskBorderEast)) && (pos & mask) == mask)
        return true;

    mask = winningPosSouthEast(index + 8);
    if (((1ull << index) & ~(maskBorderNorth | maskBorderWest)) && (pos & mask) == mask)
        return true;

    mask = winningPosSouthWest(index + 6);
    if (((1ull << index) & ~(maskBorderNorth | maskBorderEast)) && (pos & mask) == mask)
        return true;

    return false;
}

size_t Board::moveGen(Move moves[MAX_MOVES])
{
    size_t legalMoves = 0;
    // if we have free peaces, then we must play with them first
    if (getFreePieces() != 0)
    {
        BB boardCombined = m_Black | m_White;
        BB boardUs = (m_SideToMove == Player::white) ? m_White : m_Black;

        for (size_t i = 0; i < 49; i++)
        {
            if ((boardCombined & 1) == 0 && (i != 24))
            {
                moves[legalMoves] = Move(0xff, (uint8_t)i);
                legalMoves++;
            }

            boardCombined >>= 1;
        }
    }
    // only allowed to move pieces once we have placed all others
    // todo: test what happens if we allowed piece-movements before all pieces are placed?
    else 
    {
        BB boardCombined = m_Black | m_White;
        BB boardUs = (m_SideToMove == Player::white) ? m_White : m_Black;

        // for each piece we find, it can be placed anywhere where there isn't a piece yet
        size_t legalMovesPerPiece = 0;

        // 1. Push all empty fields
        for (size_t i = 0; i < 49; i++)
        {
            if ((boardCombined & 1) == 0 && (i != 24))
            {
                moves[legalMovesPerPiece] = Move(0xff, (uint8_t)i);
                legalMovesPerPiece++;
            }

            boardCombined >>= 1;
        }

        // now copy all that for each placed piece
        for (size_t i = 0, src = (size_t)(- 1); i < 5; i++)
        {
            // find next piece
            for (; (boardUs & 1) == 0; boardUs >>= 1, src++);
            boardUs >>= 1;
            src++;

            for (size_t k = 0; k < legalMovesPerPiece; k++, legalMoves++)
            {
                const Move move = Move((uint8_t)src, moves[k].dst);
                moves[legalMoves] = move;
            }
        }
    }


    return legalMoves;
}

void Board::makeMove(const Move& move)
{
    if (m_SideToMove == Player::white)
    {
        if (move.src >= 49)
            m_FreeWhite--;
        else m_White ^= 1ull << move.src;
        m_White ^= 1ull << move.dst;
        m_SideToMove = Player::black;
    }
    else
    {
        if (move.src >= 49)
            m_FreeBlack--;
        else m_Black ^= 1ull << move.src;
        m_Black ^= 1ull << move.dst;
        m_SideToMove = Player::white;
    }
}

void Board::unmakeMove(const Move& move)
{
    if (m_SideToMove == Player::black)
    {
        if (move.src >= 49)
            m_FreeWhite++;
        else m_White ^= 1ull << move.src;
        m_White ^= 1ull << move.dst;
        m_SideToMove = Player::white;
    }
    else
    {
        if (move.src >= 49)
            m_FreeBlack++;
        else m_Black ^= 1ull << move.src;
        m_Black ^= 1ull << move.dst;
        m_SideToMove = Player::black;
    }
}
