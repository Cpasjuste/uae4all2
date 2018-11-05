#include<stdio.h>
#include<stdlib.h>
#include<SDL.h>

#ifdef USE_SDL2
#include "sdl2_to_sdl1.h"
#endif

#include "vkbd.h"

#include "keyboard.h"

#define VKBD_MIN_HOLDING_TIME 200
#define VKBD_MOVE_DELAY 50

extern int keycode2amiga(SDL_keysym *prKeySym);

extern int mainMenu_displayHires;
extern int mainMenu_vkbdLanguage;

static int vkbd_x=VKBD_X;
static int vkbd_y=VKBD_Y;
static int vkbd_transparency=255;
static int vkbd_just_blinked=0;
static Uint32 vkbd_last_press_time=0;
static Uint32 vkbd_last_move_time=0;

// Keep track of keys that are sticky
t_vkbd_sticky_key vkbd_sticky_key[] = 
{
	{ AK_LSH, false, true, 70},
	{ AK_RSH, false, true, 81},
	{ AK_CTRL, false, true, 54},
	{ AK_LALT, false, true, 85},
	{ AK_RALT, false, true, 89},
	{ AK_LAMI, false, true, 86},
	{ AK_RAMI, false, true, 88}
};

int vkbd_let_go_of_direction=0;
int vkbd_mode=0;
int vkbd_move=0;
int vkbd_key=KEYCODE_NOTHING;
SDLKey vkbd_button2=(SDLKey)0; //not implemented
int vkbd_keysave=KEYCODE_NOTHING;

#if !defined (DREAMCAST) && !defined (GP2X) && !defined (PSP) && !defined (GIZMONDO) 

int vkbd_init(void) { return 0; }
void vkbd_init_button2(void) { }
void vkbd_quit(void) { }
int vkbd_process(void) { return KEYCODE_NOTHING; }
void vkbd_displace_up(void) { };
void vkbd_displace_down(void) { };
void vkbd_transparency_up(void) { };
void vkbd_transparency_down(void) { };
#else

#define MAX_KEY_SMALL 110

extern SDL_Surface *prSDLScreen;

static SDL_Surface *ksur;
static SDL_Surface *ksurHires;
static SDL_Surface *canvas; // intermediate surface used to highlight sticky keys
static SDL_Surface *canvasHires; // intermediate surface used to highlight sticky keys
#ifdef LARGEKEYBOARD // The new larger keyboard uses transparency and supports shift.
static SDL_Surface *ksurShift;
static SDL_Surface *ksurShiftHires;
#else
static SDL_Surface *vkey[MAX_KEY_SMALL];
#endif

static int vkbd_actual=0, vkbd_color=0;

#ifdef GP2X
extern char launchDir [256];
#endif

typedef struct{
	SDL_Rect rect;
	unsigned char up,down,left,right;
#ifdef LARGEKEYBOARD
	int key;
#else
	SDLKey key; //Amiga Keycode, defined in keyboard.h, some use shift etc.
#endif
} t_vkbd_rect;

static t_vkbd_rect *vkbd_rect;

