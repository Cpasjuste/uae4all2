#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "sysconfig.h"
#include "sysdeps.h"
#include "config.h"
#include "menu.h"
#if defined(__PSP2__) // NOT __SWITCH__
#include <unistd.h>
#include "psp2-dirent.h"
#else
#ifndef __SWITCH__
#include <sys/mman.h>
#endif
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#endif
#include "uae.h"
#include "options.h"
#include "sound.h"
#include "gui.h"
#include "gp2x.h"
#include "gp2xutil.h"
#include <limits.h>
#include <SDL.h>

#ifndef PATH_MAX
	#define PATH_MAX 256
#endif

#if defined(__PSP2__) || defined(__SWITCH__)
#define SDL_PollEvent PSP2_PollEvent
#endif

char romFileName[PATH_MAX];

/* What is being loaded, floppy/hd dir/hdf */
int menu_load_type;

extern char filename0[256];
extern char filename1[256];
extern char filename2[256];
extern char filename3[256];
extern char currentDir[300];
extern char config_load_filename[300];

const char *text_str_load_separator="----------------------------------------";
const char *text_str_load_dir="#DIR#";
static const char *text_str_load_title="            Filemanager            -";
int text_dir_num_files=0, text_dir_num_files_index=0;

#define SHOW_MAX_FILES 13
#define MAX_FILES_PER_DIR 10240

extern int run_menuFileinfo(char* fileName);
static int min_in_dir=0, max_in_dir=SHOW_MAX_FILES;

extern int current_drive;
extern int current_hdf;

