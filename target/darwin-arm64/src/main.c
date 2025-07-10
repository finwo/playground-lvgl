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

#include "AppModule/init.h"

#define SDL_HOR_RES 960
#define SDL_VER_RES 640

lv_display_t *lvDisplay;
lv_indev_t *lvMouse;
lv_indev_t *lvMouseWheel;
lv_indev_t *lvKeyboard;

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

  if (appmodule_init()) {
    log_fatal("Error setting up AppModule");
    return 1;
  }

  Uint32 lastTick = SDL_GetTicks();
  while(1) {
    SDL_Delay(5);
    Uint32 current = SDL_GetTicks();
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
