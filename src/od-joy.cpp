 /*
	* UAE - The Un*x Amiga Emulator
	*
	* Joystick emulation for Linux and BSD. They share too much code to
	* split this file.
	*
	* Copyright 1997 Bernd Schmidt
	* Copyright 1998 Krister Walfridsson
	*/

#include "sysconfig.h"
#include "sysdeps.h"

#include "config.h"
#include "uae.h"
#include "options.h"
#include "memory-uae.h"
#include "custom.h"
#include "joystick.h"
#include "SDL.h"
#include "menu.h"
#include "menu_config.h"
#include "math.h"

#ifdef USE_UAE4ALL_VKBD
#include "vkbd.h"
static int can_change_vkbd_transparency=1;
#endif

#ifdef ANDROIDSDL
#include <android/log.h>
#endif

#ifdef GP2X
#include "gp2x.h"
#include "xwin.h"
extern int gp2xMouseEmuOn;
extern int gp2xButtonRemappingOn;
#if defined(PANDORA) || defined (ANDROIDSDL)
extern int dpadUp[4];
extern int dpadDown[4];
extern int dpadLeft[4];
extern int dpadRight[4];
extern int buttonA[4];
extern int buttonB[4];
extern int buttonX[4];
extern int buttonY[4];
extern int triggerL[4];
extern int triggerR[4];
extern int buttonSelect[4];
extern int buttonStart[4];
#endif
#ifdef __PSP2__
extern int rAnalogX;
extern int rAnalogY;
extern int lAnalogX;
extern int lAnalogY;
extern int mainMenu_leftStickMouse;
extern int mainMenu_touchControls;
extern int mainMenu_deadZone;
int delay2=0; // for 2nd player non-custom autofire 
#endif

extern char launchDir[300];
bool switch_autofire=false;
int delay=0;
#endif

int nr_joysticks;

SDL_Joystick *uae4all_joy0, *uae4all_joy1, *uae4all_joy2, *uae4all_joy3;
extern SDL_Surface *prSDLScreen;

