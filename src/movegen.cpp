#include "movegen.h"

#include <cmath>

namespace MoveGen {

// Flag de inicialização das tabelas de ataques
static bool table_attacks_ready = false;

// Tabela de ataques do rei
static uint64_t king_attacks[64];

// Tabela de ataques dos cavalos
static uint64_t knight_attacks[64];

// Encontra o índice do bit menos significativo (LSB) em um bitboard
inline int get_lsb(uint64_t bb) {
    if (bb == 0) return -1;
    return __builtin_ctzll(bb);
}

// Inicializa a tabela de ataques do rei
void init_king_attacks() {
    // Itera sobre todas as casas do tabuleiro
    for (int from = 0; from < 64; ++from) {
        king_attacks[from] = 0ULL;
        int from_file = from % 8;

        // Deltas para as 8 direções possíveis (norte, sul, leste, oeste,
        // noroeste, nordeste, sudoeste, sudeste)
        const int deltas[8] = {-1, +1, -8, +8, -9, -7, +7, +9};

        // Para cada direção possível
        // (movimentos horizontais, verticais e diagonais)
        for (int delta : deltas) {
            int to = from + delta;

            // Verifica se está dentro dos limites do tabuleiro
            if (to >= 0 && to < 64) {
                // Verifica se o movimento não dá a volta na borda do tabuleiro
                int to_file = to % 8;
                // Um movimento horizontal ou vertical válido não deve mudar a
                // coluna em mais de 1 casa
                if (std::abs(from_file - to_file) <= 1)
                    king_attacks[from] |= (1ULL << to);
            }
        }
    }
}

// Inicializa a tabela de ataques dos cavalos
void init_knight_attacks() {
    // Itera sobre todas as casas do tabuleiro
    for (int from = 0; from < 64; ++from) {
        knight_attacks[from] = 0ULL;
        int from_file = from % 8;

        // Deltas para os 8 saltos possíveis do cavalo
        // (2 casas em uma direção e 1 casa na perpendicular)
        const int deltas[8] = {17, 15, 10, 6, -17, -15, -10, -6};

        // Para cada salto possível
        for (int delta : deltas) {
            int to = from + delta;

            // Verifica se está dentro dos limites do tabuleiro
            if (to >= 0 && to < 64) {
                // Verifica se o salto não dá a volta na borda do tabuleiro
                int to_file = to % 8;
                // A diferença de colunas para um salto de cavalo nunca pode ser
                // maior que 2
                if (std::abs(from_file - to_file) <= 2)
                    knight_attacks[from] |= (1ULL << to);
            }
        }
    }
}

// Gera todos os movimentos válidos para os peões brancos
std::vector<Move> gen_white_pawn_moves(const Board& board) {
    std::vector<Move> moves;  // Vetor para armazenar os movimentos válidos

    /**
     * 1. Avanço simples (uma casa para frente)
     */

    // Simula todos os peões brancos avançando uma casa
    uint64_t push_targets = (board.white_pawns << 8);

    // Mantém apenas os movimentos para casas que estão vazias
    uint64_t valid_pushes = push_targets & ~board.all_occupied;

    // Itera sobre os destinos válidos e adiciona os movimentos
    // Enquanto houver bits '1' (movimentos válidos)
    uint64_t remaining = valid_pushes;
    while (remaining) {
        // Calcula o índice do destino e da origem
        int to = get_lsb(remaining);
        int from = to - 8;

        // Adiciona o movimento à lista e remove o bit processado
        moves.push_back(Move(from, to));
        remaining &= remaining - 1;
    }

    /**
     * 2. Avanço duplo (duas casas para frente) quando o peão está na
     * segunda fileira
     */

    // Mascara para identificar os peões brancos na segunda fileira
    const uint64_t RANK_2 = 0x000000000000FF00ULL;

    // Obtém os peões brancos que estão na segunda fileira
    uint64_t rank2_pawns = board.white_pawns & RANK_2;

    // Simula o avanço de uma casa e mantém apenas para casas vazias
    uint64_t mid = (rank2_pawns << 8) & ~board.all_occupied;
    uint64_t dbl_targets = (mid << 8) & ~board.all_occupied;

    // Itera sobre os destinos válidos e adiciona os movimentos
    // Enquanto houver bits '1' (movimentos válidos)
    remaining = dbl_targets;
    while (remaining) {
        // Calcula o índice do destino e da origem
        int to = get_lsb(remaining);
        int from = to - 16;

        // Adiciona o movimento à lista e remove o bit processado
        moves.push_back(Move(from, to));
        remaining &= remaining - 1;
    }

    /**
     * Lógica para capturas, etc será adicionada aqui.
     * */

    return moves;
}

// Gera todos os movimentos válidos para os peões pretos
std::vector<Move> gen_black_pawn_moves(const Board& board) {
    std::vector<Move> moves;  // Vetor para armazenar os movimentos válidos

    /**
     * 1. Avanço simples (uma casa para frente)
     */

    // Simula todos os peões pretos avançando uma casa
    uint64_t push_targets = (board.black_pawns >> 8);

    // Mantém apenas os movimentos para casas que estão vazias
    uint64_t valid_pushes = push_targets & ~board.all_occupied;

    // Itera sobre os destinos válidos e adiciona os movimentos
    // Enquanto houver bits '1' (movimentos válidos)
    uint64_t remaining = valid_pushes;
    while (remaining) {
        // Calcula o índice do destino e da origem
        int to = get_lsb(remaining);
        int from = to + 8;

        // Adiciona o movimento à lista e remove o bit processado
        moves.push_back(Move(from, to));
        remaining &= remaining - 1;
    }

    /**
     * 2. Avanço duplo (duas casas para frente) quando o peão está na
     * sétima fileira
     */

    // Mascara para identificar os peões pretos na sétima fileira
    const uint64_t RANK_7 = 0x00FF000000000000ULL;

    // Obtém os peões pretos que estão na sétima fileira
    uint64_t rank7_pawns = board.black_pawns & RANK_7;

    // Simula o avanço de uma casa e mantém apenas para casas vazias
    uint64_t mid = (rank7_pawns >> 8) & ~board.all_occupied;
    uint64_t dbl_targets = (mid >> 8) & ~board.all_occupied;

    // Itera sobre os destinos válidos e adiciona os movimentos
    // Enquanto houver bits '1' (movimentos válidos)
    remaining = dbl_targets;
    while (remaining) {
        // Calcula o índice do destino e da origem
        int to = get_lsb(remaining);
        int from = to + 16;

        // Adiciona o movimento à lista e remove o bit processado
        moves.push_back(Move(from, to));
        remaining &= remaining - 1;
    }

    /**
     * Lógica para capturas, etc será adicionada aqui.
     * */

    return moves;
}

// Gera todos os movimentos válidos para o rei branco
std::vector<Move> gen_white_king_moves(const Board& board) {
    std::vector<Move> moves;  // Vetor para armazenar os movimentos válidos

    // Obtém o rei branco do estado do tabuleiro
    uint64_t king = board.white_king;

    // Se não houver rei branco (em um fluxo normal, isso não deve acontecer),
    // retorna um vetor vazio
    if (king == 0) return moves;

    // Encontra a posição do rei branco (único bit 1 no bitboard)
    int from = get_lsb(king);

    // Obtém todos os movimentos possíveis do rei nessa casa
    // e mantém apenas os que não colidem com peças brancas
    uint64_t targets = king_attacks[from] & ~board.white_occupied;

    // Itera sobre cada movimento válido
    uint64_t remaining = targets;
    while (remaining) {
        int to = get_lsb(remaining);

        // Adiciona o movimento à lista e remove o movimento processado
        // do bitboard de movimentos restantes
        moves.push_back(Move(from, to));
        remaining &= remaining - 1;
    }

    return moves;
}

// Gera todos os movimentos válidos para o rei preto
std::vector<Move> gen_black_king_moves(const Board& board) {
    std::vector<Move> moves;  // Vetor para armazenar os movimentos válidos

    // Obtém o rei preto do estado do tabuleiro
    uint64_t king = board.black_king;

    // Se não houver rei preto (em um fluxo normal, isso não deve acontecer),
    // retorna um vetor vazio
    if (king == 0) return moves;

    // Encontra a posição do rei preto (único bit 1 no bitboard)
    int from = get_lsb(king);

    // Obtém todos os movimentos possíveis do rei nessa casa
    // e mantém apenas os que não colidem com peças pretas
    uint64_t targets = king_attacks[from] & ~board.black_occupied;

    // Itera sobre cada movimento válido
    uint64_t remaining = targets;
    while (remaining) {
        int to = get_lsb(remaining);

        // Adiciona o movimento à lista e remove o movimento processado
        // do bitboard de movimentos restantes
        moves.push_back(Move(from, to));
        remaining &= remaining - 1;
    }

    return moves;
}

// Gera todos os movimentos válidos para os cavalos brancos
std::vector<Move> gen_white_knight_moves(const Board& board) {
    std::vector<Move> moves;  // Vetor para armazenar os movimentos válidos

    // Pega o bitboard de todos os cavalos brancos
    uint64_t knights = board.white_knights;

    // Em um fluxo normal de jogo, a tabela de ataques dos cavalos já deve
    // ter sido inicializada se chegou aqui.

    // Itera sobre cada cavalo branco no tabuleiro
    while (knights) {
        // Pega a casa do cavalo branco atual (o bit menos significativo)
        int from = get_lsb(knights);

        // Obtém todos os saltos possíveis do cavalo nessa casa e mantém apenas
        // os que não colidem com peças brancas
        uint64_t targets = knight_attacks[from] & ~board.white_occupied;

        // Itera sobre cada salto válido
        uint64_t remaining = targets;
        while (remaining) {
            int to = get_lsb(remaining);

            // Adiciona o movimento à lista e remove o bit processado
            moves.push_back(Move(from, to));
            remaining &= remaining - 1;
        }

        // Remove o cavalo processado do bitboard
        knights &= knights - 1;
    }

    return moves;
}

// Gera todos os movimentos válidos para os cavalos pretos
std::vector<Move> gen_black_knight_moves(const Board& board) {
    std::vector<Move> moves;

    uint64_t knights = board.black_knights;
    while (knights) {
        // Pega a casa do cavalo preto atual (o bit menos significativo)
        int from = get_lsb(knights);

        // Obtém todos os saltos possíveis do cavalo nessa casa e mantém apenas
        // os que não colidem com peças pretas
        uint64_t targets = knight_attacks[from] & ~board.black_occupied;

        // Itera sobre cada salto válido
        uint64_t remaining = targets;
        while (remaining) {
            int to = get_lsb(remaining);

            // Adiciona o movimento à lista e remove o bit processado
            moves.push_back(Move(from, to));
            remaining &= remaining - 1;
        }

        // Remove o cavalo processado do bitboard
        knights &= knights - 1;
    }

    return moves;
}

// Gera todos os movimentos válidos para as torres brancas
std::vector<Move> gen_white_rook_moves(const Board& board) {
    std::vector<Move> moves;  // Vetor para armazenar os movimentos válidos

    // Pega o bitboard de todas as torres brancas
    uint64_t rooks = board.white_rooks;

    // Itera sobre cada torre branca no tabuleiro
    while (rooks) {
        int from_square = get_lsb(rooks);

        // Deltas para as 4 direções possíveis (norte, sul, leste, oeste)
        const int deltas[4] = {8, -8, 1, -1};

        // Para cada uma das 4 direções
        for (int delta : deltas) {
            int to_square = from_square;  // Posição inicial da torre

            // Vai deslizando na direção do delta
            while (true) {
                to_square += delta;  // Move uma casa na direção

                // Se ultrapassar os limites do tabuleiro, sai do loop
                if (to_square < 0 || to_square >= 64) break;

                // Evita dar a volta na borda do tabuleiro
                // (ex: de h1 para a2)
                int from_file = (to_square - delta) % 8;
                int to_file = to_square % 8;
                // Um movimento horizontal ou vertical válido não deve mudar a
                // coluna em mais de 1 casa
                if (std::abs(from_file - to_file) > 1) break;

                // Cria uma máscara de bit para a casa de destino
                uint64_t to_bit = (1ULL << to_square);

                // Se a casa já estiver ocupada por uma peça branca,
                // não pode mover para lá e sai o loop
                if (board.white_occupied & to_bit) break;

                // Se a casa já estiver ocupada por uma peça preta,
                // adiciona o movimento e sai do loop
                if (board.black_occupied & to_bit) {
                    moves.push_back(Move(from_square, to_square));
                    break;
                }

                // A casa está vazia, adiciona o movimento
                // e continua deslizando na mesma direção
                moves.push_back(Move(from_square, to_square));
            }
        }

        // Remove a torre processada do bitboard
        rooks &= rooks - 1;
    }

    return moves;
}

// Gera todos os movimentos válidos para as torres pretas
std::vector<Move> gen_black_rook_moves(const Board& board) {
    std::vector<Move> moves;  // Vetor para armazenar os movimentos válidos

    // Pega o bitboard de todas as torres pretas
    uint64_t rooks = board.black_rooks;

    // Itera sobre cada torre preta no tabuleiro
    while (rooks) {
        int from_square = get_lsb(rooks);

        // Delta para as 4 direções possíveis (norte, sul, leste, oeste)
        const int deltas[4] = {8, -8, 1, -1};

        for (int delta : deltas) {
            int to_square = from_square;  // Posição inicial da torre

            // Vai deslizando na direção do delta
            while (true) {
                to_square += delta;  // Move uma casa na direção

                // Se ultrapassar os limites do tabuleiro, sai do loop
                if (to_square < 0 || to_square >= 64) break;

                // Evita dar a volta na borda do tabuleiro
                // (ex: de h1 para a2)
                int from_file = (to_square - delta) % 8;
                int to_file = to_square % 8;
                // Um movimento horizontal ou vertical válido não deve mudar a
                // coluna em mais de 1 casa
                if (std::abs(from_file - to_file) > 1) break;

                // Cria uma máscara de bit para a casa de destino
                uint64_t to_bit = (1ULL << to_square);

                // Se a casa já estiver ocupada por uma peça preta,
                // não pode mover para lá e sai do loop
                if (board.black_occupied & to_bit) break;

                // Se a casa já estiver ocupada por uma peça branca,
                // adiciona o movimento e sai do loop
                if (board.white_occupied & to_bit) {
                    moves.push_back(Move(from_square, to_square));
                    break;
                }

                // A casa está vazia, adiciona o movimento e continua deslizando
                // na mesma direção
                moves.push_back(Move(from_square, to_square));
            }
        }

        // Remove a torre processada do bitboard
        rooks &= rooks - 1;
    }

    return moves;
}

// Gera todos os movimentos válidos para os bispos brancos
std::vector<Move> gen_white_bishop_moves(const Board& board) {
    std::vector<Move> moves;  // Vetor para armazenar os movimentos válidos

    // Pega o bitboard de todos os bispos brancos
    uint64_t bishops = board.white_bishops;

    // Itera sobre cada bispo branco no tabuleiro
    while (bishops) {
        int from_square = get_lsb(bishops);

        // Deltas para as 4 direções diagonais:
        // Noroeste, Nordeste, Sudoeste, Sudeste
        const int deltas[4] = {7, 9, -9, -7};

        // Para cada uma das 4 direções diagonais
        for (int delta : deltas) {
            int to_square = from_square;

            // Vai deslizando na direção do delta
            while (true) {
                to_square += delta;  // Move uma casa na direção

                // Se ultrapassar os limites do tabuleiro, sai do loop
                if (to_square < 0 || to_square >= 64) break;

                // Evita dar a volta na borda do tabuleiro
                // (ex: de h1 para a2)
                int from_file = (to_square - delta) % 8;
                int to_file = to_square % 8;
                // Um movimento diagonal válido não deve mudar a coluna
                // em mais de 1 casa
                if (std::abs(from_file - to_file) != 1) break;

                // Cria uma máscara de bit para a casa de destino
                uint64_t to_bit = (1ULL << to_square);

                // Se a casa já estiver ocupada por uma peça branca,
                // não pode mover para lá e sai do loop
                if (board.white_occupied & to_bit) break;

                // Se a casa já estiver ocupada por uma peça preta,
                // adiciona o movimento e sai do loop
                if (board.black_occupied & to_bit) {
                    moves.push_back(Move(from_square, to_square));
                    break;
                }

                // A casa está vazia, adiciona o movimento e continua deslizando
                // na mesma direção
                moves.push_back(Move(from_square, to_square));
            }
        }

        // Remove o bispo processado do bitboard
        bishops &= bishops - 1;
    }

    return moves;
}

// Gera todos os movimentos válidos para os bispos pretos
std::vector<Move> gen_black_bishop_moves(const Board& board) {
    std::vector<Move> moves;  // Vetor para armazenar os movimentos válidos

    // Pega o bitboard de todos os bispos pretos
    uint64_t bishops = board.black_bishops;

    // Itera sobre cada bispo preto no tabuleiro
    while (bishops) {
        int from_square = get_lsb(bishops);

        // Deltas para as 4 direções diagonais:
        // Noroeste, Nordeste, Sudoeste, Sudeste
        const int deltas[4] = {7, 9, -9, -7};

        // Para cada uma das 4 direções diagonais
        for (int delta : deltas) {
            int to_square = from_square;

            // Vai deslizando na direção do delta
            while (true) {
                to_square += delta;  // Move uma casa na direção

                // Se ultrapassar os limites do tabuleiro, sai do loop
                if (to_square < 0 || to_square >= 64) break;

                // Evita dar a volta na borda do tabuleiro
                // (ex: de h1 para a2)
                int from_file = (to_square - delta) % 8;
                int to_file = to_square % 8;
                // Um movimento diagonal válido não deve mudar a coluna
                // em mais de 1 casa
                if (std::abs(from_file - to_file) != 1) break;

                // Cria uma máscara de bit para a casa de destino
                uint64_t to_bit = (1ULL << to_square);

                // Se a casa já estiver ocupada por uma peça preta,
                // não pode mover para lá e sai do loop
                if (board.black_occupied & to_bit) break;

                // Se a casa já estiver ocupada por uma peça branca,
                // adiciona o movimento e sai do loop
                if (board.white_occupied & to_bit) {
                    moves.push_back(Move(from_square, to_square));
                    break;
                }

                // A casa está vazia, adiciona o movimento e continua deslizando
                // na mesma direção
                moves.push_back(Move(from_square, to_square));
            }
        }

        // Remove o bispo processado do bitboard
        bishops &= bishops - 1;
    }

    return moves;
}

// Gera todos os movimentos válidos para as damas brancas
std::vector<Move> gen_white_queen_moves(const Board& board) {
    std::vector<Move> moves;
    uint64_t queens = board.white_queens;

    // Itera sobre cada dama branca no tabuleiro
    while (queens) {
        int from_square = get_lsb(queens);

        // Deltas para as 8 direções possíveis:
        const int deltas[8] = {8, -8, 1,  -1,   // Retas (Torre)
                               7, 9,  -9, -7};  // Diagonais (Bispo)

        // Para cada uma das 8 direções
        for (int delta : deltas) {
            int to_square = from_square;

            // Vai deslizando na direção do delta
            while (true) {
                to_square += delta;

                // Se ultrapassar os limites do tabuleiro, sai do loop
                if (to_square < 0 || to_square >= 64) break;

                // Evita dar a volta na borda do tabuleiro
                // Mesma lógica que as torres e bispos
                int from_file_of_step = (to_square - delta) % 8;
                int to_file_of_step = to_square % 8;
                // Um movimento diagonal, horizontal ou vertical válido
                // não deve mudar a coluna em mais de 1 casa
                if (std::abs(from_file_of_step - to_file_of_step) > 1) break;

                // Cria uma máscara de bit para a casa de destino
                uint64_t to_bit = (1ULL << to_square);

                // Se a casa já estiver ocupada por uma peça branca,
                // não pode mover para lá e sai do loop
                if (board.white_occupied & to_bit) break;

                // Se a casa já estiver ocupada por uma peça preta,
                // adiciona o movimento e sai do loop
                if (board.black_occupied & to_bit) {
                    moves.push_back(Move(from_square, to_square));
                    break;
                }

                // A casa está vazia, adiciona o movimento e continua deslizando
                // na mesma direção
                moves.push_back(Move(from_square, to_square));
            }
        }

        queens &= queens - 1;  // Remove a dama processada do bitboard
    }

    return moves;
}

// Gera todos os movimentos válidos para as damas pretas
std::vector<Move> gen_black_queen_moves(const Board& board) {
    std::vector<Move> moves;
    uint64_t queens = board.black_queens;

    // Itera sobre cada dama preta no tabuleiro
    while (queens) {
        int from_square = get_lsb(queens);

        // Deltas para as 8 direções possíveis:
        const int deltas[8] = {8, -8, 1,  -1,   // Retas (Torre)
                               7, 9,  -9, -7};  // Diagonais (Bispo)

        // Para cada uma das 8 direções
        for (int delta : deltas) {
            int to_square = from_square;

            // Vai deslizando na direção do delta
            while (true) {
                to_square += delta;

                // Se ultrapassar os limites do tabuleiro, sai do loop
                if (to_square < 0 || to_square >= 64) break;

                // Evita dar a volta na borda do tabuleiro
                // Mesma lógica que as torres e bispos
                int from_file_of_step = (to_square - delta) % 8;
                int to_file_of_step = to_square % 8;
                // Um movimento diagonal, horizontal ou vertical válido
                // não deve mudar a coluna em mais de 1 casa
                if (std::abs(from_file_of_step - to_file_of_step) > 1) break;

                // Cria uma máscara de bit para a casa de destino
                uint64_t to_bit = (1ULL << to_square);

                // Se a casa já estiver ocupada por uma peça preta,
                // não pode mover para lá e sai do loop
                if (board.black_occupied & to_bit) break;

                // Se a casa já estiver ocupada por uma peça branca,
                // adiciona o movimento e sai do loop
                if (board.white_occupied & to_bit) {
                    moves.push_back(Move(from_square, to_square));
                    break;
                }

                // A casa está vazia, adiciona o movimento e continua deslizando
                // na mesma direção
                moves.push_back(Move(from_square, to_square));
            }
        }

        queens &= queens - 1;  // Remove a dama processada do bitboard
    }
    return moves;
}

// Gera todos os movimentos válidos para o jogador atual
std::vector<Move> gen_all_moves(const Board& board) {
    std::vector<Move> all_moves;  // Guarda todos os movimentos válidos
    std::vector<Move> buffer;     // Guarda movimentos de peças individuais

    // Inicializa as tabelas de ataques se ainda não estiverem prontas
    if (!table_attacks_ready) {
        init_king_attacks();         // Ataques do rei
        init_knight_attacks();       // Ataques dos cavalos
        table_attacks_ready = true;  // Marca que as tabelas estão prontas
    }

    // Gera o movimento o vetor de movimentos possíveis para o jogador atual
    // Calcula o vetor de cada conjunto de peças individualmente e adiciona
    // no vetor geral após isso
    if (board.turn == WHITE) {
        buffer = gen_white_pawn_moves(board);  // Peões brancos
        all_moves.insert(all_moves.end(), buffer.begin(), buffer.end());

        buffer = gen_white_king_moves(board);  // Rei branco
        all_moves.insert(all_moves.end(), buffer.begin(), buffer.end());

        buffer = gen_white_knight_moves(board);  // Cavalos brancos
        all_moves.insert(all_moves.end(), buffer.begin(), buffer.end());

        buffer = gen_white_rook_moves(board);  // Torres brancas
        all_moves.insert(all_moves.end(), buffer.begin(), buffer.end());

        buffer = gen_white_bishop_moves(board);  // Bispos brancos
        all_moves.insert(all_moves.end(), buffer.begin(), buffer.end());

        buffer = gen_white_queen_moves(board);  // Damas brancas
        all_moves.insert(all_moves.end(), buffer.begin(), buffer.end());
    } else {
        buffer = gen_black_pawn_moves(board);  // Peões pretos
        all_moves.insert(all_moves.end(), buffer.begin(), buffer.end());

        buffer = gen_black_king_moves(board);  // Rei preto
        all_moves.insert(all_moves.end(), buffer.begin(), buffer.end());

        buffer = gen_black_knight_moves(board);  // Cavalos pretos
        all_moves.insert(all_moves.end(), buffer.begin(), buffer.end());

        buffer = gen_black_rook_moves(board);  // Torres pretas
        all_moves.insert(all_moves.end(), buffer.begin(), buffer.end());

        buffer = gen_black_bishop_moves(board);  // Bispos pretos
        all_moves.insert(all_moves.end(), buffer.begin(), buffer.end());

        buffer = gen_black_queen_moves(board);  // Damas pretas
        all_moves.insert(all_moves.end(), buffer.begin(), buffer.end());
    }

    return all_moves;
}

}  // namespace MoveGen