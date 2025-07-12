#ifndef BOARD_H
#define BOARD_H
#include <cstdint>
#include <vector>

#include "move.h"

enum Color { WHITE, BLACK };

struct UndoInfo {
    Move move;
    PieceType captured_piece;
};

class Board {
   public:
    uint64_t white_pawns;
    uint64_t black_pawns;
    uint64_t white_king;
    uint64_t black_king;
    uint64_t white_knights;
    uint64_t black_knights;
    uint64_t white_rooks;
    uint64_t black_rooks;
    uint64_t white_bishops;
    uint64_t black_bishops;
    uint64_t white_queens;
    uint64_t black_queens;

    uint64_t white_occupied;
    uint64_t black_occupied;
    uint64_t all_occupied;

    Color turn;

    Board();

    
    void make_move(const Move& move_to_apply);
    void undo_move();

    void print_board(const std::vector<int>& highlighted_squares = {}) const;
    void print_history() const;

   private:
    void init_board_state();

    std::vector<UndoInfo> history;
};

#endif