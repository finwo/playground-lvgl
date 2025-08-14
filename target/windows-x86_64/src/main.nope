#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl/src/drivers/sdl/lv_sdl_window.h"

// #include <SDL2/SDL_keyboard.h>
#include <stdint.h>

#ifndef _GNU_SOURCE
#include "finwo/asprintf.h"
#endif

#include <windows.h>
#include <winuser.h>

// Only used to suppress warnings caused by unused parameters.
#ifndef UNUSED
#define UNUSED(x) (void)x
#endif

#include "lvgl/lvgl.h"
#include "rxi/log.h"
#include "kgabis/parson.h"

// #include "lfs.h"
// #include "bd/lfs_rambd.h"

#include "util/get_bin_path.h"
#include "util/fs.h"
#include "util/time_millis.h"

#include "AppModule/appmodule.h"

lv_display_t *lvDisplay;
lv_indev_t *lvMouse;
lv_indev_t *lvMouseWheel;
lv_indev_t *lvKeyboard;

int display_scaling;
int display_width;
int display_height;

PBYTE KEYS;

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
  log_set_level(LOG_ERROR);

  // Seed random, to not always produce the exact same results
  srand(time(NULL));

  lv_init();

  // Workaround for sdl2 `-m32` crash
  // https://bugs.launchpad.net/ubuntu/+source/libsdl2/+bug/1775067/comments/7
  #ifndef WIN32
    setenv("DBUS_FATAL_WARNINGS", "0", 1);
  #endif

  char *binPath = get_bin_path();
  if (!binPath) {
    log_fatal("Could not detect binary path");
    exit(1);
  }

  char *binDir = dirname(strdup(binPath));
  if (!binDir) {
    log_fatal("Could not detect binary directory path");
    exit(1);
  }

  // Ensure global defs exist
  char *globalConfigFile = NULL;
  asprintf(&globalConfigFile, "%s%s", binDir, "/assets/global.json");
  if (!file_exists(globalConfigFile, "r")) {
    log_fatal("assets/global.json is not available");
    exit(1);
  }

  // Read the config with minimal validation
  struct buf *globalConfigContents = file_get_contents(globalConfigFile);
  JSON_Value *config_root = json_parse_string(globalConfigContents->data);
  if (json_value_get_type(config_root) != JSONObject) {
    log_fatal("assets/global.json is not an object");
    exit(1);
  }
  JSON_Object *obj_root = json_value_get_object(config_root);
  if (!json_object_has_value_of_type(obj_root, "display", JSONObject)) {
    log_fatal("assets/global.json missing display object");
    exit(1);
  }

  // Fetch display configuration
  JSON_Object *obj_display = json_object_get_object(obj_root, "display");
  if (!json_object_has_value_of_type(obj_display, "scaling", JSONNumber)) {
    log_fatal("assets/global.json missing display.scaling number");
    exit(1);
  }
  if (!json_object_has_value_of_type(obj_display, "width", JSONNumber)) {
    log_fatal("assets/global.json missing display.width number");
    exit(1);
  }
  if (!json_object_has_value_of_type(obj_display, "height", JSONNumber)) {
    log_fatal("assets/global.json missing display.height number");
    exit(1);
  }
  display_scaling = (int)json_object_get_number(obj_display, "scaling");
  display_width   = (int)json_object_get_number(obj_display, "width");
  display_height  = (int)json_object_get_number(obj_display, "height");
  if (display_scaling < 1 || display_scaling > 2) {
    log_fatal("Invalid display scaling, only 1 or 2 allowed");
    exit(1);
  }

  /* Register the log print callback */
  #if LV_USE_LOG != 0
  lv_log_register_print_cb(lv_log_print_g_cb);
  #endif

  int32_t zoom_level = 100;
  bool allow_dpi_override = false;
  bool simulator_mode = false;
  lvDisplay = lv_windows_create_display(
      L"LVGL Playground",
      display_width  * display_scaling,
      display_height * display_scaling,
      zoom_level,
      allow_dpi_override,
      simulator_mode);
  if (!lvDisplay) {
    log_fatal("Could not initialize display");
    return -1;
  }

  lv_lock();

  lvMouse = lv_windows_acquire_pointer_indev(lvDisplay);
  if (!lvMouse) {
    log_fatal("Could not initialize mouse");
    return -1;
  }

  lvKeyboard = lv_windows_acquire_keypad_indev(lvDisplay);
  if (!lvKeyboard) {
    log_fatal("Could not initialize keyboard");
    return -1;
  }

  // TODO: this the mouse wheel?
  // lv_indev_t* encoder_device = lv_windows_acquire_encoder_indev(lvDisplay);
  // if (!encoder_device) {
  //     return -1;
  // }

  // TODO:
  // lv_sdl_window_set_resizeable(lvDisplay, false);

  // Setup keyboard handling
  // No need to update in loop, points to SDL internal array
  // No need to translate keycodes, we're following USB standard just like SDL
  KEYS = calloc(256, sizeof(BYTE));
  if (!GetKeyboardState(KEYS)) {
    log_fatal("Could not get initial keyboard state");
    return 1;
  }

  if (appmodule_setup(obj_root)) {
    log_fatal("Error setting up AppModule");
    return 1;
  }

  lv_unlock();

  long long lastTick = time_millis();
  while (1) {
    long long current = time_millis();
    appmodule_loop(current - lastTick);
    uint32_t time_till_next = lv_timer_handler();
    if(time_till_next == LV_NO_TIMER_READY) time_till_next = LV_DEF_REFR_PERIOD;
    lv_delay_ms(time_till_next);
    lastTick = current;
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

#ifdef __cplusplus
} // extern "C"
#endif
