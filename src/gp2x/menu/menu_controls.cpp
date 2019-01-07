#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sysconfig.h"
#include "sysdeps.h"
#include "config.h"
#include "menu.h"

#include <sys/stat.h>
#include <unistd.h>
#if defined(__PSP2__) // NOT __SWITCH__
#include "psp2-dirent.h"
#else
#include <dirent.h>
#endif

#include "uae.h"
#include "options.h"
#include "sound.h"
#include <SDL.h>
#include "gp2x.h"
#include <SDL_ttf.h>
#include "menu_config.h"

#if defined(__PSP2__) || defined(__SWITCH__)
#define SDL_PollEvent PSP2_PollEvent
#endif

#define MAX_CUSTOM_ID 96
#define MIN_CUSTOM_ID -28

const char *text_str_controls_separator="---------------------------------------";
const char *text_str_controls_title=    "         Custom Controls              -";
char tmpchar[256];
char mapping[32]="";
int menuControls = 0;
static int editingStick = 0;
extern int quit_pressed_in_submenu;
extern int emulating;
extern int nr_joysticks;
extern void close_joystick();
extern void init_joystick();

enum { 
	MENUCONTROLS_RETURNMAIN = 0,
#ifdef ANDROIDSDL
  MENUCONTROLS_ONSCREEN,
#endif
  MENUCONTROLS_CUSTOM_ON_OFF,
#if defined(__PSP2__) || defined(__SWITCH__)
  MENUCONTROLS_CUSTOM_CONTROLLER_NR,
  MENUCONTROLS_CUSTOMSET,
#else
  MENUCONTROLS_DPAD,
#endif
  MENUCONTROLS_STICK,
  MENUCONTROLS_UP,
  MENUCONTROLS_DOWN,
  MENUCONTROLS_LEFT,
  MENUCONTROLS_RIGHT,
  MENUCONTROLS_A,
  MENUCONTROLS_Y,
  MENUCONTROLS_X,
  MENUCONTROLS_B,
  MENUCONTROLS_L,
  MENUCONTROLS_R,
#ifdef __SWITCH__
  MENUCONTROLS_L2,
  MENUCONTROLS_R2,
#endif
	MENUCONTROLS_END
};

