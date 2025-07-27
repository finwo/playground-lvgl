
#include "appmodule.h"
#include "lvgl/src/core/lv_obj_pos.h"
#include "lvgl/src/core/lv_obj_tree.h"
#include "lvgl/src/display/lv_display.h"
#include "lvgl/src/indev/lv_indev.h"
#include "lvgl/src/misc/lv_area.h"
#include "lvgl/src/misc/lv_color.h"
#include "lvgl/src/misc/lv_types.h"
#include "lvgl/src/others/observer/lv_observer.h"
#include "lvgl/src/widgets/image/lv_image.h"
#include <math.h>
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>

#include "rxi/log.h"

#include "AppModule/appmodule.h"
#include "util/rng.h"

int32_t horizon_speed  = -320;
int32_t horizon_tick   =    0;
int32_t horizon_offset =    0;

int32_t trex_speed  = -10;
int32_t trex_tick   =  0;
int32_t trex_offset =  0;

const int32_t anim_start_move_duration = 500;
const int32_t anim_start_move_steps    =   3;

int32_t anim_runner_substep = 0;
int32_t anim_runner_step    = 0;

#ifndef MAX
#define MAX(a,b) ((a>b)?(a):(b))
#endif

#ifndef MIN
#define MIN(a,b) ((a<b)?(a):(b))
#endif

// int _get_asset_idx_by_name(const char *name) {
//   for(int i = 0; appmodule_assets[i].name; i++) {
//     if (strcmp(name, appmodule_assets[i].name)) continue;
//     return i;
//   }
//   return -1;
// }

// Checks for alpha collisions between lv_image objects
// Works fully within draw coordinates
// return true = collision
bool game_check_collision(lv_obj_t *a, lv_obj_t *b) {
  int32_t a_x1 = lv_obj_get_x(a);
  int32_t a_x2 = lv_obj_get_x2(a) - 1;
  int32_t a_y1 = lv_obj_get_y(a);
  int32_t a_y2 = lv_obj_get_y2(a) - 1;


  int32_t b_x1 = lv_obj_get_x(b);
  int32_t b_x2 = lv_obj_get_x2(b) - 1;
  int32_t b_y1 = lv_obj_get_y(b);
  int32_t b_y2 = lv_obj_get_y2(b) - 1;

  if (
    (b_x2 >= a_x1) &&
    (b_x1 <= a_x2) &&
    (b_y2 >= a_y1) &&
    (b_y1 <= a_y2)
  ) {
    // Here = bounding boxes overlap

    // Fetch coordinates we need to check for collisions
    int32_t check_x1 = MAX(b_x1, a_x1);
    int32_t check_x2 = MIN(b_x2, a_x2);
    int32_t check_y1 = MAX(b_y1, a_y1);
    int32_t check_y2 = MIN(b_y2, a_y2);

    // Fetch the objects' spritesheets
    // We're using the alpha channel as collision masks
    lv_image_dsc_t *dsc_a = lv_image_get_src(a);
    lv_image_dsc_t *dsc_b = lv_image_get_src(b);

    // Fetch the offsets within the objects' spritesheets
    int32_t a_sourceX = -lv_image_get_offset_x(a);
    int32_t a_sourceY = -lv_image_get_offset_y(a);
    int32_t b_sourceX = -lv_image_get_offset_x(b);
    int32_t b_sourceY = -lv_image_get_offset_y(b);

    // And calculate the display to spritesheet offset only once
    int32_t a_offsetX = a_sourceX - a_x1;
    int32_t a_offsetY = a_sourceY - a_y1;
    int32_t b_offsetX = b_sourceX - b_x1;
    int32_t b_offsetY = b_sourceY - b_y1;

    // TODO: can we do this with gpu blending instead?
    for(int32_t y = check_y1; y <= check_y2; y++) {
      for(int32_t x = check_x1; x <= check_x2; x++) {
        int32_t a_checkX = x + a_offsetX;
        int32_t a_checkY = y + a_offsetY;
        int32_t b_checkX = x + b_offsetX;
        int32_t b_checkY = y + b_offsetY;

        int32_t a_check_idx = (((a_checkY * dsc_a->header.w) + a_checkX) * 4) + 3;
        int32_t b_check_idx = (((b_checkY * dsc_b->header.w) + b_checkX) * 4) + 3;

        // If both sprites have opacity at check location, we collide
        if (
          (dsc_a->data[a_check_idx] >= 128) &&
          (dsc_b->data[b_check_idx] >= 128)
        ) {
          // Overlapping alpha found
          return true;
        }

      }
    }
  }

  // Fallthrough = no collision found
  return false;
}