//Keyboard parameters
#ifndef LARGEKEYBOARD 
//Small Keyboard this is the old one from Pandora version based on SDL keys
static t_vkbd_rect vkbd_rect_Small[]=
{
	{{  8, 3, 6, 6 },89,10, 9, 1, SDLK_F1},	// 0
	{{ 18, 3, 6, 6 },89,12, 0, 2, SDLK_F2},	// 1
	{{ 28, 3, 6, 6 },90,14, 1, 3, SDLK_F3},	// 2
	{{ 38, 3, 6, 6 },90,15, 2, 4, SDLK_F4},	// 3
	{{ 48, 3, 6, 6 },90,17, 3, 5, SDLK_F5},	// 4
	{{ 58, 3, 6, 6 },90,18, 4, 6, SDLK_F6},	// 5 
	{{ 69, 3, 6, 6 },90,20, 5, 7, SDLK_F7},	// 6
	{{ 79, 3, 6, 6 },90,22, 6, 8, SDLK_F8},	// 7 
	{{ 90, 3, 6, 6 },90,23, 7, 9, SDLK_F9},	// 8 
	{{100, 3, 6, 6 },91,24, 8, 0, SDLK_F10},// 9 

	{{  2,12, 6, 6 }, 0,31,30,11, SDLK_ESCAPE}, // 10 
	{{  9,12, 6, 6 }, 0,32,10,12, SDLK_1},	// 11
	{{ 16,12, 6, 6 }, 1,33,11,13, SDLK_2},	// 12
	{{ 23,12, 6, 6 }, 2,34,12,14, SDLK_3},	// 13
	{{ 29,12, 6, 6 }, 2,35,13,15, SDLK_4},	// 14
	{{ 36,12, 6, 6 }, 3,36,14,16, SDLK_5},	// 15
	{{ 42,12, 6, 6 }, 4,37,15,17, SDLK_6},	// 16
	{{ 49,12, 6, 6 }, 4,38,16,18, SDLK_7},	// 17
	{{ 56,12, 6, 6 }, 5,39,17,19, SDLK_8},	// 18
	{{ 62,12, 6, 6 }, 5,40,18,20, SDLK_9},	// 19
	{{ 69,12, 6, 6 }, 6,41,19,21, SDLK_0},	// 20
	{{ 76,12, 6, 6 }, 7,42,20,22, SDLK_UNDERSCORE},	// 21
	{{ 83,12, 6, 6 }, 7,43,21,23, SDLK_EQUALS},	// 22
	{{ 90,12, 6, 6 }, 8,64,22,24, SDLK_BACKSLASH},	// 23
	{{ 97,12, 9, 6 }, 9,44,23,25, SDLK_BACKSPACE},// 24
	{{109,12,10, 6 },66,45,24,26, SDLK_PRINT},	// 25
	{{118,12,10, 6 },68,47,25,27, SDLK_PAUSE},	// 26
	{{131,12, 6, 6 },92,48,26,28, SDLK_LEFTPAREN},	// 27
	{{138,12, 6, 6 },92,49,27,29, SDLK_RIGHTPAREN},	// 28
	{{145,12, 6, 6 },93,50,28,30, SDLK_KP_DIVIDE},	// 29
	{{152,12, 6, 6 },88,51,29,10, SDLK_KP_MULTIPLY},	// 30

	{{  2,17,10, 6 },10,52,51,32, SDLK_TAB},	// 31 
	{{ 12,17, 6, 6 },11,53,31,33, SDLK_q},	// 32 
	{{ 19,17, 6, 6 },12,54,32,34, SDLK_w},	// 33
	{{ 25,17, 6, 6 },13,55,33,35, SDLK_e},	// 34
	{{ 32,17, 6, 6 },14,56,34,36, SDLK_r},	// 35
	{{ 39,17, 6, 6 },15,57,35,37, SDLK_t},	// 36
	{{ 46,17, 6, 6 },16,58,36,38, SDLK_y},	// 37
	{{ 52,17, 6, 6 },17,59,37,39, SDLK_u},	// 38
	{{ 59,17, 6, 6 },18,60,38,40, SDLK_i},	// 39
	{{ 65,17, 6, 6 },19,61,39,41, SDLK_o},	// 40
	{{ 72,17, 6, 6 },20,62,40,42, SDLK_p},	// 41
	{{ 79,17, 6, 6 },21,63,41,43, SDLK_LEFTBRACKET},	// 42
	{{ 86,17, 6, 6 },22,64,42,44, SDLK_RIGHTBRACKET},	// 43
	{{100,17, 6, 6 },24,65,43,45, SDLK_DELETE},	// 44
	{{109,17, 6, 6 },25,66,44,46, SDLK_INSERT},	// 45
	{{115,17, 6, 6 },25,67,45,47, SDLK_UP},	// 46
	{{122,17, 6, 6 },26,68,46,48, SDLK_HOME},	// 47
	{{131,17, 6, 6 },27,69,47,49, SDLK_KP7},	// 48
	{{138,17, 6, 6 },28,70,48,50, SDLK_KP8},	// 49
	{{145,17, 6, 6 },29,71,49,51, SDLK_KP9},	// 50
	{{152,17, 6, 6 },30,72,50,31, SDLK_KP_MINUS},	// 51

	{{  2,22,12, 6 },31,73,72,53, SDLK_CAPSLOCK},	// 52
	{{ 14,22, 6, 6 },32,74,52,54, SDLK_a},	// 53
	{{ 21,22, 6, 6 },33,75,53,55, SDLK_s},	// 54
	{{ 27,22, 6, 6 },34,76,54,56, SDLK_d},	// 55
	{{ 34,22, 6, 6 },35,77,55,57, SDLK_f},	// 56
	{{ 41,22, 6, 6 },36,78,56,58, SDLK_g},	// 57
	{{ 48,22, 6, 6 },37,79,57,59, SDLK_h},	// 58
	{{ 54,22, 6, 6 },38,80,58,60, SDLK_j},	// 59
	{{ 61,22, 6, 6 },39,81,59,61, SDLK_k},	// 60
	{{ 67,22, 6, 6 },40,82,60,62, SDLK_l},	// 61
	{{ 74,22, 6, 6 },41,83,61,63, SDLK_SEMICOLON},	// 62
	{{ 81,22, 6, 6 },42,84,62,64, SDLK_COLON},	// 63
	{{ 92,18, 7,10 },23,84,63,65, SDLK_RETURN},	// 64
	{{100,22, 6, 6 },44, 9,64,66, SDLK_QUOTE},	// 65
	{{109,22, 6, 6 },45,25,65,67, SDLK_LEFT},	// 66
	{{115,22, 6, 6 },46,25,66,68, SDLK_DOWN},	// 67
	{{122,22, 6, 6 },47,26,67,69, SDLK_RIGHT},	// 68
	{{131,22, 6, 6 },48,85,68,70, SDLK_KP4},	// 69
	{{138,22, 6, 6 },49,86,69,71, SDLK_KP5},	// 70
	{{145,22, 6, 6 },50,87,70,72, SDLK_KP6},	// 71
	{{152,22, 6, 6 },51,88, 71,52, SDLK_KP_PLUS},	// 72

	{{  2,28,15, 6 },52,89,88,74, SDLK_RSHIFT},	// 73
	{{ 17,28, 6, 6 },53,89,73,75, SDLK_z},	// 74
	{{ 24,28, 6, 6 },54,90,74,76, SDLK_x},	// 75
	{{ 30,28, 6, 6 },55,90,75,77, SDLK_c},	// 76
	{{ 37,28, 6, 6 },56,90,76,78, SDLK_v},	// 77
	{{ 44,28, 6, 6 },57,90,77,79, SDLK_b},	// 78
	{{ 51,28, 6, 6 },58,90,78,80, SDLK_n},	// 79
	{{ 57,28, 6, 6 },59,90,79,81, SDLK_m},	// 80
	{{ 64,28, 6, 6 },60,90,80,82, SDLK_COMMA},	// 81
	{{ 70,28, 6, 6 },61,90,81,83, SDLK_PERIOD},	// 82
	{{ 77,28, 6, 6 },62,91,82,84, SDLK_COLON},	// 83
	{{ 84,28,10, 6 },63,91,83,85, SDLK_LSHIFT},	// 84
	{{131,28, 6, 6 },69,92,84,86, SDLK_KP1},	// 85
	{{138,28, 6, 6 },70,92,85,87, SDLK_KP2},	// 86
	{{145,28, 6, 6 },71,93,86,88, SDLK_KP3},	// 87
	{{152,29, 6,10 },72,30,87,73, SDLK_KP_ENTER},	// 88

	{{ 10,34,10, 5 },73, 1,88,90, SDLK_LCTRL},	// 89
	{{ 20,34,60, 5 },78, 4,89,91, SDLK_SPACE},	// 90
	{{ 80,34,10, 5 },84, 8,90,92, SDLK_RCTRL},	// 91
	{{131,34,12, 5 },86,28,91,93, SDLK_KP0},	// 92
	{{145,34, 6, 5 },87,29,92,88, SDLK_KP_PERIOD},	// 93
};
#else
//This is the new US keyboard, based directly on amiga-side keys
//last four numbers: next keys in up,down,left,right directions
static t_vkbd_rect vkbd_rect_US[]= 
{
	{{  1,  1, 29, 11 },85,17,16, 1, AK_ESC}, // 0, row 1 start 
	{{ 31,  1, 14, 11 },86,18, 0, 2, AK_F1},	// 1
	{{ 46,  1, 14, 11 },87,19, 1, 3, AK_F2},	// 2
	{{ 61,  1, 14, 11 },87,20, 2, 4, AK_F3},	// 3
	{{ 76,  1, 14, 11 },87,21, 3, 5, AK_F4},	// 4
	{{ 91,  1, 14, 11 },87,22, 4, 6, AK_F5},	// 5 
	{{106,  1, 14, 11 },87,23, 5, 7, AK_F6},	// 6
	{{121,  1, 14, 11 },87,24, 6, 8, AK_F7},	// 7 
	{{136,  1, 14, 11 },87,25, 7, 9, AK_F8},	// 8 
	{{151,  1, 14, 11 },87,26, 8,10, AK_F9},	// 9
	{{166,  1, 14, 11 },87,27, 9,11, AK_F10},	// 10
	{{181,  1, 29, 11 },88,28,10,12, AK_DEL},	// 11
	{{211,  1, 29, 11 },90,30,11,13, AK_HELP},	// 12
	{{241,  1, 14, 11 },92,32,12,14, AK_NPLPAREN},	// 13
	{{256,  1, 14, 11 },69,33,13,15, AK_NPRPAREN},	// 14
	{{271,  1, 14, 11 },69,34,14,16, AK_NPDIV},	// 15
	{{286,  1, 14, 11 },69,35,15,0, AK_NPMUL},	// 16
	
	{{  1, 13, 29, 11 }, 0,36,35,18, AK_BACKQUOTE}, // 17, row 2 start 
	{{ 31, 13, 14, 11 }, 1,37,17,19, AK_1},	// 18
	{{ 46, 13, 14, 11 }, 2,38,18,20, AK_2},	// 19
	{{ 61, 13, 14, 11 }, 3,39,19,21, AK_3},	// 20
	{{ 76, 13, 14, 11 }, 4,40,20,22, AK_4},	// 21
	{{ 91, 13, 14, 11 }, 5,41,21,23, AK_5},	// 22
	{{106, 13, 14, 11 }, 6,42,22,24, AK_6},	// 23
	{{121, 13, 14, 11 }, 7,43,23,25, AK_7},	// 24 
	{{136, 13, 14, 11 }, 8,44,24,26, AK_8},	// 25
	{{151, 13, 14, 11 }, 9,45,25,27, AK_9},	// 26
	{{166, 13, 14, 11 },10,46,26,28, AK_0},	// 27
	{{181, 13, 14, 11 },11,47,27,29, AK_MINUS},	// 28
	{{196, 13, 14, 11 },11,48,28,30, AK_EQUAL},	// 29
	{{211, 13, 14, 11 },12,49,29,31, AK_BACKSLASH},	// 30
	{{226, 13, 14, 11 },12,49,30,32, AK_BS},	// 31
	{{241, 13, 14, 11 },13,50,31,33, AK_NP7},	// 32
	{{256, 13, 14, 11 },14,51,32,34, AK_NP8},	// 33
	{{271, 13, 14, 11 },15,52,33,35, AK_NP9},	// 34
	{{286, 13, 14, 11 },16,53,34,17, AK_NPSUB},	// 35
	
	{{  1, 25, 29, 11 }, 17,54,53,37, AK_TAB}, // 36, row 3 start 
	{{ 31, 25, 14, 11 }, 18,55,36,38, AK_Q},	// 37
	{{ 46, 25, 14, 11 }, 19,56,37,39, AK_W},	// 38
	{{ 61, 25, 14, 11 }, 20,57,38,40, AK_E},	// 39
	{{ 76, 25, 14, 11 }, 21,58,39,41, AK_R},	// 40
	{{ 91, 25, 14, 11 }, 22,59,40,42, AK_T},	// 41
	{{106, 25, 14, 11 }, 23,60,41,43, AK_Y},	// 42
	{{121, 25, 14, 11 }, 24,61,42,44, AK_U},	// 43 
	{{136, 25, 14, 11 }, 25,62,43,45, AK_I},	// 44
	{{151, 25, 14, 11 }, 26,63,44,46, AK_O},	// 45
	{{166, 25, 14, 11 }, 27,64,45,47, AK_P},	// 46
	{{181, 25, 14, 11 }, 28,65,46,48, AK_LBRACKET},	// 47
	{{196, 25, 14, 11 }, 29,49,47,49, AK_RBRACKET},	// 48
	{{211, 25, 29, 23 }, 30,81,48,50, AK_RET},	// 49
	{{241, 25, 14, 11 }, 32,66,49,51, AK_NP4},	// 50
	{{256, 25, 14, 11 }, 33,67,50,52, AK_NP5},	// 51
	{{271, 25, 14, 11 }, 34,68,51,53, AK_NP6},	// 52
	{{286, 25, 14, 11 }, 35,69,52,36, AK_NPADD},	// 53
	
	{{  1, 37, 29, 11 }, 36,70,69,55, AK_CTRL}, // 54, row 4 start 
	{{ 31, 37, 14, 11 }, 37,70,54,56, AK_A},	// 55
	{{ 46, 37, 14, 11 }, 38,71,55,57, AK_S},	// 56
	{{ 61, 37, 14, 11 }, 39,72,56,58, AK_D},	// 57
	{{ 76, 37, 14, 11 }, 40,73,57,59, AK_F},	// 58
	{{ 91, 37, 14, 11 }, 41,74,58,60, AK_G},	// 59
	{{106, 37, 14, 11 }, 42,75,59,61, AK_H},	// 60
	{{121, 37, 14, 11 }, 43,76,60,62, AK_J},	// 61 
	{{136, 37, 14, 11 }, 44,77,61,63, AK_K},	// 62
	{{151, 37, 14, 11 }, 45,78,62,64, AK_L},	// 63
	{{166, 37, 14, 11 }, 46,79,63,65, AK_SEMICOLON},	// 64
	{{181, 37, 14, 11 }, 47,80,64,49, AK_QUOTE},	// 65
	{{241, 37, 14, 11 }, 50,83,49,67, AK_NP1},	// 66
	{{256, 37, 14, 11 }, 51,83,66,68, AK_NP2},	// 67
	{{271, 37, 14, 11 }, 52,84,67,69, AK_NP3},	// 68
	{{286, 37, 14, 34 }, 53,16,68,54, AK_ENT},	// 69
	
	{{  1, 49, 44, 11 }, 54,85,84,71, AK_LSH}, // 70, row 5 start 
	{{ 46, 49, 14, 11 }, 56,87,70,72, AK_Z},	// 71
	{{ 61, 49, 14, 11 }, 57,87,71,73, AK_X},	// 72
	{{ 76, 49, 14, 11 }, 58,87,72,74, AK_C},	// 73
	{{ 91, 49, 14, 11 }, 59,87,73,75, AK_V},	// 74
	{{106, 49, 14, 11 }, 60,87,74,76, AK_B},	// 75
	{{121, 49, 14, 11 }, 61,87,75,77, AK_N},	// 76 
	{{136, 49, 14, 11 }, 62,87,76,78, AK_M},	// 77
	{{151, 49, 14, 11 }, 63,87,77,79, AK_COMMA},	// 78
	{{166, 49, 14, 11 }, 64,87,78,80, AK_PERIOD},	// 79
	{{181, 49, 14, 11 }, 65,88,79,81, AK_SLASH},	// 80
	{{196, 49, 29, 11 }, 49,89,80,82, AK_RSH},	// 81
	{{226, 49, 14, 11 }, 49,91,81,83, AK_UP},	// 82
	{{241, 49, 29, 11 }, 66,92,82,84, AK_NP0},	// 83
	{{271, 49, 14, 11 }, 67,69,83,69, AK_NPDEL},	// 84
	
	{{  1, 61, 29, 11 }, 70,0,69,86, AK_LALT}, // 85, row 6 start 
	{{ 31, 61, 14, 11 }, 70,1,85,87, AK_LAMI},	// 86
	{{ 46, 61,134, 11 }, 71,2,86,88, AK_SPC},	// 87
	{{181, 61, 14, 11 }, 80,11,87,89, AK_RAMI},	// 88
	{{196, 61, 14, 11 }, 81,11,88,90, AK_RALT},	// 89
	{{211, 61, 14, 11 }, 81,12,89,91, AK_LF},	// 90
	{{226, 61, 14, 11 }, 82,12,90,92, AK_DN},	// 91 
	{{241, 61, 14, 11 }, 83,13,91,69, AK_RT},	// 92
};