#if !defined(__PSP2__) && !defined(__SWITCH__)
#ifdef PANDORA
static int scandir_cmp(const void *p1, const void *p2)
{
	struct dirent **d1 = (struct dirent **)p1, **d2 = (struct dirent **)p2;
#else
static int scandir_cmp(const dirent **p1, const dirent **p2)
{
        const struct dirent **d1 = (const struct dirent **)p1, **d2 = (const struct dirent **)p2;
#endif
	if ((*d1)->d_type == (*d2)->d_type) return alphasort(d1, d2);
	if ((*d1)->d_type == DT_DIR) return -1;
	if ((*d2)->d_type == DT_DIR) return  1;
	return alphasort(d1, d2);
}
#endif

static const char *filter_exts[] = {
	".adf", ".gz",".rom",".adf.gz"
};

static int scandir_filter(const struct dirent *ent)
{
	const char *p;
	int i;
	return 1;

	if (ent == NULL || ent->d_name == NULL)
		return 0;
	if (strlen(ent->d_name) < 5)
		return 1;

	p = ent->d_name + strlen(ent->d_name) - 4;

	for (i = 0; i < sizeof(filter_exts)/sizeof(filter_exts[0]); i++)
	{
		if (strcmp(p, filter_exts[i]) == 0)
			return 0;
	}

	return 1;
}

static void extractFileName(char * str,char *buffer)
{
	char *p=str+strlen(str)-1;
	while(*p != '/') p--;
	p++;
	strcpy(buffer,p);
}

static void draw_dirlist(char *curdir, struct dirent **namelist, int n, int sel)
{
	int i,j;
	n--;
	static int b=0;
	int bb=(b%6)/3;
	SDL_Rect r;
	extern SDL_Surface *text_screen;
	r.x=80-64; r.y=0; r.w=150-24+64+64; r.h=240;
	text_draw_background();

	if (menu_load_type == MENU_LOAD_HD_DIR)
#ifdef __PSP2__
		text_draw_window(2,2,41,25,"  Press SELECT to load HD-dir (Tri.=Info)  ");
#else
#ifdef __SWITCH__
		text_draw_window(2,2,41,25,"  Press MINUS to load HD-dir (X = Info)  ");
#else
		text_draw_window(2,2,41,25,"  Press L-key to load HD-dir  ");
#endif
#endif
	else if (menu_load_type == MENU_LOAD_HDF)
#ifdef __PSP2__
		text_draw_window(2,2,41,25,"       Select .HDF-file (Triangle = Info)      ");
#else
#ifdef __SWITCH__
		text_draw_window(2,2,41,25,"       Select .HDF-file (X = Info)      ");
#else
		text_draw_window(2,2,41,25,"       Select .HDF-file       ");
#endif
#endif
	else if (menu_load_type == MENU_LOAD_CONFIG)
#ifdef __PSP2__
		text_draw_window(2,2,41,25,"       Select .CONF-file (Triangle = Info)      ");
#else
#ifdef __SWITCH__
		text_draw_window(2,2,41,25,"       Select .CONF-file (X = Info)      ");
#else
		text_draw_window(2,2,41,25,"       Select .CONF-file       ");
#endif
#endif
	else if (menu_load_type == MENU_LOAD_DELETE_CONFIG)
#ifdef __PSP2__
		text_draw_window(2,2,41,25,"       Delete .CONF-file (Triangle = Info)      ");
#else
#ifdef __SWITCH__
		text_draw_window(2,2,41,25,"       Delete .CONF-file (X = Info)      ");
#else
		text_draw_window(2,2,41,25,"       Delete .CONF-file       ");
#endif
#endif
#ifdef __PSP2__
	else if (current_drive==0)
		text_draw_window(2,2,41,25," Insert .ADF into DF0 (Triangle = Info) ");
	else if (current_drive==1)
		text_draw_window(2,2,41,25," Insert .ADF into DF1 (Triangle = Info)");
	else if (current_drive==2)
		text_draw_window(2,2,41,25," Insert .ADF into DF2 (Triangle = Info)");
	else if (current_drive==3)
		text_draw_window(2,2,41,25," Insert .ADF into DF3 (Triangle = Info)");
#else
#ifdef __SWITCH__
	else if (current_drive==0)
		text_draw_window(2,2,41,25," Insert .ADF into DF0 (X = Info)");
	else if (current_drive==1)
		text_draw_window(2,2,41,25," Insert .ADF into DF1 (X = Info)");
	else if (current_drive==2)
		text_draw_window(2,2,41,25," Insert .ADF into DF2 (X = Info)");
	else if (current_drive==3)
		text_draw_window(2,2,41,25," Insert .ADF into DF3 (X = Info)");
#else
	else if (current_drive==0)
		text_draw_window(2,2,41,25," Insert .ADF or .ADZ into DF0 ");
	else if (current_drive==1)
		text_draw_window(2,2,41,25," Insert .ADF or .ADZ into DF1 ");
	else if (current_drive==2)
		text_draw_window(2,2,41,25," Insert .ADF or .ADZ into DF2 ");
	else if (current_drive==3)
		text_draw_window(2,2,41,25," Insert .ADF or .ADZ into DF3 ");
#endif
#endif
	else
		text_draw_window(2,2,41,25,text_str_load_title);

	if (sel<min_in_dir)
	{
		min_in_dir=sel;
		max_in_dir=sel+SHOW_MAX_FILES;
	}
	else
		if (sel>=max_in_dir)
		{
			max_in_dir=sel+1;
			min_in_dir=max_in_dir-SHOW_MAX_FILES;
		}
	if (max_in_dir>n)
		max_in_dir=n-min_in_dir;

	for (i=min_in_dir,j=3;i<max_in_dir;i++,j+=2)
	{
		
		write_text(3,j,text_str_load_separator);
		SDL_SetClipRect(text_screen,&r);
		
	if ((sel+1==i+1)&&(bb))
			write_text_inv(4,j+1,namelist[i+1]->d_name);
		else
			write_text(4,j+1,namelist[i+1]->d_name);


		SDL_SetClipRect(text_screen,NULL);

		if (namelist[i+1]->d_type==DT_DIR)
			write_text(38,j+1,text_str_load_dir);
	}
	write_text(3,j,text_str_load_separator);

	text_flip();

	b++;
}

#if defined(__PSP2__) || defined(__SWITCH__)
static int cmpDirentString(const void *first, const void *second)
{
	dirent *direntA = *(dirent * const *) first;
	dirent *direntB = *(dirent * const *) second;
	// Directories on the top
	if (direntA->d_type != direntB->d_type)
	{
		if (direntA->d_type == DT_DIR)
			return -1;
		else
			return 1;
	}
	// Filenames sorted case-insensitive
	char *a = direntA->d_name;
	char *b = direntB->d_name;
	while (*a && *b) {
        if (tolower(*a) != tolower(*b)) {
            break;
        }
        ++a;
        ++b;
   }
   int r = tolower(*a) - tolower(*b);
	if (r) return r;
   // if equal ignoring case, use opposite of strcmp() result to get
   // lower before upper
   return -strcmp(a, b); //aka: return strcmp(b, a)
}
#endif

static int menuLoadLoop(char *curr_path)
{
	char *ret = NULL, *fname = NULL;

	struct dirent **namelist;
	DIR *dir;
	int n, sel = 0;
	int sel_last = 0;
	unsigned long inp = 0;

	min_in_dir=0;
	max_in_dir=SHOW_MAX_FILES;

#if defined(__PSP2__) || defined(__SWITCH__)
#if !defined(__SWITCH__)
	if(strcmp(curr_path, "/") == 0 || curr_path[0] == 0) 
	{
		struct dirent *ent = NULL;
		n = 0;
		namelist = (struct dirent **)malloc(MAX_FILES_PER_DIR * sizeof(struct dirent *));
		namelist[0] = (struct dirent *)malloc(sizeof(struct dirent));
		strcpy(namelist[0]->d_name, ".");
		namelist[0]->d_type = DT_DIR; n++;
#if defined(__SWITCH__)
		namelist[1] = (struct dirent *)malloc(sizeof(struct dirent));
		strcpy(namelist[1]->d_name, "sdmc:");
		namelist[1]->d_type = DT_DIR; n++;
#else
		namelist[1] = (struct dirent *)malloc(sizeof(struct dirent));
		strcpy(namelist[1]->d_name, "ux0:");
		namelist[1]->d_type = DT_DIR; n++;
		namelist[2] = (struct dirent *)malloc(sizeof(struct dirent));
		strcpy(namelist[2]->d_name, "uma0:");
		namelist[2]->d_type = DT_DIR; n++;
#endif
		curr_path[0] == 0;
	}
	else
#endif
	{
		if ((dir = opendir(curr_path)) == NULL)
		{
			printf("opendir failed: %s\n", curr_path);
			char *p;
			for (p = curr_path + strlen(curr_path) - 1; p > curr_path && *p != '/'; p--);
			*p = 0;
			fname = p+1;
			dir = opendir(curr_path);
		}

		struct dirent *ent = NULL;
		n = 0;
		namelist =  (struct dirent **)malloc(MAX_FILES_PER_DIR * sizeof(struct dirent *));
		namelist[0] = (struct dirent *)malloc(sizeof(struct dirent));
		strcpy(namelist[0]->d_name, ".");
		namelist[0]->d_type = DT_DIR; n++;
		namelist[1] = (struct dirent *)malloc(sizeof(struct dirent));
		strcpy(namelist[1]->d_name, "..");
		namelist[1]->d_type = DT_DIR; n++;

		while ((ent = readdir (dir)) != NULL) {
			if(n >= MAX_FILES_PER_DIR-1)
				break;
			namelist[n] = (struct dirent *)malloc(sizeof(struct dirent));
			memcpy(namelist[n], ent, sizeof(struct dirent));
			n++;
		}
		closedir(dir);
	}

	if(n <= 0) {
		return 0;
	}
	
	//sort alphabetically
	qsort(namelist, n, sizeof(dirent*), cmpDirentString);
	
	if (n<10) SDL_Delay(70);
	else SDL_Delay(40);
#else
	// is this a dir or a full path?
	if ((dir = opendir(curr_path)))
		closedir(dir);
	else 
	{
		char *p;
		for (p = curr_path + strlen(curr_path) - 1; p > curr_path && *p != '/'; p--);
		*p = 0;
		fname = p+1;
	}
	
	n = scandir(curr_path, &namelist, scandir_filter, scandir_cmp);

	if (n < 0) 
	{
		// try root
		n = scandir("/", &namelist, scandir_filter, scandir_cmp);
		if (n < 0) 
		{
			// oops, we failed
			printf("dir: "); printf(curr_path); printf("\n");
			perror("scandir");
			return 0;
		}
	}
	if (n<10) usleep(70*1024);
	else usleep(40*1024);
#endif
	// try to find sel
	if (fname != NULL) 
	{
		int i;
		for (i = 1; i < n; i++) 
		{
			if (strcmp(namelist[i]->d_name, fname) == 0) 
			{
				sel = i - 1;
				break;
			}
		}
	}

	int loaded=0;
	int delay=0;
	SDL_Event event;
	int left=0, right=0, up=0, down=0, hit0=0, hit1=0, hit2=0, hit3=0, hit4=0, hitL=0;
	while(hit0+hit1+hitL==0)
	{
		//unsigned long keys;
		draw_dirlist(curr_path, namelist, n, sel);
		delay ++;
		
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
		
		while (SDL_PollEvent(&event) > 0 && hit0+hit1+hitL==0)
		{
			left=right=up=down=hit0=hit1=hit2=hit3=hit4=hitL=0;
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
					case SDLK_LALT: hit1=1; break;
#if !defined(__PSP2__) && !defined(__SWITCH__)
					case SDLK_LCTRL: hit2=1; break;
					case SDLK_HOME: hit0=1; break;
					case SDLK_END: hit0=1; break;
					case SDLK_PAGEUP: hit0=1; break;
#endif
					case SDLK_RSHIFT: hit3=1; break;
					case SDLK_RCTRL: hit4=1; break;
#if defined(__PSP2__) || defined(__SWITCH__)
					// SELECT to select HD dir
					case SDLK_LCTRL: hitL=1; break;
					// TRIANGLE for fileinfo
					case SDLK_PAGEUP: hit2=1; break;
					// CIRCLE for cancel
					case SDLK_END: hit1=1; break;
#endif
					case SDLK_l: hitL=1;
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
			
			if(up)  { sel--;   if (sel < 0)   sel = n-2; /*usleep(10*1024);*/ }
			if(down)  { sel++;   if (sel > n-2) sel = 0;/*usleep(10*1024);*/}
			if(left)  { sel-=10; if (sel < 0)   sel = 0;/*usleep(10*1024);*/}
			if(hit3)     { sel-=24; if (sel < 0)   sel = 0;/*usleep(10*1024);*/}
			if(right) { sel+=10; if (sel > n-2) sel = n-2;/*usleep(10*1024);*/}
			if(hit4)     { sel+=24; if (sel > n-2) sel = n-2;/*usleep(10*1024);*/}
			if(hit2)     { run_menuFileinfo(namelist[sel+1]->d_name);}
			if(hit0 || hitL)
			{
				if (namelist[sel+1]->d_type == DT_REG) 
				{
					int df;
					int newlen = strlen(curr_path) + strlen(namelist[sel+1]->d_name) + 2;
					char *p; 
					char *filename;
					filename=(char*)malloc(newlen);
					strcpy(filename, curr_path);
					p = filename + strlen(filename) - 1;
					while (*p == '/' && p >= filename) *p-- = 0;
					strcat(filename, "/");
					strcat(filename, namelist[sel+1]->d_name);
					printf("Selecting file %s\n",filename);
					switch (menu_load_type)
					{
						case MENU_LOAD_FLOPPY:
							if (current_drive==0){strcpy(uae4all_image_file0,filename);extractFileName(uae4all_image_file0, filename0);df=0;}
							else if(current_drive==1) {strcpy(uae4all_image_file1,filename);extractFileName(uae4all_image_file1, filename1);df=1;}
							else if(current_drive==2) {strcpy(uae4all_image_file2,filename);extractFileName(uae4all_image_file2, filename2);df=2;}
							else if(current_drive==3) {strcpy(uae4all_image_file3,filename);extractFileName(uae4all_image_file3, filename3);df=3;}
							printf("DF0 %s\n",uae4all_image_file0);
							break;
						case MENU_LOAD_HDF:
							if (strstr(filename, ".hdf") == NULL)
								showWarning("HDF file must be selected");
							else
								if (current_hdf==0)
									strcpy(uae4all_hard_file0, filename);
								else if (current_hdf==1)
									strcpy(uae4all_hard_file1, filename);
								else if (current_hdf==2)
									strcpy(uae4all_hard_file2, filename);
								else if (current_hdf==3)
									strcpy(uae4all_hard_file3, filename);
							break;
						case MENU_LOAD_CONFIG:
						case MENU_LOAD_DELETE_CONFIG:
							if (strstr(filename, ".conf") == NULL)
								showWarning("CONF file must be selected");
							else
								strcpy(config_load_filename,filename);
							break;
					}
					loaded=1;
					if ((menu_load_type != MENU_LOAD_CONFIG) && (menu_load_type != MENU_LOAD_DELETE_CONFIG))
						strcpy(currentDir,filename);
					free(filename);
					break;
				}
				else if (namelist[sel+1]->d_type == DT_DIR)
				{
					int newlen = strlen(curr_path) + strlen(namelist[sel+1]->d_name) + 2;
					char *p;
					char *newdir;
					/* Hard file dir is being selected ? (L-key is used to select dir) */
					if ((menu_load_type == MENU_LOAD_HD_DIR) && hitL)
					{
						strcpy(uae4all_hard_dir, curr_path);
						strcat(uae4all_hard_dir, "/");
						strcat(uae4all_hard_dir, namelist[sel+1]->d_name);
						loaded = 1;
						break;
					}
					else
					{
						newdir=(char*)malloc(newlen);
						if (strcmp(namelist[sel+1]->d_name, "..") == 0)
						{
							char *start = curr_path;
							p = start + strlen(start) - 1;
							while (*p == '/' && p > start) p--;
							while (*p != '/' && p > start) p--;
							if (p <= start) strcpy(newdir, "/");
							else { strncpy(newdir, start, p-start); newdir[p-start] = 0; }
						} 
						else if (strcmp(namelist[sel+1]->d_name, "uma0:") == 0)
						{
							strcpy(newdir, "uma0:/");
						}
						else if (strcmp(namelist[sel+1]->d_name, "ux0:") == 0)
						{
							strcpy(newdir, "ux0:/");
						}
						//else if (strcmp(namelist[sel+1]->d_name, "sdmc:") == 0)
						//{
						//	strcpy(newdir, "sdmc:/");
						//}
						else 
						{
							strcpy(newdir, curr_path);
							p = newdir + strlen(newdir) - 1;
							while (*p == '/' && p >= newdir) *p-- = 0;
							strcat(newdir, "/");
							strcat(newdir, namelist[sel+1]->d_name);
						}
						if ((menu_load_type != MENU_LOAD_CONFIG) && (menu_load_type != MENU_LOAD_DELETE_CONFIG))
							strcpy(currentDir,newdir);
						loaded = menuLoadLoop(newdir);
						free(newdir);
						break;
					}
				} 
			}
			if(hit1)
				break;
		}
	}
	if (n > 0) 
	{
		while(n--) free(namelist[n]);
		free(namelist);
	}

	return loaded;
}

static void raise_loadMenu()
{
	int i;

	text_draw_background();
	text_flip();
#if !defined(__PSP2__) && !defined(__SWITCH__)
	for(i=0;i<10;i++)
	{
		text_draw_background();
		text_draw_window(80-64,(10-i)*24,160+64+64,220,text_str_load_title);
		text_flip();
	}
#endif
}

static void unraise_loadMenu()
{
	int i;
#if !defined(__PSP2__) && !defined(__SWITCH__)
	for(i=9;i>=0;i--)
	{
		text_draw_background();
		text_draw_window(80-64,(10-i)*24,160+64+64,220,text_str_load_title);
		text_flip();
	}
#endif
	text_draw_background();
	text_flip();
}

int run_menuLoad(char *curr_path, int aLoadType)
{
	menu_load_type = aLoadType;

	raise_loadMenu();
	int ret=menuLoadLoop(curr_path);
	unraise_loadMenu();
	return ret;
}
