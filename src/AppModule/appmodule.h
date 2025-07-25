#ifndef __APPMODULE_H__
#define __APPMODULE_H__

#include "lvgl/lvgl.h"
#include "kgabis/parson.h"
#include "lvgl/src/misc/lv_types.h"

// #include "lfs.h"

// extern lfs_t      lfs;
// extern lfs_file_t file;

extern const bool *KEYS;

#define APP_KEYCODE_ESCAPE 41
#define APP_KEYCODE_SPACE  44
#define APP_KEYCODE_DOWN   81
#define APP_KEYCODE_UP     82

extern lv_display_t *lvDisplay;
extern lv_indev_t *lvMouse;
extern lv_indev_t *lvMouseWheel;
extern lv_indev_t *lvKeyboard;

extern lv_obj_t *screen_main;

struct appmodule_asset {
  char *name;
  char *start;
  char *end;
  int   sprites;
  bool dump;
  lv_image_dsc_t *dsc;
};

extern struct appmodule_asset *appmodule_assets;

// extern lv_subject_t subj_horizon_offset;

int appmodule_setup(JSON_Object *obj_config_root);
void appmodule_loop(uint32_t elapsedTime);

extern int time_window;
extern int display_scaling;
extern int display_width;
extern int display_height;

extern JSON_Object *obj_spritesheet;
extern const lv_draw_buf_t *buf_spritesheet;

#define GAME_STATE_WAITING    0
#define GAME_STATE_ANIM_INTRO 1
#define GAME_STATE_RUNNING    2
#define GAME_STATE_DEAD       3

extern int game_state;

struct game_position {
  int x;
  int y;
};

// Velocity is in pixels per timeWindow
struct game_velocity {
  int x;
  int y;
  int x_tick;
  int y_tick;
};

struct game_obj_base {
  struct game_position pos;
  struct game_velocity speed;
};

struct trait_sprite {
  char *texture;
  int source_x;
  int source_y;
  int width;
  int height;
  int index;
};

struct game_obj_drawn {
  struct game_obj_base base;
  struct trait_sprite sprite;
  lv_obj_t *el;
};


extern struct game_obj_drawn **clouds;

extern int cloud_count;
extern int cloud_desired;
extern int cloud_minY;
extern int cloud_maxY;
extern int cloud_sourceX;
extern int cloud_sourceY;
extern int cloud_width;
extern int cloud_height;
extern int cloud_speed;


extern struct game_obj_drawn **horizon_lines;
extern int horizon_line_sprite_count;
extern int horizon_line_count;
extern int horizon_line_desired;
extern int horizon_line_yPos;
extern int horizon_line_sourceX;
extern int horizon_line_sourceY;
extern int horizon_line_width;
extern int horizon_line_height;
extern int horizon_line_speed;

extern double runner_speed_current;
extern double runner_speed_start;
extern double runner_speed_max;

extern struct game_obj_drawn *runner;

extern int    runner_idle_sourceX;
extern int    runner_idle_sourceY;
extern int    runner_idle_width;
extern int    runner_idle_height;
extern int    runner_idle_count;

extern int    runner_walk_sourceX;
extern int    runner_walk_sourceY;
extern int    runner_walk_width;
extern int    runner_walk_height;
extern int    runner_walk_count;

extern int    runner_duck_sourceX;
extern int    runner_duck_sourceY;
extern int    runner_duck_width;
extern int    runner_duck_height;
extern int    runner_duck_count;

extern lv_obj_t *label_hiscore;
extern lv_obj_t *label_score;

#endif // __APPMODULE_H__