//UK KEYBOARD
static t_vkbd_rect vkbd_rect_UK[]= 
{
	{{  1,  1, 29, 11 },85,17,16, 1, AK_ESC}, // 0, row 1 start 
	{{ 31,  1, 14, 11 },86,18, 0, 2, AK_F1},	// 1
	{{ 46,  1, 14, 11 },87,19, 1, 3, AK_F2},	// 2
	{{ 61,  1, 14, 11 },87,20, 2, 4, AK_F3},	// 3
	{{ 76,  1, 14, 11 },87,21, 3, 5, AK_F4},	// 4
	{{ 91,  1, 14, 11 },87,22, 4, 6, AK_F5},	// 5 
	{{106,  1, 14, 11 },87,23, 5, 7, AK_F6},	// 6
	{{121,  1, 14, 11 },87,24, 6, 8, AK_F7},	// 7 
	{{136,  1, 14, 11 },87,25, 7, 9, AK_F8},	// 8 
	{{151,  1, 14, 11 },87,26, 8,10, AK_F9},	// 9
	{{166,  1, 14, 11 },87,27, 9,11, AK_F10},	// 10
	{{181,  1, 29, 11 },88,28,10,12, AK_DEL},	// 11
	{{211,  1, 29, 11 },90,30,11,13, AK_HELP},	// 12
	{{241,  1, 14, 11 },92,32,12,14, AK_NPLPAREN},	// 13
	{{256,  1, 14, 11 },69,33,13,15, AK_NPRPAREN},	// 14
	{{271,  1, 14, 11 },69,34,14,16, AK_NPDIV},	// 15
	{{286,  1, 14, 11 },69,35,15,0, AK_NPMUL},	// 16
	
	{{  1, 13, 29, 11 }, 0,36,35,18, AK_BACKQUOTE}, // 17, row 2 start 
	{{ 31, 13, 14, 11 }, 1,37,17,19, AK_1},	// 18
	{{ 46, 13, 14, 11 }, 2,38,18,20, AK_2},	// 19
	{{ 61, 13, 14, 11 }, 3,39,19,21, AK_3},	// 20
	{{ 76, 13, 14, 11 }, 4,40,20,22, AK_4},	// 21
	{{ 91, 13, 14, 11 }, 5,41,21,23, AK_5},	// 22
	{{106, 13, 14, 11 }, 6,42,22,24, AK_6},	// 23
	{{121, 13, 14, 11 }, 7,43,23,25, AK_7},	// 24 
	{{136, 13, 14, 11 }, 8,44,24,26, AK_8},	// 25
	{{151, 13, 14, 11 }, 9,45,25,27, AK_9},	// 26
	{{166, 13, 14, 11 },10,46,26,28, AK_0},	// 27
	{{181, 13, 14, 11 },11,47,27,29, AK_MINUS},	// 28
	{{196, 13, 14, 11 },11,48,28,30, AK_EQUAL},	// 29
	{{211, 13, 14, 11 },12,49,29,31, AK_BACKSLASH},	// 30
	{{226, 13, 14, 11 },12,49,30,32, AK_BS},	// 31
	{{241, 13, 14, 11 },13,50,31,33, AK_NP7},	// 32
	{{256, 13, 14, 11 },14,51,32,34, AK_NP8},	// 33
	{{271, 13, 14, 11 },15,52,33,35, AK_NP9},	// 34
	{{286, 13, 14, 11 },16,53,34,17, AK_NPSUB},	// 35
	
	{{  1, 25, 29, 11 }, 17,54,53,37, AK_TAB}, // 36, row 3 start 
	{{ 31, 25, 14, 11 }, 18,55,36,38, AK_Q},	// 37
	{{ 46, 25, 14, 11 }, 19,56,37,39, AK_W},	// 38
	{{ 61, 25, 14, 11 }, 20,57,38,40, AK_E},	// 39
	{{ 76, 25, 14, 11 }, 21,58,39,41, AK_R},	// 40
	{{ 91, 25, 14, 11 }, 22,59,40,42, AK_T},	// 41
	{{106, 25, 14, 11 }, 23,60,41,43, AK_Y},	// 42
	{{121, 25, 14, 11 }, 24,61,42,44, AK_U},	// 43 
	{{136, 25, 14, 11 }, 25,62,43,45, AK_I},	// 44
	{{151, 25, 14, 11 }, 26,63,44,46, AK_O},	// 45
	{{166, 25, 14, 11 }, 27,64,45,47, AK_P},	// 46
	{{181, 25, 14, 11 }, 28,65,46,48, AK_LBRACKET},	// 47
	{{196, 25, 14, 11 }, 29,49,47,49, AK_RBRACKET},	// 48
	{{211, 25, 29, 23 }, 30,81,48,50, AK_RET},	// 49
	{{241, 25, 14, 11 }, 32,66,49,51, AK_NP4},	// 50
	{{256, 25, 14, 11 }, 33,67,50,52, AK_NP5},	// 51
	{{271, 25, 14, 11 }, 34,68,51,53, AK_NP6},	// 52
	{{286, 25, 14, 11 }, 35,69,52,36, AK_NPADD},	// 53
	
	{{  1, 37, 29, 11 }, 36,70,69,55, AK_CTRL}, // 54, row 4 start 
	{{ 31, 37, 14, 11 }, 37,93,54,56, AK_A},	// 55 *
	{{ 46, 37, 14, 11 }, 38,71,55,57, AK_S},	// 56
	{{ 61, 37, 14, 11 }, 39,72,56,58, AK_D},	// 57
	{{ 76, 37, 14, 11 }, 40,73,57,59, AK_F},	// 58
	{{ 91, 37, 14, 11 }, 41,74,58,60, AK_G},	// 59
	{{106, 37, 14, 11 }, 42,75,59,61, AK_H},	// 60
	{{121, 37, 14, 11 }, 43,76,60,62, AK_J},	// 61 
	{{136, 37, 14, 11 }, 44,77,61,63, AK_K},	// 62
	{{151, 37, 14, 11 }, 45,78,62,64, AK_L},	// 63
	{{166, 37, 14, 11 }, 46,79,63,65, AK_SEMICOLON},	// 64
	{{181, 37, 14, 11 }, 47,80,64,49, AK_QUOTE},	// 65
	{{241, 37, 14, 11 }, 50,83,49,67, AK_NP1},	// 66
	{{256, 37, 14, 11 }, 51,83,66,68, AK_NP2},	// 67
	{{271, 37, 14, 11 }, 52,84,67,69, AK_NP3},	// 68
	{{286, 37, 14, 34 }, 53,16,68,54, AK_ENT},	// 69
	
	{{  1, 49, 29, 11 }, 54,85,84,93, AK_LSH}, // 70, row 5 start * 
	{{ 46, 49, 14, 11 }, 56,87,93,72, AK_Z},	// 71 *
	{{ 61, 49, 14, 11 }, 57,87,71,73, AK_X},	// 72
	{{ 76, 49, 14, 11 }, 58,87,72,74, AK_C},	// 73
	{{ 91, 49, 14, 11 }, 59,87,73,75, AK_V},	// 74
	{{106, 49, 14, 11 }, 60,87,74,76, AK_B},	// 75
	{{121, 49, 14, 11 }, 61,87,75,77, AK_N},	// 76 
	{{136, 49, 14, 11 }, 62,87,76,78, AK_M},	// 77
	{{151, 49, 14, 11 }, 63,87,77,79, AK_COMMA},	// 78
	{{166, 49, 14, 11 }, 64,87,78,80, AK_PERIOD},	// 79
	{{181, 49, 14, 11 }, 65,88,79,81, AK_SLASH},	// 80
	{{196, 49, 29, 11 }, 49,89,80,82, AK_RSH},	// 81
	{{226, 49, 14, 11 }, 49,91,81,83, AK_UP},	// 82
	{{241, 49, 29, 11 }, 66,92,82,84, AK_NP0},	// 83
	{{271, 49, 14, 11 }, 67,69,83,69, AK_NPDEL},	// 84
	
	{{  1, 61, 29, 11 }, 70,0,69,86, AK_LALT}, // 85, row 6 start 
	{{ 31, 61, 14, 11 }, 93,1,85,87, AK_LAMI},	// 86 *
	{{ 46, 61,134, 11 }, 71,2,86,88, AK_SPC},	// 87
	{{181, 61, 14, 11 }, 80,11,87,89, AK_RAMI},	// 88
	{{196, 61, 14, 11 }, 81,11,88,90, AK_RALT},	// 89
	{{211, 61, 14, 11 }, 81,12,89,91, AK_LF},	// 90
	{{226, 61, 14, 11 }, 82,12,90,92, AK_DN},	// 91 
	{{241, 61, 14, 11 }, 83,13,91,69, AK_RT},	// 92
	// UK extra keys
	{{31, 49, 14, 11 }, 55,86,70,71, AK_LTGT},	// 93	*
};