// remember to also edit MIN_CUSTOM_ID and MAX_CUSTOM_ID
static void getMapping(int customId)
{
	switch(customId)
	{
		case -28: strcpy(mapping, "Quick Load"); break;
		case -27: strcpy(mapping, "Quick Save"); break;
		case -26: strcpy(mapping, "Speed Up Mouse"); break;
		case -25: strcpy(mapping, "Slow Down Mouse"); break;
		case -24: strcpy(mapping, "4th Joystick RIGHT"); break;
		case -23: strcpy(mapping, "4th Joystick LEFT"); break;
		case -22: strcpy(mapping, "4th Joystick DOWN"); break;
		case -21: strcpy(mapping, "4th Joystick UP"); break;
		case -20: strcpy(mapping, "4th Joystick Fire 2"); break;
		case -19: strcpy(mapping, "4th Joystick Fire 1"); break;
		case -18: strcpy(mapping, "3rd Joystick RIGHT"); break;
		case -17: strcpy(mapping, "3rd Joystick LEFT"); break;
		case -16: strcpy(mapping, "3rd Joystick DOWN"); break;
		case -15: strcpy(mapping, "3rd Joystick UP"); break;
		case -14: strcpy(mapping, "3rd Joystick Fire 2"); break;
		case -13: strcpy(mapping, "3rd Joystick Fire 1"); break;
		case -12: strcpy(mapping, "2nd Joystick RIGHT"); break;
		case -11: strcpy(mapping, "2nd Joystick LEFT"); break;
		case -10: strcpy(mapping, "2nd Joystick DOWN"); break;
		case -9: strcpy(mapping, "2nd Joystick UP"); break;
		case -8: strcpy(mapping, "Joystick RIGHT"); break;
		case -7: strcpy(mapping, "Joystick LEFT"); break;
		case -6: strcpy(mapping, "Joystick DOWN"); break;
		case -5: strcpy(mapping, "Joystick UP (Jump)"); break;
		case -4: strcpy(mapping, "Joystick Fire Button 2"); break;
		case -3: strcpy(mapping, "Joystick Fire Button 1"); break;
		case -2: strcpy(mapping, "Mouse Right Btn (Joy2 Btn2)"); break;
		case -1: strcpy(mapping, "Mouse Left Btn (Joy2 Btn1)"); break;
		case 0: strcpy(mapping, "---"); break;
		case 1: strcpy(mapping, "arrow UP"); break;
		case 2: strcpy(mapping, "arrow DOWN"); break;
		case 3: strcpy(mapping, "arrow LEFT"); break;
		case 4: strcpy(mapping, "arrow RIGHT"); break;
		case 5: strcpy(mapping, "numpad 0"); break;
		case 6: strcpy(mapping, "numpad 1"); break;
		case 7: strcpy(mapping, "numpad 2"); break;
		case 8: strcpy(mapping, "numpad 3"); break;
		case 9: strcpy(mapping, "numpad 4"); break;
		case 10: strcpy(mapping, "numpad 5"); break;
		case 11: strcpy(mapping, "numpad 6"); break;
		case 12: strcpy(mapping, "numpad 7"); break;
		case 13: strcpy(mapping, "numpad 8"); break;
		case 14: strcpy(mapping, "numpad 9"); break;
		case 15: strcpy(mapping, "numpad ENTER"); break;
		case 16: strcpy(mapping, "numpad DIVIDE"); break;
		case 17: strcpy(mapping, "numpad MULTIPLY"); break;
		case 18: strcpy(mapping, "numpad MINUS"); break;
		case 19: strcpy(mapping, "numpad PLUS"); break;
		case 20: strcpy(mapping, "numpad DELETE"); break;
		case 21: strcpy(mapping, "numpad LEFT PARENTHESIS"); break;
		case 22: strcpy(mapping, "numpad RIGHT PARENTHESIS"); break;
		case 23: strcpy(mapping, "SPACE"); break;
		case 24: strcpy(mapping, "BACKSPACE"); break;
		case 25: strcpy(mapping, "TAB"); break;
		case 26: strcpy(mapping, "RETURN"); break;
		case 27: strcpy(mapping, "ESCAPE"); break;
		case 28: strcpy(mapping, "DELETE"); break;
		case 29: strcpy(mapping, "left SHIFT"); break;
		case 30: strcpy(mapping, "right SHIFT"); break;
		case 31: strcpy(mapping, "CAPS LOCK"); break;
		case 32: strcpy(mapping, "CTRL"); break;
		case 33: strcpy(mapping, "left ALT"); break;
		case 34: strcpy(mapping, "right ALT"); break;
		case 35: strcpy(mapping, "left AMIGA key"); break;
		case 36: strcpy(mapping, "right AMIGA key"); break;
		case 37: strcpy(mapping, "HELP"); break;
		case 38: strcpy(mapping, "left bracket"); break;
		case 39: strcpy(mapping, "right bracket"); break;
		case 40: strcpy(mapping, "semicolon"); break;
		case 41: strcpy(mapping, "comma"); break;
		case 42: strcpy(mapping, "period"); break;
		case 43: strcpy(mapping, "slash"); break;
		case 44: strcpy(mapping, "backslash"); break;
		case 45: strcpy(mapping, "quote"); break;
		case 46: strcpy(mapping, "numbersign"); break;
		case 47: strcpy(mapping, "less than - greater than"); break;
		case 48: strcpy(mapping, "backquote"); break;
		case 49: strcpy(mapping, "minus"); break;
		case 50: strcpy(mapping, "equal"); break;
		case 51: strcpy(mapping, "A"); break;
		case 52: strcpy(mapping, "B"); break;
		case 53: strcpy(mapping, "C"); break;
		case 54: strcpy(mapping, "D"); break;
		case 55: strcpy(mapping, "E"); break;
		case 56: strcpy(mapping, "F"); break;
		case 57: strcpy(mapping, "G"); break;
		case 58: strcpy(mapping, "H"); break;
		case 59: strcpy(mapping, "I"); break;
		case 60: strcpy(mapping, "J"); break;
		case 61: strcpy(mapping, "K"); break;
		case 62: strcpy(mapping, "L"); break;
		case 63: strcpy(mapping, "M"); break;
		case 64: strcpy(mapping, "N"); break;
		case 65: strcpy(mapping, "O"); break;
		case 66: strcpy(mapping, "P"); break;
		case 67: strcpy(mapping, "Q"); break;
		case 68: strcpy(mapping, "R"); break;
		case 69: strcpy(mapping, "S"); break;
		case 70: strcpy(mapping, "T"); break;
		case 71: strcpy(mapping, "U"); break;
		case 72: strcpy(mapping, "V"); break;
		case 73: strcpy(mapping, "W"); break;
		case 74: strcpy(mapping, "X"); break;
		case 75: strcpy(mapping, "Y"); break;
		case 76: strcpy(mapping, "Z"); break;
		case 77: strcpy(mapping, "1"); break;
		case 78: strcpy(mapping, "2"); break;
		case 79: strcpy(mapping, "3"); break;
		case 80: strcpy(mapping, "4"); break;
		case 81: strcpy(mapping, "5"); break;
		case 82: strcpy(mapping, "6"); break;
		case 83: strcpy(mapping, "7"); break;
		case 84: strcpy(mapping, "8"); break;
		case 85: strcpy(mapping, "9"); break;
		case 86: strcpy(mapping, "0"); break;
		case 87: strcpy(mapping, "F1"); break;
		case 88: strcpy(mapping, "F2"); break;
		case 89: strcpy(mapping, "F3"); break;
		case 90: strcpy(mapping, "F4"); break;
		case 91: strcpy(mapping, "F5"); break;
		case 92: strcpy(mapping, "F6"); break;
		case 93: strcpy(mapping, "F7"); break;
		case 94: strcpy(mapping, "F8"); break;
		case 95: strcpy(mapping, "F9"); break;
		case 96: strcpy(mapping, "F10");
	}
	  /*
	  -28 quick load savestate
	  -27 quick save savestate
	  -26 fast mouse modifier
	  -25 slow mouse modifier
	  -12 2nd joy right
	  -11 2nd joy left
	  -10 2nd joy down
     -9 2nd joy up
	  -8 joy right
	  -7 joy left
	  -6 joy down
    -5 joy up
		-4 joy fire button 2
		-3 joy fire button 1
		-2 mouse right button
		-1 mouse left button
		 0 ---
		 1 AK_UP 0x4C
		 2 AK_DN 0x4D
		 3 AK_LF 0x4F
		 4 AK_RT 0x4E
		 5 AK_NP0 0x0F
		 6 AK_NP1 0x1D
		 7 AK_NP2 0x1E
		 8 AK_NP3 0x1F
		 9 AK_NP4 0x2D
		10 AK_NP5 0x2E
		11 AK_NP6 0x2F
		12 AK_NP7 0x3D
		13 AK_NP8 0x3E
		14 AK_NP9 0x3F
		15 AK_ENT 0x43
		16 AK_NPDIV 0x5C
		17 AK_NPMUL 0x5D
		18 AK_NPSUB 0x4A
		19 AK_NPADD 0x5E
		20 AK_NPDEL 0x3C
		21 AK_NPLPAREN 0x5A
		22 AK_NPRPAREN 0x5B
		23 AK_SPC 0x40
		24 AK_BS 0x41
		25 AK_TAB 0x42
		26 AK_RET 0x44
		27 AK_ESC 0x45
		28 AK_DEL 0x46
		29 AK_LSH 0x60
		30 AK_RSH 0x61
		31 AK_CAPSLOCK 0x62
		32 AK_CTRL 0x63
		33 AK_LALT 0x64
		34 AK_RALT 0x65
		35 AK_LAMI 0x66
		36 AK_RAMI 0x67
		37 AK_HELP 0x5F
		38 AK_LBRACKET 0x1A
		39 AK_RBRACKET 0x1B
		40 AK_SEMICOLON 0x29
		41 AK_COMMA 0x38
		42 AK_PERIOD 0x39
		43 AK_SLASH 0x3A
		44 AK_BACKSLASH 0x0D
		45 AK_QUOTE 0x2A
		46 AK_NUMBERSIGN 0x2B
		47 AK_LTGT 0x30
		48 AK_BACKQUOTE 0x00
		49 AK_MINUS 0x0B
		50 AK_EQUAL 0x0C
		51 AK_A 0x20
		52 AK_B 0x35
		53 AK_C 0x33
		54 AK_D 0x22
		55 AK_E 0x12
		56 AK_F 0x23
		57 AK_G 0x24
		58 AK_H 0x25
		59 AK_I 0x17
		60 AK_J 0x26
		61 AK_K 0x27
		62 AK_L 0x28
		63 AK_M 0x37
		64 AK_N 0x36
		65 AK_O 0x18
		66 AK_P 0x19
		67 AK_Q 0x10
		68 AK_R 0x13
		69 AK_S 0x21
		70 AK_T 0x14
		71 AK_U 0x16
		72 AK_V 0x34
		73 AK_W 0x11
		74 AK_X 0x32
		75 AK_Y 0x15
		76 AK_Z 0x31
		77 AK_0 0x0A
		78 AK_1 0x01
		79 AK_2 0x02
		80 AK_3 0x03
		81 AK_4 0x04
		82 AK_5 0x05
		83 AK_6 0x06
		84 AK_7 0x07
		85 AK_8 0x08
		86 AK_9 0x09
		87 AK_F1 0x50
		88 AK_F2 0x51
		89 AK_F3 0x52
		90 AK_F4 0x53
		91 AK_F5 0x54
		92 AK_F6 0x55
		93 AK_F7 0x56
		94 AK_F8 0x57
		95 AK_F9 0x58
		96 AK_F10 0x59*/
}

