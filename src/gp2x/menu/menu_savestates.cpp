#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "sysconfig.h"
#include "sysdeps.h"
#include "config.h"
#include "menu.h"
#include "uae.h"
#include "options.h"
#include "sound.h"
#include "gui.h"
#include <SDL.h>
#include "gp2x.h"
#include <SDL_ttf.h>
#include "savestate.h"

#if defined(__PSP2__) || defined(__SWITCH__)
#define SDL_PollEvent PSP2_PollEvent
#endif

static const char *text_str_title="SaveStates";
static const char *text_str_savestate="SaveState";
static const char *text_str_0="0";
static const char *text_str_1="1";
static const char *text_str_2="2";
static const char *text_str_3="3";
static const char *text_str_4="4";
static const char *text_str_5="5";
static const char *text_str_6="6";
static const char *text_str_7="7";
static const char *text_str_8="8";
static const char *text_str_9="9";
static const char *text_str_10="10";
static const char *text_str_auto="auto";
static const char *text_str_loadmem="Load State";
static const char *text_str_savemem="Save State";
static const char *text_str_deletemem="Delete State";
static const char *text_str_savestateslocation="Location";
static const char *text_str_separator="-------------------------------------";
static const char *text_str_exit="Back to Main Menu";

extern int emulating;
extern int saveMenu_n_savestate;
extern int quit_pressed_in_submenu;
extern int mainMenu_bootHD;
extern int mainMenu_useSavesFolder;

extern void extractFileName(char * str,char *buffer);

int saveMenu_case=-1;

int leftMargin = 4;
int tabstop1 = 14;

enum { SAVE_MENU_CASE_EXIT, SAVE_MENU_CASE_LOAD_MEM, SAVE_MENU_CASE_SAVE_MEM, SAVE_MENU_CASE_DELETE_MEM, SAVE_MENU_CASE_LOAD_VMU, SAVE_MENU_CASE_SAVE_VMU, SAVE_MENU_CASE_CANCEL };

