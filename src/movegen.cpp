#include "movegen.h"

#include <cmath>

namespace MoveGen {

static uint64_t knight_attacks[64];
static bool knight_attacks_ready = false;

// Encontra o índice do bit menos significativo (LSB) em um bitboard
inline int get_lsb(uint64_t bb) {
    if (bb == 0) return -1;
    return __builtin_ctzll(bb);
}

void init_knight_attacks() {
    if (knight_attacks_ready) return;

    for (int sq = 0; sq < 64; ++sq) {
        knight_attacks[sq] = 0ULL;
        int rank = sq / 8;
        int file = sq % 8;

        const int dr[] = {-2, -2, -1, -1, 1, 1, 2, 2};
        const int df[] = {-1, 1, -2, 2, -2, 2, -1, 1};

        for (int i = 0; i < 8; ++i) {
            int r = rank + dr[i];
            int f = file + df[i];

            if (r >= 0 && r < 8 && f >= 0 && f < 8) {
                int target = r * 8 + f;
                knight_attacks[sq] |= (1ULL << target);
            }
        }
    }
    knight_attacks_ready = true;
}

// Retorna os movimentos possíveis para peões brancos
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

// Retorna os movimentos possíveis para peões pretos
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

// Retorna os movimentos possíveis para o rei branco
std::vector<Move> gen_white_king_moves(const Board& board) {
    std::vector<Move> moves;  // Vetor para armazenar os movimentos válidos

    // Obtém o rei branco do estado do tabuleiro
    uint64_t king = board.white_king;

    // Se não houver rei branco (em um fluxo normal, isso não deve acontecer),
    // retorna um vetor vazio
    if (king == 0) return moves;

    // Encontra a posição do rei branco (único bit 1 no bitboard)
    int from = get_lsb(king);

    // Calcula as casas adjacentes onde o rei possivelmente pode se mover
    int targets[8] = {from - 1, from + 1, from - 8, from + 8,
                      from - 9, from - 7, from + 7, from + 9};

    // Guarda a coluna de origem do rei
    int from_file = from % 8;

    // Itera sobre as casas adjacentes
    for (int to : targets) {
        // Verifica se o destino está fora do tabuleiro
        if (to < 0 || to >= 64) continue;

        // Evita dar a volta na borda do tabuleiro(ex: de a8 para h1)
        int to_file = to % 8;
        if (std::abs(from_file - to_file) > 1) continue;

        // Verifica se o destino já está ocupado por uma peça branca
        if (board.white_occupied & (1ULL << to)) continue;

        // Adiciona o movimento válido ao vetor
        moves.push_back(Move(from, to));
    }

    return moves;
}

// Retorna os movimentos possíveis para o rei preto
std::vector<Move> gen_black_king_moves(const Board& board) {
    std::vector<Move> moves;

    // Obtém o rei preto do estado do tabuleiro
    uint64_t king = board.black_king;

    // Se não houver rei preto (em um fluxo normal, isso não deve acontecer),
    // retorna um vetor vazio
    if (king == 0) return moves;

    // Encontra a posição do rei preto (único bit 1 no bitboard)
    int from = get_lsb(king);

    // Calcula as casas adjacentes onde o rei possivelmente pode se mover
    int targets[8] = {from - 1, from + 1, from - 8, from + 8,
                      from - 9, from - 7, from + 7, from + 9};

    // Guarda a coluna de origem do rei
    int from_file = from % 8;

    // Itera sobre as casas adjacentes
    for (int to : targets) {
        // Verifica se o destino está fora do tabuleiro
        if (to < 0 || to >= 64) continue;

        // Evita dar a volta na borda do tabuleiro (ex: de a8 para h1)
        int to_file = to % 8;
        if (std::abs(from_file - to_file) > 1) continue;

        // Verifica se o destino já está ocupado por uma peça preta
        if (board.black_occupied & (1ULL << to)) continue;

        // Adiciona o movimento válido ao vetor
        moves.push_back(Move(from, to));
    }

    return moves;
}

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

//// Gera todos os movimentos válidos para o jogador atual
std::vector<Move> gen_all_moves(const Board& board) {
    std::vector<Move> all_moves;  // Guarda todos os movimentos válidos
    std::vector<Move> buffer;     // Guarda movimentos de peças individuais

    init_knight_attacks();  // Inicializa os ataques dos cavalos,

    // Gera movimentos para peões e reis, dependendo da cor do jogador
    if (board.turn == WHITE) {
        // Gera movimentos para peões brancos e inclui no vetor
        buffer = gen_white_pawn_moves(board);
        all_moves.insert(all_moves.end(), buffer.begin(), buffer.end());

        // Gera movimentos para o rei branco e inclui no vetor
        buffer = gen_white_king_moves(board);
        all_moves.insert(all_moves.end(), buffer.begin(), buffer.end());

        // Gera movimentos para os cavalos brancos e inclui no vetor
        buffer = gen_white_knight_moves(board);
        all_moves.insert(all_moves.end(), buffer.begin(), buffer.end());
    } else {
        // Gera movimentos para peões pretos e inclui no vetor
        buffer = gen_black_pawn_moves(board);
        all_moves.insert(all_moves.end(), buffer.begin(), buffer.end());

        // Gera movimentos para o rei preto e inclui no vetor
        buffer = gen_black_king_moves(board);
        all_moves.insert(all_moves.end(), buffer.begin(), buffer.end());

        // Gera movimentos para os cavalos pretos e inclui no vetor
        buffer = gen_black_knight_moves(board);
        all_moves.insert(all_moves.end(), buffer.begin(), buffer.end());
    }

    return all_moves;
}

}  // namespace MoveGen