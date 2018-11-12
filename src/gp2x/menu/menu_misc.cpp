#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "sysconfig.h"
#include "sysdeps.h"
#include "config.h"
#include "menu.h"

#include <sys/stat.h>
#include <unistd.h>
#if defined(__PSP2__) // NOT __SWITCH__
#include "psp2-dirent.h"
#else
#include<dirent.h>
#endif

#include "uae.h"
#include "options.h"
#include "sound.h"
#include <SDL.h>
#include "gp2x.h"
#include <SDL_ttf.h>
#include "custom.h"
#include "menu_config.h"

#if defined(__PSP2__) || defined(__SWITCH__)
#define SDL_PollEvent PSP2_PollEvent
#endif

const char *text_str_misc_separator="----------------------------------";
static const char *text_str_misc_title=    "            Miscellanous         -";
static const char *text_str_stylus_offset="StylusOffset";
static const char *text_str_0px="0px";
static const char *text_str_1px="1px";
static const char *text_str_3px="3px";
static const char *text_str_5px="5px";
static const char *text_str_8px="8px";
static const char *text_str_tap_delay="Tap delay";
static const char *text_str_normal="normal";
static const char *text_str_short="short";
static const char *text_str_none="no";
static const char *text_str_mouse_multiplier="Mouse Speed";
int menuMisc = 0;

extern int kickstart;
extern int quit_pressed_in_submenu;
extern int emulating;

enum { 
	MENUMISC_RETURNMAIN = 0,
	MENUMISC_CPU,
	MENUMISC_CHIPSET,
	MENUMISC_KICKSTART,
	MENUMISC_CPUSPEED,
	MENUMISC_BLITTER,
	MENUMISC_SPRITECOLLISIONS,
#if !defined(__PSP2__) && !defined(__SWITCH__)
#ifdef PANDORA
	MENUMISC_PANDORASPEED,
#endif
#endif // __PSP2__
#ifdef ANDROIDSDL
	MENUMISC_ONSCREEN,
#endif
	MENUMISC_CONTROLCFG,
	MENUMISC_JOYSTICK,
	MENUMISC_AUTOFIRERATE,
#if defined(__PSP2__) || defined(__SWITCH__)
	MENUMISC_CUSTOMAUTOFIREBUTTON,
#endif
#if defined(__PSP2__) || defined(__SWITCH__)
	MENUMISC_MOUSEEMULATION,
	MENUMISC_LEFTSTICKMOUSE,
#endif
	MENUMISC_MOUSEMULTIPLIER,
#if !defined(__PSP2__) && !defined(__SWITCH__) // No stylus on Vita
	MENUMISC_STYLUSOFFSET,
#endif
#ifdef __SWITCH__
	MENUMISC_SWAPAB,
#endif
#if defined(__PSP2__) || defined(__SWITCH__)
	MENUMISC_DEADZONE,
	MENUMISC_TOUCHCONTROLS,
#else
	MENUMISC_TAPDELAY,
#endif
	MENUMISC_END
};
	
