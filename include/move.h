#ifndef MOVE_H
#define MOVE_H

struct Move {
    int from;
    int to;

    Move(int from, int to) : from(from), to(to) {}
    Move() : from(-1), to(-1) {}

    bool operator==(const Move& other) const {
        return from == other.from && to == other.to;
    }
};

#endif