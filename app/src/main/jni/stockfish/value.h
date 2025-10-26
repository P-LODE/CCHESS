#if !defined(VALUE_H_INCLUDED)
#define VALUE_H_INCLUDED

#include "piece.h"

enum ValueType {
    VALUE_TYPE_NONE = 0,
    VALUE_TYPE_UPPER = 1,
    VALUE_TYPE_LOWER = 2,
    VALUE_TYPE_EXACT = VALUE_TYPE_UPPER | VALUE_TYPE_LOWER
};

enum Value {
  VALUE_DRAW = 0,
  VALUE_KNOWN_WIN = 15000,
  VALUE_MATE = 30000,
  VALUE_INFINITE = 30001,
  VALUE_NONE = 30002,
  VALUE_ENSURE_SIGNED = -1
};

enum Score {
    ENSURE_32_BITS_SIZE_P = (1 << 16),
    ENSURE_32_BITS_SIZE_N = -(1 << 16)
};

inline Value mg_value(Score s) {
    return Value(((int(s) + 32768) & ~0xffff) / 0x10000);
}

#if define(IS_64BIT) && (!defined(__GNUC__) || defined(__INTEL_COMPILER))
inline Value eg_value(Score s) {
    return Value(int16_t(s & 0xffff));
}
#else
inline Value eg_value(Score s) {
    return Value((int)(unsigned(s) & 0x7fffu) - (int)(unsigned(s) & 0x8000u));
}
#endif

inline Score make_score(int mg, int eg) {
    return Score((mg << 16) + eg);
}

inline Score operator- (Score s) {
    return Score(-int(s));
}

inline Score operator+ (Score s1, Score s2) {
    return Score(int(s1) + int(s2));
}

inline Score operator- (Score s1, Score s2) {
    return Score(int(s1) - int(s2));
}

inline void operator+= (Score& s1, Score& s2) {
    s1 = Score(int(s1) + int(s2));
}

inline void operator-= (Score& s1, Score& s2) {
    s1 = Score(int(s1) - int(s2));
}

inline Score operator* (int i, Score s) {
    return Score(i * int(s));
}

inline Score operator/ (Score s, int i) {
    return make_score(mg_value(s) / i, eg_value(s) / i);
}

inline Score operator*(Score s1, Score s2);

const Value PawnValueMidgame   = Value(0x0C6);
const Value PawnValueEndgame   = Value(0x102);
const Value KnightValueMidgame = Value(0x331);
const Value KnightValueEndgame = Value(0x34E);
const Value BishopValueMidgame = Value(0x344);
const Value BishopValueEndgame = Value(0x359);
const Value RookValueMidgame   = Value(0x4F6);
const Value RookValueEndgame   = Value(0x4FE);
const Value QueenValueMidgame  = Value(0x9D9);
const Value QueenValueEndgame  = Value(0x9FE);

const Value PieceValueMidgame[17] = {
  Value(0),
  PawnValueMidgame, KnightValueMidgame, BishopValueMidgame,
  RookValueMidgame, QueenValueMidgame,
  Value(0), Value(0), Value(0),
  PawnValueMidgame, KnightValueMidgame, BishopValueMidgame,
  RookValueMidgame, QueenValueMidgame,
  Value(0), Value(0), Value(0)
};

const Value PieceValueEndgame[17] = {
  Value(0),
  PawnValueEndgame, KnightValueEndgame, BishopValueEndgame,
  RookValueEndgame, QueenValueEndgame,
  Value(0), Value(0), Value(0),
  PawnValueEndgame, KnightValueEndgame, BishopValueEndgame,
  RookValueEndgame, QueenValueEndgame,
  Value(0), Value(0), Value(0)
};


const Score TempoValue = make_score(48, 22);

inline Value operator+ (Value v, int i) { return Value(int(v) + i); }
inline Value operator+ (Value v1, Value v2) { return Value(int(v1) + int(v2)); }
inline void operator+= (Value &v1, Value v2) {
  v1 = Value(int(v1) + int(v2));
}
inline Value operator- (Value v, int i) { return Value(int(v) - i); }
inline Value operator- (Value v) { return Value(-int(v)); }
inline Value operator- (Value v1, Value v2) { return Value(int(v1) - int(v2)); }
inline void operator-= (Value &v1, Value v2) {
  v1 = Value(int(v1) - int(v2));
}
inline Value operator* (Value v, int i) { return Value(int(v) * i); }
inline void operator*= (Value &v, int i) { v = Value(int(v) * i); }
inline Value operator* (int i, Value v) { return Value(int(v) * i); }
inline Value operator/ (Value v, int i) { return Value(int(v) / i); }
inline void operator/= (Value &v, int i) { v = Value(int(v) / i); }


inline Value value_mate_in(int ply) {
  return Value(VALUE_MATE - Value(ply));
}

inline Value value_mated_in(int ply) {
  return Value(-VALUE_MATE + Value(ply));
}

inline bool is_upper_bound(ValueType vt) {
  return (int(vt) & int(VALUE_TYPE_UPPER)) != 0;
}

inline bool is_lower_bound(ValueType vt) {
  return (int(vt) & int(VALUE_TYPE_LOWER)) != 0;
}

inline Value piece_value_midgame(PieceType pt) {
  return PieceValueMidgame[pt];
}

inline Value piece_value_endgame(PieceType pt) {
  return PieceValueEndgame[pt];
}

inline Value piece_value_midgame(Piece p) {
  return PieceValueMidgame[p];
}

inline Value piece_value_endgame(Piece p) {
  return PieceValueEndgame[p];
}


extern Value value_to_tt(Value v, int ply);
extern Value value_from_tt(Value v, int ply);
extern int value_to_centipawns(Value v);
extern Value value_from_centipawns(int cp);
extern const std::string value_to_string(Value v);

#endif 
