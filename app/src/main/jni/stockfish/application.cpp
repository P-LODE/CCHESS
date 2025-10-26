#include "bitboard.h"
#include "direction.h"
#include "endgame.h"
#include "evaluate.h"
#include "material.h"
#include "mersenne.h"
#include "misc.h"
#include "movepick.h"
#include "position.h"
#include "search.h"
#include "thread.h"
#include "ucioption.h"

using namespace std;

Application::Application() {
    init_mersenne();
    init_direction_table();
    init_bitboards();
    init_uci_options();
    Position::init_zobrist();
    Position::init_piece_square_tables();
    init_eval(1);
    init_bitbases();
    init_search();
    init_threads();

    for (int i = abs(get_system_time() % 10000); i > 0; i--) {
        genrand_int32();
    }
}

void Application::initialize() {
    static Application singleton;
}

void Application::free_resources() {
    exit_threads();
    quit_eval();
}

void Application::exit_with_failure() {
    exit(EXIT_FAILURE);
}
