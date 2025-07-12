#ifndef MOVE_H
#define MOVE_H

#include <cstdint>

// Enum para os tipos de peças.
enum PieceType { NONE, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING };

// Enum para as peças de promoção.
enum PromotionType {
    PROMO_KNIGHT = 0,  // 00
    PROMO_BISHOP = 1,  // 01
    PROMO_ROOK = 2,    // 10
    PROMO_QUEEN = 3    // 11
};

// Namespace para os tipos de movimento.
namespace MoveType {
const int NORMAL = 0;      // 00
const int PROMOTION = 1;   // 01
const int EN_PASSANT = 2;  // 10
const int CASTLING = 3;    // 11
};  // namespace MoveType

class Move {
   private:
    uint16_t data;  // Armazena a informação do movimento compactada em 16 bits.

   public:
    // Construtor para um movimento vazio.
    Move() : data(0) {}

    // Construtor principal que empacota a informação do movimento.
    // Parâmetros:
    // - from: casa de origem (0-63)
    // - to: casa de destino (0-63)
    // - move_type: tipo de movimento (MoveType::PROMOTION, etc.)
    // - promo_type: tipo de peça para promoção
    Move(int from, int to, int move_type = MoveType::NORMAL, int promo_type = 0)
        : data((to & 0x3F)                   // 6 bits para a casa de destino
               | ((from & 0x3F) << 6)        // 6 bits para a casa de origem
               | ((move_type & 0x3) << 12)   // 2 bits para o tipo de movimento
               | ((promo_type & 0x3) << 14)  // 2 bits para o tipo de promoção)

          ) {};

    // Retorna a casa de origem do movimento.
    inline int from() const { return (data >> 6) & 0x3F; }

    // Retorna a casa de destino do movimento.
    inline int to() const { return data & 0x3F; }

    // Retorna o tipo de movimento.
    inline int type() const { return (data >> 12) & 0x3; }

    // Retorna o tipo de peça para promoção.
    inline int promotion_piece_type() const { return (data >> 14) & 0x3; }

    // Sobrecarga do operador de igualdade para comparar dois movimentos.
    bool operator==(const Move& other) const { return data == other.data; }
};

#endif