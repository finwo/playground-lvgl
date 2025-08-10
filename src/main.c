#include "lvgl/src/drivers/sdl/lv_sdl_window.h"

#include <SDL2/SDL_keyboard.h>
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
#include "kgabis/parson.h"

// #include "lfs.h"
// #include "bd/lfs_rambd.h"

#include "util/get_bin_path.h"
#include "util/fs.h"

#include "AppModule/appmodule.h"

#define SDL_HOR_RES 600
#define SDL_VER_RES 150

lv_display_t *lvDisplay;
lv_indev_t *lvMouse;
lv_indev_t *lvMouseWheel;
lv_indev_t *lvKeyboard;

// lfs_t      lfs;
// lfs_file_t file;

int display_scaling;
int display_width;
int display_height;

const bool *KEYS;

#if LV_USE_LOG != 0
static void lv_log_print_g_cb(lv_log_level_t level, const char * buf) {
    UNUSED(level);
    UNUSED(buf);
}
#endif

// // Not an option, crashes lvgl because we rob the events
// void handle_sdl_keyboard() {
//   SDL_Event event;
//   while (SDL_PollEvent(&event)) {
//     switch (event.type) {
//       case SDL_KEYDOWN:
//         if (event.key.keysym.sym >= 0 && event.key.keysym.sym < 322) {
//           KEYS[event.key.keysym.sym] = true;
//         }
//         break;
//       case SDL_KEYUP:
//         if (event.key.keysym.sym >= 0 && event.key.keysym.sym < 322) {
//           KEYS[event.key.keysym.sym] = false;
//         }
//         break;
//       default:
//         break;
//     }
//   }
// }

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

  /* Add a display
  * Use the 'monitor' driver which creates window on PC's monitor to simulate a display*/
  lvDisplay    = lv_sdl_window_create(display_width * display_scaling, display_height * display_scaling);
  lvMouse      = lv_sdl_mouse_create();
  lvMouseWheel = lv_sdl_mousewheel_create();
  lvKeyboard   = lv_sdl_keyboard_create();

  lv_sdl_window_set_resizeable(lvDisplay, false);

  // Setup keyboard handling
  // No need to update in loop, points to SDL internal array
  // No need to translate keycodes, we're following USB standard just like SDL
  KEYS = (const bool *)SDL_GetKeyboardState(NULL);

  if (appmodule_setup(obj_root)) {
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
