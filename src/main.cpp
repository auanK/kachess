#include <iostream>
#include <string>
#include <vector>

#include "board.h"
#include "move.h"
#include "movegen.h"

std::string square_to_algebraic(int square_index) {
    if (square_index < 0 || square_index > 63) {
        return "??";
    }
    char file_char = 'a' + (square_index % 8);
    char rank_char = '1' + (square_index / 8);
    std::string algebraic_notation = "";
    algebraic_notation += file_char;
    algebraic_notation += rank_char;
    return algebraic_notation;
}

int algebraic_to_square(const std::string& square_notation) {
    if (square_notation.length() != 2) {
        return -1;
    }
    char file_char = square_notation[0];
    char rank_char = square_notation[1];

    if (file_char < 'a' || file_char > 'h' || rank_char < '1' ||
        rank_char > '8') {
        return -1;
    }
    int file_index = file_char - 'a';
    int rank_index = rank_char - '1';
    return rank_index * 8 + file_index;
}

int main() {
    Board game_board;
    std::string user_input;

    while (true) {
        game_board.print_board();
        std::cout << "> ";
        std::cin >> user_input;

        if (user_input == "quit") {
            break;
        }

        if (user_input.length() != 4) {
            continue;
        }

        std::string from_square_str = user_input.substr(0, 2);
        std::string to_square_str = user_input.substr(2, 2);

        int from_square = algebraic_to_square(from_square_str);
        int to_square = algebraic_to_square(to_square_str);

        if (from_square == -1 || to_square == -1) {
            continue;
        }

        Move user_move(from_square, to_square);
        bool is_valid_move = false;

        std::vector<Move> legal_moves = MoveGen::gen_all_moves(game_board);

        for (const Move& legal_mv : legal_moves) {
            if (user_move == legal_mv) {
                is_valid_move = true;
                break;
            }
        }

        if (is_valid_move) {
            game_board.apply_move(user_move);
        }
    }

    std::cout << "Exiting." << std::endl;
    return 0;
}