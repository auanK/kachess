#include "board.h"

#include <iostream>

Board::Board() { init_board_state(); }

void Board::init_board_state() {
    // Inicializando os bitboards para as peças
    white_pawns = 0ULL;
    black_pawns = 0ULL;
    white_king = 0ULL;
    black_king = 0ULL;
    white_knights = 0ULL;
    black_knights = 0ULL;

    // Configurando uma fileira de peões brancos
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 11111111  <- Peões brancos (fileira 2, casas 8 a 15)
    // 00000000
    white_pawns = 0x000000000000FF00ULL;

    // Configurando uma fileira de peões pretos
    // 00000000
    // 11111111  <- Peões pretos (fileira 7, casas 48 a 55)
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    black_pawns = 0x00FF000000000000ULL;

    // Configurando o rei branco
    // O rei branco começa na casa e1, correspondente ao índice 4
    // (1ULL << 4) cria um valor de 64 bits com o bit 4 definido como 1.
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00010000  <- Rei branco (casa 4, fileira 1)
    white_king = (1ULL << 4);

    // Configurando o rei preto
    // O rei preto começa na casa e8, correspondente ao índice 60
    // 00001000 <- Rei preto (casa 60, fileira 8)
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    black_king = (1ULL << 60);

    // Configurando os cavalos brancos
    // Os cavalos brancos começam nas casas b1 e g1
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 01000010 <- Cavalos brancos (casas 1 e 6, fileira 1)
    white_knights = (1ULL << 1) | (1ULL << 6);

    // Configurando os cavalos pretos
    // Os cavalos pretos começam nas casas b8 e g8
    // 01000010 <- Cavalos pretos (casas 57 e 62, fileira 8)
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    black_knights = (1ULL << 57) | (1ULL << 62);

    // Calculando os bitboards de ocupação
    white_occupied = white_pawns | white_king | white_knights;
    black_occupied = black_pawns | black_king | black_knights;
    all_occupied = white_occupied | black_occupied;

    // Definir o turno inicial
    turn = WHITE;
}

// Aplica um movimento no tabuleiro
void Board::apply_move(const Move& move_to_apply) {
    int from = move_to_apply.from;
    int to = move_to_apply.to;

    // Cria uma máscara de bit para as casas de origem e destino
    uint64_t from_bit = (1ULL << from);
    uint64_t to_bit = (1ULL << to);

    // Flag de sucesso do movimento
    bool piece_successfully_moved = false;

    // Verifica de qual cor é o turno atual e aplica o movimento
    if (turn == WHITE) {
        // Move um peão branco
        if (white_pawns & from_bit) {
            white_pawns &= ~from_bit;
            white_pawns |= to_bit;
            white_occupied &= ~from_bit;
            white_occupied |= to_bit;
            piece_successfully_moved = true;
        }
        // Move o rei branco
        else if (white_king & from_bit) {
            white_king &= ~from_bit;
            white_king |= to_bit;
            white_occupied &= ~from_bit;
            white_occupied |= to_bit;
            piece_successfully_moved = true;
        } else if (white_knights & from_bit) {
            white_knights &= ~from_bit;
            white_knights |= to_bit;
            white_occupied &= ~from_bit;
            white_occupied |= to_bit;
            piece_successfully_moved = true;
        }

        /**
         * O restante dos movimentos (como torres, bispos e
         * rainhas) serão implementados aqui.
         */
    } else {
        // Move um peão preto
        if (black_pawns & from_bit) {
            black_pawns &= ~from_bit;
            black_pawns |= to_bit;
            black_occupied &= ~from_bit;
            black_occupied |= to_bit;
            piece_successfully_moved = true;
        }
        // Move o rei preto
        else if (black_king & from_bit) {
            black_king &= ~from_bit;
            black_king |= to_bit;
            black_occupied &= ~from_bit;
            black_occupied |= to_bit;
            piece_successfully_moved = true;
        } else if (black_knights & from_bit) {
            black_knights &= ~from_bit;
            black_knights |= to_bit;
            black_occupied &= ~from_bit;
            black_occupied |= to_bit;
            piece_successfully_moved = true;
        }

        /**
         * O restante dos movimentos (como torres, bispos e
         * rainhas) serão implementados aqui.
         */
    }

    // Caso o movimento não tenha sido bem-sucedido, mostra uma mensagem de erro
    // no console (Em um fluxo normal, isso não deveria acontecer, as
    // verificações de validade devem ser feitas antes de chamar apply_move)
    if (!piece_successfully_moved) {
        std::cerr
            << "Movimento inválido: peça não encontrada na casa de origem."
            << std::endl;
        return;
    }

    // Atualiza o bitboard de ocupação
    all_occupied = white_occupied | black_occupied;

    // Alterna o turno
    turn = (turn == WHITE) ? BLACK : WHITE;
}

// Função temporária para imprimir o tabuleiro no console
// Futuramente irei usar FEN para representar o estado do tabuleiro
void Board::print_board() const {
    std::cout << std::endl;

    for (int rank = 7; rank >= 0; --rank) {
        std::cout << (rank + 1) << " | ";
        for (int file = 0; file < 8; ++file) {
            int square_index = rank * 8 + file;
            uint64_t square_bit = (1ULL << square_index);
            char piece_char = '.';

            if (white_pawns & square_bit) {
                piece_char = 'P';
            } else if (black_pawns & square_bit) {
                piece_char = 'p';
            } else if (white_king & square_bit) {
                piece_char = 'K';
            } else if (black_king & square_bit) {
                piece_char = 'k';
            } else if (white_knights & square_bit) {
                piece_char = 'N';
            } else if (black_knights & square_bit) {
                piece_char = 'n';
            }

            std::cout << piece_char << " ";
        }
        std::cout << std::endl;
    }

    std::cout << "   -----------------" << std::endl;
    std::cout << "    a b c d e f g h" << std::endl;
    std::cout << std::endl;

    std::cout << "Turno: " << (turn == WHITE ? "BRANCAS" : "PRETAS")
              << std::endl;
    std::cout << std::endl;
}