//GERMAN KEYBOARD
static t_vkbd_rect vkbd_rect_GER[]= 
{
	{{  1,  1, 29, 11 },85,17,16, 1, AK_ESC}, // 0, row 1 start 
	{{ 31,  1, 14, 11 },86,18, 0, 2, AK_F1},	// 1
	{{ 46,  1, 14, 11 },87,19, 1, 3, AK_F2},	// 2
	{{ 61,  1, 14, 11 },87,20, 2, 4, AK_F3},	// 3
	{{ 76,  1, 14, 11 },87,21, 3, 5, AK_F4},	// 4
	{{ 91,  1, 14, 11 },87,22, 4, 6, AK_F5},	// 5 
	{{106,  1, 14, 11 },87,23, 5, 7, AK_F6},	// 6
	{{121,  1, 14, 11 },87,24, 6, 8, AK_F7},	// 7 
	{{136,  1, 14, 11 },87,25, 7, 9, AK_F8},	// 8 
	{{151,  1, 14, 11 },87,26, 8,10, AK_F9},	// 9
	{{166,  1, 14, 11 },87,27, 9,11, AK_F10},	// 10
	{{181,  1, 29, 11 },88,28,10,12, AK_DEL},	// 11
	{{211,  1, 29, 11 },90,30,11,13, AK_HELP},	// 12
	{{241,  1, 14, 11 },92,32,12,14, AK_NPLPAREN},	// 13
	{{256,  1, 14, 11 },69,33,13,15, AK_NPRPAREN},	// 14
	{{271,  1, 14, 11 },69,34,14,16, AK_NPDIV},	// 15
	{{286,  1, 14, 11 },69,35,15,0, AK_NPMUL},	// 16
	
	{{  1, 13, 29, 11 }, 0,36,35,18, AK_BACKQUOTE}, // 17, row 2 start 
	{{ 31, 13, 14, 11 }, 1,37,17,19, AK_1},	// 18
	{{ 46, 13, 14, 11 }, 2,38,18,20, AK_2},	// 19
	{{ 61, 13, 14, 11 }, 3,39,19,21, AK_3},	// 20
	{{ 76, 13, 14, 11 }, 4,40,20,22, AK_4},	// 21
	{{ 91, 13, 14, 11 }, 5,41,21,23, AK_5},	// 22
	{{106, 13, 14, 11 }, 6,42,22,24, AK_6},	// 23
	{{121, 13, 14, 11 }, 7,43,23,25, AK_7},	// 24 
	{{136, 13, 14, 11 }, 8,44,24,26, AK_8},	// 25
	{{151, 13, 14, 11 }, 9,45,25,27, AK_9},	// 26
	{{166, 13, 14, 11 },10,46,26,28, AK_0},	// 27
	{{181, 13, 14, 11 },11,47,27,29, AK_MINUS},	// 28
	{{196, 13, 14, 11 },11,48,28,30, AK_EQUAL},	// 29
	{{211, 13, 14, 11 },12,49,29,31, AK_BACKSLASH},	// 30
	{{226, 13, 14, 11 },12,49,30,32, AK_BS},	// 31
	{{241, 13, 14, 11 },13,50,31,33, AK_NP7},	// 32
	{{256, 13, 14, 11 },14,51,32,34, AK_NP8},	// 33
	{{271, 13, 14, 11 },15,52,33,35, AK_NP9},	// 34
	{{286, 13, 14, 11 },16,53,34,17, AK_NPSUB},	// 35
	
	{{  1, 25, 29, 11 }, 17,54,53,37, AK_TAB}, // 36, row 3 start 
	{{ 31, 25, 14, 11 }, 18,55,36,38, AK_Q},	// 37
	{{ 46, 25, 14, 11 }, 19,56,37,39, AK_W},	// 38
	{{ 61, 25, 14, 11 }, 20,57,38,40, AK_E},	// 39
	{{ 76, 25, 14, 11 }, 21,58,39,41, AK_R},	// 40
	{{ 91, 25, 14, 11 }, 22,59,40,42, AK_T},	// 41
	{{106, 25, 14, 11 }, 23,60,41,43, AK_Y},	// 42
	{{121, 25, 14, 11 }, 24,61,42,44, AK_U},	// 43 
	{{136, 25, 14, 11 }, 25,62,43,45, AK_I},	// 44
	{{151, 25, 14, 11 }, 26,63,44,46, AK_O},	// 45
	{{166, 25, 14, 11 }, 27,64,45,47, AK_P},	// 46
	{{181, 25, 14, 11 }, 28,65,46,48, AK_LBRACKET},	// 47
	{{196, 25, 14, 11 }, 29,94,47,49, AK_RBRACKET},	// 48 *
	{{211, 25, 29, 23 }, 30,81,48,50, AK_RET},	// 49
	{{241, 25, 14, 11 }, 32,66,49,51, AK_NP4},	// 50
	{{256, 25, 14, 11 }, 33,67,50,52, AK_NP5},	// 51
	{{271, 25, 14, 11 }, 34,68,51,53, AK_NP6},	// 52
	{{286, 25, 14, 11 }, 35,69,52,36, AK_NPADD},	// 53
	
	{{  1, 37, 29, 11 }, 36,70,69,55, AK_CTRL}, // 54, row 4 start 
	{{ 31, 37, 14, 11 }, 37,93,54,56, AK_A},	// 55 *
	{{ 46, 37, 14, 11 }, 38,71,55,57, AK_S},	// 56
	{{ 61, 37, 14, 11 }, 39,72,56,58, AK_D},	// 57
	{{ 76, 37, 14, 11 }, 40,73,57,59, AK_F},	// 58
	{{ 91, 37, 14, 11 }, 41,74,58,60, AK_G},	// 59
	{{106, 37, 14, 11 }, 42,75,59,61, AK_H},	// 60
	{{121, 37, 14, 11 }, 43,76,60,62, AK_J},	// 61 
	{{136, 37, 14, 11 }, 44,77,61,63, AK_K},	// 62
	{{151, 37, 14, 11 }, 45,78,62,64, AK_L},	// 63
	{{166, 37, 14, 11 }, 46,79,63,65, AK_SEMICOLON},	// 64
	{{181, 37, 14, 11 }, 47,80,64,94, AK_QUOTE},	// 65 *
	{{241, 37, 14, 11 }, 50,83,49,67, AK_NP1},	// 66
	{{256, 37, 14, 11 }, 51,83,66,68, AK_NP2},	// 67
	{{271, 37, 14, 11 }, 52,84,67,69, AK_NP3},	// 68
	{{286, 37, 14, 34 }, 53,16,68,54, AK_ENT},	// 69
	
	{{  1, 49, 29, 11 }, 54,85,84,93, AK_LSH}, // 70, row 5 start * 
	{{ 46, 49, 14, 11 }, 56,87,93,72, AK_Z},	// 71 *
	{{ 61, 49, 14, 11 }, 57,87,71,73, AK_X},	// 72
	{{ 76, 49, 14, 11 }, 58,87,72,74, AK_C},	// 73
	{{ 91, 49, 14, 11 }, 59,87,73,75, AK_V},	// 74
	{{106, 49, 14, 11 }, 60,87,74,76, AK_B},	// 75
	{{121, 49, 14, 11 }, 61,87,75,77, AK_N},	// 76 
	{{136, 49, 14, 11 }, 62,87,76,78, AK_M},	// 77
	{{151, 49, 14, 11 }, 63,87,77,79, AK_COMMA},	// 78
	{{166, 49, 14, 11 }, 64,87,78,80, AK_PERIOD},	// 79
	{{181, 49, 14, 11 }, 65,88,79,81, AK_SLASH},	// 80
	{{196, 49, 27, 11 }, 49,89,80,82, AK_RSH},	// 81
	{{226, 49, 14, 11 }, 49,91,81,83, AK_UP},	// 82
	{{241, 49, 29, 11 }, 66,92,82,84, AK_NP0},	// 83
	{{271, 49, 14, 11 }, 67,69,83,69, AK_NPDEL},	// 84
	
	{{  1, 61, 29, 11 }, 70,0,69,86, AK_LALT}, // 85, row 6 start 
	{{ 31, 61, 14, 11 }, 93,1,85,87, AK_LAMI},	// 86 *
	{{ 46, 61,134, 11 }, 71,2,86,88, AK_SPC},	// 87
	{{181, 61, 14, 11 }, 80,11,87,89, AK_RAMI},	// 88
	{{196, 61, 14, 11 }, 81,11,88,90, AK_RALT},	// 89
	{{211, 61, 14, 11 }, 81,12,89,91, AK_LF},	// 90
	{{226, 61, 14, 11 }, 82,12,90,92, AK_DN},	// 91 
	{{241, 61, 14, 11 }, 83,13,91,69, AK_RT},	// 92	
	//German extra keys
	{{31, 49, 14, 11 }, 55,86,70,71, AK_LTGT},	// 93	*
	{{196, 37, 14, 11 }, 48,81,65,49, AK_NUMBERSIGN}, // 94 *	
};
#endif

