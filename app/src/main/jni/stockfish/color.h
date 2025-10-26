#if !defined(COLOR_H_INCLUDED)
#define COLOR_H_INCLUDED

enum Color {
  WHITE,
  BLACK,
  COLOR_NONE
};

inline void operator++ (Color &c, int) { 
    c = Color(int(c) + 1); 
}

inline Color opposite_color(Color c) {
  return Color(int(c) ^ 1);
}

inline bool color_is_ok(Color c) {
  return c == WHITE || c == BLACK;
}

#endif
