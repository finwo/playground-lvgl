#ifndef __APP_MAIN_H__
#define __APP_MAIN_H__

#include "lvgl/lvgl.h"

extern lv_display_t *lvDisplay;
extern lv_indev_t *lvMouse;
extern lv_indev_t *lvMouseWheel;
extern lv_indev_t *lvKeyboard;

int appmodule_init();

#endif // __APP_MAIN_H__