void vkbd_init_button2(void)
{
	vkbd_button2=(SDLKey)0;
}

void vkbd_reset_sticky_keys(void) 
{
	for (int i=0; i<NUM_STICKY; i++)
	{
		vkbd_sticky_key[i].can_switch=true;
		vkbd_sticky_key[i].stuck=false;
	}
}

int vkbd_init(void)
{
	int i;
	char tmpchar[256];
	char tmpchar2[256];
	char vkbdFileName[256];
	char vkbdHiresFileName[256];
#ifdef LARGEKEYBOARD
	char vkbdShiftFileName[256];
	char vkbdShiftHiresFileName[256];
	switch (mainMenu_vkbdLanguage)
	{
		case 1:
			snprintf(vkbdFileName, 256, "vkbdUKLarge.bmp");
			snprintf(vkbdHiresFileName, 256, "vkbdUKLargeHires.bmp");
			snprintf(vkbdShiftFileName, 256, "vkbdUKLargeShift.bmp");
			snprintf(vkbdShiftHiresFileName, 256, "vkbdUKLargeShiftHires.bmp");
			vkbd_rect=vkbd_rect_UK;
			break;
		case 2:
			snprintf(vkbdFileName, 256, "vkbdGERLarge.bmp");
			snprintf(vkbdHiresFileName, 256, "vkbdGERLargeHires.bmp");
			snprintf(vkbdShiftFileName, 256, "vkbdGERLargeShift.bmp");
			snprintf(vkbdShiftHiresFileName, 256, "vkbdGERLargeShiftHires.bmp");
			vkbd_rect=vkbd_rect_GER;
			break;
		default:
			snprintf(vkbdFileName, 256, "vkbdUSLarge.bmp");
			snprintf(vkbdHiresFileName, 256, "vkbdUSLargeHires.bmp");
			snprintf(vkbdShiftFileName, 256, "vkbdUSLargeShift.bmp");
			snprintf(vkbdShiftHiresFileName, 256, "vkbdUSLargeShiftHires.bmp");
			vkbd_rect=vkbd_rect_US;
			break;
	}
#else
	snprintf(vkbdFileName, 256, "vkbd.bmp");
	snprintf(vkbdFileName, 256, "vkbdHires.bmp");
	vkbd_rect=vkbd_rect_Small;	
#endif

#if defined(__PSP2__) || defined(__SWITCH__)
	snprintf(tmpchar, 256, "%s%s", DATA_PREFIX, vkbdFileName);
	snprintf(tmpchar2, 256, "%s%s", DATA_PREFIX, vkbdHiresFileName);
#else
#ifdef GP2X
	snprintf(tmpchar, 256, "%s/data/%s", launchDir, vkbdFileName);
	snprintf(tmpchar2, 256, "%s/data/%s", launchDir, vkbdHiresFileName);
#else
#ifdef GIZMONDO
	snprintf(tmpchar, 256, "%s", "\\SD Card\\uae4all\\data\\%s",vkbdFileName);
	snprintf(tmpchar2, 256, "%s", "\\SD Card\\uae4all\\data\\%s",vkbdHiresFileName);
#else
	snprintf(tmpchar, 256, "%s%s", DATA_PREFIX, vkbdFileName);
	snprintf(tmpchar2, 256, "%s%s", DATA_PREFIX, vkbdHiresFileName);
#endif
#endif
#endif //__PSP2__

	SDL_Surface *tmp = SDL_LoadBMP(tmpchar);

	if (tmp==NULL)
	{
		printf("Virtual Keyboard Bitmap Error: %s\n",SDL_GetError());
		return -1;
	}
	ksur=SDL_DisplayFormat(tmp);
	SDL_FreeSurface(tmp);

	tmp = SDL_LoadBMP(tmpchar2);
	
	if (tmp==NULL)
	{
		printf("Virtual Keyboard Bitmap Error: %s\n",SDL_GetError());
		return -1;
	}
	ksurHires=SDL_DisplayFormat(tmp);
	SDL_FreeSurface(tmp);

	// intermediate surfaces to highlight sticky keys on
	canvas=SDL_DisplayFormat(ksur);
	canvasHires=SDL_DisplayFormat(ksurHires);

//for large keyboard, load another image for shifted keys, and set transparency
#ifdef LARGEKEYBOARD 
#if defined(__PSP2__) || defined(__SWITCH__)
	snprintf(tmpchar, 256, "%s%s", DATA_PREFIX, vkbdShiftFileName);
	snprintf(tmpchar2, 256, "%s%s", DATA_PREFIX, vkbdShiftHiresFileName);
#else
#ifdef GP2X
	snprintf(tmpchar, 256, "%s/data/%s", launchDir, vkbdShiftFileName);
	snprintf(tmpchar2, 256, "%s/data/%s", launchDir, vkbdShiftHiresFileName);
#else
#ifdef GIZMONDO
	snprintf(tmpchar, 256, "%s", "\\SD Card\\uae4all\\data\\%s",vkbdShiftFileName);
	snprintf(tmpchar2, 256, "%s", "\\SD Card\\uae4all\\data\\%s",vkbdShiftHiresFileName);
#else
	snprintf(tmpchar, 256, "%s%s", DATA_PREFIX, vkbdShiftFileName);
	snprintf(tmpchar2, 256, "%s%s", DATA_PREFIX, vkbdShiftHiresFileName);
#endif
#endif
#endif //__PSP2__
	
	tmp = SDL_LoadBMP(tmpchar);

	if (tmp==NULL)
	{
		printf("Virtual Keyboard Bitmap Error: %s\n",SDL_GetError());
		return -1;
	}
	ksurShift=SDL_DisplayFormat(tmp);
	SDL_FreeSurface(tmp);
	
	tmp = SDL_LoadBMP(tmpchar2);

	if (tmp==NULL)
	{
		printf("Virtual Keyboard Bitmap Error: %s\n",SDL_GetError());
		return -1;
	}
	ksurShiftHires=SDL_DisplayFormat(tmp);
	SDL_FreeSurface(tmp);

	vkbd_transparency=128; //default transparency is 128 for LARGEKEYBOARD
	SDL_SetAlpha(canvas, SDL_SRCALPHA | SDL_RLEACCEL, vkbd_transparency);
	SDL_SetAlpha(canvasHires, SDL_SRCALPHA | SDL_RLEACCEL, vkbd_transparency);
#else //LARGEKEYBOARD
//if using the small keyboard, load in the zoomed keys instead
	for(i=0;i<MAX_KEY_SMALL;i++)
		vkey[i]=NULL;
	for(i=0;i<MAX_KEY_SMALL;i++)
	{
#if defined(__PSP2__) // NOT __SWITCH__
		snprintf(tmpchar, 256, "app0:/data/key%i.bmp", i);
#else
#ifdef GP2X
		snprintf(tmpchar, 256, "%s/data/key%i.bmp", launchDir, i);
#else
#ifdef GIZMONDO
		snprintf(tmpchar, 256, "\\SD Card\\uae4all\\data\\key%i.bmp", i);
#else
		snprintf(tmpchar, 256, DATA_PREFIX "key%i.bmp", i);
#endif
#endif
#endif //__PSP2__

		tmp=SDL_LoadBMP(tmpchar);
		if (tmp==NULL)
			break;
		vkey[i]=SDL_DisplayFormat(tmp);
		SDL_FreeSurface(tmp);
	}
#endif // LARGEKEYBOARD

	vkbd_actual=0;
#if !defined(__PSP2__) && !defined(__SWITCH__) //no need to show keyboard on first startup
	vkbd_redraw();
#endif
	for (int i=0; i<NUM_STICKY; i++)
	{
		vkbd_sticky_key[i].stuck=false;
	}
#ifdef LARGEKEYBOARD
	vkbd_x=(prSDLScreen->w-ksur->w)/2;
	vkbd_y=prSDLScreen->h-ksur->h;
#else
	vkbd_x=VKBD_X;
	vkbd_y=(prSDLScreen->h-ksur->h);
#endif
	vkbd_mode=0;
	vkbd_move=0;
	vkbd_last_press_time=0;
	vkbd_last_move_time=0;
	vkbd_key=KEYCODE_NOTHING;
	vkbd_button2=(SDLKey)0;
	vkbd_keysave=KEYCODE_NOTHING;
	return 0;
}

