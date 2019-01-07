extern void SetDefaultMenuSettings(int general);
extern void UpdateMemorySettings();
extern void UpdateCPUModelSettings();
extern void UpdateChipsetSettings();
extern void SetPresetMode(int mode);
extern void reset_hdConf(void);
extern void kill_hd_configs();
#if defined(__PSP2__) || defined(__SWITCH__)
extern void remap_custom_controls();
#endif

#ifndef _MENU_CONFIG_CPP

extern int mainMenu_chipMemory;
extern int mainMenu_slowMemory;
extern int mainMenu_fastMemory;
extern int mainMenu_bootHD;
extern int mainMenu_filesysUnits;
extern int hd_dir_unit_nr;
extern int hd_file_unit_nr;
extern int mainMenu_drives;
extern int mainMenu_floppyspeed;
extern int mainMenu_CPU_model;
extern int mainMenu_chipset;
extern int mainMenu_spriteCollisions;
extern int mainMenu_sound;
extern int mainMenu_soundStereo;
extern int mainMenu_soundStereoSep;
extern int mainMenu_CPU_speed;
extern int mainMenu_cpuSpeed;
extern int mainMenu_joyConf;
extern int mainMenu_joyPort;
extern int mainMenu_autofireRate;
extern int mainMenu_customAutofireButton;
extern int mainMenu_showStatus;
extern int mainMenu_background;
extern int mainMenu_mouseMultiplier;
extern int mainMenu_mouseEmulation;
extern int mainMenu_stylusOffset;
extern int mainMenu_tapDelay;
extern int mainMenu_customControls;
extern int mainMenu_custom_currentlyEditingControllerNr;
extern int mainMenu_custom_dpad;
extern int mainMenu_custom_up[MAX_NUM_CONTROLLERS];
extern int mainMenu_custom_down[MAX_NUM_CONTROLLERS];
extern int mainMenu_custom_left[MAX_NUM_CONTROLLERS];
extern int mainMenu_custom_right[MAX_NUM_CONTROLLERS];
extern int mainMenu_custom_stickup[MAX_NUM_CONTROLLERS];
extern int mainMenu_custom_stickdown[MAX_NUM_CONTROLLERS];
extern int mainMenu_custom_stickleft[MAX_NUM_CONTROLLERS];
extern int mainMenu_custom_stickright[MAX_NUM_CONTROLLERS];
extern int mainMenu_custom_A[MAX_NUM_CONTROLLERS];
extern int mainMenu_custom_B[MAX_NUM_CONTROLLERS];
extern int mainMenu_custom_X[MAX_NUM_CONTROLLERS];
extern int mainMenu_custom_Y[MAX_NUM_CONTROLLERS];
extern int mainMenu_custom_L[MAX_NUM_CONTROLLERS];
extern int mainMenu_custom_R[MAX_NUM_CONTROLLERS];
#ifdef __SWITCH__
extern int mainMenu_custom_L2[MAX_NUM_CONTROLLERS];
extern int mainMenu_custom_R2[MAX_NUM_CONTROLLERS];
extern int mainMenu_custom_L3[MAX_NUM_CONTROLLERS];
extern int mainMenu_custom_R3[MAX_NUM_CONTROLLERS];
#endif
extern int mainMenu_displayedLines;
extern int mainMenu_displayHires;
extern char presetMode[20];
extern int presetModeId;
extern int mainMenu_cutLeft;
extern int mainMenu_cutRight;
extern int mainMenu_ntsc;
extern int mainMenu_frameskip;
extern int mainMenu_vkbdLanguage;
extern int mainMenu_autofire;
extern int visibleAreaWidth;

extern int saveMenu_n_savestate;

extern int mainMenu_autosave;
extern int mainMenu_button1;
extern int mainMenu_button2;
extern int mainMenu_autofireButton1;
extern int mainMenu_jump;

extern int gp2xClockSpeed;
extern int mainMenu_scanlines;
extern int mainMenu_enableScreenshots;
extern int mainMenu_enableScripts;

