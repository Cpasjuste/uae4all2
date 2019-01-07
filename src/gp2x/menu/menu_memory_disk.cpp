#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "sysconfig.h"
#include "sysdeps.h"
#include "config.h"
#include "menu.h"
#include "menu_config.h"

#include <sys/stat.h>
#include <unistd.h>
#if defined(__PSP2__) // NOT __SWITCH__
#include "psp2-dirent.h"
#else
#include <dirent.h>
#endif

#include "autoconf.h"
#include "uae.h"
#include "options.h"
#include "sound.h"
#include "gui.h"
#include <SDL.h>
#include "gp2x.h"
#include <SDL_ttf.h>

/* PocketUAE config file. Used for parsing PocketUAE-like options. */
#include "cfgfile.h" 

#if defined(__PSP2__) || defined(__SWITCH__)
#define SDL_PollEvent PSP2_PollEvent
#endif

const char *text_str_memdisk_separator="--------------------------------------";
static const char *text_str_memdisk_title=    "     Memory and Disk Options     -";
const char *text_str_off="off";
const char *text_str_on="on";
const char *text_str_512K="512K";
const char *text_str_1M="1M";
const char *text_str_1_5M="1.5M";
const char *text_str_2M="2M";
const char *text_str_4M="4M";
const char *text_str_8M="8M";

int menuMemDisk = 0;
int current_hdf = 0;
extern int quit_pressed_in_submenu;
extern int emulating;

enum { 
	MENUDISK_RETURNMAIN = 0,
	MENUDISK_CHIPMEM,
	MENUDISK_SLOWMEM,
	MENUDISK_FASTMEM,
	MENUDISK_BOOTHD,
	MENUDISK_HDDIR,
	MENUDISK_HDFILE,
	MENUDISK_SAVEHDCONF,
	MENUDISK_FLOPPYSPEED,
	MENUDISK_END
};

extern char currentDir[300];
extern void reset_hdConf();

