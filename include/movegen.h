#ifndef MOVEGEN_H
#define MOVEGEN_H

#include <vector>

#include "board.h"
#include "move.h"

namespace MoveGen {

std::vector<Move> generate_white_pawn_moves(const Board& board_state);
std::vector<Move> generate_white_king_moves(const Board& board_state);
std::vector<Move> generate_black_pawn_moves(const Board& board_state);
std::vector<Move> generate_black_king_moves(const Board& board_state);
std::vector<Move> generate_all_moves(const Board& board_state);

}  // namespace MoveGen

#endif