static void draw_miscMenu(int c)
{
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

	int menuLine = 2;
	static int b=0;
	int bb=(b%6)/3;
	SDL_Rect r;
	extern SDL_Surface *text_screen;
	char cpuSpeed[8];
	char tmpString[16];
	
	r.x=80-64; r.y=0; r.w=110+64+64; r.h=240;

	text_draw_background();
	text_draw_window(2,1,40,30,text_str_misc_title);

	// MENUMISC_RETURNMAIN
	if (menuMisc == MENUMISC_RETURNMAIN && bb)
		write_text_inv(3, menuLine, "Return to main menu");
	else
		write_text(3, menuLine, "Return to main menu");

	menuLine++;
	write_text(leftMargin,menuLine,text_str_misc_separator);
	menuLine++;

	// MENUMISC_CPU
	write_text(leftMargin,menuLine,"CPU");
	if ((mainMenu_CPU_model==0)&&((menuMisc!=MENUMISC_CPU)||(bb)))
	  write_text_inv(tabstop1,menuLine,"68000");
	else
	  write_text(tabstop1,menuLine,"68000");
	
	if ((mainMenu_CPU_model==1)&&((menuMisc!=MENUMISC_CPU)||(bb)))
	  write_text_inv(tabstop4,menuLine,"68020");
	else
	  write_text(tabstop4,menuLine,"68020");

	// MENUMISC_CHIPSET
	menuLine+=2;
	write_text(leftMargin,menuLine,"Chipset");
	
	if (((mainMenu_chipset & 0xff)==0)&&((menuMisc!=MENUMISC_CHIPSET)||(bb)))
		write_text_inv(tabstop1,menuLine,"OCS");
	else
		write_text(tabstop1,menuLine,"OCS");
	
	if (((mainMenu_chipset & 0xff)==1)&&((menuMisc!=MENUMISC_CHIPSET)||(bb)))
		write_text_inv(tabstop3,menuLine,"ECS");
	else
		write_text(tabstop3,menuLine,"ECS");
	
	if (((mainMenu_chipset & 0xff)==2)&&((menuMisc!=MENUMISC_CHIPSET)||(bb)))
		write_text_inv(tabstop5,menuLine,"AGA");
	else
		write_text(tabstop5,menuLine,"AGA");
				
	// MENUMISC_KICKSTART
	menuLine+=2;
	write_text(leftMargin,menuLine,"Kickstart");
	if ((kickstart==0)&&((menuMisc!=MENUMISC_KICKSTART)||(bb)))
		write_text_inv(tabstop1,menuLine,"1.2");
	else
		write_text(tabstop1,menuLine,"1.2");

	if ((kickstart==1)&&((menuMisc!=MENUMISC_KICKSTART)||(bb)))
		write_text_inv(tabstop3,menuLine,"1.3");
	else
		write_text(tabstop3,menuLine,"1.3");

	if ((kickstart==2)&&((menuMisc!=MENUMISC_KICKSTART)||(bb)))
		write_text_inv(tabstop5,menuLine,"2.0");
	else
		write_text(tabstop5,menuLine,"2.0");

	if ((kickstart==3)&&((menuMisc!=MENUMISC_KICKSTART)||(bb)))
		write_text_inv(tabstop7,menuLine,"3.1");
	else
		write_text(tabstop7,menuLine,"3.1");

	// MENUMISC_CPUSPEED
	menuLine+=2;
	write_text(leftMargin,menuLine,"CPU Speed");
	if ((mainMenu_CPU_speed==0)&&((menuMisc!=MENUMISC_CPUSPEED)||(bb)))
		write_text_inv(tabstop1,menuLine,"7MHz");
	else
		write_text(tabstop1,menuLine,"7MHz");

	if ((mainMenu_CPU_speed==1)&&((menuMisc!=MENUMISC_CPUSPEED)||(bb)))
		write_text_inv(tabstop4,menuLine,"14MHz");
	else
		write_text(tabstop4,menuLine,"14MHz");

	if ((mainMenu_CPU_speed==2)&&((menuMisc!=MENUMISC_CPUSPEED)||(bb)))
		write_text_inv(tabstop8,menuLine,"28MHz");
	else
		write_text(tabstop8,menuLine,"28MHz");

	// MENUMISC_BLITTER
	menuLine+=2;
	write_text(leftMargin,menuLine,"Blitter");
	
	if (((mainMenu_chipset & 0xff00)!=0x100 && (mainMenu_chipset & 0xff00)!=0x200)&&((menuMisc!=MENUMISC_BLITTER)||(bb)))
		write_text_inv(tabstop1,menuLine,"Normal");
	else
		write_text(tabstop1,menuLine,"Normal");

	if (((mainMenu_chipset & 0xff00)==0x100)&&((menuMisc!=MENUMISC_BLITTER)||(bb)))
		write_text_inv(tabstop5-2,menuLine,"Immediate");
	else
		write_text(tabstop5-2,menuLine,"Immediate");

	if (((mainMenu_chipset & 0xff00)==0x200)&&((menuMisc!=MENUMISC_BLITTER)||(bb)))
		write_text_inv(tabstop9,menuLine,"Improved");
	else
		write_text(tabstop9,menuLine,"Improved");

	// MENUMISC_SPRITECOLLISIONS
	menuLine+=2;
	write_text(leftMargin,menuLine,"Sprite Collisions");
	if ((mainMenu_spriteCollisions==0)&&((menuMisc!=MENUMISC_SPRITECOLLISIONS)||(bb)))
	  write_text_inv(tabstop4,menuLine,"Off");
	else
	  write_text(tabstop4,menuLine,"Off");
	
	if ((mainMenu_spriteCollisions==1)&&((menuMisc!=MENUMISC_SPRITECOLLISIONS)||(bb)))
	  write_text_inv(tabstop6,menuLine,"On");
	else
	  write_text(tabstop6,menuLine,"On");

#if !defined(__PSP2__) && !defined(__SWITCH__)
#ifdef PANDORA
  // MENUMISC_PANDORASPEED
	menuLine+=2;
	write_text(leftMargin,menuLine,"Pandora CPU-Speed");
	snprintf((char*)cpuSpeed, 8, "%d",mainMenu_cpuSpeed);
	if ((menuMisc!=MENUMISC_PANDORASPEED)||(bb))
		write_text_inv(tabstop4-1,menuLine,cpuSpeed);
	else
		write_text(tabstop4-1,menuLine,cpuSpeed);
	write_text(tabstop6-1,menuLine,"MHz");
#endif
#endif // __PSP2__

#ifdef ANDROIDSDL
  // MENUMISC_ONSCREEN
	menuLine+=2;
	write_text(leftMargin,menuLine,"On-Screen Control");
	if ((mainMenu_onScreen==1)&&((menuMisc!=MENUMISC_ONSCREEN)||(bb)))
		write_text_inv(tabstop3,menuLine,"Show");
	else
		write_text(tabstop3,menuLine,"Show");
	if ((mainMenu_onScreen==0)&&((menuMisc!=MENUMISC_ONSCREEN)||(bb)))
		write_text_inv(tabstop6,menuLine,"Hide");
	else
		write_text(tabstop6,menuLine,"Hide");
#endif
  
	menuLine++;
	write_text(leftMargin,menuLine,text_str_misc_separator);
	menuLine++;

	// MENUMISC_CONTROLCFG
	write_text(leftMargin,menuLine,"Control Config");

	if ((mainMenu_joyConf==0)&&((menuMisc!=MENUMISC_CONTROLCFG)||(bb)))
		write_text_inv(tabstop2,menuLine,"1");
	else
		write_text(tabstop2,menuLine,"1");

	if ((mainMenu_joyConf==1)&&((menuMisc!=MENUMISC_CONTROLCFG)||(bb)))
		write_text_inv(tabstop3,menuLine,"2");
	else
		write_text(tabstop3,menuLine,"2");

	if ((mainMenu_joyConf==2)&&((menuMisc!=MENUMISC_CONTROLCFG)||(bb)))
		write_text_inv(tabstop4,menuLine,"3");
	else
		write_text(tabstop4,menuLine,"3");

	if ((mainMenu_joyConf==3)&&((menuMisc!=MENUMISC_CONTROLCFG)||(bb)))
		write_text_inv(tabstop5,menuLine,"4");
	else
		write_text(tabstop5,menuLine,"4");

	menuLine+=2;
#if defined(__PSP2__)
	if (mainMenu_joyConf==0) write_text(3,menuLine,"(Sq=Autofire X=Fire Tr=Space O=2nd)");
	else if (mainMenu_joyConf==1) write_text(3,menuLine,"(Sq=Fire X=Autofire Tr=Space O=2nd)");
	else if (mainMenu_joyConf==2) write_text(3,menuLine,"(Sq=Autofire X=Jump Tr=Fire O=2nd)");
	else if (mainMenu_joyConf==3) write_text(3,menuLine,"(Sq=Fire X=Jump Tr=Autofire O=2nd)");
#elif defined(__SWITCH__)
	if (mainMenu_joyConf==0) write_text(3,menuLine,"(Y=Autofire B=Fire X=Space A=2nd)");
	else if (mainMenu_joyConf==1) write_text(3,menuLine,"(Y=Fire B=Autofire X=Space A=2nd)");
	else if (mainMenu_joyConf==2) write_text(3,menuLine,"(Y=Autofire B=Jump X=Fire A=2nd)");
	else if (mainMenu_joyConf==3) write_text(3,menuLine,"(Y=Fire B=Jump X=Autofire A=2nd)");
#else
	if (mainMenu_joyConf==0) write_text(3,menuLine,"(A=Autofire X=Fire Y=Space B=2nd)");
	else if (mainMenu_joyConf==1) write_text(3,menuLine,"(A=Fire X=Autofire Y=Space B=2nd)");
	else if (mainMenu_joyConf==2) write_text(3,menuLine,"(A=Autofire X=Jump Y=Fire B=2nd)");
	else if (mainMenu_joyConf==3) write_text(3,menuLine,"(A=Fire X=Jump Y=Autofire B=2nd)");
#endif

  // MENUMISC_JOYSTICK
	menuLine+=2;
	write_text(leftMargin,menuLine,"Joystick");
  
	if ((mainMenu_joyPort==1)&&((menuMisc!=MENUMISC_JOYSTICK)||(bb)))
		write_text_inv(tabstop3-2,menuLine,"Port0");
	else
		write_text(tabstop3-2,menuLine,"Port0");

	if ((mainMenu_joyPort==2)&&((menuMisc!=MENUMISC_JOYSTICK)||(bb)))
		write_text_inv(tabstop6-2,menuLine,"Port1");
	else
		write_text(tabstop6-2,menuLine,"Port1");

	if ((mainMenu_joyPort==0)&&((menuMisc!=MENUMISC_JOYSTICK)||(bb)))
		write_text_inv(tabstop9-1,menuLine,"Both");
	else
		write_text(tabstop9-1,menuLine,"Both");
  
	// MENUMISC_AUTOFIRERATE
	menuLine+=2;
	write_text(leftMargin,menuLine,"Autofire Rate");

	if ((mainMenu_autofireRate==8)&&((menuMisc!=MENUMISC_AUTOFIRERATE)||(bb)))
		write_text_inv(tabstop3-2,menuLine,"Light");
	else
		write_text(tabstop3-2,menuLine,"Light");

	if ((mainMenu_autofireRate==4)&&((menuMisc!=MENUMISC_AUTOFIRERATE)||(bb)))
		write_text_inv(tabstop6-2,menuLine,"Medium");
	else
		write_text(tabstop6-2,menuLine,"Medium");

	if ((mainMenu_autofireRate==2)&&((menuMisc!=MENUMISC_AUTOFIRERATE)||(bb)))
		write_text_inv(tabstop9-1,menuLine,"Heavy");
	else
		write_text(tabstop9-1,menuLine,"Heavy");

#if defined(__PSP2__) || defined(__SWITCH__)
	// MENUMISC_CUSTOMAUTOFIREBUTTON
	menuLine+=2;
	write_text(leftMargin,menuLine,"Custom Autofire Button");

	switch (mainMenu_customAutofireButton)
	{
		case 0:
			strcpy(tmpString,"None");
			break;
		case 1:
#ifdef __SWITCH__
			strcpy(tmpString,"Y");
#else
			strcpy(tmpString,"Square");
#endif
			break;
		case 2:
#ifdef __SWITCH__
			strcpy(tmpString,"X");
#else
			strcpy(tmpString,"Triangle");
#endif
			break;
		case 3:
#ifdef __SWITCH__
			strcpy(tmpString,"A");
#else
			strcpy(tmpString,"Circle");
#endif
			break;
		case 4:
#ifdef __SWITCH__
			strcpy(tmpString,"B");
#else
			strcpy(tmpString,"Cross");
#endif
			break;
		case 5:
			strcpy(tmpString,"L");
			break;
		case 6:
			strcpy(tmpString,"R");
			break;
#ifdef __SWITCH__
		case 7:
			strcpy(tmpString,"ZL");
			break;
		case 8:
			strcpy(tmpString,"ZR");
			break;
#endif
	}
	if ((menuMisc!=MENUMISC_CUSTOMAUTOFIREBUTTON)||(bb))
		write_text_inv(tabstop6,menuLine,tmpString);
	else
		write_text(tabstop6,menuLine,tmpString);
#endif

	menuLine++;
	write_text(leftMargin,menuLine,text_str_misc_separator);
	menuLine++;

#if defined(__PSP2__) || defined(__SWITCH__)
	// MENUMISC_MOUSEEMULATION
	write_text(leftMargin,menuLine,"Mouse");	
	if ((mainMenu_mouseEmulation==0) && ((menuMisc!=MENUMISC_MOUSEEMULATION)||(bb)))
		write_text_inv(tabstop1-8,menuLine,"Off");
	else
		write_text(tabstop1-8,menuLine,"Off");
	if ((mainMenu_mouseEmulation==1) && ((menuMisc!=MENUMISC_MOUSEEMULATION)||(bb)))
		write_text_inv(tabstop3-8,menuLine,"On");
	else
		write_text(tabstop3-8,menuLine,"On");	
	write_text(tabstop3-5,menuLine,"(can disturb 2nd player)");

	// MENUMISC_LEFTSTICKMOUSE
	menuLine+=2;
	write_text(leftMargin,menuLine,"Mouse Control");	
	if (mainMenu_leftStickMouse==0)
	{
		if ((menuMisc!=MENUMISC_LEFTSTICKMOUSE)||(bb))
			write_text_inv(tabstop2,menuLine,"Right Stick");
		else
			write_text(tabstop2,menuLine,"Right Stick  ");
	} 
	else if (mainMenu_leftStickMouse==1)
	{
		if ((menuMisc!=MENUMISC_LEFTSTICKMOUSE)||(bb))
			write_text_inv(tabstop2,menuLine,"Left Stick");
		else
			write_text(tabstop2,menuLine,"Left Stick  ");
	}
#endif

	// MENUMISC_MOUSEMULTIPLIER
	menuLine+=2;
	write_text(leftMargin,menuLine,text_str_mouse_multiplier);

	snprintf((char*)cpuSpeed, 8, "%4.2f", mainMenu_mouseMultiplier/100.0f);
	if ((menuMisc!=MENUMISC_MOUSEMULTIPLIER)||(bb))
		write_text_inv(tabstop2,menuLine,cpuSpeed);
	else
		write_text(tabstop2,menuLine,cpuSpeed);

#ifdef __SWITCH__
	// MENUMISC_SWAPAB
	write_text(tabstop6-2,menuLine,"Menu OK");
	if (mainMenu_swapAB==0)
	{
		if ((menuMisc!=MENUMISC_SWAPAB)||(bb))
			write_text_inv(tabstop9,menuLine,"B Btn.");
		else
			write_text(tabstop9,menuLine,"B Btn. ");
	}
	else if (mainMenu_swapAB==1)
	{
		if ((menuMisc!=MENUMISC_SWAPAB)||(bb))
			write_text_inv(tabstop9,menuLine,"A Btn.");
		else
			write_text(tabstop9,menuLine,"A Btn. ");
	}
#endif

#if !defined(__PSP2__) && !defined(__SWITCH__)
	// MENUMISC_STYLUSOFFSET
	menuLine+=2;
	write_text(leftMargin,menuLine,text_str_stylus_offset);

	if ((mainMenu_stylusOffset==0)&&((menuMisc!=MENUMISC_STYLUSOFFSET)||(bb)))
		write_text_inv(tabstop1,menuLine,text_str_0px);
	else
		write_text(tabstop1,menuLine,text_str_0px);

	if ((mainMenu_stylusOffset==2)&&((menuMisc!=MENUMISC_STYLUSOFFSET)||(bb)))
		write_text_inv(tabstop3,menuLine,text_str_1px);
	else
		write_text(tabstop3,menuLine,text_str_1px);

	if ((mainMenu_stylusOffset==6)&&((menuMisc!=MENUMISC_STYLUSOFFSET)||(bb)))
		write_text_inv(tabstop5,menuLine,text_str_3px);
	else
		write_text(tabstop5,menuLine,text_str_3px);

	if ((mainMenu_stylusOffset==10)&&((menuMisc!=MENUMISC_STYLUSOFFSET)||(bb)))
		write_text_inv(tabstop7,menuLine,text_str_5px);
	else
		write_text(tabstop7,menuLine,text_str_5px);

	if ((mainMenu_stylusOffset==16)&&((menuMisc!=MENUMISC_STYLUSOFFSET)||(bb)))
		write_text_inv(tabstop9,menuLine,text_str_8px);
	else
		write_text(tabstop9,menuLine,text_str_8px);
#endif //__PSP2__
#if defined(__PSP2__) || defined(__SWITCH__)

	//Analog Stick Deadzone settings on Vita
	//MENUMISC_DEADZONE
	menuLine+=2;
	write_text(leftMargin,menuLine,"Mouse Deadzone");
  	snprintf((char*)cpuSpeed, 8, "%d", mainMenu_deadZone);
  	if ((menuMisc!=MENUMISC_DEADZONE)||(bb))
		write_text_inv(tabstop2,menuLine,cpuSpeed);
	else
		write_text(tabstop2,menuLine,cpuSpeed);

	// MENUMISC_TOUCHCONTROLS
	write_text(tabstop6-2,menuLine,"Touch");
	if (mainMenu_touchControls==0)
	{
		if ((menuMisc!=MENUMISC_TOUCHCONTROLS)||(bb))
			write_text_inv(tabstop9,menuLine,"Off");
		else
			write_text(tabstop9,menuLine,"Off  ");
	}
	else if (mainMenu_touchControls==1)
	{
		if ((menuMisc!=MENUMISC_TOUCHCONTROLS)||(bb))
			write_text_inv(tabstop9,menuLine,"Front");
		else
			write_text(tabstop9,menuLine,"Front  ");
	}
	else if (mainMenu_touchControls==2)
	{
		if ((menuMisc!=MENUMISC_TOUCHCONTROLS)||(bb))
#ifdef __SWITCH__
			write_text_inv(tabstop9,menuLine,"Front");
#else
			write_text_inv(tabstop9,menuLine,"Both");
#endif
		else
#ifdef __SWITCH__
			write_text(tabstop9,menuLine,"Front  ");
#else
			write_text(tabstop9,menuLine,"Both  ");
#endif
	}

#else
	// MENUMISC_TAPDELAY
	menuLine+=2;
	write_text(leftMargin,menuLine,text_str_tap_delay);

	if ((mainMenu_tapDelay==10)&&((menuMisc!=MENUMISC_TAPDELAY)||(bb)))
		write_text_inv(tabstop1,menuLine,text_str_normal);
	else
		write_text(tabstop1,menuLine,text_str_normal);

	if ((mainMenu_tapDelay==5)&&((menuMisc!=MENUMISC_TAPDELAY)||(bb)))
		write_text_inv(tabstop5,menuLine,text_str_short);
	else
		write_text(tabstop5,menuLine,text_str_short);

	if ((mainMenu_tapDelay==2)&&((menuMisc!=MENUMISC_TAPDELAY)||(bb)))
		write_text_inv(tabstop9,menuLine,text_str_none);
	else
		write_text(tabstop9,menuLine,text_str_none);
#endif

	text_flip();
	b++;
}

