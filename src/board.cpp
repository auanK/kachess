#include "board.h"

#include <algorithm>
#include <iostream>
#include <vector>

// Construtor da classe Board
Board::Board() { init_board_state(); }

// Inicializa o estado do tabuleiro com a posição inicial padrão do xadrez
void Board::init_board_state() {
    // Configurando uma fileira de peões brancos
    // Os peões brancos começam na fileira 2
    // Correspondem aos índices 8 a 15
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 11111111  <- Peças
    // 00000000
    white_pawns = 0x000000000000FF00ULL;

    // Configurando uma fileira de peões pretos
    // Os peões pretos começam na fileira 7
    // Correspondem aos índices 48 a 55
    // 00000000
    // 11111111  <- Peças
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    black_pawns = 0x00FF000000000000ULL;

    // Configurando o rei branco
    // O rei branco começa na casa e1, correspondente ao índice 4
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00010000  <- Peças
    white_king = (1ULL << 4);

    // Configurando o rei preto
    // O rei preto começa na casa e8, correspondente ao índice 60
    // 00001000 <-  Peças
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
    // Correspondem aos índices 1 e 6
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 01000010 <-  Peças
    white_knights = (1ULL << 1) | (1ULL << 6);

    // Configurando os cavalos pretos
    // Os cavalos pretos começam nas casas b8 e g8
    // Correspondem aos índices 57 e 62
    // 01000010 <- Peças
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    black_knights = (1ULL << 57) | (1ULL << 62);

    // Configurando as torres brancas
    // As torres brancas começam nas casas a1 e h1
    // Correspondem aos índices 0 e 7
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 10000001 <- Peças
    white_rooks = (1ULL << 0) | (1ULL << 7);

    // Configurando as torres pretas
    // As torres pretas começam nas casas a8 e h8
    // Correspondem aos índices 56 e 63
    // 10000001 <- Peças
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    black_rooks = (1ULL << 56) | (1ULL << 63);

    // Configurando os bispos brancos
    // Os bispos brancos começam nas casas c1 e f1
    // Correspondem aos índices 2 e 5
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00100100 <- Peças
    white_bishops = (1ULL << 2) | (1ULL << 5);

    // Configurando os bispos pretos
    // Os bispos pretos começam nas casas c8 e f8
    // Correspondem aos índices 58 e 61
    // 00100100 <- Peças
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    black_bishops = (1ULL << 58) | (1ULL << 61);

    // Configurando as rainhas brancas
    // As rainhas brancas começam na casa d1, correspondente ao índice 3
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00001000 <- Peças
    white_queens = (1ULL << 3);

    // Configurando as rainhas pretas
    // As rainhas pretas começam na casa d8, correspondente ao índice 59
    // 00001000 <- Peças
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    // 00000000
    black_queens = (1ULL << 59);

    // Calculando os bitboards de ocupação
    white_occupied = white_pawns | white_king | white_knights | white_rooks |
                     white_bishops | white_queens;
    black_occupied = black_pawns | black_king | black_knights | black_rooks |
                     black_bishops | black_queens;
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

    // Verifica de qual cor é o turno atual e procura a peça na casa de origem
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
        }
        // Move um cavalo branco
        else if (white_knights & from_bit) {
            white_knights &= ~from_bit;
            white_knights |= to_bit;
            white_occupied &= ~from_bit;
            white_occupied |= to_bit;
            piece_successfully_moved = true;
        }
        // Move uma torre branca
        else if (white_rooks & from_bit) {
            white_rooks &= ~from_bit;
            white_rooks |= to_bit;
            white_occupied &= ~from_bit;
            white_occupied |= to_bit;
            piece_successfully_moved = true;
        }
        // Move um bispo branco
        else if (white_bishops & from_bit) {
            white_bishops &= ~from_bit;
            white_bishops |= to_bit;
            white_occupied &= ~from_bit;
            white_occupied |= to_bit;
            piece_successfully_moved = true;
        }
        // Move uma rainha branca
        else if (white_queens & from_bit) {
            white_queens &= ~from_bit;
            white_queens |= to_bit;
            white_occupied &= ~from_bit;
            white_occupied |= to_bit;
            piece_successfully_moved = true;
        }
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
        }
        // Move um cavalo preto
        else if (black_knights & from_bit) {
            black_knights &= ~from_bit;
            black_knights |= to_bit;
            black_occupied &= ~from_bit;
            black_occupied |= to_bit;
            piece_successfully_moved = true;
        }
        // Move uma torre preta
        else if (black_rooks & from_bit) {
            black_rooks &= ~from_bit;
            black_rooks |= to_bit;
            black_occupied &= ~from_bit;
            black_occupied |= to_bit;
            piece_successfully_moved = true;
        }
        // Move um bispo preto
        else if (black_bishops & from_bit) {
            black_bishops &= ~from_bit;
            black_bishops |= to_bit;
            black_occupied &= ~from_bit;
            black_occupied |= to_bit;
            piece_successfully_moved = true;
        }
        // Move uma rainha preta
        else if (black_queens & from_bit) {
            black_queens &= ~from_bit;
            black_queens |= to_bit;
            black_occupied &= ~from_bit;
            black_occupied |= to_bit;
            piece_successfully_moved = true;
        }
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
void Board::print_board(const std::vector<int>& highlighted_squares) const {
    const std::string HIGHLIGHT_BG = "\033[42m";
    const std::string RESET_COLOR = "\033[0m";

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
            } else if (white_rooks & square_bit) {
                piece_char = 'R';
            } else if (black_rooks & square_bit) {
                piece_char = 'r';
            } else if (white_bishops & square_bit) {
                piece_char = 'B';
            } else if (black_bishops & square_bit) {
                piece_char = 'b';
            } else if (white_queens & square_bit) {
                piece_char = 'Q';
            } else if (black_queens & square_bit) {
                piece_char = 'q';
            }

            bool is_highlighted =
                !highlighted_squares.empty() &&
                (std::find(highlighted_squares.begin(),
                           highlighted_squares.end(),
                           square_index) != highlighted_squares.end());

            if (is_highlighted) {
                std::cout << HIGHLIGHT_BG << piece_char << RESET_COLOR << " ";
                continue;
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