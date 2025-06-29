#ifndef MOVEGEN_H
#define MOVEGEN_H

#include <vector>

#include "board.h"
#include "move.h"

namespace MoveGen {

std::vector<Move> gen_white_pawn_moves(const Board& board);
std::vector<Move> gen_white_king_moves(const Board& board);
std::vector<Move> gen_black_pawn_moves(const Board& board);
std::vector<Move> gen_black_king_moves(const Board& board);
std::vector<Move> gen_white_knight_moves(const Board& board);
std::vector<Move> gen_black_knight_moves(const Board& board);
std::vector<Move> gen_all_moves(const Board& board);

}  // namespace MoveGen

#endif