#ifndef MOVE_H
#define MOVE_H

struct Move {
    int from_square;
    int to_square;

    Move(int from, int to) : from_square(from), to_square(to) {}
    Move() : from_square(-1), to_square(-1) {}

    bool operator==(const Move& other) const {
        return from_square == other.from_square && to_square == other.to_square;
    }
};

#endif