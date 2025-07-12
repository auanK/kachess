#ifndef MOVEGEN_H
#define MOVEGEN_H

#include <vector>

#include "board.h"
#include "move.h"

namespace MoveGen {
    
bool is_square_attacked(int square, Color attacker, const Board& board);
    
std::vector<Move> gen_white_pawn_moves(const Board& board);
std::vector<Move> gen_white_king_moves(const Board& board);
std::vector<Move> gen_black_pawn_moves(const Board& board);
std::vector<Move> gen_black_king_moves(const Board& board);
std::vector<Move> gen_white_knight_moves(const Board& board);
std::vector<Move> gen_black_knight_moves(const Board& board);
std::vector<Move> gen_white_rook_moves(const Board& board);
std::vector<Move> gen_black_rook_moves(const Board& board);
std::vector<Move> gen_white_bishop_moves(const Board& board);
std::vector<Move> gen_black_bishop_moves(const Board& board);
std::vector<Move> gen_white_queen_moves(const Board& board);
std::vector<Move> gen_black_queen_moves(const Board& board);

std::vector<Move> gen_all_moves(const Board& board);
std::vector<Move> gen_legal_moves(Board& board);


}  // namespace MoveGen

#endif