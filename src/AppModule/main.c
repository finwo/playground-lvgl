
#include "appmodule.h"
#include "lvgl/src/core/lv_obj_pos.h"
#include "lvgl/src/core/lv_obj_tree.h"
#include "lvgl/src/display/lv_display.h"
#include "lvgl/src/misc/lv_area.h"
#include "lvgl/src/others/observer/lv_observer.h"
#include "lvgl/src/widgets/image/lv_image.h"
#include <math.h>
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>

#include "AppModule/appmodule.h"
#include "util/rng.h"

int32_t horizon_speed  = -320;
int32_t horizon_tick   =    0;
int32_t horizon_offset =    0;

int32_t trex_speed  = -10;
int32_t trex_tick   =  0;
int32_t trex_offset =  0;

// int _get_asset_idx_by_name(const char *name) {
//   for(int i = 0; appmodule_assets[i].name; i++) {
//     if (strcmp(name, appmodule_assets[i].name)) continue;
//     return i;
//   }
//   return -1;
// }

void appmodule_loop(uint32_t elapsedTime) {
  int i;

  if (game_state == GAME_STATE_WAITING) {

    // Space pressed = go to intro animation
    if (KEYS[APP_KEYCODE_SPACE]) {
      printf("Space was pressed, starting...\n");
      game_state = GAME_STATE_ANIM_INTRO;
    }

    // No game actions during wait

  } else if (game_state == GAME_STATE_ANIM_INTRO) {

    // Move the runner into position in 500ms
    if (runner->base.pos.x < (runner_normal_width/display_scaling/2)) {

      // Smooth walk
      runner->base.speed.x_tick += elapsedTime * (runner_normal_width/display_scaling/2)*time_window/500;
      runner->base.pos.x        += runner->base.speed.x_tick / time_window;
      runner->base.speed.x_tick  = runner->base.speed.x_tick % time_window;
      // Move the image itself
      lv_obj_set_x(runner->el, (runner->base.pos.x * display_scaling) + (runner->base.speed.x_tick * display_scaling / time_window));
      lv_obj_set_y(runner->el, (runner->base.pos.y * display_scaling) + (runner->base.speed.y_tick * display_scaling / time_window));
    } else {

      // Ensure the runner is properly aligned
      runner->base.pos.x        = runner_normal_width/display_scaling/2;
      runner->base.speed.x_tick = 0;
      lv_obj_set_x(runner->el, (runner->base.pos.x * display_scaling));

      // Runner is in position, let's start
      game_state = GAME_STATE_RUNNING;
    }

  }





  // // Update clouds
  // for(i=0 ; i < cloud_count ; i++) {
  //   struct game_obj_drawn *_cloud = clouds[i];

  //   // Glide left (because speed.x<0)
  //   _cloud->base.speed.x_tick += elapsedTime * _cloud->base.speed.x;
  //   _cloud->base.pos.x        += _cloud->base.speed.x_tick / time_window;
  //   _cloud->base.speed.x_tick  = _cloud->base.speed.x_tick % time_window;

  //   // Out of bounds left = "respawn"
  //   if ((_cloud->base.speed.x < 0) && (_cloud->base.pos.x < (0 - cloud_width / display_scaling))) {
  //     _cloud->base.pos.x   = display_width;
  //     _cloud->base.pos.y   = rand_between(cloud_minY, cloud_maxY);
  //   }

  //   // Out of bounds right = "respawn"
  //   if ((_cloud->base.speed.x > 0) && (_cloud->base.pos.x > display_width)) {
  //     _cloud->base.pos.x   = 0 - (cloud_width / display_scaling);
  //     _cloud->base.pos.y   = rand_between(cloud_minY, cloud_maxY);
  //   }

  //   // Move the image itself
  //   lv_obj_set_x(_cloud->el, (_cloud->base.pos.x * display_scaling) + (_cloud->base.speed.x_tick * display_scaling / time_window));
  //   lv_obj_set_y(_cloud->el, (_cloud->base.pos.y * display_scaling) + (_cloud->base.speed.y_tick * display_scaling / time_window));

  //   // if (!i) {
  //   //   printf("0: %d,%d -- %d\n", _cloud->base.pos.x, _cloud->base.pos.y, _cloud->base.speed.x_tick);
  //   // }

  // }

  // // Update ground
  // for(i=0; i < horizon_line_count ; i++) {
  //   struct game_obj_drawn *_horizon_line = horizon_lines[i];

  //   // Glide left (because speed.x<0)
  //   _horizon_line->base.speed.x_tick += elapsedTime * _horizon_line->base.speed.x;
  //   _horizon_line->base.pos.x        += _horizon_line->base.speed.x_tick / time_window;
  //   _horizon_line->base.speed.x_tick  = _horizon_line->base.speed.x_tick % time_window;

  //   // Out of bounds left = "respawn" with new random from ground sets
  //   if ((_horizon_line->base.speed.x < 0) && (_horizon_line->base.pos.x < (0 - horizon_line_width / display_scaling))) {
  //     _horizon_line->base.pos.x += (horizon_line_width/display_scaling) * horizon_line_count;
  //     lv_image_set_offset_x(_horizon_line->el, 0 - horizon_line_sourceX - (rand_between(0,3)*horizon_line_width));
  //   }

  //   // Move the image itself
  //   lv_obj_set_x(_horizon_line->el, (_horizon_line->base.pos.x * display_scaling) + (_horizon_line->base.speed.x_tick * display_scaling / time_window));
  //   lv_obj_set_y(_horizon_line->el, (_horizon_line->base.pos.y * display_scaling) + (_horizon_line->base.speed.y_tick * display_scaling / time_window));
  // }


  // // Add a cloud when not enough are present
  // clouds = calloc(1, sizeof(struct game_obj_drawn));
  // clouds[0].base.pos.x = displayWidth;
  // clouds[0].base.pos.y = ;





  // // Cycle through trex sprites
  // int trex_idx = _get_asset_idx_by_name("assets/trex.png");
  // if (trex_idx >= 0) {
  //   struct appmodule_asset asset_trex = appmodule_assets[trex_idx];
  //   trex_tick   += elapsedTime * trex_speed;
  //   trex_offset += (trex_tick / TIME_WINDOW);
  //   trex_tick    = trex_tick % TIME_WINDOW;
  //   trex_offset  = trex_offset % asset_trex.sprites;

  //   lv_obj_t *img_trex = lv_obj_find_by_name(NULL, "trex");
  //   lv_image_set_offset_x(img_trex, trex_offset * asset_trex.dsc->header.w / asset_trex.sprites);
  // }



}

#ifdef __cplusplus
} // extern "C"
#endif