void appmodule_loop(uint32_t elapsedTime) {
  int i;

  if (cactus_drag_dragging) {
    lv_point_t pointCursor;
    lv_indev_get_point(lvMouse, &pointCursor);
    lv_obj_set_pos(cactus, pointCursor.x - cactus_drag_offset_x, pointCursor.y - cactus_drag_offset_y);
  }

  if (game_state == GAME_STATE_WAITING) {

    // Space pressed = go to intro animation
    if (KEYS[APP_KEYCODE_SPACE]) {
      log_trace("Space was pressed, starting");
      game_state = GAME_STATE_ANIM_INTRO;

      lv_image_set_offset_x(runner->el, -runner_walk_sourceX);
      lv_image_set_offset_y(runner->el, -runner_walk_sourceY);
    }

    // No game actions during wait

  } else if (game_state == GAME_STATE_ANIM_INTRO) {

    // Move the runner into position in 500ms
    if (runner->base.pos.x < (runner_idle_width/display_scaling/2)) {

      // Move the runner
      runner->base.speed.x_tick += elapsedTime * (runner_idle_width/display_scaling/2)*time_window/anim_start_move_duration;
      runner->base.pos.x        += runner->base.speed.x_tick / time_window;
      runner->base.speed.x_tick  = runner->base.speed.x_tick % time_window;
      // Move the image itself
      lv_obj_set_x(runner->el, (runner->base.pos.x * display_scaling) + (runner->base.speed.x_tick * display_scaling / time_window));
      lv_obj_set_y(runner->el, (runner->base.pos.y * display_scaling) + (runner->base.speed.y_tick * display_scaling / time_window));

      // Animate runner
      anim_runner_substep += anim_start_move_steps * elapsedTime;
      anim_runner_step    += anim_runner_substep / anim_start_move_duration;
      anim_runner_substep  = anim_runner_substep % anim_start_move_duration;
      anim_runner_step     = anim_runner_step % runner_walk_count;
      lv_image_set_offset_x(runner->el, -runner_walk_sourceX - (runner_walk_width*anim_runner_step));

    } else {

      // Ensure the runner is properly aligned
      runner->base.pos.x        = runner_idle_width/display_scaling/2;
      runner->base.speed.x_tick = 0;
      lv_obj_set_x(runner->el, (runner->base.pos.x * display_scaling));

      // Runner is in position, let's start
      game_state           = GAME_STATE_RUNNING;
      runner_speed_current = runner_speed_start;

      printf("Runner speed: %f\n", runner_speed_current);
    }


  } else if (game_state == GAME_STATE_RUNNING) {

    // Jump
    if (KEYS[APP_KEYCODE_SPACE] && (runner->base.pos.y == runner_groundY)) {
      runner->base.speed.y = -5500;
    }

    // Gravity
    if (KEYS[APP_KEYCODE_SPACE]) {
      runner->base.speed.y += 20 * elapsedTime;
    } else if (KEYS[APP_KEYCODE_DOWN]) {
      runner->base.speed.y += 50 * elapsedTime;
    } else {
      runner->base.speed.y += 36 * elapsedTime;
    }

    // Move runner Y according to velocity
    runner->base.speed.y_tick += elapsedTime * runner->base.speed.y;
    runner->base.pos.y        += runner->base.speed.y_tick / time_window;
    runner->base.speed.y_tick  = runner->base.speed.y_tick % time_window;

    // Hit the ground
    if (runner->base.pos.y >= runner_groundY && runner->base.speed.y > 0) {
      runner->base.speed.y_tick = 0;
      runner->base.speed.y      = 0;
      runner->base.pos.y        = runner_groundY;
    }

    // Move runner image
    lv_obj_set_x(runner->el, (runner->base.pos.x * display_scaling) + (runner->base.speed.x_tick * display_scaling / time_window));
    lv_obj_set_y(runner->el, (runner->base.pos.y * display_scaling) + (runner->base.speed.y_tick * display_scaling / time_window));

    // Animate runner
    anim_runner_substep += runner_speed_current * time_window / 1000 * elapsedTime;
    anim_runner_step    += anim_runner_substep / time_window;
    anim_runner_substep  = anim_runner_substep % time_window;
    anim_runner_step     = anim_runner_step % (runner_walk_count*runner_duck_count);

    // Normal walking sprite
    lv_image_set_offset_x(runner->el, -runner_walk_sourceX - (runner_walk_width*(anim_runner_step%runner_walk_count)));
    lv_image_set_offset_y(runner->el, -runner_walk_sourceY);
    lv_obj_set_width(runner->el, runner_walk_width);
    lv_obj_set_height(runner->el, runner_walk_height);

    // Allow the runner to duck
    if (KEYS[APP_KEYCODE_DOWN]) {
      lv_image_set_offset_x(runner->el, -runner_duck_sourceX - (runner_duck_width*(anim_runner_step%runner_duck_count)));
      lv_image_set_offset_y(runner->el, -runner_duck_sourceY);
      lv_obj_set_width(runner->el, runner_duck_width);
      lv_obj_set_height(runner->el, runner_duck_height);
      lv_obj_set_pos(
        runner->el,
        (runner->base.pos.x * display_scaling) + (runner->base.speed.x_tick * display_scaling / time_window) + ((runner_walk_width - runner_duck_width)/2),
        (runner->base.pos.y * display_scaling) + (runner->base.speed.y_tick * display_scaling / time_window) + (runner_walk_height - runner_duck_height)
      );
    }

    // Jumping = different sprite, offset sprite if needed
    if (runner->base.pos.y != runner_groundY) {
      lv_image_set_offset_x(runner->el, -runner_jump_sourceX);
      lv_image_set_offset_y(runner->el, -runner_jump_sourceY);
      lv_obj_set_width(runner->el, runner_jump_width);
      lv_obj_set_height(runner->el, runner_jump_height);
      lv_obj_set_pos(
        runner->el,
        (runner->base.pos.x * display_scaling) + (runner->base.speed.x_tick * display_scaling / time_window) + ((runner_walk_width - runner_jump_width)/2),
        (runner->base.pos.y * display_scaling) + (runner->base.speed.y_tick * display_scaling / time_window) + ((runner_walk_height - runner_jump_height)/2)
      );
    }

    // Update ground
    for(i=0; i < horizon_line_count ; i++) {
      struct game_obj_drawn *_horizon_line = horizon_lines[i];

      // Glide left (because <0)
      _horizon_line->base.speed.x_tick -= elapsedTime * runner_speed_current * time_window / 30;
      _horizon_line->base.pos.x        += _horizon_line->base.speed.x_tick / time_window;
      _horizon_line->base.speed.x_tick  = _horizon_line->base.speed.x_tick % time_window;

      // Out of bounds left = "respawn" with new random from ground sets
      if (_horizon_line->base.pos.x < (0 - horizon_line_width / display_scaling)) {
        _horizon_line->base.pos.x += (horizon_line_width/display_scaling) * horizon_line_count;
        lv_image_set_offset_x(_horizon_line->el, 0 - horizon_line_sourceX - (rand_between(0,horizon_line_sprite_count-1)*horizon_line_width));
      }

      // Move the image itself
      lv_obj_set_x(_horizon_line->el, (_horizon_line->base.pos.x * display_scaling) + (_horizon_line->base.speed.x_tick * display_scaling / time_window));
      lv_obj_set_y(_horizon_line->el, (_horizon_line->base.pos.y * display_scaling) + (_horizon_line->base.speed.y_tick * display_scaling / time_window));
    }

    // Update clouds
    for(i=0 ; i < cloud_count ; i++) {
      struct game_obj_drawn *_cloud = clouds[i];

      // Glide left (because speed.x<0)
      _cloud->base.speed.x_tick -= elapsedTime * runner_speed_current * time_window / 30 * cloud_speed;
      _cloud->base.pos.x        += _cloud->base.speed.x_tick / time_window;
      _cloud->base.speed.x_tick  = _cloud->base.speed.x_tick % time_window;

      // Out of bounds left = "respawn"
      if (_cloud->base.pos.x < (0 - cloud_width / display_scaling)) {
        _cloud->base.pos.x   = rand_between(display_width, display_width * 2);
        _cloud->base.pos.y   = rand_between(cloud_minY, cloud_maxY);
      }

      // Move the image itself
      lv_obj_set_x(_cloud->el, (_cloud->base.pos.x * display_scaling) + (_cloud->base.speed.x_tick * display_scaling / time_window));
      lv_obj_set_y(_cloud->el, (_cloud->base.pos.y * display_scaling) + (_cloud->base.speed.y_tick * display_scaling / time_window));
    }

    if (game_check_collision(runner->el, cactus)) {
      lv_obj_set_style_bg_color(runner->el, lv_color_hex(0xFF0000), 0);
      lv_obj_set_style_bg_opa(runner->el, LV_OPA_COVER, 0);
    } else {
      lv_obj_set_style_bg_opa(runner->el, LV_OPA_TRANSP, 0);
    }
  }






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
