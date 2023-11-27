/**
 * @file
 * @author Vinícius Gabriel Linden
 * @date 2022-12-05
 * @copyright nubix Software-Design GmbH
 */

#include "common/debug.hpp"

void debug_clear_screen() {
    printf("%c[2J%c[H", 27, 27);
}
