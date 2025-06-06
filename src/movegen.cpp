#include "movegen.h"

namespace MoveGen {

// Encontra o índice do bit menos significativo (LSB) em um bitboard
inline int get_lsb_index(uint64_t bitboard) {
    if (bitboard == 0) return -1;
    return __builtin_ctzll(bitboard);
}

// Retorna os movimentos possíveis para peões brancos
std::vector<Move> generate_white_pawn_moves(const Board& board_state) {
    std::vector<Move> moves;  // Vetor para armazenar os movimentos válidos

    /**
     * 1. Avanço simples (uma casa para frente)
     */

    // Simula todos os peões brancos avançando uma casa
    uint64_t white_pawns_single_push_targets = (board_state.white_pawns << 8);

    // Mantém apenas os movimentos para casas que estão vazias
    uint64_t valid_single_pushes =
        white_pawns_single_push_targets & (~board_state.all_occupied);

    // Itera sobre os destinos válidos e adiciona os movimentos
    // Enquanto houver bits '1' (movimentos válidos)
    uint64_t pushes_remaining = valid_single_pushes;
    while (pushes_remaining) {
        // Calcula o índice do destino e da origem
        int to_square = get_lsb_index(pushes_remaining);
        int from_square = to_square - 8;

        // Adiciona o movimento à lista e remove o bit processado
        moves.push_back(Move(from_square, to_square));
        pushes_remaining &= pushes_remaining - 1;
    }

    /**
     * 2. Avanço duplo (duas casas para frente) quando o peão está na
     * segunda fileira
     */

    // Mascara para identificar os peões brancos na segunda fileira
    const uint64_t RANK_2_WHITE_PAWNS_MASK = 0x000000000000FF00ULL;

    // Obtém os peões brancos que estão na segunda fileira
    uint64_t pawns_on_rank_2 =
        board_state.white_pawns & RANK_2_WHITE_PAWNS_MASK;

    // Simula o avanço de uma casa e mantém apenas para casas vazias
    uint64_t single_push_targets_from_rank_2 = (pawns_on_rank_2 << 8);
    uint64_t valid_intermediate_squares =
        single_push_targets_from_rank_2 & (~board_state.all_occupied);

    // Simula o avanço de mais uma casa e mantém apenas para casas vazias
    uint64_t double_push_targets = (valid_intermediate_squares << 8);
    uint64_t valid_double_pushes =
        double_push_targets & (~board_state.all_occupied);

    // Itera sobre os destinos válidos e adiciona os movimentos
    // Enquanto houver bits '1' (movimentos válidos)
    uint64_t double_pushes_remaining = valid_double_pushes;
    while (double_pushes_remaining) {
        // Pega o índice da casa de destino (o bit menos significativo) e
        // calcula a casa de origem
        int to_square = get_lsb_index(double_pushes_remaining);
        int from_square = to_square - 16;

        // Adiciona o movimento à lista e remove o bit processado
        moves.push_back(Move(from_square, to_square));
        double_pushes_remaining &= double_pushes_remaining - 1;
    }

    /**
     * Lógica para capturas, etc será adicionada aqui.
     * */

    return moves;
}

// Retorna os movimentos possíveis para peões pretos
std::vector<Move> generate_black_pawn_moves(const Board& board_state) {
    std::vector<Move> moves;  // Vetor para armazenar os movimentos válidos

    /**
     * 1. Avanço simples (uma casa para frente)
     */

    // Simula todos os peões pretos avançando uma casa
    uint64_t black_pawns_single_push_targets = (board_state.black_pawns >> 8);

    // Mantém apenas os movimentos para casas que estão vazias
    uint64_t valid_single_pushes =
        black_pawns_single_push_targets & (~board_state.all_occupied);

    // Itera sobre os destinos válidos e adiciona os movimentos
    // Enquanto houver bits '1' (movimentos válidos)
    uint64_t pushes_remaining = valid_single_pushes;
    while (pushes_remaining) {
        // Calcula o índice do destino e da origem
        int to_square = get_lsb_index(pushes_remaining);
        int from_square = to_square + 8;

        // Adiciona o movimento à lista e remove o bit processado
        moves.push_back(Move(from_square, to_square));
        pushes_remaining &= pushes_remaining - 1;
    }

    /**
     * 2. Avanço duplo (duas casas para frente) quando o peão está na
     * sétima fileira
     */

    // Mascara para identificar os peões pretos na sétima fileira
    const uint64_t RANK_7_BLACK_PAWNS_MASK = 0x00FF000000000000ULL;

    // Obtém os peões pretos que estão na sétima fileira
    uint64_t pawns_on_rank_7 =
        board_state.black_pawns & RANK_7_BLACK_PAWNS_MASK;

    // Simula o avanço de uma casa e mantém apenas para casas vazias
    uint64_t single_push_targets_from_rank_7 = (pawns_on_rank_7 >> 8);
    uint64_t valid_intermediate_squares =
        single_push_targets_from_rank_7 & (~board_state.all_occupied);

    // Simula o avanço de mais uma casa e mantém apenas para casas vazias
    uint64_t double_push_targets = (valid_intermediate_squares >> 8);
    uint64_t valid_double_pushes =
        double_push_targets & (~board_state.all_occupied);

    // Itera sobre os destinos válidos e adiciona os movimentos
    // Enquanto houver bits '1' (movimentos válidos)
    uint64_t double_pushes_remaining = valid_double_pushes;
    while (double_pushes_remaining) {
        // Pega o índice da casa de destino (o bit menos significativo) e
        // calcula a casa de origem
        int to_square = get_lsb_index(double_pushes_remaining);
        int from_square = to_square + 16;

        // Adiciona o movimento à lista e remove o bit processado
        moves.push_back(Move(from_square, to_square));
        double_pushes_remaining &= double_pushes_remaining - 1;
    }

    /**
     * Lógica para capturas, etc será adicionada aqui.
     * */

    return moves;
}

// Retorna os movimentos possíveis para o rei branco
std::vector<Move> generate_white_king_moves(const Board& board_state) {
    std::vector<Move> moves;  // Vetor para armazenar os movimentos válidos

    // Obtém o rei branco do estado do tabuleiro
    uint64_t white_king = board_state.white_king;

    // Se não houver rei branco (em um fluxo normal, isso não deve acontecer),
    // retorna um vetor vazio
    if (white_king == 0) {
        return moves;
    }

    // Encontra a posição do rei branco (único bit no bitboard)
    int king_square = get_lsb_index(white_king);

    // Calcula as casas adjacentes onde o rei possivelmente pode se mover
    int potential_target_squares[8] = {
        king_square - 1, king_square + 1, king_square - 8, king_square + 8,
        king_square - 9, king_square - 7, king_square + 7, king_square + 9};

    // Itera sobre os possíveis movimentos
    for (int target_square : potential_target_squares) {
        // Fora do tabuleiro
        if (target_square < 0 || target_square >= 64) {
            continue;
        }

        // Evita movimentos que cruzem colunas
        // Exemplo: se o rei está na casa h1 e se move para a casa a1
        int king_file = king_square % 8;
        int target_file = target_square % 8;

        int file_difference = king_file - target_file;

        // Se a diferença de coluna for maior que 1 ou menor que -1,
        // quer dizer que o rei pulou mais de uma coluna
        if (file_difference > 1 || file_difference < -1) {
            continue;
        }

        // O destino não pode estar ocupado por uma peça branca
        if (board_state.white_occupied & (1ULL << target_square)) {
            continue;
        }

        // Adiciona o movimento válido à lista
        moves.push_back(Move(king_square, target_square));
    }

    return moves;
}

// Retorna os movimentos possíveis para o rei preto
std::vector<Move> generate_black_king_moves(const Board& board_state) {
    std::vector<Move> moves;  // Vetor para armazenar os movimentos válidos

    // Obtém o rei preto do estado do tabuleiro
    uint64_t black_king = board_state.black_king;

    // Se não houver rei preto (em um fluxo normal, isso não deve acontecer),
    // retorna um vetor vazio
    if (black_king == 0) {
        return moves;
    }

    // Encontra a posição do rei preto (teoricamente, o único bit 1 no bitboard)
    int king_square = get_lsb_index(black_king);

    // Calcula as casas adjacentes onde o rei possivelmente pode se mover
    int potential_target_squares[8] = {
        king_square - 1, king_square + 1, king_square - 8, king_square + 8,
        king_square - 9, king_square - 7, king_square + 7, king_square + 9};

    // Itera sobre os possíveis movimentos
    for (int target_square : potential_target_squares) {
        // Fora do tabuleiro
        if (target_square < 0 || target_square >= 64) {
            continue;
        }

        // Evita movimentos que cruzem colunas
        // Exemplo: se o rei está na casa h8 e se move para a casa a8
        int king_file = king_square % 8;
        int target_file = target_square % 8;

        int file_difference = king_file - target_file;
        if (file_difference > 1 || file_difference < -1) {
            continue;
        }

        // O destino não pode estar ocupado por uma peça preta
        if (board_state.black_occupied & (1ULL << target_square)) {
            continue;
        }

        moves.push_back(Move(king_square, target_square));
    }

    return moves;
}

// Gera todos os movimentos válidos para o jogador atual
std::vector<Move> generate_all_moves(const Board& board_state) {
    // Vetor para guardar todos os movimentos válidos
    std::vector<Move> all_player_moves;

    // Vetor auxiliar para armazenar movimentos de peças individuais
    std::vector<Move> piece_moves_buffer;

    // Gera movimentos para peões e reis, dependendo da cor do jogador
    if (board_state.turn == WHITE) {
        // Gera movimentos para peões brancos e inclui no vetor
        piece_moves_buffer = generate_white_pawn_moves(board_state);
        all_player_moves.insert(all_player_moves.end(),
                                piece_moves_buffer.begin(),
                                piece_moves_buffer.end());

        // Gera movimentos para o rei branco e inclui no vetor
        piece_moves_buffer = generate_white_king_moves(board_state);
        all_player_moves.insert(all_player_moves.end(),
                                piece_moves_buffer.begin(),
                                piece_moves_buffer.end());

    } else {
        // Gera movimentos para peões pretos e inclui no vetor
        piece_moves_buffer = generate_black_pawn_moves(board_state);
        all_player_moves.insert(all_player_moves.end(),
                                piece_moves_buffer.begin(),
                                piece_moves_buffer.end());

        // Gera movimentos para o rei preto e inclui no vetor
        piece_moves_buffer = generate_black_king_moves(board_state);
        all_player_moves.insert(all_player_moves.end(),
                                piece_moves_buffer.begin(),
                                piece_moves_buffer.end());
    }

    return all_player_moves;
}
}  // namespace MoveGen