static int key_miscMenu(int *c)
{
	int end=0;
	int left=0, right=0, up=0, down=0, hit0=0, hit1=0, hit2=0;
	SDL_Event event;

	while (SDL_PollEvent(&event) > 0)
	{
		left=right=up=down=hit0=hit1=hit2=0;
		if (event.type == SDL_KEYDOWN)
		{
			uae4all_play_click();
			switch(event.key.keysym.sym)
			{
			case SDLK_RIGHT: right=1; break;
			case SDLK_LEFT: left=1; break;
			case SDLK_UP: up=1; break;
			case SDLK_DOWN: down=1; break;
			case SDLK_PAGEDOWN: hit0=1; break;
			case SDLK_HOME: hit0=1; break;
			case SDLK_LALT: hit1=1; break;
			case SDLK_END: hit0=1; break;
			case SDLK_PAGEUP: hit0=1;
			case SDLK_LCTRL: hit2=1; break; //allow user to quit menu completely at any time
				//note SDLK_CTRL corresponds to ButtonSelect on Vita
			}
		}
		if (hit2) //Does the user want to cancel the menu completely?
		{
			if (emulating)
			{
				end = -1; 
				quit_pressed_in_submenu = 1; //Tell the mainMenu to cancel, too
			}
		}	
		else if (hit0)
		{
			end = -1;
		}
		else if (hit1)
		{
			end = -1;
		}
		else if (up)
		{
			if (menuMisc==0) menuMisc=MENUMISC_END-1;
			else menuMisc--;
		}
		else if (down)
		{
			if (menuMisc==MENUMISC_END-1) menuMisc=0;
			else menuMisc++;
		}
		switch (menuMisc)
		{
			case MENUMISC_CPU:
				if (left)
				{
				    if (mainMenu_CPU_model > 0)
				       mainMenu_CPU_model--;
				    else
				       mainMenu_CPU_model=1;
				}
				else if (right)
				{
				    if (mainMenu_CPU_model < 1)
				       mainMenu_CPU_model++;
				    else
				       mainMenu_CPU_model=0;
				}
				UpdateCPUModelSettings();
				break;
			case MENUMISC_CHIPSET:
				if (left)
				{
					switch (mainMenu_chipset & 0xff) //low is chipset, high is blitter
					{
						case 1:
							mainMenu_chipset=(mainMenu_chipset & 0xff00) | 0;
							break;
						case 2:
							mainMenu_chipset=(mainMenu_chipset & 0xff00) | 1;
							break;
						case 0:
							mainMenu_chipset=(mainMenu_chipset & 0xff00) | 2;
							break;
						default:
							mainMenu_chipset=(mainMenu_chipset & 0xff00) | 0;
							break;
					}
				}
				else if (right)
				{
					switch (mainMenu_chipset & 0xff) 
					{
						case 0:
							mainMenu_chipset=(mainMenu_chipset & 0xff00) | 1;
							break;
						case 1:
							mainMenu_chipset=(mainMenu_chipset & 0xff00) | 2;
							break;
						case 2:
							mainMenu_chipset=(mainMenu_chipset & 0xff00) | 0;
							break;
						default:
							mainMenu_chipset=(mainMenu_chipset & 0xff00) | 0;
							break;
					}
				}
				UpdateChipsetSettings();
				break;
			
			case MENUMISC_BLITTER:
				if (left)
				{
					switch (mainMenu_chipset & 0xff00) //low is chipset, high is blitter
					{
						case 0x200:
							mainMenu_chipset=(mainMenu_chipset & 0xff) | 0x100;
							break;
						case 0x100:
							mainMenu_chipset=(mainMenu_chipset & 0xff) | 0;
							break;
						case 0:
							mainMenu_chipset=(mainMenu_chipset & 0xff) | 0x200;
							break;
						default:
							mainMenu_chipset=(mainMenu_chipset & 0xff) | 0;
							break;
					}
				}
				else if (right)
				{
					switch (mainMenu_chipset & 0xff00)
					{
						case 0x200:
							mainMenu_chipset=(mainMenu_chipset & 0xff) | 0;
							break;
						case 0x100:
							mainMenu_chipset=(mainMenu_chipset & 0xff) | 0x200;
							break;
						case 0:
							mainMenu_chipset=(mainMenu_chipset & 0xff) | 0x100;
							break;
						default:
							mainMenu_chipset=(mainMenu_chipset & 0xff) | 0;
							break;
					}
				}
				UpdateChipsetSettings();
				break;
				
			case MENUMISC_SPRITECOLLISIONS:
				if (left||right)
			   	mainMenu_spriteCollisions = !mainMenu_spriteCollisions;
			   break;
		
			case MENUMISC_KICKSTART:
				if (left)
				{
					if (kickstart>0)
						kickstart--;
					else
						kickstart=3;
				}
				else if (right)
				{
					if (kickstart<3)
						kickstart++;
					else
						kickstart=0;
				}
				break;
			case MENUMISC_CPUSPEED:
				if (left)
				{
					if (mainMenu_CPU_speed>0)
						mainMenu_CPU_speed--;
					else
						mainMenu_CPU_speed=2;
				}
				else if (right)
				{
					if (mainMenu_CPU_speed<2)
						mainMenu_CPU_speed++;
					else
						mainMenu_CPU_speed=0;
				}
				break;

#if !defined(__PSP2__) && !defined(__SWITCH__)
#ifdef PANDORA
			case MENUMISC_PANDORASPEED:
				if(left)
					mainMenu_cpuSpeed-=10;
				else if(right)
					mainMenu_cpuSpeed+=10;
				break;
#endif
#endif //__PSP2__
#if defined(__PSP2__) || defined(__SWITCH__)
			case MENUMISC_LEFTSTICKMOUSE:
				if (left || right) 
					mainMenu_leftStickMouse = !mainMenu_leftStickMouse;
				break;
			case MENUMISC_TOUCHCONTROLS:
#ifdef __SWITCH__
				if (mainMenu_touchControls == 2)
					mainMenu_touchControls = 1;
				if (left || right)
					mainMenu_touchControls = !mainMenu_touchControls;
				break;
#else
				if (left) 
				{
					if (mainMenu_touchControls>0)
						mainMenu_touchControls--;
					else
						mainMenu_touchControls=2;
				}
				else if (right) 
				{
					if (mainMenu_touchControls<2)
						mainMenu_touchControls++;
					else
						mainMenu_touchControls=0;
				}
				break;
#endif
#ifdef __SWITCH__
				case MENUMISC_SWAPAB:
					if (left || right)
						mainMenu_swapAB = !mainMenu_swapAB;
				break;
#endif
#endif

#ifdef ANDROIDSDL
			case MENUMISC_ONSCREEN:
				if ((left)||(right))
					mainMenu_onScreen = !mainMenu_onScreen;
				break;
#endif
			case MENUMISC_CONTROLCFG:
				if (left)
				{
					if (mainMenu_joyConf>0)
						mainMenu_joyConf--;
					else
						mainMenu_joyConf=3;
				}
				else if (right)
				{
					if (mainMenu_joyConf<3)
						mainMenu_joyConf++;
					else
						mainMenu_joyConf=0;
				}
 				break;
			case MENUMISC_JOYSTICK:
				if (left)
				{
					if (mainMenu_joyPort>0)
						mainMenu_joyPort--;
					else
						mainMenu_joyPort=2;
				}
				else if (right)
				{
					if (mainMenu_joyPort<2)
						mainMenu_joyPort++;
					else
						mainMenu_joyPort=0;
				}
				break;  
			case MENUMISC_AUTOFIRERATE:
				if(left)
				{
					if(mainMenu_autofireRate==2)
						mainMenu_autofireRate=4;
					else if(mainMenu_autofireRate==4)
						mainMenu_autofireRate=8;
					else
						mainMenu_autofireRate=2;
				}
				else if (right)
				{
					if(mainMenu_autofireRate==2)
						mainMenu_autofireRate=8;
					else if(mainMenu_autofireRate==8)
						mainMenu_autofireRate=4;
					else
						mainMenu_autofireRate=2;
				}
 				break;
#if defined(__PSP2__) || defined(__SWITCH__)
			case MENUMISC_CUSTOMAUTOFIREBUTTON:
				if (left)
				{
					if (mainMenu_customAutofireButton > 0)
						mainMenu_customAutofireButton--;
					else
						mainMenu_customAutofireButton=0;
				}
				else if (right)
				{
#ifdef __SWITCH__
					if (mainMenu_customAutofireButton < 8)
						mainMenu_customAutofireButton++;
					else
						mainMenu_customAutofireButton=8;
#else
					if (mainMenu_customAutofireButton < 6)
						mainMenu_customAutofireButton++;
					else
						mainMenu_customAutofireButton=6;
#endif
				}
				break;
#endif
			case MENUMISC_MOUSEMULTIPLIER:
				if (left)
				{
					if (mainMenu_mouseMultiplier <= 50)
						mainMenu_mouseMultiplier = 25;
					else
						mainMenu_mouseMultiplier -= 25;
				}
				else if (right)
				{
					if (mainMenu_mouseMultiplier >= 375)
						mainMenu_mouseMultiplier = 400;
					else
						mainMenu_mouseMultiplier += 25;
				}
				break;
#if !defined(__PSP2__) && !defined(__SWITCH__)
			case MENUMISC_STYLUSOFFSET:
				if (left)
				{
					if (mainMenu_stylusOffset == 0)
						mainMenu_stylusOffset = 16;
					else if (mainMenu_stylusOffset == 2)
						mainMenu_stylusOffset = 0;
					else if (mainMenu_stylusOffset == 6)
						mainMenu_stylusOffset = 2;
					else if (mainMenu_stylusOffset == 10)
						mainMenu_stylusOffset = 6;
					else
						mainMenu_stylusOffset = 10;
				}
				else if (right)
				{
					if (mainMenu_stylusOffset == 0)
						mainMenu_stylusOffset = 2;
					else if (mainMenu_stylusOffset == 2)
						mainMenu_stylusOffset = 6;
					else if (mainMenu_stylusOffset == 6)
						mainMenu_stylusOffset = 10;
					else if (mainMenu_stylusOffset == 10)
						mainMenu_stylusOffset = 16;
					else
						mainMenu_stylusOffset = 0;
				}
				break;
#endif //__PSP2__

#if defined(__PSP2__) || defined(__SWITCH__)
			case MENUMISC_DEADZONE:
				if (left)
				{
					if (mainMenu_deadZone <= 0)
						mainMenu_deadZone=0;
					else if (mainMenu_deadZone >= 20000)
						mainMenu_deadZone-=10000;
					else if (mainMenu_deadZone >= 2000)
						mainMenu_deadZone-=1000;
					else if (mainMenu_deadZone >= 200)
						mainMenu_deadZone-=100;
					else if (mainMenu_deadZone >= 20)
						mainMenu_deadZone-=10;
					else if (mainMenu_deadZone >= 1)
						mainMenu_deadZone-=1;
				}
				else if (right)
				{
					if (mainMenu_deadZone >= 30000)
						mainMenu_deadZone=30000;
					else if (mainMenu_deadZone >= 10000)
						mainMenu_deadZone+=10000;
					else if (mainMenu_deadZone>=1000)
						mainMenu_deadZone+=1000;
					else if (mainMenu_deadZone>=100)
						mainMenu_deadZone+=100;
					else if (mainMenu_deadZone>=10)
						mainMenu_deadZone+=10;
					else if (mainMenu_deadZone>=0)
						mainMenu_deadZone+=1;
				}
				break;
			case MENUMISC_MOUSEEMULATION:
				if ((left)||(right))
					mainMenu_mouseEmulation = !mainMenu_mouseEmulation;
				break;
#else
			case MENUMISC_TAPDELAY:
				if (left)
				{
					if (mainMenu_tapDelay == 10)
						mainMenu_tapDelay = 2;
					else if (mainMenu_tapDelay == 5)
						mainMenu_tapDelay = 10;
					else
						mainMenu_tapDelay = 5;
				}
				else if (right)
				{
					if (mainMenu_tapDelay == 10)
						mainMenu_tapDelay = 5;
					else if (mainMenu_tapDelay == 5)
						mainMenu_tapDelay = 2;
					else
						mainMenu_tapDelay = 10;
				}
				break;
#endif
		}
	}

	return end;
}

static void raise_miscMenu()
{
	int i;

	text_draw_background();
	text_flip();
#if !defined(__PSP2__) && !defined(__SWITCH__)
	for(i=0;i<10;i++)
	{
		text_draw_background();
		text_draw_window(80-64,(10-i)*24,160+64+64,220,text_str_misc_title);
		text_flip();
	}
#endif
}

static void unraise_miscMenu()
{
	int i;
#if !defined(__PSP2__) && !defined(__SWITCH__)
	for(i=9;i>=0;i--)
	{
		text_draw_background();
		text_draw_window(80-64,(10-i)*24,160+64+64,220,text_str_misc_title);
		text_flip();
	}
#endif
	text_draw_background();
	text_flip();
}

int run_menuMisc()
{
	SDL_Event event;
	SDL_Delay(150);
	while(SDL_PollEvent(&event))
		SDL_Delay(10);
	int end=0, c=0;
	raise_miscMenu();
	while(!end)
	{
		draw_miscMenu(c);
		end=key_miscMenu(&c);
	}
	set_joyConf();
	unraise_miscMenu();
	return end;
}