void vkbd_quit(void)
{
	int i;
#ifdef LARGEKEYBOARD
	SDL_FreeSurface(ksurShift);
	SDL_FreeSurface(ksurShiftHires);
#else
	for(i=0;i<MAX_KEY_SMALL;i++)
		SDL_FreeSurface(vkey[i]);
#endif
	SDL_FreeSurface(ksur);
	SDL_FreeSurface(ksurHires);
	vkbd_mode=0;
	for (int i=0; i<NUM_STICKY; i++)
	{
		vkbd_sticky_key[i].stuck=false;
	}
}

void vkbd_redraw(void)
{
	SDL_Rect r;
	SDL_Surface *toDraw;
	SDL_Surface *myCanvas;
	if (mainMenu_displayHires)
	{
#ifdef LARGEKEYBOARD
		if (vkbd_sticky_key[0].stuck || vkbd_sticky_key[1].stuck)			
			toDraw=ksurShiftHires;
		else
#endif
			toDraw=ksurHires;
			myCanvas=canvasHires;
	}
	else
	{
#ifdef LARGEKEYBOARD
		if (vkbd_sticky_key[0].stuck || vkbd_sticky_key[1].stuck)
			toDraw=ksurShift;
		else
#endif
			toDraw=ksur;
			myCanvas=canvas;
	}

	// blit onto intermediate canvas
	r.x=0;
	r.y=0;
	r.w=toDraw->w;
	r.h=toDraw->h;
	SDL_BlitSurface(toDraw,NULL,myCanvas,&r);

#ifdef LARGEKEYBOARD
	// highlight sticky keys that are pressed with a green dot
	// do this on a canvas to ensure correct transparency on final blit
	Uint32 sticky_key_color=SDL_MapRGB(myCanvas->format, 0, 255, 0);
	for (int i=0; i<NUM_STICKY; i++) {
		if (vkbd_sticky_key[i].stuck==true) {
			int index = vkbd_sticky_key[i].index;
			if (mainMenu_displayHires)
			{
				r.x=2*vkbd_rect[index].rect.x+2;
				r.w=6;
			}
			else
			{
				r.x=vkbd_rect[index].rect.x+1;
				r.w=3;
			}
			r.y=vkbd_rect[index].rect.y+1;
			r.h=3;
			SDL_FillRect(myCanvas,&r,sticky_key_color);
		}
	}
#endif

	if (vkbd_y>prSDLScreen->h-myCanvas->h) 
		vkbd_y=prSDLScreen->h-myCanvas->h;
		
	vkbd_x=(prSDLScreen->w-myCanvas->w)/2;
	
	r.x=vkbd_x;	
	r.y=vkbd_y;	
	r.w=myCanvas->w;
	r.h=myCanvas->h;

	SDL_BlitSurface(myCanvas,NULL,prSDLScreen,&r);
}

