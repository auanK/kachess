#include "board.h"

#include <algorithm>
#include <iostream>
#include <vector>

// Construtor da classe Board
Board::Board() { init_board_state(); }

// Inicializa o estado do tabuleiro com a posição inicial padrão do xadrez
void Board::init_board_state() {
    // Os peões brancos começam na fileira 2
    // Correspondem aos índices 8 a 15
    // 00000000
    // ...
    // 11111111  <- Peças
    // 00000000
    white_pawns = 0x000000000000FF00ULL;

    // Os peões pretos começam na fileira 7
    // Correspondem aos índices 48 a 55
    // 00000000
    // 11111111  <- Peças
    // ...
    // 00000000
    black_pawns = 0x00FF000000000000ULL;

    // O rei branco começa na casa e1, correspondente ao índice 4
    // 00000000
    // ...
    // 00010000  <- Peças
    white_king = (1ULL << 4);

    // O rei preto começa na casa e8, correspondente ao índice 60
    // 00001000 <-  Peças
    // ...
    // 00000000
    black_king = (1ULL << 60);

    // Os cavalos brancos começam nas casas b1 e g1
    // Correspondem aos índices 1 e 6
    // 00000000
    // ...
    // 01000010 <-  Peças
    white_knights = (1ULL << 1) | (1ULL << 6);

    // Os cavalos pretos começam nas casas b8 e g8
    // Correspondem aos índices 57 e 62
    // 01000010 <- Peças
    // ...
    // 00000000
    black_knights = (1ULL << 57) | (1ULL << 62);

    // As torres brancas começam nas casas a1 e h1
    // Correspondem aos índices 0 e 7
    // 00000000
    // ...
    // 10000001 <- Peças
    white_rooks = (1ULL << 0) | (1ULL << 7);

    // As torres pretas começam nas casas a8 e h8
    // Correspondem aos índices 56 e 63
    // 10000001 <- Peças
    // ...
    // 00000000
    black_rooks = (1ULL << 56) | (1ULL << 63);

    // Os bispos brancos começam nas casas c1 e f1
    // Correspondem aos índices 2 e 5
    // 00000000
    // ...
    // 00100100 <- Peças
    white_bishops = (1ULL << 2) | (1ULL << 5);

    // Os bispos pretos começam nas casas c8 e f8
    // Correspondem aos índices 58 e 61
    // 00100100 <- Peças
    // ...
    // 00000000
    black_bishops = (1ULL << 58) | (1ULL << 61);

    // As rainhas brancas começam na casa d1, correspondente ao índice 3
    // 00000000
    // ...
    // 00001000 <- Peças
    white_queens = (1ULL << 3);

    // As rainhas pretas começam na casa d8, correspondente ao índice 59
    // 00001000 <- Peças
    // ...
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
void Board::make_move(const Move& move_to_apply) {
    // Cria uma máscara de bit para a casa de origem e destino
    const uint64_t from_bit = (1ULL << move_to_apply.from());
    const uint64_t to_bit = (1ULL << move_to_apply.to());

    // Salva o tipo de peça que está se movendo
    PieceType moving_piece = NONE;
    if (turn == WHITE) {
        if (white_pawns & from_bit) {
            moving_piece = PAWN;
        } else if (white_knights & from_bit) {
            moving_piece = KNIGHT;
        } else if (white_bishops & from_bit) {
            moving_piece = BISHOP;
        } else if (white_rooks & from_bit) {
            moving_piece = ROOK;
        } else if (white_queens & from_bit) {
            moving_piece = QUEEN;
        } else if (white_king & from_bit) {
            moving_piece = KING;
        }
    } else {
        if (black_pawns & from_bit) {
            moving_piece = PAWN;
        } else if (black_knights & from_bit) {
            moving_piece = KNIGHT;
        } else if (black_bishops & from_bit) {
            moving_piece = BISHOP;
        } else if (black_rooks & from_bit) {
            moving_piece = ROOK;
        } else if (black_queens & from_bit) {
            moving_piece = QUEEN;
        } else if (black_king & from_bit) {
            moving_piece = KING;
        }
    }

    // Nenhuma peça do jogador atual na casa de origem, não pode mover
    if (moving_piece == NONE) {
        std::cerr << "Erro: Movimento inválido. Nenhuma peça do jogador atual "
                     "na casa de origem."
                  << std::endl;
        return;
    }

    // Prepara a informação para desfazer o movimento
    UndoInfo undo;
    undo.move = move_to_apply;
    undo.captured_piece = NONE;

    // Pega o bitboard de ocupação do oponente
    const uint64_t opponent_occupied =
        (turn == WHITE) ? black_occupied : white_occupied;

    // Se a casa de destino está ocupada por uma peça do oponente,
    // é uma captura.
    if (opponent_occupied & to_bit) {
        // Percorre os bitboards das peças pretas para remover e salvar o tipo
        // da peça capturada.
        if (turn == WHITE) {
            if (black_pawns & to_bit) {
                undo.captured_piece = PAWN;
                black_pawns &= ~to_bit;
            } else if (black_knights & to_bit) {
                undo.captured_piece = KNIGHT;
                black_knights &= ~to_bit;
            } else if (black_bishops & to_bit) {
                undo.captured_piece = BISHOP;
                black_bishops &= ~to_bit;
            } else if (black_rooks & to_bit) {
                undo.captured_piece = ROOK;
                black_rooks &= ~to_bit;
            } else if (black_queens & to_bit) {
                undo.captured_piece = QUEEN;
                black_queens &= ~to_bit;
            }
            black_occupied &= ~to_bit;  // Atualiza o bitboard de ocupação
        }
        // Percorre os bitboards das peças brancas para remover e salvar o tipo
        // da peça capturada.
        else {
            if (white_pawns & to_bit) {
                undo.captured_piece = PAWN;
                white_pawns &= ~to_bit;
            } else if (white_knights & to_bit) {
                undo.captured_piece = KNIGHT;
                white_knights &= ~to_bit;
            } else if (white_bishops & to_bit) {
                undo.captured_piece = BISHOP;
                white_bishops &= ~to_bit;
            } else if (white_rooks & to_bit) {
                undo.captured_piece = ROOK;
                white_rooks &= ~to_bit;
            } else if (white_queens & to_bit) {
                undo.captured_piece = QUEEN;
                white_queens &= ~to_bit;
            }
            white_occupied &= ~to_bit;  //  Atualiza o bitboard de ocupação
        }
    }

    // Guarda o movimento no histórico
    history.push_back(undo);

    // Move a peça do jogador atual da casa de origem para a casa de destino e
    // atualiza o bitboard de ocupação dele.
    if (turn == WHITE) {
        switch (moving_piece) {
            case PAWN:
                white_pawns ^= (from_bit | to_bit);
                break;
            case KNIGHT:
                white_knights ^= (from_bit | to_bit);
                break;
            case BISHOP:
                white_bishops ^= (from_bit | to_bit);
                break;
            case ROOK:
                white_rooks ^= (from_bit | to_bit);
                break;
            case QUEEN:
                white_queens ^= (from_bit | to_bit);
                break;
            case KING:
                white_king ^= (from_bit | to_bit);
                break;
            case NONE:
                break;
        }
        white_occupied ^= (from_bit | to_bit);
    } else {
        switch (moving_piece) {
            case PAWN:
                black_pawns ^= (from_bit | to_bit);
                break;
            case KNIGHT:
                black_knights ^= (from_bit | to_bit);
                break;
            case BISHOP:
                black_bishops ^= (from_bit | to_bit);
                break;
            case ROOK:
                black_rooks ^= (from_bit | to_bit);
                break;
            case QUEEN:
                black_queens ^= (from_bit | to_bit);
                break;
            case KING:
                black_king ^= (from_bit | to_bit);
                break;
            case NONE:
                break;
        }
        black_occupied ^= (from_bit | to_bit);
    }

    // Atualiza o bitboard de ocupação total
    all_occupied = white_occupied | black_occupied;

    // Alterna o turno
    turn = (turn == WHITE) ? BLACK : WHITE;
}

// Desfaz o último movimento aplicado no tabuleiro
void Board::unmake_move() {
    // Se não houver histórico, não há nada para desfazer
    if (history.empty()) {
        return;
    }

    // Salvo o último movimento e o remove do histórico
    UndoInfo last_undo = history.back();
    history.pop_back();

    // Cria uma máscara de bit para a casa de origem e destino do movimento
    // que estamos desfazendo
    const uint64_t from_bit = (1ULL << last_undo.move.from());
    const uint64_t to_bit = (1ULL << last_undo.move.to());

    // Troca o turno de volta para o jogador que fez o movimento
    turn = (turn == WHITE) ? BLACK : WHITE;

    // Salva o tipo da peça que foi movida. (peça está na casa de destino)
    PieceType moved_piece = NONE;
    if (turn == WHITE) {
        if (white_pawns & to_bit) {
            moved_piece = PAWN;
        } else if (white_knights & to_bit) {
            moved_piece = KNIGHT;
        } else if (white_bishops & to_bit) {
            moved_piece = BISHOP;
        } else if (white_rooks & to_bit) {
            moved_piece = ROOK;
        } else if (white_queens & to_bit) {
            moved_piece = QUEEN;
        } else if (white_king & to_bit) {
            moved_piece = KING;
        }
    } else {
        if (black_pawns & to_bit) {
            moved_piece = PAWN;
        } else if (black_knights & to_bit) {
            moved_piece = KNIGHT;
        } else if (black_bishops & to_bit) {
            moved_piece = BISHOP;
        } else if (black_rooks & to_bit) {
            moved_piece = ROOK;
        } else if (black_queens & to_bit) {
            moved_piece = QUEEN;
        } else if (black_king & to_bit) {
            moved_piece = KING;
        }
    }

    // Move a peça de volta para a casa de origem e atualiza os bitboards
    if (turn == WHITE) {
        switch (moved_piece) {
            case PAWN:
                white_pawns ^= (from_bit | to_bit);
                break;
            case KNIGHT:
                white_knights ^= (from_bit | to_bit);
                break;
            case BISHOP:
                white_bishops ^= (from_bit | to_bit);
                break;
            case ROOK:
                white_rooks ^= (from_bit | to_bit);
                break;
            case QUEEN:
                white_queens ^= (from_bit | to_bit);
                break;
            case KING:
                white_king ^= (from_bit | to_bit);
                break;
            case NONE:
                break;
        }
        white_occupied ^= (from_bit | to_bit);
    } else {
        switch (moved_piece) {
            case PAWN:
                black_pawns ^= (from_bit | to_bit);
                break;
            case KNIGHT:
                black_knights ^= (from_bit | to_bit);
                break;
            case BISHOP:
                black_bishops ^= (from_bit | to_bit);
                break;
            case ROOK:
                black_rooks ^= (from_bit | to_bit);
                break;
            case QUEEN:
                black_queens ^= (from_bit | to_bit);
                break;
            case KING:
                black_king ^= (from_bit | to_bit);
                break;
            case NONE:
                break;
        }
        black_occupied ^= (from_bit | to_bit);
    }

    // Restaura a peça capturada, se houver
    if (last_undo.captured_piece != NONE) {
        const Color opponent_color = (turn == WHITE) ? BLACK : WHITE;
        if (opponent_color == WHITE) {
            switch (last_undo.captured_piece) {
                case PAWN:
                    white_pawns |= to_bit;
                    break;
                case KNIGHT:
                    white_knights |= to_bit;
                    break;
                case BISHOP:
                    white_bishops |= to_bit;
                    break;
                case ROOK:
                    white_rooks |= to_bit;
                    break;
                case QUEEN:
                    white_queens |= to_bit;
                    break;
                case KING:
                    white_king |= to_bit;
                    break;
                case NONE:
                    break;
            }
            white_occupied |= to_bit;
        } else {
            switch (last_undo.captured_piece) {
                case PAWN:
                    black_pawns |= to_bit;
                    break;
                case KNIGHT:
                    black_knights |= to_bit;
                    break;
                case BISHOP:
                    black_bishops |= to_bit;
                    break;
                case ROOK:
                    black_rooks |= to_bit;
                    break;
                case QUEEN:
                    black_queens |= to_bit;
                    break;
                case KING:
                    black_king |= to_bit;
                    break;
                case NONE:
                    break;
            }
            black_occupied |= to_bit;
        }
    }

    // Atualiza o bitboard de ocupação total
    all_occupied = white_occupied | black_occupied;
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

// Função temporária para imprimir o histórico de movimentos
void Board::print_history() const {
    std::cout << "Histórico de movimentos:" << std::endl;
    for (const auto& undo_info : history) {
        std::cout << "Movimento: " << undo_info.move.from() << " -> "
                  << undo_info.move.to()
                  << ", Peça capturada: " << undo_info.captured_piece
                  << std::endl;
    }
}