#if defined(__PSP2__) || defined(__SWITCH__)
void mapback_custom_controls() // assign currently used custom set to custom control set
//This is called after whenever the user changes the currently displayed controls
//or the currently displayed controller Nr
{
	int i = mainMenu_custom_currentlyEditingControllerNr;
	int j = mainMenu_custom_controlSet;
	mainMenu_customPreset_up[j][i] = mainMenu_custom_up[i];
	mainMenu_customPreset_down[j][i] = mainMenu_custom_down[i];
	mainMenu_customPreset_left[j][i] = mainMenu_custom_left[i];
	mainMenu_customPreset_right[j][i] = mainMenu_custom_right[i];
	mainMenu_customPreset_stickup[j][i] = mainMenu_custom_stickup[i];
	mainMenu_customPreset_stickdown[j][i] = mainMenu_custom_stickdown[i];
	mainMenu_customPreset_stickleft[j][i] = mainMenu_custom_stickleft[i];
	mainMenu_customPreset_stickright[j][i] = mainMenu_custom_stickright[i];
	mainMenu_customPreset_A[j][i] = mainMenu_custom_A[i];
	mainMenu_customPreset_B[j][i] = mainMenu_custom_B[i];
	mainMenu_customPreset_X[j][i] = mainMenu_custom_X[i];
	mainMenu_customPreset_Y[j][i] = mainMenu_custom_Y[i];
	mainMenu_customPreset_L[j][i] = mainMenu_custom_L[i];
	mainMenu_customPreset_R[j][i] = mainMenu_custom_R[i];
#ifdef __SWITCH__
	mainMenu_customPreset_L2[j][i] = mainMenu_custom_L2[i];
	mainMenu_customPreset_R2[j][i] = mainMenu_custom_R2[i];
	mainMenu_customPreset_L3[j][i] = mainMenu_custom_L3[i];
	mainMenu_customPreset_R3[j][i] = mainMenu_custom_R3[i];
#endif
}		
#endif //__PSP2__