void vkbd_transparency_up(void)
{
	switch (vkbd_transparency) 
	{
		case 255:
			vkbd_transparency=192;
			break;
		case 192:
			vkbd_transparency=128;
			break;
		case 128: 
			vkbd_transparency=64;
			break;
		case 64: 
			vkbd_transparency=255;
			break;
		default:
			vkbd_transparency=64;
			break;
	}		
	if (vkbd_transparency != 255)
	{
		SDL_SetAlpha(canvas, SDL_SRCALPHA | SDL_RLEACCEL, vkbd_transparency);		
		SDL_SetAlpha(canvasHires, SDL_SRCALPHA | SDL_RLEACCEL, vkbd_transparency);		
	}
	else //fully opague
	{
	 	SDL_SetAlpha(canvas, 0, 255);
	 	SDL_SetAlpha(canvasHires, 0, 255);
	}
}	

void vkbd_transparency_down(void)
{
	switch (vkbd_transparency) 
	{
		case 255:
			vkbd_transparency=64;
			break;
		case 192:
			vkbd_transparency=255;
			break;
		case 128: 
			vkbd_transparency=192;
			break;
	 	case 64:
			vkbd_transparency=128;
			break;
		default:
			vkbd_transparency=255;
			break;
	}		
	if (vkbd_transparency != 255)
	{
		SDL_SetAlpha(canvas, SDL_SRCALPHA | SDL_RLEACCEL, vkbd_transparency);		
		SDL_SetAlpha(canvasHires, SDL_SRCALPHA | SDL_RLEACCEL, vkbd_transparency);		
	}
	else //fully opague
	{
	 	SDL_SetAlpha(canvas, 0, 255);
	 	SDL_SetAlpha(canvasHires, 0, 255);				
	}
}	