static void draw_memDiskMenu(int c)
{
	static int b=0;
	int bb=(b%6)/3;		/* Inverted/normal selection drawing */

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
	SDL_Rect r;
	extern SDL_Surface *text_screen;

	r.x=80-64; r.y=0; r.w=110+64+64; r.h=240;

	text_draw_background();
	text_draw_window(2,2,40,30,text_str_memdisk_title);
	
	if ((menuMemDisk == 0)&&(c==MENUDISK_RETURNMAIN)&&(bb))
		write_text_inv(3, menuLine, "Return to main menu");
	else
		write_text(3, menuLine, "Return to main menu");

	menuLine++;
	write_text(3, menuLine, text_str_memdisk_separator);
	menuLine++;
	
	write_text(leftMargin,menuLine,"Chip Memory");
	if ((mainMenu_chipMemory==0)&&((c!=MENUDISK_CHIPMEM)||(!bb)))
		write_text_inv(tabstop3 + 4,menuLine,text_str_512K);
	else
		write_text(tabstop3 + 4,menuLine,text_str_512K);
	
	if ((mainMenu_chipMemory==1)&&((c!=MENUDISK_CHIPMEM)||(!bb)))
		write_text_inv(tabstop3 + 9,menuLine,text_str_1M);
	else
		write_text(tabstop3 + 9,menuLine,text_str_1M);

	if ((mainMenu_chipMemory==2)&&((c!=MENUDISK_CHIPMEM)||(!bb)))
		write_text_inv(tabstop3 + 12,menuLine,text_str_2M);
	else
		write_text(tabstop3 + 12,menuLine,text_str_2M);

	if ((mainMenu_chipMemory==3)&&((c!=MENUDISK_CHIPMEM)||(!bb)))
		write_text_inv(tabstop3 + 15,menuLine,text_str_4M);
	else
		write_text(tabstop3 + 15,menuLine,text_str_4M);
	
	if ((mainMenu_chipMemory==4)&&((c!=MENUDISK_CHIPMEM)||(!bb)))
		write_text_inv(tabstop3 + 18,menuLine,text_str_8M);
	else
		write_text(tabstop3 + 18,menuLine,text_str_8M);
	
	menuLine+=2;
	write_text(leftMargin,menuLine,"Slow Memory");
	if ((mainMenu_slowMemory==0)&&((c!=MENUDISK_SLOWMEM)||(!bb)))
		write_text_inv(tabstop3,menuLine,text_str_off);
	else
		write_text(tabstop3,menuLine,text_str_off);
	
	if ((mainMenu_slowMemory==1)&&((c!=MENUDISK_SLOWMEM)||(!bb)))
		write_text_inv(tabstop3 + 4,menuLine,text_str_512K);
	else
		write_text(tabstop3 + 4,menuLine,text_str_512K);
	
	if ((mainMenu_slowMemory==2)&&((c!=MENUDISK_SLOWMEM)||(!bb)))
		write_text_inv(tabstop3 + 9,menuLine,text_str_1M);
	else
		write_text(tabstop3 + 9,menuLine,text_str_1M);

	if ((mainMenu_slowMemory==3)&&((c!=MENUDISK_SLOWMEM)||(!bb)))
		write_text_inv(tabstop3 + 12,menuLine,text_str_1_5M);
	else
		write_text(tabstop3 + 12,menuLine,text_str_1_5M);
	
	menuLine+=2;
	write_text(leftMargin,menuLine,"Fast Memory");
	if ((mainMenu_fastMemory==0)&&((c!=MENUDISK_FASTMEM)||(!bb)))
		write_text_inv(tabstop3,menuLine,text_str_off);
	else
		write_text(tabstop3,menuLine,text_str_off);

	if ((mainMenu_fastMemory==1)&&((c!=MENUDISK_FASTMEM)||(!bb)))
		write_text_inv(tabstop3 + 9,menuLine,text_str_1M);
	else
		write_text(tabstop3 + 9,menuLine,text_str_1M);
	
	if ((mainMenu_fastMemory==2)&&((c!=MENUDISK_FASTMEM)||(!bb)))
		write_text_inv(tabstop3 + 12,menuLine,text_str_2M);
	else
		write_text(tabstop3 + 12,menuLine,text_str_2M);

	if ((mainMenu_fastMemory==3)&&((c!=MENUDISK_FASTMEM)||(!bb)))
		write_text_inv(tabstop3 + 15,menuLine,text_str_4M);
	else
		write_text(tabstop3 + 15,menuLine,text_str_4M);

	if ((mainMenu_fastMemory==4)&&((c!=MENUDISK_FASTMEM)||(!bb)))
		write_text_inv(tabstop3 + 18,menuLine,text_str_8M);
	else
		write_text(tabstop3 + 18,menuLine,text_str_8M);

	menuLine++;
	write_text(3, menuLine, text_str_memdisk_separator);
	menuLine++;
	
	write_text(leftMargin,menuLine,"Boot HD");
	if ((mainMenu_bootHD==0)&&((c!=MENUDISK_BOOTHD)||(!bb)))
		write_text_inv(tabstop3,menuLine,text_str_off);
	else
		write_text(tabstop3,menuLine,text_str_off);

	if ((mainMenu_bootHD==1)&&((c!=MENUDISK_BOOTHD)||(!bb)))
		write_text_inv(tabstop3 + 7,menuLine,"Dir");
	else
		write_text(tabstop3 + 7,menuLine,"Dir");
	
	if ((mainMenu_bootHD==2)&&((c!=MENUDISK_BOOTHD)||(!bb)))
		write_text_inv(tabstop3 + 12,menuLine,"File1");
	else
		write_text(tabstop3 + 12,menuLine,"File1");
	
	menuLine+=2;

	{
		char str[256];
		int i;
		
		strcpy(str, "HD Dir");
		if ((c==MENUDISK_HDDIR)&&(bb))
			write_text_inv(leftMargin + 2,menuLine,str);
		else
			write_text(leftMargin + 2,menuLine,str);
		for (i = strlen(uae4all_hard_dir); i > 0; i--)
			if ((uae4all_hard_dir[i] == '/')||(uae4all_hard_dir[i] == '\\'))
				break;
		if (i > 0) {
			strcpy(str, &uae4all_hard_dir[i+1]);
			if (strlen(str) > (MENU_MEMDISK_WINDOW_WIDTH - tabstop1))
				str[MENU_MEMDISK_WINDOW_WIDTH - tabstop1] = '\0';
			write_text(tabstop1,menuLine,str);
		} else
			write_text(tabstop1,menuLine,"");
		
		menuLine += 2;
		
		strcpy(str, "HD File1");
		if ((c==MENUDISK_HDFILE)&&(bb)&&current_hdf==0)
			write_text_inv(leftMargin + 2,menuLine,str);
		else
			write_text(leftMargin + 2,menuLine,str);
		for (i = strlen(uae4all_hard_file0); i > 0; i--)
			if ((uae4all_hard_file0[i] == '/') || (uae4all_hard_file0[i] == '\\'))
				break;
		if (i > 0) {
			strcpy(str, &uae4all_hard_file0[i+1]);
			if (strlen(str) > MENU_MEMDISK_WINDOW_WIDTH - tabstop1)
				str[MENU_MEMDISK_WINDOW_WIDTH - tabstop1] = '\0';
			write_text(tabstop1,menuLine,str);
		} else
			write_text(tabstop1,menuLine,"");

		menuLine += 2;

		strcpy(str, "HD File2");
		if ((c==MENUDISK_HDFILE)&&(bb)&&current_hdf==1)
			write_text_inv(leftMargin + 2,menuLine,str);
		else
			write_text(leftMargin + 2,menuLine,str);
		for (i = strlen(uae4all_hard_file1); i > 0; i--)
			if ((uae4all_hard_file1[i] == '/') || (uae4all_hard_file1[i] == '\\'))
				break;
		if (i > 0) {
			strcpy(str, &uae4all_hard_file1[i+1]);
			if (strlen(str) > MENU_MEMDISK_WINDOW_WIDTH - tabstop1)
				str[MENU_MEMDISK_WINDOW_WIDTH - tabstop1] = '\0';
			write_text(tabstop1,menuLine,str);
		} else
			write_text(tabstop1,menuLine,"");

		menuLine += 2;

		strcpy(str, "HD File3");
		if ((c==MENUDISK_HDFILE)&&(bb)&&current_hdf==2)
			write_text_inv(leftMargin + 2,menuLine,str);
		else
			write_text(leftMargin + 2,menuLine,str);
		for (i = strlen(uae4all_hard_file2); i > 0; i--)
			if ((uae4all_hard_file2[i] == '/') || (uae4all_hard_file2[i] == '\\'))
				break;
		if (i > 0) {
			strcpy(str, &uae4all_hard_file2[i+1]);
			if (strlen(str) > MENU_MEMDISK_WINDOW_WIDTH - tabstop1)
				str[MENU_MEMDISK_WINDOW_WIDTH - tabstop1] = '\0';
			write_text(tabstop1,menuLine,str);
		} else
			write_text(tabstop1,menuLine,"");
			
				menuLine += 2;

		strcpy(str, "HD File4");
		if ((c==MENUDISK_HDFILE)&&(bb)&&current_hdf==3)
			write_text_inv(leftMargin + 2,menuLine,str);
		else
			write_text(leftMargin + 2,menuLine,str);
		for (i = strlen(uae4all_hard_file3); i > 0; i--)
			if ((uae4all_hard_file3[i] == '/') || (uae4all_hard_file3[i] == '\\'))
				break;
		if (i > 0) {
			strcpy(str, &uae4all_hard_file3[i+1]);
			if (strlen(str) > MENU_MEMDISK_WINDOW_WIDTH - tabstop1)
				str[MENU_MEMDISK_WINDOW_WIDTH - tabstop1] = '\0';
			write_text(tabstop1,menuLine,str);
		} else
			write_text(tabstop1,menuLine,"");

	}

	menuLine += 2;
	
	write_text(3, menuLine, "(Press triangle to eject HD)");
	
	menuLine += 2;
		
	if ((c==MENUDISK_SAVEHDCONF)&&(bb))
		write_text_inv(3, menuLine, "Save Config for current HD");
	else
		write_text(3, menuLine, "Save Config for current HD");
	
	menuLine++;
	write_text(3, menuLine, text_str_memdisk_separator);
	menuLine++;

	write_text(leftMargin,menuLine,"Floppy speed:");

	if ((mainMenu_floppyspeed==100)&&((c!=MENUDISK_FLOPPYSPEED)||(bb)))
		write_text_inv(tabstop3,menuLine,"1x");
	else
		write_text(tabstop3,menuLine,"1x");

	if ((mainMenu_floppyspeed==200)&&((c!=MENUDISK_FLOPPYSPEED)||(bb)))
		write_text_inv(tabstop4+1,menuLine,"2x");
	else
		write_text(tabstop4+1,menuLine,"2x");

	if ((mainMenu_floppyspeed==400)&&((c!=MENUDISK_FLOPPYSPEED)||(bb)))
		write_text_inv(tabstop5+2,menuLine,"4x");
	else
		write_text(tabstop5+2,menuLine,"4x");

	if ((mainMenu_floppyspeed==800)&&((c!=MENUDISK_FLOPPYSPEED)||(bb)))
		write_text_inv(tabstop6+3,menuLine,"8x");
	else
		write_text(tabstop6+3,menuLine,"8x");

	menuLine++;
	write_text(3, menuLine, text_str_memdisk_separator);
	menuLine++;

	text_flip();

	b++;
}