static void draw_controlsMenu(int c)
{
#if defined(__PSP2__) || defined(__SWITCH__)
	mainMenu_custom_dpad=0; //on Vita, always use full custom configs that remap the dpad
#endif
	int ctrlNr = mainMenu_custom_currentlyEditingControllerNr;
	
	int leftMargin=3;
	int tabstop1 = 17;
	int tabstop2 = 19;
	int tabstop3 = 21;
	int tabstop4 = 23;
	int tabstop5 = 25;
	int tabstop6 = 27;
	int tabstop7 = 29;
	int tabstop8 = 31;
	int tabstop9 = 33;

	int menuLine = 3;
	static int b=0;
	int bb=(b%6)/3;
	SDL_Rect r;
	extern SDL_Surface *text_screen;
	r.x=80-64; r.y=0; r.w=110+64+64; r.h=240;

	text_draw_background();
	text_draw_window(2,2,41,30,text_str_controls_title);

	// MENUCONTROLS_RETURNMAIN
	if (menuControls == MENUCONTROLS_RETURNMAIN && bb)
		write_text_inv(3, menuLine, "Return to main menu");
	else
		write_text(3, menuLine, "Return to main menu");

	menuLine++;
	write_text(leftMargin,menuLine,text_str_controls_separator);
	menuLine++;

#ifdef ANDROIDSDL
	// MENUCONTROLS_ONSCREEN
	write_text(leftMargin,menuLine,"On-Screen Control");
	if ((mainMenu_onScreen==1)&&((menuControls!=MENUCONTROLS_ONSCREEN)||(bb)))
		write_text_inv(tabstop3,menuLine,"Show");
	else
		write_text(tabstop3,menuLine,"Show");
	if ((mainMenu_onScreen==0)&&((menuControls!=MENUCONTROLS_ONSCREEN)||(bb)))
		write_text_inv(tabstop6,menuLine,"Hide");
	else
		write_text(tabstop6,menuLine,"Hide");

	menuLine++;
	write_text(leftMargin,menuLine,text_str_controls_separator);
	menuLine++;	
#endif

	// MENUCONTROLS_CUSTOM_ON_OFF
	write_text(leftMargin,menuLine,"Custom Controls");
	if ((mainMenu_customControls==1)&&((menuControls!=MENUCONTROLS_CUSTOM_ON_OFF)||(bb)))
		write_text_inv(tabstop3,menuLine,"On");
	else
		write_text(tabstop3,menuLine,"On");
	if ((mainMenu_customControls==0)&&((menuControls!=MENUCONTROLS_CUSTOM_ON_OFF)||(bb)))
		write_text_inv(tabstop6,menuLine,"Off");
	else
		write_text(tabstop6,menuLine,"Off");

#if defined(__PSP2__) || defined(__SWITCH__)
	menuLine+=2;

	// MENUCONTROLS_CUSTOM_CONTROLLER_NR
	write_text(leftMargin,menuLine,"Edit Map for Controller");
	if ((mainMenu_custom_currentlyEditingControllerNr==0)&&((menuControls!=MENUCONTROLS_CUSTOM_CONTROLLER_NR)||(bb)))
		write_text_inv(tabstop6,menuLine,"1");
	else
		write_text(tabstop6,menuLine,"1");
	if ((mainMenu_custom_currentlyEditingControllerNr==1)&&((menuControls!=MENUCONTROLS_CUSTOM_CONTROLLER_NR)||(bb)))
		write_text_inv(tabstop7,menuLine,"2");
	else
		write_text(tabstop7,menuLine,"2");
	if ((mainMenu_custom_currentlyEditingControllerNr==2)&&((menuControls!=MENUCONTROLS_CUSTOM_CONTROLLER_NR)||(bb)))
		write_text_inv(tabstop8,menuLine,"3");
	else
		write_text(tabstop8,menuLine,"3");
	if ((mainMenu_custom_currentlyEditingControllerNr==3)&&((menuControls!=MENUCONTROLS_CUSTOM_CONTROLLER_NR)||(bb)))
		write_text_inv(tabstop9,menuLine,"4");
	else
		write_text(tabstop9,menuLine,"4");

#ifdef __SWITCH__
	if ((mainMenu_custom_currentlyEditingControllerNr==4)&&((menuControls!=MENUCONTROLS_CUSTOM_CONTROLLER_NR)||(bb)))
		write_text_inv(tabstop9+2,menuLine,"5");
	else
		write_text(tabstop9+2,menuLine,"5");
	if ((mainMenu_custom_currentlyEditingControllerNr==5)&&((menuControls!=MENUCONTROLS_CUSTOM_CONTROLLER_NR)||(bb)))
		write_text_inv(tabstop9+4,menuLine,"6");
	else
		write_text(tabstop9+4,menuLine,"6");
	if ((mainMenu_custom_currentlyEditingControllerNr==6)&&((menuControls!=MENUCONTROLS_CUSTOM_CONTROLLER_NR)||(bb)))
		write_text_inv(tabstop9+6,menuLine,"7");
	else
		write_text(tabstop9+6,menuLine,"7");
	if ((mainMenu_custom_currentlyEditingControllerNr==7)&&((menuControls!=MENUCONTROLS_CUSTOM_CONTROLLER_NR)||(bb)))
		write_text_inv(tabstop9+8,menuLine,"8");
	else
		write_text(tabstop9+8,menuLine,"8");
#endif

	// MENUCONTROLS_CUSTOMSET
	menuLine+=2;
	write_text(leftMargin,menuLine,"Custom Set");
	if ((mainMenu_custom_controlSet==0)&&((menuControls!=MENUCONTROLS_CUSTOMSET)||(bb)))
		write_text_inv(leftMargin+11,menuLine,"1");
	else
		write_text(leftMargin+11,menuLine,"1");
	if ((mainMenu_custom_controlSet==1)&&((menuControls!=MENUCONTROLS_CUSTOMSET)||(bb)))
		write_text_inv(leftMargin+13,menuLine,"2");
	else
		write_text(leftMargin+13,menuLine,"2");
	if ((mainMenu_custom_controlSet==2)&&((menuControls!=MENUCONTROLS_CUSTOMSET)||(bb)))
		write_text_inv(leftMargin+15,menuLine,"3");
	else
		write_text(leftMargin+15,menuLine,"3");
	if ((mainMenu_custom_controlSet==3)&&((menuControls!=MENUCONTROLS_CUSTOMSET)||(bb)))
		write_text_inv(leftMargin+17,menuLine,"4");
	else
		write_text(leftMargin+17,menuLine,"4");
	if ((mainMenu_custom_controlSet==4)&&((menuControls!=MENUCONTROLS_CUSTOMSET)||(bb)))
		write_text_inv(leftMargin+19,menuLine,"5");
	else
		write_text(leftMargin+19,menuLine,"5");
		
#else
	menuLine++;
	write_text(leftMargin,menuLine,text_str_controls_separator);
	menuLine++;

	// MENUCONTROLS_DPAD
	write_text(leftMargin,menuLine," DPAD");
	if ((mainMenu_custom_dpad==0)&&((menuControls!=MENUCONTROLS_DPAD)||(bb)))
		write_text_inv(tabstop1-6,menuLine,"Custom");
	else
		write_text(tabstop1-6,menuLine,"Custom");
	if ((mainMenu_custom_dpad==1)&&((menuControls!=MENUCONTROLS_DPAD)||(bb)))
		write_text_inv(tabstop2,menuLine,"Joystick");
	else
		write_text(tabstop2,menuLine,"Joystick");
	if ((mainMenu_custom_dpad==2)&&((menuControls!=MENUCONTROLS_DPAD)||(bb)))
		write_text_inv(tabstop6+2,menuLine,"Mouse");
	else
		write_text(tabstop6+2,menuLine,"Mouse");
#endif
	if (mainMenu_custom_dpad==0)
	{

		// MENUCONTROLS_STICK
		write_text(tabstop3+1+5,menuLine,"Edit");
		if ((editingStick==0)&&((menuControls!=MENUCONTROLS_STICK)||(bb)))
			write_text_inv(tabstop6+5,menuLine,"Dpad");
		else
			write_text(tabstop6+5,menuLine,"Dpad");
		if ((editingStick==1)&&((menuControls!=MENUCONTROLS_STICK)||(bb)))
			write_text_inv(tabstop9+4,menuLine,"Stick");
		else
			write_text(tabstop9+4,menuLine,"Stick");

		menuLine++;
		write_text(leftMargin,menuLine,text_str_controls_separator);
		menuLine++;

		// MENUCONTROLS_UP
		if (editingStick) {
			write_text(leftMargin,menuLine,"Stick Up");
			getMapping(mainMenu_custom_stickup[ctrlNr]);
		}
		else {
			write_text(leftMargin,menuLine,"Dpad Up");
			getMapping(mainMenu_custom_up[ctrlNr]);
		}
		if ((menuControls!=MENUCONTROLS_UP)||(bb))
			write_text_inv(tabstop1-2,menuLine,mapping);
		else
			write_text(tabstop1-2,menuLine,mapping);
		// MENUCONTROLS_DOWN
		menuLine+=2;
		if (editingStick) {
			write_text(leftMargin,menuLine,"Stick Down");
			getMapping(mainMenu_custom_stickdown[ctrlNr]);
		}
		else {
			write_text(leftMargin,menuLine,"Dpad Down");
			getMapping(mainMenu_custom_down[ctrlNr]);
		}
		if ((menuControls!=MENUCONTROLS_DOWN)||(bb))
			write_text_inv(tabstop1-2,menuLine,mapping);
		else
			write_text(tabstop1-2,menuLine,mapping);
		// MENUCONTROLS_LEFT
		menuLine+=2;
		if (editingStick) {
			write_text(leftMargin,menuLine,"Stick Left");
			getMapping(mainMenu_custom_stickleft[ctrlNr]);
		}
		else {
			write_text(leftMargin,menuLine,"Dpad Left");
			getMapping(mainMenu_custom_left[ctrlNr]);
		}
		if ((menuControls!=MENUCONTROLS_LEFT)||(bb))
			write_text_inv(tabstop1-2,menuLine,mapping);
		else
			write_text(tabstop1-2,menuLine,mapping);
		// MENUCONTROLS_RIGHT
		menuLine+=2;
		if (editingStick) {
			write_text(leftMargin,menuLine,"Stick Right");
			getMapping(mainMenu_custom_stickright[ctrlNr]);
		}
		else {
			write_text(leftMargin,menuLine,"Dpad Right");
			getMapping(mainMenu_custom_right[ctrlNr]);
		}
		if ((menuControls!=MENUCONTROLS_RIGHT)||(bb))
			write_text_inv(tabstop1-2,menuLine,mapping);
		else
			write_text(tabstop1-2,menuLine,mapping);
	}

	// MENUCONTROLS_A
#ifdef __SWITCH__
	menuLine+=2;
#else
	menuLine+=3;
#endif
#if defined(__PSP2__)
	write_text(leftMargin,menuLine," Square");
#elif defined(__SWITCH__)
	write_text(leftMargin,menuLine,"    Y");
#else
	write_text(leftMargin,menuLine,"    (A)");
#endif
	getMapping(mainMenu_custom_A[ctrlNr]);
	if ((menuControls!=MENUCONTROLS_A)||(bb))
		write_text_inv(tabstop1-2,menuLine,mapping);
	else
		write_text(tabstop1-2,menuLine,mapping);

	// MENUCONTROLS_Y
	menuLine+=2;
#if defined(__PSP2__)
	write_text(leftMargin,menuLine," Triangle");
#elif defined(__SWITCH__)
	write_text(leftMargin,menuLine,"    X");
#else
	write_text(leftMargin,menuLine,"    (Y)");
#endif
	getMapping(mainMenu_custom_Y[ctrlNr]);
	if ((menuControls!=MENUCONTROLS_Y)||(bb))
		write_text_inv(tabstop1-2,menuLine,mapping);
	else
		write_text(tabstop1-2,menuLine,mapping);
		
	// MENUCONTROLS_X
	menuLine+=2;
#if defined(__PSP2__)
	write_text(leftMargin,menuLine," Cross");
#elif defined(__SWITCH__)
	write_text(leftMargin,menuLine,"    B");
#else
	write_text(leftMargin,menuLine,"    (X)");
#endif
	getMapping(mainMenu_custom_X[ctrlNr]);
	if ((menuControls!=MENUCONTROLS_X)||(bb))
		write_text_inv(tabstop1-2,menuLine,mapping);
	else
		write_text(tabstop1-2,menuLine,mapping);

	// MENUCONTROLS_B
	menuLine+=2;
#if defined(__PSP2__)
	write_text(leftMargin,menuLine," Circle");
#elif defined(__SWITCH__)
	write_text(leftMargin,menuLine,"    A");
#else
	write_text(leftMargin,menuLine,"    (B)");
#endif
	getMapping(mainMenu_custom_B[ctrlNr]);
	if ((menuControls!=MENUCONTROLS_B)||(bb))
		write_text_inv(tabstop1-2,menuLine,mapping);
	else
		write_text(tabstop1-2,menuLine,mapping);


	// MENUCONTROLS_L
#ifdef __SWITCH__
	menuLine+=2;
#else
	menuLine+=3;
#endif
	write_text(leftMargin,menuLine,"    L");
	getMapping(mainMenu_custom_L[ctrlNr]);
	if ((menuControls!=MENUCONTROLS_L)||(bb))
		write_text_inv(tabstop1-2,menuLine,mapping);
	else
		write_text(tabstop1-2,menuLine,mapping);
	// MENUCONTROLS_R
	menuLine+=2;
	write_text(leftMargin,menuLine,"    R");
	getMapping(mainMenu_custom_R[ctrlNr]);
	if ((menuControls!=MENUCONTROLS_R)||(bb))
		write_text_inv(tabstop1-2,menuLine,mapping);
	else
		write_text(tabstop1-2,menuLine,mapping);

#ifdef __SWITCH__
	// MENUCONTROLS_L2
	menuLine+=2;
	write_text(leftMargin,menuLine,"   ZL");
	getMapping(mainMenu_custom_L2[ctrlNr]);
	if ((menuControls!=MENUCONTROLS_L2)||(bb))
		write_text_inv(tabstop1-2,menuLine,mapping);
	else
		write_text(tabstop1-2,menuLine,mapping);
	// MENUCONTROLS_R2
	menuLine+=2;
	write_text(leftMargin,menuLine,"   ZR");
	getMapping(mainMenu_custom_R2[ctrlNr]);
	if ((menuControls!=MENUCONTROLS_R2)||(bb))
		write_text_inv(tabstop1-2,menuLine,mapping);
	else
		write_text(tabstop1-2,menuLine,mapping);
#endif

#if defined(__PSP2__)
	menuLine++;
	write_text(leftMargin,menuLine,text_str_controls_separator);
	menuLine++;
	if (nr_joysticks==1)
		write_text(leftMargin,menuLine,"1 Controller found (L-trigger rescan)");
	else
	{
		snprintf(tmpchar, 255, "%d Controllers found (L-trigger rescan)",nr_joysticks);
		write_text(leftMargin,menuLine,tmpchar);
	}
#endif

	text_flip();
	b++;
}

