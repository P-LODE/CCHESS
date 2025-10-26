#if !defined(BITBOARD_H_INCLUDED)
#define BITBOARD_H_INCLUDED

#include "direction.h"
#include "piece.h"
#include "square.h"
#include "types.h"


const Bitboard EmptyBoardBB = 0ULL;

const Bitboard WhiteSquaresBB = 0x55AA55AA55AA55AAULL;
const Bitboard BlackSquaresBB = 0xAA55AA55AA55AA55ULL;

const Bitboard FileABB = 0x0101010101010101ULL;
const Bitboard FileBBB = 0x0202020202020202ULL;
const Bitboard FileCBB = 0x0404040404040404ULL;
const Bitboard FileDBB = 0x0808080808080808ULL;
const Bitboard FileEBB = 0x1010101010101010ULL;
const Bitboard FileFBB = 0x2020202020202020ULL;
const Bitboard FileGBB = 0x4040404040404040ULL;
const Bitboard FileHBB = 0x8080808080808080ULL;

const Bitboard Rank1BB = 0xFFULL;
const Bitboard Rank2BB = 0xFF00ULL;
const Bitboard Rank3BB = 0xFF0000ULL;
const Bitboard Rank4BB = 0xFF000000ULL;
const Bitboard Rank5BB = 0xFF00000000ULL;
const Bitboard Rank6BB = 0xFF0000000000ULL;
const Bitboard Rank7BB = 0xFF000000000000ULL;
const Bitboard Rank8BB = 0xFF00000000000000ULL;

extern const Bitboard SquaresByColorBB[2];
extern const Bitboard FileBB[8];
extern const Bitboard NeighboringFilesBB[8];
extern const Bitboard ThisAndNeighboringFilesBB[8];
extern const Bitboard RankBB[8];
extern const Bitboard RelativeRankBB[2][8];
extern const Bitboard InFrontBB[2][8];

extern Bitboard SetMaskBB[65];
extern Bitboard ClearMaskBB[65];

extern Bitboard StepAttackBB[16][64];
extern Bitboard RayBB[64][8];
extern Bitboard BetweenBB[64][64];

extern Bitboard SquaresInFrontMask[2][64];
extern Bitboard PassedPawnMask[2][64];
extern Bitboard AttackSpanMask[2][64];

extern const uint64_t RMult[64];
extern const int RShift[64];
extern Bitboard RMask[64];
extern int RAttackIndex[64];
extern Bitboard RAttacks[0x19000];

extern const uint64_t BMult[64];
extern const int BShift[64];
extern Bitboard BMask[64];
extern int BAttackIndex[64];
extern Bitboard BAttacks[0x1480];

extern Bitboard BishopPseudoAttacks[64];
extern Bitboard RookPseudoAttacks[64];
extern Bitboard QueenPseudoAttacks[64];

extern uint8_t BitCount8Bit[256];

inline Bitboard bit_is_set(Bitboard b, Square s) {
  return b & SetMaskBB[s];
}

inline void set_bit(Bitboard *b, Square s) {
  *b |= SetMaskBB[s];
}

inline void clear_bit(Bitboard *b, Square s) {
  *b &= ClearMaskBB[s];
}

inline Bitboard make_move_bb(Square from, Square to) {
  return SetMaskBB[from] | SetMaskBB[to];
}

inline void do_move_bb(Bitboard *b, Bitboard move_bb) {
  *b ^= move_bb;
}

inline Bitboard rank_bb(Rank r) {
  return RankBB[r];
}

inline Bitboard rank_bb(Square s) {
  return rank_bb(square_rank(s));
}

inline Bitboard file_bb(File f) {
  return FileBB[f];
}

inline Bitboard file_bb(Square s) {
  return file_bb(square_file(s));
}

inline Bitboard neighboring_files_bb(File f) {
  return NeighboringFilesBB[f];
}

inline Bitboard neighboring_files_bb(Square s) {
  return NeighboringFilesBB[square_file(s)];
}

inline Bitboard this_and_neighboring_files_bb(File f) {
  return ThisAndNeighboringFilesBB[f];
}

inline Bitboard this_and_neighboring_files_bb(Square s) {
  return ThisAndNeighboringFilesBB[square_file(s)];
}

inline Bitboard relative_rank_bb(Color c, Rank r) {
  return RelativeRankBB[c][r];
}

inline Bitboard in_front_bb(Color c, Rank r) {
  return InFrontBB[c][r];
}

inline Bitboard in_front_bb(Color c, Square s) {
  return InFrontBB[c][square_rank(s)];
}

inline Bitboard behind_bb(Color c, Rank r) {
  return InFrontBB[opposite_color(c)][r];
}

inline Bitboard behind_bb(Color c, Square s) {
  return InFrontBB[opposite_color(c)][square_rank(s)];
}

inline Bitboard ray_bb(Square s, SignedDirection d) {
  return RayBB[s][d];
}


#if defined(IS_64BIT)

inline Bitboard rook_attacks_bb(Square s, Bitboard blockers) {
  Bitboard b = blockers & RMask[s];
  return RAttacks[RAttackIndex[s] + ((b * RMult[s]) >> RShift[s])];
}

inline Bitboard bishop_attacks_bb(Square s, Bitboard blockers) {
  Bitboard b = blockers & BMask[s];
  return BAttacks[BAttackIndex[s] + ((b * BMult[s]) >> BShift[s])];
}

#else

inline Bitboard rook_attacks_bb(Square s, Bitboard blockers) {
  Bitboard b = blockers & RMask[s];
  return RAttacks[RAttackIndex[s] +
                  (unsigned(int(b) * int(RMult[s]) ^
                            int(b >> 32) * int(RMult[s] >> 32))
                   >> RShift[s])];
}

inline Bitboard bishop_attacks_bb(Square s, Bitboard blockers) {
  Bitboard b = blockers & BMask[s];
  return BAttacks[BAttackIndex[s] +
                  (unsigned(int(b) * int(BMult[s]) ^
                            int(b >> 32) * int(BMult[s] >> 32))
                   >> BShift[s])];
}

#endif


inline Bitboard queen_attacks_bb(Square s, Bitboard blockers) {
  return rook_attacks_bb(s, blockers) | bishop_attacks_bb(s, blockers);
}

inline Bitboard squares_between(Square s1, Square s2) {
  return BetweenBB[s1][s2];
}

inline Bitboard squares_in_front_of(Color c, Square s) {
  return SquaresInFrontMask[c][s];
}

inline Bitboard squares_behind(Color c, Square s) {
  return SquaresInFrontMask[opposite_color(c)][s];
}

inline Bitboard passed_pawn_mask(Color c, Square s) {
  return PassedPawnMask[c][s];
}

inline Bitboard attack_span_mask(Color c, Square s) {
  return AttackSpanMask[c][s];
}

#if defined(USE_BSFQ)

inline Square first_1(Bitboard b) {
  Bitboard dummy;
  __asm__("bsfq %1, %0": "=r"(dummy): "rm"(b) );
  return (Square)(dummy);
}

inline Square pop_1st_bit(Bitboard* b) {
  const Square s = first_1(*b);
  *b &= ~(1ULL<<s);
  return s;
}

#else

extern Square first_1(Bitboard b);
extern Square pop_1st_bit(Bitboard* b);

#endif

extern void print_bitboard(Bitboard b);
extern void init_bitboards();

#endif
