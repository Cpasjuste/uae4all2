cmake_minimum_required(VERSION 3.0)
#set(CMAKE_VERBOSE_MAKEFILE ON)

if (NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if (DEFINED ENV{VITASDK})
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
    else ()
        message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
    endif ()
endif ()

execute_process(COMMAND "date" "+%Y.%m.%d"
        OUTPUT_VARIABLE BUILD_DATE OUTPUT_STRIP_TRAILING_WHITESPACE)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(BUILD_DEBUG true CACHE BOOL "Build with psp2shell support")
endif()

enable_language(ASM)

project(uae4all2)
include("${VITASDK}/share/vita.cmake" REQUIRED)

set(TITLEID "UAE4ALL00")

set(SRC_FILES
        src/psp2/psp2_shader.cpp
        src/psp2/psp2_input.c
        src/psp2/psp2-dirent.c
		src/psp2/psp2_touch.c
        src/vkbd/vkbd.cpp
        src/neon_helper.s
        src/gp2x/memset.s
        src/gp2x/memcpy.s
        src/audio.cpp
        src/autoconf.cpp
        src/blitfunc.cpp
        src/blittable.cpp
        src/blitter.cpp
        src/cfgfile.cpp
        src/cia.cpp
        src/savedisk.cpp
        src/savestate.cpp
        src/custom.cpp
        src/disk.cpp
        src/drawing.cpp
        src/ersatz.cpp
        src/expansion.cpp
        src/filesys.cpp
        src/fsdb.cpp
        src/fsdb_unix.cpp
        src/fsusage.cpp
        src/gfxutil.cpp
        src/hardfile.cpp
        src/keybuf.cpp
        src/main.cpp
        src/memory.cpp
        src/missing.cpp
        src/native2amiga.cpp
        src/gui.cpp
        src/od-joy.cpp
        src/scsi-none.cpp
        src/sdlgfx.cpp
        src/writelog.cpp
        src/zfile.cpp
        src/menu/fade.cpp
        src/gp2x/gp2x.cpp
        src/gp2x/inputmode.cpp
        src/gp2x/menu/menu_helper.cpp
        src/gp2x/menu/menu_config.cpp
        src/gp2x/menu/menu.cpp
        src/gp2x/menu/menu_fileinfo.cpp
        src/gp2x/menu/menu_load.cpp
        src/gp2x/menu/menu_main.cpp
        src/gp2x/menu/menu_savestates.cpp
        src/gp2x/menu/menu_misc.cpp
        src/gp2x/menu/menu_controls.cpp
        src/gp2x/menu/menu_display.cpp
        src/gp2x/menu/menu_memory_disk.cpp
        src/sound_sdl_new.cpp
        src/m68k/fame/famec.cpp
        src/m68k/fame/m68k_intrf.cpp
        src/m68k/m68k_cmn_intrf.cpp
        )

include_directories(
        ${VITASDK}/arm-vita-eabi/include/SDL
        ../common/include
        include
        src
        src/psp2
        src/gp2x
        src/menu
        src/include
        src/gp2x/menu
        src/vkbd
)

set(FLAGS
        -Wno-write-strings
        -DVERSION=\"${BUILD_DATE}\"
        -DGP2X -DPANDORA -D__PSP2__ -DUSE_AUTOCONFIG -DUSE_ZFILE -DUSE_FAME_CORE -DWITH_TESTMODE
        -DUSE_SDL -DGCCCONSTFUNC="__attribute__\(\(const\)\)" -DUSE_UNDERSCORE
        -DSHM_SUPPORT_LINKS=0 -DUNALIGNED_PROFITABLE -DOPTIMIZED_FLAGS -DOS_WITHOUT_MEMORY_MANAGEMENT
        -DROM_PATH_PREFIX=\"ux0:/data/uae4all/roms/\" -DDATA_PREFIX=\"app0:/data/\" -DSAVE_PREFIX=\"ux0:/data/uae4all/saves/\"
        -DUSE_UAE4ALL_VKBD
        -DVKBD_ALWAYS
        -DLARGEKEYBOARD
        -DUSE_ARMV7
        -DUSE_ARMNEON
        -marm -mfpu=neon -mcpu=cortex-a9 -march=armv7-a -mfloat-abi=hard -ffast-math
        -fno-asynchronous-unwind-tables -funroll-loops
        -mword-relocations -fno-unwind-tables -fno-optimize-sibling-calls
        -mvectorize-with-neon-quad -funsafe-math-optimizations
        -mlittle-endian -munaligned-access
        )

set(LDFLAGS
        SDL SDL_image
        ${CMAKE_SOURCE_DIR}/src/psp2/vita2d_fbo/lib/libvita2d.a
        ${CMAKE_SOURCE_DIR}/src/psp2/vita-shader-collection/lib/libvitashaders.a
        SceSysmodule_stub SceDisplay_stub SceGxm_stub
        SceCtrl_stub ScePgf_stub ScePower_stub SceCommonDialog_stub
        SceAudio_stub SceShellSvc_stub SceHid_stub SceTouch_stub png jpeg z m c
        )

if (BUILD_DEBUG)
    list(APPEND FLAGS -DDEBUG -DDEBUG_UAE4ALL -DUAE_CONSOLE)
    list(APPEND LDFLAGS psp2shell SceSysmodule_stub SceNetCtl_stub
            SceNet_stub SceAppUtil_stub SceAppMgr_stub
            )
    set(UNSAFE_FLAG UNSAFE)
else ()
    list(APPEND FLAGS -O3)
endif (BUILD_DEBUG)

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_compile_options(${PROJECT_NAME} PRIVATE ${FLAGS})
target_link_libraries(${PROJECT_NAME} ${LDFLAGS})

vita_create_self(${PROJECT_NAME}.self ${PROJECT_NAME} ${UNSAFE_FLAG})
add_custom_target(${PROJECT_NAME}.vpk
        DEPENDS ${PROJECT_NAME}.self
        COMMAND rm -rf vpk && mkdir -p vpk/sce_sys
        COMMAND ${VITASDK}/bin/vita-mksfoex -s TITLE_ID=${TITLEID} "${PROJECT_NAME}" vpk/sce_sys/param.sfo
        COMMAND cp ${PROJECT_NAME}.self vpk/eboot.bin
        COMMAND cp -r ${CMAKE_SOURCE_DIR}/psp2data/* vpk
        COMMAND cd vpk && zip -r ../uae4all-${BUILD_DATE}.vpk . && cd ..
        )
