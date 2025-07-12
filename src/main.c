#include "lvgl/src/drivers/sdl/lv_sdl_window.h"
#include "lvgl/src/libs/lodepng/lv_lodepng.h"
#include <stdint.h>

#ifdef _WIN32
#include <windows.h>
#else
// #include <pthread.h>
#include <unistd.h>
#endif

// Only used to suppress warnings caused by unused parameters.
#ifndef UNUSED
#define UNUSED(x) (void)x
#endif

#include "lvgl/lvgl.h"
#include "rxi/log.h"

// #include "lfs.h"
// #include "bd/lfs_rambd.h"

#include "AppModule/appmodule.h"

#define SDL_HOR_RES 960
#define SDL_VER_RES 640

lv_display_t *lvDisplay;
lv_indev_t *lvMouse;
lv_indev_t *lvMouseWheel;
lv_indev_t *lvKeyboard;

// lfs_t      lfs;
// lfs_file_t file;

#if LV_USE_LOG != 0
static void lv_log_print_g_cb(lv_log_level_t level, const char * buf) {
    UNUSED(level);
    UNUSED(buf);
}
#endif

#ifndef WINTERM
#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) {
  UNUSED(hInst);
  UNUSED(hPrevInst);
  UNUSED(lpCmdLine);
  UNUSED(nCmdShow);
#else
int main() {
#endif
#else // no WINTERM
int main() {
#endif

  lv_init();

  // printf("--- Initializing rambd\n");
  // const struct lfs_rambd_config bdcfg = {
  //   .prog_size   = 4096,
  //   .read_size   = 4096,
  //   .erase_size  = 4096,
  //   .erase_count = 512,
  // };
  // const struct lfs_config cfg = {
  //   .context        = calloc(1, sizeof(lfs_rambd_t)),
  //   .read           = lfs_rambd_read,
  //   .prog           = lfs_rambd_prog,
  //   .erase          = lfs_rambd_erase,
  //   .sync           = lfs_rambd_sync,
  //   .read_size      = bdcfg.read_size,
  //   .prog_size      = bdcfg.prog_size,
  //   .block_size     = bdcfg.erase_size,
  //   .block_count    = bdcfg.erase_count,
  //   .cache_size     = bdcfg.read_size,
  //   .lookahead_size = bdcfg.read_size,
  //   .block_cycles   = -1,
  // };
  // lfs_rambd_create(&cfg, &bdcfg);
  // printf("  - created\n");
  // // mount the filesystem
  // int err = lfs_mount(&lfs, &cfg);
  // printf("  - mount : %d\n", err);
  // // reformat if we can't mount the filesystem
  // // this should only happen on the first boot
  // if (err) {
  //   err = lfs_format(&lfs, &cfg);
  //   printf("  - format: %d\n", err);
  //   err = lfs_mount(&lfs, &cfg);
  //   printf("  - mount : %d\n", err);
  // }
  // lv_littlefs_set_handler(&lfs);
  // printf("  - handler set\n");

  // Workaround for sdl2 `-m32` crash
  // https://bugs.launchpad.net/ubuntu/+source/libsdl2/+bug/1775067/comments/7
  #ifndef WIN32
    setenv("DBUS_FATAL_WARNINGS", "0", 1);
  #endif

  /* Register the log print callback */
  #if LV_USE_LOG != 0
  lv_log_register_print_cb(lv_log_print_g_cb);
  #endif

  /* Add a display
  * Use the 'monitor' driver which creates window on PC's monitor to simulate a display*/
  lvDisplay    = lv_sdl_window_create(SDL_HOR_RES, SDL_VER_RES);
  lvMouse      = lv_sdl_mouse_create();
  lvMouseWheel = lv_sdl_mousewheel_create();
  lvKeyboard   = lv_sdl_keyboard_create();

  lv_sdl_window_set_resizeable(lvDisplay, false);

  if (appmodule_setup()) {
    log_fatal("Error setting up AppModule");
    return 1;
  }

  Uint32 lastTick = SDL_GetTicks();
  while(1) {
    SDL_Delay(5);
    Uint32 current = SDL_GetTicks();
    appmodule_loop(current - lastTick);
    lv_tick_inc(current - lastTick); // Update the tick timer. Tick is new for LVGL 9
    lastTick = current;
    lv_timer_handler(); // Update the UI-
  }

  return 0;
}

#ifdef WINTERM
#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) {
  UNUSED(hInst);
  UNUSED(hPrevInst);
  UNUSED(lpCmdLine);
  UNUSED(nCmdShow);
  return main();
}
#endif
#endif
