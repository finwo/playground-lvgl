#include "lvgl/src/misc/lv_event.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl/src/core/lv_obj_property.h"
#include "lvgl/src/misc/lv_area.h"
#include "lvgl/src/misc/lv_types.h"
#include "lvgl/src/widgets/label/lv_label.h"

#include <stdio.h>

#ifdef _WIN32
// #include <windows.h>
#else
// #include <pthread.h>
// #include <unistd.h>
#endif


#include "lvgl/lvgl.h"
#include "rxi/log.h"

#include "AppModule/init.h"
#include "util/incbin.h"

INCBIN(component_my_button, "AppModule/components/my_button.xml");
INCBIN(screen_main        , "AppModule/screens/main.xml"        );
INCBIN(screen_about       , "AppModule/screens/about.xml"       );

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

  lv_xml_register_event_cb(NULL, "my_callback_1", appmodule_switch_screen);

  // lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x2255AA), LV_PART_MAIN);
  lv_xml_component_register_from_data("my_button"   , component_my_button_start);
  lv_xml_component_register_from_data("screen_about", screen_about_start       );
  lv_xml_component_register_from_data("screen_main" , screen_main_start        );

  // /* Can be local */
  // const char * my_button_attrs[] = {
  //     "btn_text", "calzone",
  //     NULL, NULL,
  // };

  lv_obj_t * screen_main  = lv_xml_create(NULL, "screen_main", NULL);
  lv_obj_add_event_cb(screen_main, appmodule_main_screen_events, LV_EVENT_CLICKED | LV_EVENT_PRESSED, NULL);
  lv_scr_load(screen_main);

  // lv_obj_t * my_button = lv_xml_create(lv_screen_active(), "my_button", my_button_attrs);
  // lv_obj_align(my_button, LV_ALIGN_CENTER, 0, 0);

  // // lv_prop_id_t  btnTextId      = lv_obj_property_get_id(my_button, "btn_text");
  // lv_property_t my_button_prop = {
  //   .id = lv_obj_property_get_id(my_button, "btn_text"),
  // };

  // lv_obj_t * lv_label_0 = lv_label_create(my_button);
  // lv_label_set_text(lv_label_0, "calzone");
  // lv_obj_set_align(lv_label_0, LV_ALIGN_TOP_LEFT);

  // my_button_prop.ptr = "Click me!";
  // my_button_prop.id  = btnTextId;
  // my_button_prop

  // lv_result_t result = lv_obj_set_property(my_button, &my_button_prop);

  // /*Create widgets*/
  // lv_obj_t * label = lv_label_create(lv_screen_active());
  // lv_label_set_text(label, "Hello from LVGL!");
  // lv_obj_set_style_text_color(lv_screen_active(), lv_color_hex(0xffffff), LV_PART_MAIN);
  // lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

  return 0;
}

#ifdef __cplusplus
} // extern "C"
#endif
