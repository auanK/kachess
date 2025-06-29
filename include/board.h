#ifndef BOARD_H
#define BOARD_H
#include <cstdint>

#include "move.h"

enum Color { WHITE, BLACK };

class Board {
   public:
    uint64_t white_pawns;
    uint64_t black_pawns;
    uint64_t white_king;
    uint64_t black_king;
    uint64_t white_knights;
    uint64_t black_knights;

    uint64_t white_occupied;
    uint64_t black_occupied;
    uint64_t all_occupied;

    Color turn;

    Board();

    void print_board() const;

    void apply_move(const Move& move_to_apply);

   private:
    void init_board_state();
};

#endif