static inline void draw_savestatesMenu(int c)
{
	static int b=0;
	int bb=(b%6)/3;
	SDL_Rect r;
	extern SDL_Surface *text_screen;
	r.x=80-64; r.y=0; r.w=110+64+64; r.h=240;

	text_draw_background();
	text_draw_window(3,4,39,20,text_str_title);

	write_text(leftMargin,6,text_str_separator);
	
	write_text(leftMargin,7,text_str_savestate);

	if ((saveMenu_n_savestate==0)&&((c!=0)||(bb)))
		write_text_inv(tabstop1,7,text_str_0);
	else
		write_text(tabstop1,7,text_str_0);

	if ((saveMenu_n_savestate==1)&&((c!=0)||(bb)))
		write_text_inv(tabstop1+2,7,text_str_1);
	else
		write_text(tabstop1+2,7,text_str_1);

	if ((saveMenu_n_savestate==2)&&((c!=0)||(bb)))
		write_text_inv(tabstop1+4,7,text_str_2);
	else
		write_text(tabstop1+4,7,text_str_2);

	if ((saveMenu_n_savestate==3)&&((c!=0)||(bb)))
		write_text_inv(tabstop1+6,7,text_str_3);
	else
		write_text(tabstop1+6,7,text_str_3);

	if ((saveMenu_n_savestate==4)&&((c!=0)||(bb)))
		write_text_inv(tabstop1+8,7,text_str_4);
	else
		write_text(tabstop1+8,7,text_str_4);

	if ((saveMenu_n_savestate==5)&&((c!=0)||(bb)))
		write_text_inv(tabstop1+10,7,text_str_5);
	else
		write_text(tabstop1+10,7,text_str_5);
	
	if ((saveMenu_n_savestate==6)&&((c!=0)||(bb)))
		write_text_inv(tabstop1+12,7,text_str_6);
	else
		write_text(tabstop1+12,7,text_str_6);

	if ((saveMenu_n_savestate==7)&&((c!=0)||(bb)))
		write_text_inv(tabstop1+14,7,text_str_7);
	else
		write_text(tabstop1+14,7,text_str_7);

	if ((saveMenu_n_savestate==8)&&((c!=0)||(bb)))
		write_text_inv(tabstop1+16,7,text_str_8);
	else
		write_text(tabstop1+16,7,text_str_8);

	if ((saveMenu_n_savestate==9)&&((c!=0)||(bb)))
		write_text_inv(tabstop1+18,7,text_str_9);
	else
		write_text(tabstop1+18,7,text_str_9);

	if ((saveMenu_n_savestate==10)&&((c!=0)||(bb)))
		write_text_inv(tabstop1+20,7,text_str_10);
	else
		write_text(tabstop1+20,7,text_str_10);
	
	if ((saveMenu_n_savestate==11)&&((c!=0)||(bb)))
		write_text_inv(tabstop1+23,7,text_str_auto);
	else
		write_text(tabstop1+23,7,text_str_auto);

	write_text(leftMargin,8,text_str_separator);

	write_text(leftMargin,10,text_str_separator);

	if ((c==1)&&(bb))
		write_text_inv(leftMargin,11,text_str_loadmem);
	else
		write_text(leftMargin,11,text_str_loadmem);

	write_text(leftMargin,12,text_str_separator);

	if ((c==2)&&(bb))
		write_text_inv(leftMargin,13,text_str_savemem);
	else
		write_text(leftMargin,13,text_str_savemem);

	write_text(leftMargin,14,text_str_separator);

	if ((c==3)&&(bb))
		write_text_inv(leftMargin,15,text_str_deletemem);
	else
		write_text(leftMargin,15,text_str_deletemem);

	write_text(leftMargin,20,text_str_separator);

	write_text(leftMargin,21,text_str_savestateslocation);
	if (mainMenu_useSavesFolder==0)
	{
		if ((c!=4)||(bb))
			write_text_inv(tabstop1,21,"Same as ROM ");
		else
			write_text(tabstop1,21,"Same as ROM ");
	}
	else if (mainMenu_useSavesFolder==1)
	{
		if ((c!=4)||(bb))
			write_text_inv(tabstop1,21,"Saves Folder");
		else
			write_text(tabstop1,21,"Saves Folder");
	}

	write_text(leftMargin,22,text_str_separator);

	if ((c==5)&&(bb))
		write_text_inv(leftMargin,23,text_str_exit);
	else
		write_text(leftMargin,23,text_str_exit);
	write_text(leftMargin,24,text_str_separator);

	text_flip();
	b++;
}