static int key_memDiskMenu(int *c)
{
	int end=0;
	int left=0, right=0, up=0, down=0, hit0=0, hit1=0, hit2=0, del=0;

	SDL_Event event;

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
		left=right=up=down=hit0=hit1=hit2=del=0;
		if (event.type == SDL_QUIT)
			end=-1;
		else if (event.type == SDL_KEYDOWN)
		{
			uae4all_play_click();
			switch(event.key.keysym.sym)
			{
				case SDLK_RIGHT: right=1; break;
				case SDLK_LEFT: left=1; break;
				case SDLK_UP: up=1; break;
				case SDLK_DOWN: down=1; break;
				case SDLK_PAGEDOWN: hit0=1; break;
				case SDLK_END: hit1=1; break;
				case SDLK_DELETE: case SDLK_BACKSPACE: 
				case SDLK_ESCAPE: case SDLK_PAGEUP: del=1; break;
				case SDLK_LCTRL: hit2=1; break; //allow user to quit menu completely at any time
				//note SDLK_LCTRL corresponds to ButtonSelect on Vita
#if !defined(__PSP2__) && !defined(__SWITCH__)
				case SDLK_HOME: hit0=1; break;
				case SDLK_LALT: hit1=1; break;
				case SDLK_RSHIFT: hit0=1; break;
				case SDLK_RCTRL: hit0=1; break;
#endif
				default:
					break;
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
		else if (hit1)
		{
			end=-1;
		}
		else if (up)
		{
			if (menuMemDisk==MENUDISK_RETURNMAIN) menuMemDisk=MENUDISK_END - 1;
			else if (menuMemDisk==MENUDISK_HDFILE && current_hdf>0) 
					current_hdf--;
			else if (menuMemDisk==MENUDISK_SAVEHDCONF)
			{
				current_hdf=3;
				menuMemDisk=MENUDISK_HDFILE;
			}
			else
				menuMemDisk--;
		}
		else if (down)
		{
			if (menuMemDisk==MENUDISK_END - 1) menuMemDisk=MENUDISK_RETURNMAIN;
			else if (menuMemDisk==MENUDISK_HDFILE && current_hdf<3) 
					current_hdf++;
			else if (menuMemDisk==MENUDISK_HDDIR)
			{
				current_hdf=0;
				menuMemDisk=MENUDISK_HDFILE;
			}
			else
				menuMemDisk++;	
		}
		switch (menuMemDisk)
		{
			case MENUDISK_RETURNMAIN:
				if (hit0)
					end = 1;
				break;
			case MENUDISK_CHIPMEM:
				if ((left)||(right)) {
					if (right) {
						if (mainMenu_chipMemory < 4)
							mainMenu_chipMemory++;
						else
							mainMenu_chipMemory = 0;
					} else if (left) {
						if (mainMenu_chipMemory > 0)
							mainMenu_chipMemory--;
						else
							mainMenu_chipMemory = 4;
					}
					UpdateMemorySettings();
				}
				break;
			case MENUDISK_SLOWMEM:
				if ((left)||(right)) {
					if (right) {
						if (mainMenu_slowMemory < 3)
							mainMenu_slowMemory++;
						else
							mainMenu_slowMemory = 0;
					} else if (left) {
						if (mainMenu_slowMemory > 0)
							mainMenu_slowMemory--;
						else
							mainMenu_slowMemory = 3;
					}
					UpdateMemorySettings();
				}
				break;
			case MENUDISK_FASTMEM:
				if ((left) || (right)) {
					if (right) {
						if (mainMenu_fastMemory < 4)
							mainMenu_fastMemory++;
						else
							mainMenu_fastMemory = 0;
					} else if (left) {
						if (mainMenu_fastMemory > 0)
							mainMenu_fastMemory--;
						else
							mainMenu_fastMemory = 4;
					}
				
					/* Fast memory > 0 => max 2MB chip memory */
					if ((mainMenu_fastMemory > 0) && (mainMenu_chipMemory > 2))
						mainMenu_chipMemory = 2;
					UpdateMemorySettings();
				}
				break;
			case MENUDISK_BOOTHD:
				if (left) {
					if (mainMenu_bootHD > 0)
						mainMenu_bootHD--;
					else
						mainMenu_bootHD = 2;
					reset_hdConf();
				} else if (right) {
					if (mainMenu_bootHD < 2)
						mainMenu_bootHD++;
					else
						mainMenu_bootHD = 0;
					reset_hdConf();
				}
				break;
			case MENUDISK_HDDIR:
				if (hit0) {
					if (run_menuLoad(currentDir, MENU_LOAD_HD_DIR)) {
						make_hard_dir_cfg_line(uae4all_hard_dir);
						reset_hdConf();
						loadconfig(4);
					}
				} else if (del) {
					uae4all_hard_dir[0] = '\0';
					reset_hdConf();
				}
				break;
			case MENUDISK_HDFILE:
				if (hit0) {
					if (run_menuLoad(currentDir, MENU_LOAD_HDF)) {
						if (current_hdf==0)
							make_hard_file_cfg_line(uae4all_hard_file0);
						else if (current_hdf==1)
							make_hard_file_cfg_line(uae4all_hard_file1);
						else if (current_hdf==2)
							make_hard_file_cfg_line(uae4all_hard_file2);
						else if (current_hdf==3)
							make_hard_file_cfg_line(uae4all_hard_file3);
						reset_hdConf();
						if (current_hdf==0) {
							loadconfig(2);
						}
					}
				} else if (del) {
					if (current_hdf==0)
						uae4all_hard_file0[0] = '\0';
					else if (current_hdf==1)
						uae4all_hard_file1[0] = '\0';
					else if (current_hdf==2)
						uae4all_hard_file2[0] = '\0';
					else if (current_hdf==3)
						uae4all_hard_file3[0] = '\0';
					reset_hdConf();
				}
				break;
			case MENUDISK_SAVEHDCONF:
				if (hit0)
				{
					if (saveconfig(2))
						showWarning("Config saved for current HD");
				}
				break;
			case MENUDISK_FLOPPYSPEED:
				if (left)
				{
					if (mainMenu_floppyspeed>100)
						mainMenu_floppyspeed/=2;
					else
						mainMenu_floppyspeed=800;
				}
				else if (right)
				{
					if (mainMenu_floppyspeed<800)
						mainMenu_floppyspeed*=2;
					else
						mainMenu_floppyspeed=100;
				}
		}
	}
	
	*c = menuMemDisk;
	
	return end;
}

static void raise_memDiskMenu()
{
	int i;

	text_draw_background();
	text_flip();
#if !defined(__PSP2__) && !defined(__SWITCH__)
	for(i=0;i<10;i++)
	{
		text_draw_background();
		text_draw_window(80-64,(10-i)*24,160+64+64,220,text_str_memdisk_title);
		text_flip();
	}
#endif
}

static void unraise_memDiskMenu()
{
	int i;
#if !defined(__PSP2__) && !defined(__SWITCH__)
	for(i=9;i>=0;i--)
	{
		text_draw_background();
		text_draw_window(80-64,(10-i)*24,160+64+64,220,text_str_memdisk_title);
		text_flip();
	}
#endif
	text_draw_background();
	text_flip();
}

int run_menuMemDisk()
{
	SDL_Event event;
	
	SDL_Delay(150);
	while(SDL_PollEvent(&event))
		SDL_Delay(10);
	int end=0,c=0;
	raise_memDiskMenu();
	
	while(!end)
	{
		draw_memDiskMenu(c);
		end = key_memDiskMenu(&c);
	}
	
	unraise_memDiskMenu();
	return end;
}