void read_joystick(int nr, unsigned int *dir, int *button)
{
#ifndef MAX_AUTOEVENTS
		int x_axis, y_axis;
		int left = 0, right = 0, top = 0, bot = 0, upRight=0, downRight=0, upLeft=0, downLeft=0, x=0, y=0, a=0, b=0;
		int len, i, num;

		SDL_Joystick *joy = nr == 0 ? uae4all_joy0 : uae4all_joy1;

		*dir = 0;
		*button = 0;

		nr = (~nr)&0x1;

	// are we trying to figure out the regular GP2X controls for the primary (or both) joysticks?
	int usingRegularControls = (!mainMenu_customControls) && ((mainMenu_joyPort == 0) || (nr == 1 && mainMenu_joyPort == 2) || (nr == 0 && mainMenu_joyPort == 1));
	
	//PSP2 updates joysticks in handle_events function which is always called 
	//just before read_joystick is called. No need to update them again here
#ifndef __PSP2__
	SDL_JoystickUpdate();
#endif
/* Temporary disabled
#ifdef ANDROIDSDL
		if (nr_joysticks > 2)
		{
	int axis0,axis1,axis2,axis3;
	axis0 = SDL_JoystickGetAxis(uae4all_joy2, 0);
	axis1 = SDL_JoystickGetAxis(uae4all_joy2, 1);
	axis2 = SDL_JoystickGetAxis(uae4all_joy2, 2);
	axis3 = SDL_JoystickGetAxis(uae4all_joy2, 3);

	if ((axis0 > 512) || (axis2 > 512)) right=1;
	if ((axis0 < -512) || (axis2 < -512)) left=1;
	if ((axis1 < -512) || (axis3 < -512)) top=1;
	if ((axis1 > 512) || (axis3 > 512)) bot=1;
}
#endif
*/
	int mouseScale = mainMenu_mouseMultiplier * 8 * 16;
	mouseScale /= 100;
	//slow down mouse motion if custom "slow mouse" button is held
	if(mainMenu_customControls)
	{
		for (int i=0; i<nr_joysticks; i++)
		{
			if( 
				(
					(mainMenu_custom_A[i]==-13 && buttonA[i]) ||
					(mainMenu_custom_B[i]==-13 && buttonB[i]) ||
					(mainMenu_custom_X[i]==-13 && buttonX[i]) ||
					(mainMenu_custom_Y[i]==-13 && buttonY[i]) ||
					(mainMenu_custom_L[i]==-13 && triggerL[i]) ||
					(mainMenu_custom_R[i]==-13 && triggerR[i])
				) ||
				(
					(mainMenu_custom_dpad == 0) && 
					(
						(mainMenu_custom_up[i]==-13 && dpadUp[i]) ||
						(mainMenu_custom_down[i]==-13 && dpadDown[i]) ||
						(mainMenu_custom_left[i]==-13 && dpadLeft[i]) ||
						(mainMenu_custom_right[i]==-13 && dpadRight[i]))
					)
				)
			{
				mouseScale/=8;
				break;
			}
		}
	}

//Digital mouseemu hotkeys: Triangle changes mouse speed etc.
#if !defined(__PSP2__) && defined(USE_UAE4ALL_VKBD)
	if (!vkbd_mode && ((mainMenu_customControls && mainMenu_custom_dpad==2) || gp2xMouseEmuOn || (triggerL[0] && !triggerR[0] && !gp2xButtonRemappingOn)))
#else
#if defined(__PSP2__) && defined(USE_UAE4ALL_VKBD)
	//on Vita, the L trigger is by default mapped to a mousebutton
	//so remove the hard coded LTrigger here that was enabling the digital mouse
	if (!vkbd_mode && ((mainMenu_customControls && mainMenu_custom_dpad==2) || gp2xMouseEmuOn))
#else
#if defined(__PSP2__)
	if ((mainMenu_customControls && mainMenu_custom_dpad==2) || gp2xMouseEmuOn)
#else
	if (((mainMenu_customControls && mainMenu_custom_dpad==2) || gp2xMouseEmuOn || (triggerL[0] && !triggerR[0] && !gp2xButtonRemappingOn)))
#endif
#endif
#endif
	{
		if (buttonY[0]) {
			mouseScale = mainMenu_mouseMultiplier * 16;
			mouseScale /= 100;
		}
#if defined(PANDORA) || defined(ANDROIDSDL)
		if (dpadLeft[0])
#else
		if (SDL_JoystickGetButton(joy, GP2X_BUTTON_LEFT))
#endif
		{
			lastmx -= mouseScale;
			newmousecounters=1;
		}
#if !( defined(PANDORA) || defined(ANDROIDSDL) )
		if (SDL_JoystickGetButton(joy, GP2X_BUTTON_UPLEFT))
		{
			lastmx -= mouseScale;
			lastmy -= mouseScale;
			newmousecounters=1;
		}
		if (SDL_JoystickGetButton(joy, GP2X_BUTTON_DOWNLEFT))
		{
			lastmx -= mouseScale;
			lastmy += mouseScale;
			newmousecounters=1;
		}
#endif
		if (dpadRight[0])
		{
			lastmx += mouseScale;
			newmousecounters=1;
		}
		if (dpadUp[0])
		{
			lastmy -= mouseScale;
			newmousecounters=1;
		}
		if (dpadDown[0])
		{
			lastmy += mouseScale;
			newmousecounters=1;
		}
	}
	else if (!triggerR[0] /*R+dpad = arrow keys*/ && !(mainMenu_customControls && mainMenu_custom_dpad==0) && usingRegularControls)
	{
//regular direction controls for main Joystick (or both if "both" is set.)
#if !defined(AROS) && !defined(__PSP2__)
		if (dpadRight || SDL_JoystickGetAxis(joy, 0) > 0) right=1;
		if (dpadLeft || SDL_JoystickGetAxis(joy, 0) < 0) left=1;
		if (dpadUp || SDL_JoystickGetAxis(joy, 1) < 0) top=1;
		if (dpadDown || SDL_JoystickGetAxis(joy, 1) > 0) bot=1;
#else
		if (dpadRight[0]) right=1;
		if (dpadLeft[0]) left=1;
		if (dpadUp[0]) top=1;
		if (dpadDown[0]) bot=1;
#endif

		if (mainMenu_joyConf)
		{
#ifdef USE_UAE4ALL_VKBD
			if (((buttonX[0] && mainMenu_jump > -1) ) && !vkbd_mode)
#else
			if (((buttonX[0] && mainMenu_jump > -1) || SDL_JoystickGetButton(joy, mainMenu_jump)))
#endif
				top = 1;
		}
	}

// regular button controls without custom remapping
#ifdef USE_UAE4ALL_VKBD
	if (usingRegularControls && !(gp2xMouseEmuOn) && !(gp2xButtonRemappingOn) && !vkbd_mode)
#else
	if (usingRegularControls && !(gp2xMouseEmuOn) && !(gp2xButtonRemappingOn))
#endif
	{
		if (
			(mainMenu_autofire & switch_autofire & delay>mainMenu_autofireRate)
			||
				(
					(
						(mainMenu_autofireButton1==GP2X_BUTTON_B && buttonA[0])
						||
						(mainMenu_autofireButton1==GP2X_BUTTON_X && buttonX[0])
						||
						(mainMenu_autofireButton1==GP2X_BUTTON_Y && buttonY[0])
					)
					& delay>mainMenu_autofireRate
				)
			)
		{
			if(!buttonB[0])
				*button=1;
			delay=0;
			*button |= (buttonB[0] & 1) << 1;
		}
		else
		{
#ifdef __PSP2__
			*button = ((mainMenu_button1==GP2X_BUTTON_B && buttonA[0]) || (mainMenu_button1==GP2X_BUTTON_X && buttonX[0]) || (mainMenu_button1==GP2X_BUTTON_Y && buttonY[0])) & 1;
#else
#if !(defined(ANDROIDSDL) || defined(AROS))
			*button = ((mainMenu_button1==GP2X_BUTTON_B && buttonA[0]) || (mainMenu_button1==GP2X_BUTTON_X && buttonX[0]) || (mainMenu_button1==GP2X_BUTTON_Y && buttonY[0]) || SDL_JoystickGetButton(joy, mainMenu_button1)) & 1;
#else
			*button = ((mainMenu_button1==GP2X_BUTTON_B && buttonA[0]) || (mainMenu_button1==GP2X_BUTTON_X && buttonX[0]) || (mainMenu_button1==GP2X_BUTTON_Y && buttonY[0])) & 1;
#endif
#endif //__PSP2__
			delay++;
#ifdef __PSP2__
			*button |= ((buttonB[0]) & 1) << 1;
#else
#if defined(PANDORA) && !defined(AROS)
			*button |= ((buttonB[0] || SDL_JoystickGetButton(joy, mainMenu_button2)) & 1) << 1;
#else
			*button |= ((buttonB[0]) & 1) << 1;
#endif
#endif //__PSP2__
		}
	}

//Analog Mouse on PSP2, only update once per frame (when nr==1)
#ifdef __PSP2__
#ifdef USE_UAE4ALL_VKBD
	if (!vkbd_mode && nr)
#else
	if (nr)
#endif
	{
		//VITA: always use an analog stick (default: right stick) for mouse pointer movements
		//here we are using a small deadzone
		//This can be disabled in the menu because it interferes with Joystick Port 0
		if (mainMenu_mouseEmulation)
		{
			float analogX=0.0f;
			float analogY=0.0f;
			float deadZone=(float) mainMenu_deadZone;
			float scalingFactor=1.0f;
			float magnitude=0.0f;

			if (mainMenu_leftStickMouse)
			{
				analogX=(float) lAnalogX;
				analogY=(float) lAnalogY;
			}
			else
			{
				analogX=(float) rAnalogX;
				analogY=(float) rAnalogY;
			}
			//radial and scaled deadzone
			//http://www.third-helix.com/2013/04/12/doing-thumbstick-dead-zones-right.html
			//max movement is mouseScale.
			//that way, when in one of the other mouse modes,
			//the Y button to change scale still works
			magnitude=sqrt(analogX*analogX+analogY*analogY);
			if (magnitude >= deadZone)
			{
				scalingFactor=1.0f/magnitude*(magnitude-deadZone)/(32769.0f-deadZone);
				analogX = analogX * scalingFactor;
				analogY = analogY * scalingFactor;
				lastmx += (int) (analogX * mouseScale);
				lastmy += (int) (analogY * mouseScale);
				newmousecounters=1;
			}
		}
		else
		{
			newmousecounters=0;
		}
	}
#endif //__PSP2__

#ifdef USE_UAE4ALL_VKBD
	if(mainMenu_customControls && !vkbd_mode)
#else
	if(mainMenu_customControls)
#endif
	{
		for (int i=0; i<nr_joysticks; i++)
		{
			// The main (joyport)) joystick mapping directions
			if (nr == 1)
			{
				if((mainMenu_custom_A[i]==-5 && buttonA[i]) || (mainMenu_custom_B[i]==-5 && buttonB[i]) || (mainMenu_custom_X[i]==-5 && buttonX[i]) || (mainMenu_custom_Y[i]==-5 && buttonY[i]) || (mainMenu_custom_L[i]==-5 && triggerL[i]) || (mainMenu_custom_R[i]==-5 && triggerR[i]))
					top = 1;
				else if(mainMenu_custom_dpad == 0)
				{
					if((mainMenu_custom_up[i]==-5 && dpadUp[i]) || (mainMenu_custom_down[i]==-5 && dpadDown[i]) || (mainMenu_custom_left[i]==-5 && dpadLeft[i]) || (mainMenu_custom_right[i]==-5 && dpadRight[i]))
						top = 1;
				}

				if((mainMenu_custom_A[i]==-6 && buttonA[i]) || (mainMenu_custom_B[i]==-6 && buttonB[i]) || (mainMenu_custom_X[i]==-6 && buttonX[i]) || (mainMenu_custom_Y[i]==-6 && buttonY[i]) || (mainMenu_custom_L[i]==-6 && triggerL[i]) || (mainMenu_custom_R[i]==-6 && triggerR[i]))
					bot = 1;
				else if(mainMenu_custom_dpad == 0)
				{
					if((mainMenu_custom_up[i]==-6 && dpadUp[i]) || (mainMenu_custom_down[i]==-6 && dpadDown[i]) || (mainMenu_custom_left[i]==-6 && dpadLeft[i]) || (mainMenu_custom_right[i]==-6 && dpadRight[i]))
						bot = 1;
				}

				if((mainMenu_custom_A[i]==-7 && buttonA[i]) || (mainMenu_custom_B[i]==-7 && buttonB[i]) || (mainMenu_custom_X[i]==-7 && buttonX[i]) || (mainMenu_custom_Y[i]==-7 && buttonY[i]) || (mainMenu_custom_L[i]==-7 && triggerL[i]) || (mainMenu_custom_R[i]==-7 && triggerR[i]))
					left = 1;
				else if(mainMenu_custom_dpad == 0)
				{
					if((mainMenu_custom_up[i]==-7 && dpadUp[i]) || (mainMenu_custom_down[i]==-7 && dpadDown[i]) || (mainMenu_custom_left[i]==-7 && dpadLeft[i]) || (mainMenu_custom_right[i]==-7 && dpadRight[i]))
						left = 1;
				}

				if((mainMenu_custom_A[i]==-8 && buttonA[i]) || (mainMenu_custom_B[i]==-8 && buttonB[i]) || (mainMenu_custom_X[i]==-8 && buttonX[i]) || (mainMenu_custom_Y[i]==-8 && buttonY[i]) || (mainMenu_custom_L[i]==-8 && triggerL[i]) || (mainMenu_custom_R[i]==-8 && triggerR[i]))
					right = 1;
				else if(mainMenu_custom_dpad == 0)
				{
					if((mainMenu_custom_up[i]==-8 && dpadUp[i]) || (mainMenu_custom_down[i]==-8 && dpadDown[i]) || (mainMenu_custom_left[i]==-8 && dpadLeft[i]) || (mainMenu_custom_right[i]==-8 && dpadRight[i]))
						right = 1;
				}
			}
			// The second (mouseport) joystick mapping directions
			else if (nr == 0)
			{
				if((mainMenu_custom_A[i]==-9 && buttonA[i]) || (mainMenu_custom_B[i]==-9 && buttonB[i]) || (mainMenu_custom_X[i]==-9 && buttonX[i]) || (mainMenu_custom_Y[i]==-9 && buttonY[i]) || (mainMenu_custom_L[i]==-9 && triggerL[i]) || (mainMenu_custom_R[i]==-9 && triggerR[i]))
					top = 1;
				else if(mainMenu_custom_dpad == 0)
				{
					if((mainMenu_custom_up[i]==-9 && dpadUp[i]) || (mainMenu_custom_down[i]==-9 && dpadDown[i]) || (mainMenu_custom_left[i]==-9 && dpadLeft[i]) || (mainMenu_custom_right[i]==-9 && dpadRight[i]))
						top = 1;
				}

				if((mainMenu_custom_A[i]==-10 && buttonA[i]) || (mainMenu_custom_B[i]==-10 && buttonB[i]) || (mainMenu_custom_X[i]==-10 && buttonX[i]) || (mainMenu_custom_Y[i]==-10 && buttonY[i]) || (mainMenu_custom_L[i]==-10 && triggerL[i]) || (mainMenu_custom_R[i]==-10 && triggerR[i]))
					bot = 1;
				else if(mainMenu_custom_dpad == 0)
				{
					if((mainMenu_custom_up[i]==-10 && dpadUp[i]) || (mainMenu_custom_down[i]==-10 && dpadDown[i]) || (mainMenu_custom_left[i]==-10 && dpadLeft[i]) || (mainMenu_custom_right[i]==-10 && dpadRight[i]))
						bot = 1;
				}

				if((mainMenu_custom_A[i]==-11 && buttonA[i]) || (mainMenu_custom_B[i]==-11 && buttonB[i]) || (mainMenu_custom_X[i]==-11 && buttonX[i]) || (mainMenu_custom_Y[i]==-11 && buttonY[i]) || (mainMenu_custom_L[i]==-11 && triggerL[i]) || (mainMenu_custom_R[i]==-11 && triggerR[i]))
					left = 1;
				else if(mainMenu_custom_dpad == 0)
				{
					if((mainMenu_custom_up[i]==-11 && dpadUp[i]) || (mainMenu_custom_down[i]==-11 && dpadDown[i]) || (mainMenu_custom_left[i]==-11 && dpadLeft[i]) || (mainMenu_custom_right[i]==-11 && dpadRight[i]))
						left = 1;
				}

				if((mainMenu_custom_A[i]==-12 && buttonA[i]) || (mainMenu_custom_B[i]==-12 && buttonB[i]) || (mainMenu_custom_X[i]==-12 && buttonX[i]) || (mainMenu_custom_Y[i]==-12 && buttonY[i]) || (mainMenu_custom_L[i]==-12 && triggerL[i]) || (mainMenu_custom_R[i]==-12 && triggerR[i]))
					right = 1;
				else if(mainMenu_custom_dpad == 0)
				{
					if((mainMenu_custom_up[i]==-12 && dpadUp[i]) || (mainMenu_custom_down[i]==-12 && dpadDown[i]) || (mainMenu_custom_left[i]==-12 && dpadLeft[i]) || (mainMenu_custom_right[i]==-12 && dpadRight[i]))
						right = 1;
				}
			}
		}
	}

	if(!gp2xMouseEmuOn && !gp2xButtonRemappingOn)
	{
#ifdef USE_UAE4ALL_VKBD
		if(mainMenu_customControls && !vkbd_mode)
#else
		if(mainMenu_customControls)
#endif
		{
			for (int i=0; i<nr_joysticks; i++)
			{
				//The main (joyport) joystick mapping buttons
				if (nr == 1)
				{
					if((mainMenu_custom_A[i]==-3 && buttonA[i]) || (mainMenu_custom_B[i]==-3 && buttonB[i]) || (mainMenu_custom_X[i]==-3 && buttonX[i]) || (mainMenu_custom_Y[i]==-3 && buttonY[i]) || (mainMenu_custom_L[i]==-3 && triggerL[i]) || (mainMenu_custom_R[i]==-3 && triggerR[i]))
						*button = 1;
					else if(mainMenu_custom_dpad == 0)
					{
						if((mainMenu_custom_up[i]==-3 && dpadUp[i]) || (mainMenu_custom_down[i]==-3 && dpadDown[i]) || (mainMenu_custom_left[i]==-3 && dpadLeft[i]) || (mainMenu_custom_right[i]==-3 && dpadRight[i]))
							*button = 1;
					}
					if((mainMenu_custom_A[i]==-4 && buttonA[i]) || (mainMenu_custom_B[i]==-4 && buttonB[i]) || (mainMenu_custom_X[i]==-4 && buttonX[i]) || (mainMenu_custom_Y[i]==-4 && buttonY[i]) || (mainMenu_custom_L[i]==-4 && triggerL[i]) || (mainMenu_custom_R[i]==-4 && triggerR[i]))
						*button |= 1 << 1;
					else if(mainMenu_custom_dpad == 0)
					{
						if((mainMenu_custom_up[i]==-4 && dpadUp[i]) || (mainMenu_custom_down[i]==-4 && dpadDown[i]) || (mainMenu_custom_left[i]==-4 && dpadLeft[i]) || (mainMenu_custom_right[i]==-4 && dpadRight[i]))
							*button |= 1 << 1;
					}
				}
				//The second (mouseport) joystick mapping buttons
				else if (nr == 0)
				{
					if((mainMenu_custom_A[i]==-1 && buttonA[i]) || (mainMenu_custom_B[i]==-1 && buttonB[i]) || (mainMenu_custom_X[i]==-1 && buttonX[i]) || (mainMenu_custom_Y[i]==-1 && buttonY[i]) || (mainMenu_custom_L[i]==-1 && triggerL[i]) || (mainMenu_custom_R[i]==-1 && triggerR[i]))
						*button = 1;
					else if(mainMenu_custom_dpad == 0)
					{
						if((mainMenu_custom_up[i]==-1 && dpadUp[i]) || (mainMenu_custom_down[i]==-1 && dpadDown[i]) || (mainMenu_custom_left[i]==-1 && dpadLeft[i]) || (mainMenu_custom_right[i]==-1 && dpadRight[i]))
							*button = 1;
					}

					if((mainMenu_custom_A[i]==-2 && buttonA[i]) || (mainMenu_custom_B[i]==-2 && buttonB[i]) || (mainMenu_custom_X[i]==-2 && buttonX[i]) || (mainMenu_custom_Y[i]==-2 && buttonY[i]) || (mainMenu_custom_L[i]==-2 && triggerL[i]) || (mainMenu_custom_R[i]==-2 && triggerR[i]))
						*button |= 1 << 1;
					else if(mainMenu_custom_dpad == 0)
					{
						if((mainMenu_custom_up[i]==-2 && dpadUp[i]) || (mainMenu_custom_down[i]==-2 && dpadDown[i]) || (mainMenu_custom_left[i]==-2 && dpadLeft[i]) || (mainMenu_custom_right[i]==-2 && dpadRight[i]))
							*button |= 1 << 1;
					}
				}
			}
		}
	}

#ifdef USE_UAE4ALL_VKBD
	if (vkbd_mode && nr)
	{
		// move around the virtual keyboard instead

		// if Start+Dpad is used (or right analog stick on Vita)
		// move the keyboard itself and/or change transparency
#ifdef __PSP2__
		if (rAnalogY < -1024*10)
#else
		if (buttonStart[0] && (dpadUp[0] || top))
#endif
		{
			vkbd_displace_up();
		}
#ifdef __PSP2__
		else if (rAnalogY > 1024*10)
#else
		else if (buttonStart[0] && (dpadDown[0] || bot))
#endif
		{
			vkbd_displace_down();
		}
#ifdef __PSP2__
		else if (rAnalogX < -1024*10)
#else
		else if (buttonStart[0] && (dpadLeft[0] || left))
#endif
		{
			if (can_change_vkbd_transparency)
			{
				vkbd_transparency_up();
				can_change_vkbd_transparency=0;
			}
		}
#ifdef __PSP2__
		else if (rAnalogX > 1024*10)
#else
		else if (buttonStart[0] && (dpadRight[0] || right))
#endif
		{
			if (can_change_vkbd_transparency)
			{
				vkbd_transparency_down();
				can_change_vkbd_transparency=0;
			}
		}
		else
		{
			can_change_vkbd_transparency=1;
			if (left || dpadLeft[0])
				vkbd_move |= VKBD_LEFT;
			else
			{
				vkbd_move &= ~VKBD_LEFT;
				if (right || dpadRight[0])
					vkbd_move |= VKBD_RIGHT;
				else
					vkbd_move &= ~VKBD_RIGHT;
			}
			if (top || dpadUp[0])
				vkbd_move |= VKBD_UP;
			else
			{
				vkbd_move &= ~VKBD_UP;
				if (bot || dpadDown[0])
					vkbd_move |= VKBD_DOWN;
				else
					vkbd_move &= ~VKBD_DOWN;
			}
#ifdef __PSP2__ //we know the Vita has many buttons available so use those
			if (buttonX[0])
			{
				vkbd_move=VKBD_BUTTON;
				buttonX[0] = 0;
				*button = 0;
			}
			else if (buttonY[0])
			{
				vkbd_move=VKBD_BUTTON_SHIFT;
				buttonY[0] = 0;
				*button = 0;
			}
			else if (buttonA[0])
			{
				vkbd_move=VKBD_BUTTON_BACKSPACE;
				buttonA[0] = 0;
				*button = 0;
			}
			else if (buttonB[0])
			{
				vkbd_move=VKBD_BUTTON_RESET_STICKY;
				buttonB[0] = 0;
				*button = 0;
			}
#else // in other cases where those buttons might not be available, use the amiga joystick
			if (*button || buttonX[0] )
			{
				vkbd_move=VKBD_BUTTON;
				buttonX[0] = 0;
				*button = 0;
			}
#endif
			else { //button release, pressing sticky keys is possible again.
				for (int i=0; i<NUM_STICKY; i++)
				{
					vkbd_sticky_key[i].can_switch=true;
				}
			}
			// TODO: add vkbd_button2 mapped to button2
		}
	}
	else
#endif
	{
#ifdef __PSP2__
		// On Vita, map the second player to always using the GP2X mapping, in addition to everything else
		// Unless there is a custom mapping
		if (!mainMenu_customControls && (nr == 0 && mainMenu_joyPort == 2) || (nr == 1 && mainMenu_joyPort == 1))
		{
			if (dpadRight[1])
				right = 1;
			else if (dpadLeft[1])
				left = 1;
			if (dpadUp[1])
				top = 1;
			else if (dpadDown[1])
				bot = 1;
			if (
				(mainMenu_autofire & switch_autofire & delay2>mainMenu_autofireRate)
				||
					(
						(
							(mainMenu_autofireButton1==GP2X_BUTTON_B && buttonA[1])
							||
							(mainMenu_autofireButton1==GP2X_BUTTON_X && buttonX[1])
							||
							(mainMenu_autofireButton1==GP2X_BUTTON_Y && buttonY[1])
						)
						& delay2>mainMenu_autofireRate
					)
				)
			{
				if(!buttonB[1])
					*button=1;
				delay2=0;
				*button |= (buttonB[1] & 1) << 1;
			}
			else
			{
				if ((mainMenu_button1==GP2X_BUTTON_B && buttonA[1]) || (mainMenu_button1==GP2X_BUTTON_X && buttonX[1]) || (mainMenu_button1==GP2X_BUTTON_Y && buttonY[1]))
					*button |= 0x01;
				if (buttonB[1])
					*button |= (0x01 << 1);
				delay2++;
			}
		}
#endif //__PSP2__
		// normal joystick movement
		// make sure it is impossible to press left + right etc
		if (left)
			top = !top;
		if (right)
			bot = !bot;
		*dir = bot | (right << 1) | (top << 8) | (left << 9);
	}

#ifndef __PSP2__
//If not on Vita, zero the "other" joystick
	if (!mainMenu_customControls)
	{
		if(mainMenu_joyPort != 0)
		{
		 // Only one joystick active
			if((nr == 0 && mainMenu_joyPort == 2) || (nr == 1 && mainMenu_joyPort == 1))
			{
				*dir = 0;
				*button = 0;
			}
		}
	}


#endif //__PSP2__
#endif // MAXAUTOEVENTS
}

void init_joystick(void)
{
	int i;
	nr_joysticks = SDL_NumJoysticks();
	if (nr_joysticks > 0)
		uae4all_joy0 = SDL_JoystickOpen(0);
	if (nr_joysticks > 1)
		uae4all_joy1 = SDL_JoystickOpen(1);
	else
		uae4all_joy1 = NULL;
	if (nr_joysticks > 2)
		uae4all_joy2 = SDL_JoystickOpen(2);
	else
		uae4all_joy2 = NULL;
	if (nr_joysticks > 3)
		uae4all_joy3 = SDL_JoystickOpen(3);
	else
		uae4all_joy3 = NULL;
}

void close_joystick(void)
{
	if (nr_joysticks > 0)
		SDL_JoystickClose (uae4all_joy0);
	if (nr_joysticks > 1)
		SDL_JoystickClose (uae4all_joy1);
	if (nr_joysticks > 2)
		SDL_JoystickClose (uae4all_joy2);
	if (nr_joysticks > 3)
		SDL_JoystickClose (uae4all_joy3);
}