static int key_controlsMenu(int *c)
{
	int end=0;
	static int delay=0;
	int left=0, right=0, up=0, down=0, hit0=0, hit1=0, hit2=0, hit3=0, del=0;
	int ctrlNr = mainMenu_custom_currentlyEditingControllerNr;
	SDL_Event event;
#if !defined(__PSP2__) && !defined(__SWITCH__)	// this can be snappy on Vita no need to worry about touch
	delay ++;
	if (delay<5) return end;
	delay=0;
#endif
	static int holdingUp=0;
	static int holdingDown=0;
	static int holdingRight=0;
	static int holdingLeft=0;
	static Uint32 menu_last_press_time=0;
	static Uint32 menu_last_move_time=0;
	Uint32 now=SDL_GetTicks();
	if (holdingLeft || holdingRight || holdingUp || holdingDown)
	{
		if (now-menu_last_press_time>MENU_MIN_HOLDING_TIME && now-menu_last_move_time>MENU_MOVE_DELAY)
		{
			menu_last_move_time=now;
			SDL_Event ev;
			ev.type = SDL_KEYDOWN;
			if (holdingLeft)
				ev.key.keysym.sym = SDLK_LEFT;
			else if (holdingRight)
				ev.key.keysym.sym = SDLK_RIGHT;
			else if (holdingUp)
				ev.key.keysym.sym = SDLK_UP;
			else if (holdingDown)
				ev.key.keysym.sym = SDLK_DOWN;
			SDL_PushEvent(&ev);
		}
	}
	while (SDL_PollEvent(&event) > 0)
	{
		left=right=up=down=hit0=hit1=hit2=hit3=del=0;
		if (event.type == SDL_KEYDOWN)
		{
			uae4all_play_click();
			switch(event.key.keysym.sym)
			{
				case SDLK_RIGHT: right=1; break;
				case SDLK_LEFT: left=1; break;
				case SDLK_UP: up=1; break;
				case SDLK_DOWN: down=1; break;
				case SDLK_PAGEUP: del=1; break; //Note: PAGEUP is Triangle on Vita
				case SDLK_LCTRL: hit2=1; break; //allow user to quit menu completely at any time
				//note SDLK_CTRL corresponds to ButtonSelect on Vita
#if defined(__PSP2__) || defined(__SWITCH__)
				case SDLK_RSHIFT: hit3=1; break; //SDLK_RSHIFT is triggerL on Vita
				case SDLK_PAGEDOWN: hit0=1; break; //SDLK_PAGEDOWN is the ok button on Vita
				case SDLK_END: hit1=1; break; // SDLK_END is the cancel button on Vita
#else
				case SDLK_HOME: hit0=1; break;
				case SDLK_PAGEDOWN: hit0=1; break;
				case SDLK_LALT: hit1=1; break;
				case SDLK_END: hit0=1; break;
#endif
			}
		}

		if (event.type == SDL_KEYUP)
		{
			switch(event.key.keysym.sym)
			{
				case SDLK_RIGHT:
					holdingRight=0;
					break;
				case SDLK_LEFT:
					holdingLeft=0;
					break;
				case SDLK_UP:
					holdingUp=0;
					break;
				case SDLK_DOWN:
					holdingDown=0;
					break;
				default:
					break;
			}
		}
		
		if (left && !holdingLeft)
		{
			holdingLeft=1;
			menu_last_press_time=now;
		}
		if (right && !holdingRight) 
		{
			holdingRight=1;
			menu_last_press_time=now;
		}
		if (up && !holdingUp) 
		{
			holdingUp=1;
			menu_last_press_time=now;
		}
		if (down && !holdingDown) 
		{
			holdingDown=1;
			menu_last_press_time=now;
		}

		if (hit2) //Does the user want to cancel the menu completely?
		{
			if (emulating)
			{
				end = -1; 
				quit_pressed_in_submenu = 1; //Tell the mainMenu to cancel, too
			}
		}
#if defined(__PSP2__) || defined(__SWITCH__)
		else if (hit3) // L pressed on Vita?
		{
			//See if new joysticks have been paired
			close_joystick();
			SDL_QuitSubSystem(SDL_INIT_JOYSTICK);	
			SDL_InitSubSystem(SDL_INIT_JOYSTICK);
			init_joystick();	
		}
#endif
#if !defined(__PSP2__) && !defined(__SWITCH__)
		else if (hit0)
		{
			end = -1;
		}
#endif
		else if (hit1)
		{
			end = -1;
		}
		else if (up)
		{
#if !defined(__PSP2__) && !defined(__SWITCH__)			
			if (menuControls==MENUCONTROLS_A && mainMenu_custom_dpad>0) menuControls=MENUCONTROLS_DPAD;
			else 
#endif
#ifdef __SWITCH__
			if (menuControls==MENUCONTROLS_RETURNMAIN) menuControls=MENUCONTROLS_R2;
#else
			if (menuControls==MENUCONTROLS_RETURNMAIN) menuControls=MENUCONTROLS_R;
#endif
			else menuControls--;
		}
		else if (down)
		{
#if !defined(__PSP2__) && !defined(__SWITCH__)
			if (menuControls==MENUCONTROLS_DPAD && mainMenu_custom_dpad>0) menuControls=MENUCONTROLS_A;
			else 
#endif
#ifdef __SWITCH__
			if (menuControls==MENUCONTROLS_R2) menuControls=MENUCONTROLS_RETURNMAIN;
#else
			if (menuControls==MENUCONTROLS_R) menuControls=MENUCONTROLS_RETURNMAIN;
#endif
			else menuControls++;
		}
		switch (menuControls)
		{
			
#ifdef ANDROIDSDL
			case MENUCONTROLS_ONSCREEN:
				if ((left)||(right))
						mainMenu_onScreen = !mainMenu_onScreen;
				break;
#endif
			case MENUCONTROLS_RETURNMAIN:
				if (hit0)
					end = -1;
				break;
			case MENUCONTROLS_CUSTOM_ON_OFF:
				if ((left)||(right))
				{
						mainMenu_customControls = !mainMenu_customControls;
						remap_custom_controls();
				}
				break;
#if defined(__PSP2__) || defined(__SWITCH__)
			case MENUCONTROLS_CUSTOM_CONTROLLER_NR:
				if (left)
				{
					if (mainMenu_custom_currentlyEditingControllerNr>0)
					{
						mainMenu_custom_currentlyEditingControllerNr--;
						remap_custom_controls();
					}
				}
				else if (right)
				{
					if (mainMenu_custom_currentlyEditingControllerNr<(MAX_NUM_CONTROLLERS-1))
					{
						mainMenu_custom_currentlyEditingControllerNr++;
						remap_custom_controls();
					}
				}
				break;

			case MENUCONTROLS_CUSTOMSET:
				if (left)
				{
					if (mainMenu_custom_controlSet>0)
						mainMenu_custom_controlSet--;
					else
						mainMenu_custom_controlSet=MAX_NUM_CUSTOM_PRESETS-1;
					remap_custom_controls();
				}
				else if (right)
				{
					if (mainMenu_custom_controlSet<MAX_NUM_CUSTOM_PRESETS-1)
						mainMenu_custom_controlSet++;
					else
						mainMenu_custom_controlSet=0;
					remap_custom_controls();
				}
				break;
#else	
			case MENUCONTROLS_DPAD:
				if (left)
				{
					if (mainMenu_custom_dpad>0)
						mainMenu_custom_dpad--;
					else
						mainMenu_custom_dpad=2;
				}
				else if (right)
				{
					if (mainMenu_custom_dpad<2)
						mainMenu_custom_dpad++;
					else
						mainMenu_custom_dpad=0;
				}
				break;
#endif					
			case MENUCONTROLS_STICK:
				if (left || right)
					editingStick=!editingStick;
				break;

			case MENUCONTROLS_UP:
				if (left)
				{
					if (editingStick) {
						if (mainMenu_custom_stickup[ctrlNr]>MIN_CUSTOM_ID)
							mainMenu_custom_stickup[ctrlNr]--;
						else
							mainMenu_custom_stickup[ctrlNr]=MAX_CUSTOM_ID;
					} else {
						if (mainMenu_custom_up[ctrlNr]>MIN_CUSTOM_ID)
							mainMenu_custom_up[ctrlNr]--;
						else
							mainMenu_custom_up[ctrlNr]=MAX_CUSTOM_ID;
					}
#if defined(__PSP2__) || defined(__SWITCH__)
					mapback_custom_controls(); //record this change in custom control set
#endif
				}
				else if (right)
				{
					if (editingStick) {
						if (mainMenu_custom_stickup[ctrlNr]<MAX_CUSTOM_ID)
							mainMenu_custom_stickup[ctrlNr]++;
						else
							mainMenu_custom_stickup[ctrlNr]=MIN_CUSTOM_ID;
					} else {
						if (mainMenu_custom_up[ctrlNr]<MAX_CUSTOM_ID)
							mainMenu_custom_up[ctrlNr]++;
						else
							mainMenu_custom_up[ctrlNr]=MIN_CUSTOM_ID;
					}
#if defined(__PSP2__) || defined(__SWITCH__)
					mapback_custom_controls(); //record this change in custom control set
#endif
				}
				else if (del)
				{
					if (editingStick) {
						mainMenu_custom_stickup[ctrlNr]=0;
					} else {
						mainMenu_custom_up[ctrlNr]=0;
					}
#if defined(__PSP2__) || defined(__SWITCH__)
					mapback_custom_controls(); //record this change in custom control set
#endif
				}	
				break;
			case MENUCONTROLS_DOWN:
				if (left)
				{
					if (editingStick) {
						if (mainMenu_custom_stickdown[ctrlNr]>MIN_CUSTOM_ID)
							mainMenu_custom_stickdown[ctrlNr]--;
						else
							mainMenu_custom_stickdown[ctrlNr]=MAX_CUSTOM_ID;
					} else {
						if (mainMenu_custom_down[ctrlNr]>MIN_CUSTOM_ID)
							mainMenu_custom_down[ctrlNr]--;
						else
							mainMenu_custom_down[ctrlNr]=MAX_CUSTOM_ID;
					}
#if defined(__PSP2__) || defined(__SWITCH__)
					mapback_custom_controls(); //record this change in custom control set
#endif
				}
				else if (right)
				{
					if (editingStick) {
						if (mainMenu_custom_stickdown[ctrlNr]<MAX_CUSTOM_ID)
							mainMenu_custom_stickdown[ctrlNr]++;
						else
							mainMenu_custom_stickdown[ctrlNr]=MIN_CUSTOM_ID;
					} else {
						if (mainMenu_custom_down[ctrlNr]<MAX_CUSTOM_ID)
							mainMenu_custom_down[ctrlNr]++;
						else
							mainMenu_custom_down[ctrlNr]=MIN_CUSTOM_ID;
					}
#if defined(__PSP2__) || defined(__SWITCH__)
					mapback_custom_controls(); //record this change in custom control set
#endif
				}
				else if (del)
				{
					if (editingStick) {
						mainMenu_custom_stickdown[ctrlNr]=0;
					} else {
						mainMenu_custom_down[ctrlNr]=0;
					}
#if defined(__PSP2__) || defined(__SWITCH__)
					mapback_custom_controls(); //record this change in custom control set
#endif
				}	
				break;
			case MENUCONTROLS_LEFT:
				if (left)
				{
					if (editingStick) {
						if (mainMenu_custom_stickleft[ctrlNr]>MIN_CUSTOM_ID)
							mainMenu_custom_stickleft[ctrlNr]--;
						else
							mainMenu_custom_stickleft[ctrlNr]=MAX_CUSTOM_ID;
					} else {
						if (mainMenu_custom_left[ctrlNr]>MIN_CUSTOM_ID)
							mainMenu_custom_left[ctrlNr]--;
						else
							mainMenu_custom_left[ctrlNr]=MAX_CUSTOM_ID;
					}
#if defined(__PSP2__) || defined(__SWITCH__)
					mapback_custom_controls(); //record this change in custom control set
#endif
				}
				else if (right)
				{
					if (editingStick) {
						if (mainMenu_custom_stickleft[ctrlNr]<MAX_CUSTOM_ID)
							mainMenu_custom_stickleft[ctrlNr]++;
						else
							mainMenu_custom_stickleft[ctrlNr]=MIN_CUSTOM_ID;
					} else {
						if (mainMenu_custom_left[ctrlNr]<MAX_CUSTOM_ID)
							mainMenu_custom_left[ctrlNr]++;
						else
							mainMenu_custom_left[ctrlNr]=MIN_CUSTOM_ID;
					}
#if defined(__PSP2__) || defined(__SWITCH__)
					mapback_custom_controls(); //record this change in custom control set
#endif
				}
				else if (del)
				{
					if (editingStick) {
						mainMenu_custom_stickleft[ctrlNr]=0;
					} else {
						mainMenu_custom_left[ctrlNr]=0;
					}
#if defined(__PSP2__) || defined(__SWITCH__)
					mapback_custom_controls(); //record this change in custom control set
#endif
				}	
				break;
			case MENUCONTROLS_RIGHT:
				if (left)
				{
					if (editingStick) {
						if (mainMenu_custom_stickright[ctrlNr]>MIN_CUSTOM_ID)
							mainMenu_custom_stickright[ctrlNr]--;
						else
							mainMenu_custom_stickright[ctrlNr]=MAX_CUSTOM_ID;
					} else {
						if (mainMenu_custom_right[ctrlNr]>MIN_CUSTOM_ID)
							mainMenu_custom_right[ctrlNr]--;
						else
							mainMenu_custom_right[ctrlNr]=MAX_CUSTOM_ID;
					}
#if defined(__PSP2__) || defined(__SWITCH__)
					mapback_custom_controls(); //record this change in custom control set
#endif
				}
				else if (right)
				{
					if (editingStick) {
						if (mainMenu_custom_stickright[ctrlNr]<MAX_CUSTOM_ID)
							mainMenu_custom_stickright[ctrlNr]++;
						else
							mainMenu_custom_stickright[ctrlNr]=MIN_CUSTOM_ID;
					} else {
						if (mainMenu_custom_right[ctrlNr]<MAX_CUSTOM_ID)
							mainMenu_custom_right[ctrlNr]++;
						else
							mainMenu_custom_right[ctrlNr]=MIN_CUSTOM_ID;
					}
#if defined(__PSP2__) || defined(__SWITCH__)
					mapback_custom_controls(); //record this change in custom control set
#endif
				}
				else if (del)
				{
					if (editingStick) {
						mainMenu_custom_stickright[ctrlNr]=0;
					} else {
						mainMenu_custom_right[ctrlNr]=0;
					}
#if defined(__PSP2__) || defined(__SWITCH__)
					mapback_custom_controls(); //record this change in custom control set
#endif
				}	
				break;
			case MENUCONTROLS_A:
				if (left)
				{
					if (mainMenu_custom_A[ctrlNr]>MIN_CUSTOM_ID)
						mainMenu_custom_A[ctrlNr]--;
					else
						mainMenu_custom_A[ctrlNr]=MAX_CUSTOM_ID;
#if defined(__PSP2__) || defined(__SWITCH__)
					mapback_custom_controls(); //record this change in custom control set
#endif
				}
				else if (right)
				{
					if (mainMenu_custom_A[ctrlNr]<MAX_CUSTOM_ID)
						mainMenu_custom_A[ctrlNr]++;
					else
						mainMenu_custom_A[ctrlNr]=MIN_CUSTOM_ID;
#if defined(__PSP2__) || defined(__SWITCH__)
					mapback_custom_controls(); //record this change in custom control set
#endif
				}
				else if (del)
				{
					mainMenu_custom_A[ctrlNr]=0;
#if defined(__PSP2__) || defined(__SWITCH__)
					mapback_custom_controls(); //record this change in custom control set
#endif
				}	
				break;
			case MENUCONTROLS_B:
				if (left)
				{
					if (mainMenu_custom_B[ctrlNr]>MIN_CUSTOM_ID)
						mainMenu_custom_B[ctrlNr]--;
					else
						mainMenu_custom_B[ctrlNr]=MAX_CUSTOM_ID;
#if defined(__PSP2__) || defined(__SWITCH__)
					mapback_custom_controls(); //record this change in custom control set
#endif
				}
				else if (right)
				{
					if (mainMenu_custom_B[ctrlNr]<MAX_CUSTOM_ID)
						mainMenu_custom_B[ctrlNr]++;
					else
						mainMenu_custom_B[ctrlNr]=MIN_CUSTOM_ID;
#if defined(__PSP2__) || defined(__SWITCH__)
					mapback_custom_controls(); //record this change in custom control set
#endif
				}
				else if (del)
				{
					mainMenu_custom_B[ctrlNr]=0;
#if defined(__PSP2__) || defined(__SWITCH__)
					mapback_custom_controls(); //record this change in custom control set
#endif
				}	
				break;
			case MENUCONTROLS_X:
				if (left)
				{
					if (mainMenu_custom_X[ctrlNr]>MIN_CUSTOM_ID)
						mainMenu_custom_X[ctrlNr]--;
					else
						mainMenu_custom_X[ctrlNr]=MAX_CUSTOM_ID;
#if defined(__PSP2__) || defined(__SWITCH__)
					mapback_custom_controls(); //record this change in custom control set
#endif
				}
				else if (right)
				{
					if (mainMenu_custom_X[ctrlNr]<MAX_CUSTOM_ID)
						mainMenu_custom_X[ctrlNr]++;
					else
						mainMenu_custom_X[ctrlNr]=MIN_CUSTOM_ID;
#if defined(__PSP2__) || defined(__SWITCH__)
					mapback_custom_controls(); //record this change in custom control set
#endif
				}
				else if (del)
				{
					mainMenu_custom_X[ctrlNr]=0;
#if defined(__PSP2__) || defined(__SWITCH__)
					mapback_custom_controls(); //record this change in custom control set
#endif
				}	
				break;
			case MENUCONTROLS_Y:
				if (left)
				{
					if (mainMenu_custom_Y[ctrlNr]>MIN_CUSTOM_ID)
						mainMenu_custom_Y[ctrlNr]--;
					else
						mainMenu_custom_Y[ctrlNr]=MAX_CUSTOM_ID;
#if defined(__PSP2__) || defined(__SWITCH__)
					mapback_custom_controls(); //record this change in custom control set
#endif
				}
				else if (right)
				{
					if (mainMenu_custom_Y[ctrlNr]<MAX_CUSTOM_ID)
						mainMenu_custom_Y[ctrlNr]++;
					else
						mainMenu_custom_Y[ctrlNr]=MIN_CUSTOM_ID;
#if defined(__PSP2__) || defined(__SWITCH__)
					mapback_custom_controls(); //record this change in custom control set
#endif
				}
				else if (del)
				{
					mainMenu_custom_Y[ctrlNr]=0;
#if defined(__PSP2__) || defined(__SWITCH__)
					mapback_custom_controls(); //record this change in custom control set
#endif
				}	
				break;
			case MENUCONTROLS_L:
				if (left)
				{
					if (mainMenu_custom_L[ctrlNr]>MIN_CUSTOM_ID)
						mainMenu_custom_L[ctrlNr]--;
					else
						mainMenu_custom_L[ctrlNr]=MAX_CUSTOM_ID;
#if defined(__PSP2__) || defined(__SWITCH__)
					mapback_custom_controls(); //record this change in custom control set
#endif
				}
				else if (right)
				{
					if (mainMenu_custom_L[ctrlNr]<MAX_CUSTOM_ID)
						mainMenu_custom_L[ctrlNr]++;
					else
						mainMenu_custom_L[ctrlNr]=MIN_CUSTOM_ID;
#if defined(__PSP2__) || defined(__SWITCH__)
					mapback_custom_controls(); //record this change in custom control set
#endif
				}
				else if (del)
				{
					mainMenu_custom_L[ctrlNr]=0;
#if defined(__PSP2__) || defined(__SWITCH__)
					mapback_custom_controls(); //record this change in custom control set
#endif
				}	
				break;
			case MENUCONTROLS_R:
				if (left)
				{
					if (mainMenu_custom_R[ctrlNr]>MIN_CUSTOM_ID)
						mainMenu_custom_R[ctrlNr]--;
					else
						mainMenu_custom_R[ctrlNr]=MAX_CUSTOM_ID;
#if defined(__PSP2__) || defined(__SWITCH__)
					mapback_custom_controls(); //record this change in custom control set
#endif
				}
				else if (right)
				{
					if (mainMenu_custom_R[ctrlNr]<MAX_CUSTOM_ID)
						mainMenu_custom_R[ctrlNr]++;
					else
						mainMenu_custom_R[ctrlNr]=MIN_CUSTOM_ID;
#if defined(__PSP2__) || defined(__SWITCH__)
					mapback_custom_controls(); //record this change in custom control set
#endif
				}
				else if (del)
				{
					mainMenu_custom_R[ctrlNr]=0;
#if defined(__PSP2__) || defined(__SWITCH__)
					mapback_custom_controls(); //record this change in custom control set
#endif
				}	
				break;
#ifdef __SWITCH__
			case MENUCONTROLS_L2:
				if (left)
				{
					if (mainMenu_custom_L2[ctrlNr]>MIN_CUSTOM_ID)
						mainMenu_custom_L2[ctrlNr]--;
					else
						mainMenu_custom_L2[ctrlNr]=MAX_CUSTOM_ID;
					mapback_custom_controls(); //record this change in custom control set
				}
				else if (right)
				{
					if (mainMenu_custom_L2[ctrlNr]<MAX_CUSTOM_ID)
						mainMenu_custom_L2[ctrlNr]++;
					else
						mainMenu_custom_L2[ctrlNr]=MIN_CUSTOM_ID;
					mapback_custom_controls(); //record this change in custom control set
				}
				else if (del)
				{
					mainMenu_custom_L2[ctrlNr]=0;
					mapback_custom_controls(); //record this change in custom control set
				}	
				break;
			case MENUCONTROLS_R2:
				if (left)
				{
					if (mainMenu_custom_R2[ctrlNr]>MIN_CUSTOM_ID)
						mainMenu_custom_R2[ctrlNr]--;
					else
						mainMenu_custom_R2[ctrlNr]=MAX_CUSTOM_ID;
					mapback_custom_controls(); //record this change in custom control set
				}
				else if (right)
				{
					if (mainMenu_custom_R2[ctrlNr]<MAX_CUSTOM_ID)
						mainMenu_custom_R2[ctrlNr]++;
					else
						mainMenu_custom_R2[ctrlNr]=MIN_CUSTOM_ID;
					mapback_custom_controls(); //record this change in custom control set
				}
				else if (del)
				{
					mainMenu_custom_R2[ctrlNr]=0;
					mapback_custom_controls(); //record this change in custom control set
				}	
				break;
#endif
		}
	}
	return end;
}

static void raise_controlsMenu()
{
	int i;

	text_draw_background();
	text_flip();
#if !defined(__PSP2__) && !defined(__SWITCH__)
	for(i=0;i<10;i++)
	{
		text_draw_background();
		text_draw_window(80-64,(10-i)*24,160+64+64,220,text_str_controls_title);
		text_flip();
	}
#endif
}

static void unraise_controlsMenu()
{
	int i;
#if !defined(__PSP2__) && !defined(__SWITCH__)
	for(i=9;i>=0;i--)
	{
		text_draw_background();
		text_draw_window(80-64,(10-i)*24,160+64+64,220,text_str_controls_title);
		text_flip();
	}
#endif
	text_draw_background();
	text_flip();
}

int run_menuControls()
{
	SDL_Event event;
	SDL_Delay(150);
	while(SDL_PollEvent(&event))
		SDL_Delay(10);
	int end=0, c=0;
	raise_controlsMenu();
	while(!end)
	{
		draw_controlsMenu(c);
		end=key_controlsMenu(&c);
	}
	set_joyConf();
	unraise_controlsMenu();
	return end;
}
