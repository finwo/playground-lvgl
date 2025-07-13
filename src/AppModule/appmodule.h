#ifndef __APPMODULE_H__
#define __APPMODULE_H__

#include "lvgl/lvgl.h"
#include "kgabis/parson.h"

// #include "lfs.h"

// extern lfs_t      lfs;
// extern lfs_file_t file;

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






struct game_position {
  int x;
  int y;
};

// Velocity is in pixels per timeWindow
struct game_velocity {
  int x;
  int y;
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
};

struct game_obj_drawn {
  struct game_obj_base base;
  struct trait_sprite sprite;
};


struct game_obj_drawn *clouds;
int cloud_count;

struct game_obj_drawn *horizon_lines;
int horizon_lines_count;






#endif // __APPMODULE_H__
