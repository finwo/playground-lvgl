
#include "lvgl/src/core/lv_obj_tree.h"
#include "lvgl/src/display/lv_display.h"
#include "lvgl/src/others/observer/lv_observer.h"
#include "lvgl/src/widgets/image/lv_image.h"
#include <math.h>
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>

#include "AppModule/appmodule.h"

#define TIME_WINDOW 10000 // 10 seconds window

int32_t horizon_speed  = -1000;
int32_t horizon_tick   =    0;
int32_t horizon_offset =    0;

int32_t trex_speed  = -10;
int32_t trex_tick   =  0;
int32_t trex_offset =  0;

int _get_asset_idx_by_name(const char *name) {
  for(int i = 0; appmodule_assets[i].name; i++) {
    if (strcmp(name, appmodule_assets[i].name)) continue;
    return i;
  }
  return -1;
}

void appmodule_loop(uint32_t elapsedTime) {

  // Move the horizon
  int horizon_idx = _get_asset_idx_by_name("assets/horizon.png");
  if (horizon_idx >= 0) {
    struct appmodule_asset asset_horizon = appmodule_assets[horizon_idx];

    horizon_tick   += elapsedTime * horizon_speed;
    horizon_offset += horizon_tick / TIME_WINDOW;
    horizon_tick    = horizon_tick % TIME_WINDOW;
    horizon_offset  = horizon_offset % asset_horizon.dsc->header.w;

    lv_obj_t *img_horizon = lv_obj_find_by_name(NULL, "horizon");
    lv_image_set_offset_x(img_horizon, horizon_offset);

    // lv_subject_set_int(&subj_horizon_offset, horizon_offset);
  }

  // Cycle through trex sprites
  int trex_idx = _get_asset_idx_by_name("assets/trex.png");
  if (trex_idx >= 0) {
    struct appmodule_asset asset_trex = appmodule_assets[trex_idx];
    trex_tick   += elapsedTime * trex_speed;
    trex_offset += (trex_tick / TIME_WINDOW);
    trex_tick    = trex_tick % TIME_WINDOW;
    trex_offset  = trex_offset % asset_trex.sprites;

    lv_obj_t *img_trex = lv_obj_find_by_name(NULL, "trex");
    lv_image_set_offset_x(img_trex, trex_offset * asset_trex.dsc->header.w / asset_trex.sprites);
  }



}

#ifdef __cplusplus
} // extern "C"
#endif