static inline int key_saveMenu(int *cp)
{
	int c=(*cp);
	int back_c=-1;
	int end=0;
	static int delay=0;
	int left=0, right=0, up=0, down=0, hit0=0, hit1=0;
	int hit2=0, hit3=0, hit4=0, hit5=0;
	SDL_Event event;
	//delay ++;
	//if (delay<3) return end;
	//delay=0;

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
		left=right=up=down=hit0=hit1=hit2=hit3=hit4=hit5=0;
		if (event.type == SDL_KEYDOWN)
		{
			uae4all_play_click();
			switch(event.key.keysym.sym)
			{
			case SDLK_RIGHT: right=1; break;
			case SDLK_LEFT: left=1; break;
			case SDLK_UP: up=1; break;
			case SDLK_DOWN: down=1; break;
			case SDLK_LCTRL: hit2=1; break; //allow user to quit menu completely at any time
			//note SDLK_CTRL corresponds to ButtonSelect on Vita
#if defined(__PSP2__) || defined(__SWITCH__)
			case SDLK_PAGEDOWN: hit0=1; break;
			case SDLK_END: hit1=1; break;
#else
			case SDLK_PAGEDOWN: hit0=1; break;
			case SDLK_HOME: hit0=1; break;
			case SDLK_LALT: hit1=1; break;
			case SDLK_END: hit0=1; break;
			case SDLK_PAGEUP: hit0=1;
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

		if (hit2) // does the user want to shut-down the whole menu?
		{
			if (emulating)
			{
				saveMenu_case=SAVE_MENU_CASE_CANCEL; // quit this menu
				end=1;
				quit_pressed_in_submenu=1; //also change mainMenu state so that it automatically exits
			}
		}
		if (hit1)
		{
			saveMenu_case=SAVE_MENU_CASE_EXIT;
			end=1;
		}
		else if (up)
		{
			if (c>0) c=(c-1)%6;
			else c=5;
		}
		else if (down)
		{
			c=(c+1)%6;
		}
		else
		if (left && c!=4)
		{
			if (saveMenu_n_savestate>0)
				saveMenu_n_savestate--;
			else
				saveMenu_n_savestate=11;
		}
		else if (right && c!=4)
		{
			if (saveMenu_n_savestate<11)
				saveMenu_n_savestate++;
			else
				saveMenu_n_savestate=0;
		}
		switch(c)
		{
			case 0:
			break;
			case 1:
			if (hit0)
			{
			saveMenu_case=SAVE_MENU_CASE_LOAD_MEM;
			end=1;
			}
			break;
			case 2:
			if (hit0)
			{
				saveMenu_case=SAVE_MENU_CASE_SAVE_MEM;
				end=1;
			}
			break;
			case 3:
			if (hit0)
			{
				saveMenu_case=SAVE_MENU_CASE_DELETE_MEM;
				end=1;
			}
			break;
			case 4:
			if (left || right)
			{
				if (mainMenu_useSavesFolder==0)
					mainMenu_useSavesFolder=1;
				else
					mainMenu_useSavesFolder=0;
				make_savestate_filenames(savestate_filename, NULL);
			}
			break;
			case 5:
			if (hit0)
			{
				saveMenu_case=SAVE_MENU_CASE_EXIT;
				end=1;
			}
			break;
		}
	}

	(*cp)=c;
	return end;
}

static inline void raise_saveMenu()
{
	int i;

	text_draw_background();
	text_flip();
#if !defined(__PSP2__) && !defined(__SWITCH__)
	for(i=0;i<8;i++)
	{
		text_draw_background();
		text_draw_window(128-(8*i),(8-i)*24,144+(8*i),172,"Savestates");
		text_flip();
		SDL_Delay(15);
	}
#endif
}

static inline void unraise_saveMenu()
{
	int i;
#if !defined(__PSP2__) && !defined(__SWITCH__)
	for(i=7;i>=0;i--)
	{
		text_draw_background();
		text_draw_window(128-(8*i),(8-i)*24,144+(8*i),160,"");
		text_flip();
		SDL_Delay(15);
	}
#endif
	text_draw_background();
	text_flip();
}

void show_error(const char *str)
{
	int i;

	for(i=0;i<40;i++)
	{
		text_draw_background();
		text_draw_window(64,64,160,40,"ERROR !");
		write_text(9,9,str);
		text_flip();
	}
	SDL_Delay(1000);
}


void make_savestate_filenames(char *save, char *thumb)
{
	save[0]='\0';
	if (thumb!=NULL)
		thumb[0]='\0';
	int i=0;
	char *hd_name=NULL;
	// savestate is named by boot unit
	// use first floppy as filename, if empty, use boot hdf/hd dir
	if (uae4all_image_file0[0]!='\0')
	{
		strcpy(save,uae4all_image_file0);
	}
	else 
	{
		if (mainMenu_bootHD == 2)
		{
			if (uae4all_hard_file0[0]!='\0')
				hd_name=uae4all_hard_file0;
			else if (uae4all_hard_file1[0]!='\0')
				hd_name=uae4all_hard_file1;
			else if (uae4all_hard_file2[0]!='\0')
				hd_name=uae4all_hard_file2;
			else if (uae4all_hard_file3[0]!='\0')
				hd_name=uae4all_hard_file3;
		} else if (mainMenu_bootHD == 1 && uae4all_hard_dir[0]!='\0')
				hd_name=uae4all_hard_dir;				
		if (hd_name!=NULL && hd_name[0]!='\0') 
		{
			int oneColonFound=0;
			for (i = strlen(hd_name); i > 0; i--)
				if (hd_name[i] == ':' && !oneColonFound)
				{
					oneColonFound=1;
#ifdef __SWITCH__
				    break;
#endif
				}
				else if (hd_name[i] == ':' && oneColonFound)
					break;
			if (i > 0) 
			{
				strcpy(save, &hd_name[i+1]);
				if (strlen(save) > 255 - 2)
					save[255 - 2] = '\0';
			} 
			else
			{
				save[0]='\0';
			}
		} 
	} //Still nothing? Use floppy numbers 2,3,4
	if (save[0]=='\0')
	{
		if (uae4all_image_file1[0]!='\0')
			strcpy(save,uae4all_image_file1);
		else if	(uae4all_image_file2[0]!='\0')
			strcpy(save,uae4all_image_file2);
		else if	(uae4all_image_file3[0]!='\0')
			strcpy(save,uae4all_image_file3);
	}
	if (thumb!=NULL)
		strcpy(thumb, save);
	switch(saveMenu_n_savestate)
	{
		case 1:
			strcat(save,"-1.asf"); 
			if (thumb!=NULL)
				strcat(thumb,"-1.png"); 
			break;
		case 2:
			strcat(save,"-2.asf"); 
			if (thumb!=NULL)
				strcat(thumb,"-2.png"); 
			break;
		case 3:
			strcat(save,"-3.asf"); 
			if (thumb!=NULL)
				strcat(thumb,"-3.png"); 
			break;
		case 4:
			strcat(save,"-4.asf"); 
			if (thumb!=NULL)
				strcat(thumb,"-4.png"); 
			break;
		case 5:
			strcat(save,"-5.asf"); 
			if (thumb!=NULL)
				strcat(thumb,"-5.png"); 
			break;
		case 6:
			strcat(save,"-6.asf"); 
			if (thumb!=NULL)
				strcat(thumb,"-6.png"); 
			break;
		case 7:
			strcat(save,"-7.asf"); 
			if (thumb!=NULL)
				strcat(thumb,"-7.png"); 
			break;
		case 8:
			strcat(save,"-8.asf"); 
			if (thumb!=NULL)
				strcat(thumb,"-8.png"); 
			break;
		case 9:
			strcat(save,"-9.asf"); 
			if (thumb!=NULL)
				strcat(thumb,"-9.png"); 
			break;
		case 10:
			strcat(save,"-10.asf"); 
			if (thumb!=NULL)
				strcat(thumb,"-10.png"); 
			break;
		case 11:
			strcat(save,"-auto.asf"); 
			if (thumb!=NULL)
				strcat(thumb,"-auto.png"); 
			break;
		default: 
			strcat(save,".asf");
			if (thumb!=NULL)
				strcat(thumb,".png"); 
	}
	if (mainMenu_useSavesFolder==1) {
		char buffer[256];
		if (save[0]!='\0' && save!=NULL) {
			memset(buffer, 0, 256);
			extractFileName(save, buffer);
			memset(save, 0, 255);
			snprintf(save, 255, "%s%s", SAVE_PREFIX, buffer);
		}
	}
}

int run_menuSavestates()
{
	static int c=0;
	int end;
	saveMenu_case=-1;

	if (!emulating)
	{
		show_error("Emulation hasn't started yet.");
		return 0;
	}

	while(saveMenu_case<0)
	{
		raise_saveMenu();
		end=0;
		while(!end)
		{
			draw_savestatesMenu(c);
			end=key_saveMenu(&c);
		}
		unraise_saveMenu();
		switch(saveMenu_case)
		{
			case SAVE_MENU_CASE_LOAD_MEM:
				{
				make_savestate_filenames(savestate_filename,NULL);
				FILE *f=fopen(savestate_filename,"rb");
				if (f)
				{
					fclose(f);
					savestate_state = STATE_DORESTORE;
					saveMenu_case=1;
				}
				else
				{
					show_error("File doesn't exist.");
					saveMenu_case=-1;
				}
				}
				break;
			case SAVE_MENU_CASE_SAVE_MEM:
				make_savestate_filenames(savestate_filename,NULL);
				savestate_state = STATE_DOSAVE;
				saveMenu_case=1;
				break;
			case SAVE_MENU_CASE_DELETE_MEM:
			{
				make_savestate_filenames(savestate_filename,NULL);
				FILE *f=fopen(savestate_filename,"rb");
				if (f) {
					fclose(f);
					if (remove(savestate_filename) == 0) {
						show_error("File deleted");
					} else {
						show_error("File doesn't exist.");
					}
				}
				saveMenu_case=-1;
				break;
			}
			case SAVE_MENU_CASE_EXIT:	
			case SAVE_MENU_CASE_CANCEL:	
				saveMenu_case=1;
				break;
			default:
				saveMenu_case=-1;
		}
	}

	return saveMenu_case;
}
