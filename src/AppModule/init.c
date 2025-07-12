#include "lvgl/lvgl.h"
#include "lvgl/src/draw/lv_image_dsc.h"
#include "lvgl/src/misc/lv_types.h"
#include "lvgl/src/draw/lv_image_decoder.h"
#include "lvgl/src/draw/lv_image_decoder_private.h"

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

#include <stdio.h>

#ifdef _WIN32
// #include <windows.h>
#else
// #include <pthread.h>
// #include <unistd.h>
#endif

#include "rxi/log.h"

#include "AppModule/init.h"
#include "util/incbin.h"

INCBIN(component_my_button, "AppModule/components/my_button.xml");
INCBIN(screen_main        , "AppModule/screens/main.xml"        );
INCBIN(screen_about       , "AppModule/screens/about.xml"       );

INCBIN(img_cloud         , "AppModule/assets/2x-cloud.png");
INCBIN(img_horizon       , "AppModule/assets/2x-horizon.png");
INCBIN(img_obstacle_large, "AppModule/assets/2x-obstacle-large.png");
INCBIN(img_obstacle_small, "AppModule/assets/2x-obstacle-small.png");
INCBIN(img_restart       , "AppModule/assets/2x-restart.png");
INCBIN(img_text          , "AppModule/assets/2x-text.png");
INCBIN(img_trex          , "AppModule/assets/2x-trex.png");

static void appmodule_switch_screen(lv_event_t * e) {
  printf("switch_screen called\n");
}

static void appmodule_main_screen_events(lv_event_t * e) {
  lv_event_code_t event_code = lv_event_get_code(e);
  printf("Event! %d\n", event_code);
}


int appmodule_init() {
  const char *loglevel = "trace";

  if (0) {
    // Intentionally empty
  } else if (!strcasecmp(loglevel, "trace")) {
    log_set_level(LOG_TRACE);
  } else if (!strcasecmp(loglevel, "debug")) {
    log_set_level(LOG_DEBUG);
  } else if (!strcasecmp(loglevel, "info")) {
    log_set_level(LOG_INFO);
  } else if (!strcasecmp(loglevel, "warn")) {
    log_set_level(LOG_WARN);
  } else if (!strcasecmp(loglevel, "error")) {
    log_set_level(LOG_ERROR);
  } else if (!strcasecmp(loglevel, "fatal")) {
    log_set_level(LOG_FATAL);
  } else {
    fprintf(stderr, "Unknown log level: %s\n", loglevel);
    return 1;
  }

  // // Write our assets to ramdisk
  // // Allows us to reference them without carrying the decoded data everywhere
  // const char *assets[] = {
  //   "assets/cloud.png"         , img_cloud_start         , img_cloud_end         ,
  //   "assets/horizon.png"       , img_horizon_start       , img_horizon_end       ,
  //   "assets/obstacle-large.png", img_obstacle_large_start, img_obstacle_large_end,
  //   "assets/obstacle-small.png", img_obstacle_small_start, img_obstacle_small_end,
  //   "assets/restart.png"       , img_restart_start       , img_restart_end       ,
  //   "assets/text.png"          , img_text_start          , img_text_end          ,
  //   "assets/trex.png"          , img_trex_start          , img_trex_end          ,
  //   NULL,
  // };
  // lfs_mkdir(&lfs, "assets");
  // for(int i = 0; assets[i]; i+=3) {
  //   lfs_file_open(&lfs, &file, assets[i], LFS_O_RDWR | LFS_O_CREAT);
  //   lfs_file_write(&lfs, &file, assets[i+1], assets[i+2] - assets[i+1]);
  //   lfs_file_close(&lfs, &file);
  // }

  struct local_asset {
    char *name;
    char *start;
    char *end;
    int   sprites;
    lv_image_dsc_t *dsc;
  };

  lv_image_dsc_t loader_dsc = {};
  lv_result_t res;
  lv_image_decoder_dsc_t dsc;
  struct local_asset assets[] = {
    { .name = "assets/cloud.png"         , .start = img_cloud_start         , .end = img_cloud_end         , .sprites = 1, },
    { .name = "assets/horizon.png"       , .start = img_horizon_start       , .end = img_horizon_end       , .sprites = 1, },
    { .name = "assets/obstacle_large.png", .start = img_obstacle_large_start, .end = img_obstacle_large_end, .sprites = 3, },
    { .name = "assets/obstacle_small.png", .start = img_obstacle_small_start, .end = img_obstacle_small_end, .sprites = 6, },
    { .name = "assets/restart.png"       , .start = img_restart_start       , .end = img_restart_end       , .sprites = 1, },
    { .name = "assets/text.png"          , .start = img_text_start          , .end = img_text_end          , .sprites = 1, },
    { .name = "assets/trex.png"          , .start = img_trex_start          , .end = img_trex_end          , .sprites = 6, },
    { .name = NULL, },
  };
  for(int i = 0; assets[i].name; i++) {
    loader_dsc.data      = assets[i].start;
    loader_dsc.data_size = assets[i].end - assets[i].start;
    lv_image_decoder_open(&dsc, &loader_dsc, NULL);
    assets[i].dsc = dsc.decoded;
  }

  // test_dsc.data = img_obstacle_small_start;
  // test_dsc.data_size = img_obstacle_small_end - img_obstacle_small_start;
  // lv_image_decoder_open(&dsc, &test_dsc, &args);
  // width_obstacle_small = dsc.decoded->header.w / sprites_obstacle_small;
  // lv_image_decoder_close(&dsc);

  // test_dsc.data = img_trex_start;
  // test_dsc.data_size = img_trex_end - img_trex_start;
  // lv_image_decoder_open(&dsc, &test_dsc, &args);
  // width_trex = dsc.decoded->header.w / sprites_trex;
  // lv_image_decoder_close(&dsc);

  // printf("large size: %d\n", width_obstacle_large);
  // printf("small size: %d\n", width_obstacle_small);
  // printf("trex size : %d\n", width_trex);

  for(int i = 0; assets[i].name; i++) {
    printf("[%*s] %*d / %d = %*d\n", 25, assets[i].name, 4, assets[i].dsc->header.w, assets[i].sprites, 4, assets[i].dsc->header.w / assets[i].sprites);
  }
  exit(0);

  // lv_xml_register_event_cb(NULL, "my_callback_1", appmodule_switch_screen);

  lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x2255AA), LV_PART_MAIN);
  // lv_xml_component_register_from_data("my_button"   , component_my_button_start);
  // lv_xml_component_register_from_data("screen_about", screen_about_start       );
  lv_xml_component_register_from_data("screen_main" , screen_main_start        );

  lv_obj_t * screen_main  = lv_xml_create(NULL, "screen_main", NULL);
  lv_obj_add_event_cb(screen_main, appmodule_main_screen_events, LV_EVENT_CLICKED | LV_EVENT_PRESSED, NULL);
  lv_scr_load(screen_main);






  return 0;
}

#ifdef __cplusplus
} // extern "C"
#endif