#ifdef ANDROIDSDL
extern int mainMenu_onScreen;
extern int mainMenu_onScreen_textinput;
extern int mainMenu_onScreen_dpad;
extern int mainMenu_onScreen_button1;
extern int mainMenu_onScreen_button2;
extern int mainMenu_onScreen_button3;
extern int mainMenu_onScreen_button4;
extern int mainMenu_onScreen_button5;
extern int mainMenu_onScreen_button6;
extern int mainMenu_custom_position;
extern int mainMenu_pos_x_textinput;
extern int mainMenu_pos_y_textinput;
extern int mainMenu_pos_x_dpad;
extern int mainMenu_pos_y_dpad;
extern int mainMenu_pos_x_button1;
extern int mainMenu_pos_y_button1;
extern int mainMenu_pos_x_button2;
extern int mainMenu_pos_y_button2;
extern int mainMenu_pos_x_button3;
extern int mainMenu_pos_y_button3;
extern int mainMenu_pos_x_button4;
extern int mainMenu_pos_y_button4;
extern int mainMenu_pos_x_button5;
extern int mainMenu_pos_y_button5;
extern int mainMenu_pos_x_button6;
extern int mainMenu_pos_y_button6;
extern float mainMenu_button_size;
extern int menuLoad_extfilter;
extern int mainMenu_quickSwitch;
extern int mainMenu_FloatingJoystick;
#endif
#if defined(ANDROIDSDL) || defined(AROS)
extern int mainMenu_vsync;
#endif
extern char custom_kickrom[256];

#if defined(__PSP2__) || defined(__SWITCH__)
extern int mainMenu_leftStickMouse;
extern int mainMenu_touchControls;
extern int mainMenu_shader;
extern int mainMenu_deadZone;
extern int mainMenu_custom_controlSet;
extern int mainMenu_customPreset_up[MAX_NUM_CUSTOM_PRESETS][MAX_NUM_CONTROLLERS];
extern int mainMenu_customPreset_down[MAX_NUM_CUSTOM_PRESETS][MAX_NUM_CONTROLLERS];
extern int mainMenu_customPreset_left[MAX_NUM_CUSTOM_PRESETS][MAX_NUM_CONTROLLERS];
extern int mainMenu_customPreset_right[MAX_NUM_CUSTOM_PRESETS][MAX_NUM_CONTROLLERS];
extern int mainMenu_customPreset_stickup[MAX_NUM_CUSTOM_PRESETS][MAX_NUM_CONTROLLERS];
extern int mainMenu_customPreset_stickdown[MAX_NUM_CUSTOM_PRESETS][MAX_NUM_CONTROLLERS];
extern int mainMenu_customPreset_stickleft[MAX_NUM_CUSTOM_PRESETS][MAX_NUM_CONTROLLERS];
extern int mainMenu_customPreset_stickright[MAX_NUM_CUSTOM_PRESETS][MAX_NUM_CONTROLLERS];
extern int mainMenu_customPreset_A[MAX_NUM_CUSTOM_PRESETS][MAX_NUM_CONTROLLERS];
extern int mainMenu_customPreset_B[MAX_NUM_CUSTOM_PRESETS][MAX_NUM_CONTROLLERS];
extern int mainMenu_customPreset_X[MAX_NUM_CUSTOM_PRESETS][MAX_NUM_CONTROLLERS];
extern int mainMenu_customPreset_Y[MAX_NUM_CUSTOM_PRESETS][MAX_NUM_CONTROLLERS];
extern int mainMenu_customPreset_L[MAX_NUM_CUSTOM_PRESETS][MAX_NUM_CONTROLLERS];
extern int mainMenu_customPreset_R[MAX_NUM_CUSTOM_PRESETS][MAX_NUM_CONTROLLERS];
#ifdef __SWITCH__
extern int mainMenu_customPreset_L2[MAX_NUM_CUSTOM_PRESETS][MAX_NUM_CONTROLLERS];
extern int mainMenu_customPreset_R2[MAX_NUM_CUSTOM_PRESETS][MAX_NUM_CONTROLLERS];
extern int mainMenu_customPreset_L3[MAX_NUM_CUSTOM_PRESETS][MAX_NUM_CONTROLLERS];
extern int mainMenu_customPreset_R3[MAX_NUM_CUSTOM_PRESETS][MAX_NUM_CONTROLLERS];
#endif
#ifdef __SWITCH__
extern int mainMenu_swapAB;
extern int mainMenu_singleJoycons;
#endif
#endif

#endif