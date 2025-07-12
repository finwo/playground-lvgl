#ifndef __APPMODULE_H__
#define __APPMODULE_H__

#include "lvgl/lvgl.h"
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

int appmodule_setup();
void appmodule_loop(uint32_t elapsedTime);

#endif // __APPMODULE_H__