void vkbd_displace_up(void)
{
	if (vkbd_y>3)
		vkbd_y-=4;
	else
		vkbd_y=0;
}

void vkbd_displace_down(void)
{
	if (vkbd_y<prSDLScreen->h-ksur->h-3)
		vkbd_y+=4;
	else
		vkbd_y=prSDLScreen->h-ksur->h;
}		

int vkbd_process(void)
{
	Uint32 now=SDL_GetTicks();
	SDL_Rect r;
	
#ifndef VKBD_ALWAYS
	if (vkbd_move)
#endif
	vkbd_redraw();
	if (vkbd_move&VKBD_BUTTON)
	{
		vkbd_move=0;
#ifndef LARGEKEYBOARD //the old small keyboard struct contains SDL codes
		SDL_keysym ks;
		ks.sym=vkbd_rect[vkbd_actual].key;
		int amigaKeyCode=keycode2amiga(&ks);
#else //the large keyboard struct contains Amiga key codes
		int amigaKeyCode=vkbd_rect[vkbd_actual].key;
#endif
		//some keys are sticky
		for (int i=0; i<NUM_STICKY; i++) 
		{
			if (amigaKeyCode == vkbd_sticky_key[i].code) 
			{
				if (vkbd_sticky_key[i].can_switch) 
				{
					vkbd_sticky_key[i].stuck=!vkbd_sticky_key[i].stuck;
					vkbd_sticky_key[i].can_switch=false;
					return amigaKeyCode;
				}
				else return (KEYCODE_NOTHING);
			}
		}
		return amigaKeyCode;
	}
	
	if (vkbd_move&VKBD_BUTTON_BACKSPACE)
	{
		vkbd_move=0;
		return AK_BS;
	}
	if (vkbd_move&VKBD_BUTTON_SHIFT)
	{
		vkbd_move=0;
		// hotkey for shift toggle (arbitrarily use left shift here)
		if (vkbd_sticky_key[0].can_switch)
		{
			vkbd_sticky_key[0].stuck=!vkbd_sticky_key[0].stuck;
			vkbd_sticky_key[0].can_switch=false;
			return(AK_LSH);
		} else
			return(KEYCODE_NOTHING);
	}
	if (vkbd_move&VKBD_BUTTON_RESET_STICKY)
	{
		vkbd_move=0;
		// hotkey to reset all sticky keys at once
		vkbd_reset_sticky_keys();
		return(KEYCODE_STICKY_RESET); // special return code to reset amiga keystate
	}
	if (vkbd_move&VKBD_LEFT || vkbd_move&VKBD_RIGHT || vkbd_move&VKBD_UP || vkbd_move&VKBD_DOWN) 
	{
		if (vkbd_let_go_of_direction) //just pressing down
			vkbd_last_press_time=now;
		if (
				(
				now-vkbd_last_press_time>VKBD_MIN_HOLDING_TIME 
				&& now-vkbd_last_move_time>VKBD_MOVE_DELAY
				) 
				|| vkbd_let_go_of_direction
			) 
		{
			vkbd_last_move_time=now;
			if (vkbd_move&VKBD_LEFT)
				vkbd_actual=vkbd_rect[vkbd_actual].left;
			else if (vkbd_move&VKBD_RIGHT)
				vkbd_actual=vkbd_rect[vkbd_actual].right;
			if (vkbd_move&VKBD_UP)
				vkbd_actual=vkbd_rect[vkbd_actual].up;
			else if (vkbd_move&VKBD_DOWN)
				vkbd_actual=vkbd_rect[vkbd_actual].down;
		}
		vkbd_let_go_of_direction=0;
	}
	else
		vkbd_let_go_of_direction=1;
		
	if (mainMenu_displayHires)
	{
		r.x=vkbd_x+2*vkbd_rect[vkbd_actual].rect.x;
		r.w=2*vkbd_rect[vkbd_actual].rect.w;
	}
	else
	{
		r.x=vkbd_x+vkbd_rect[vkbd_actual].rect.x;
		r.w=vkbd_rect[vkbd_actual].rect.w;
	}
	r.y=vkbd_y+vkbd_rect[vkbd_actual].rect.y;
	r.h=vkbd_rect[vkbd_actual].rect.h;
	if (!vkbd_just_blinked)
	{	
		SDL_FillRect(prSDLScreen,&r,vkbd_color);
		vkbd_just_blinked=1;
		//vkbd_color = ~vkbd_color;
	}
	else
	{
		vkbd_just_blinked=0;
	}
//Draw the zoomed in key preview, only do this when using the small keyboard
#ifndef LARGEKEYBOARD
#ifndef VKBD_ALWAYS
	if (vkbd_move && vkey[vkbd_actual]!=NULL)
	{
#endif
		if (mainMenu_displayHires)
			r.x=VKBD_X+ksurHires->w+2;
		else
			r.x=VKBD_X+ksur->w+2;
		r.y=prSDLScreen->h-40+2;
		r.w=vkey[vkbd_actual]->w;
		r.h=vkey[vkbd_actual]->h;
		SDL_BlitSurface(vkey[vkbd_actual],NULL,prSDLScreen,&r);
#ifndef VKBD_ALWAYS
	}
#endif
#endif //!LARGEKEYBOARD
	return KEYCODE_NOTHING; //nothing on the vkbd was pressed
}
#endif
