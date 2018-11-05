#include <SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

int PSP2_PollEvent(SDL_Event *event);

#ifdef __PSP2__ // NOT __SWITCH__
#define PAD_UP 8
#define PAD_DOWN 6
#define PAD_LEFT 7
#define PAD_RIGHT 9
#define PAD_TRIANGLE 0
#define PAD_SQUARE 3
#define PAD_CROSS 2
#define PAD_CIRCLE 1
#define PAD_SELECT 10
#define PAD_START 11
#define PAD_L 4
#define PAD_R 5
#endif

#ifdef __SWITCH__
#define PAD_UP 13
#define PAD_DOWN 15
#define PAD_LEFT 12
#define PAD_RIGHT 14
#define PAD_TRIANGLE 2
#define PAD_SQUARE 3
#define PAD_CROSS 1
#define PAD_CIRCLE 0
#define PAD_SELECT 11
#define PAD_START 10
#define PAD_L 6
#define PAD_R 7
#endif

#ifdef __cplusplus
}
#endif
