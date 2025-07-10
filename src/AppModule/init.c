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

  lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x2255AA), LV_PART_MAIN);

  lv_xml_component_register_from_data("my_button",
#include "AppModule/components/my_button.h"
  );

  /* Can be local */
  const char * my_button_attrs[] = {
      "btn_text", "calzone",
      NULL, NULL,
  };


  lv_obj_t * my_button = lv_xml_create(lv_screen_active(), "my_button", my_button_attrs);
  lv_obj_align(my_button, LV_ALIGN_CENTER, 0, 0);

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

  // // // printf("Result: %d\n", result);
  // printf("text : %d\n", my_button_prop.id);
  // printf("pizza: %d\n", lv_obj_property_get_id(my_button, "btn_text"));

  // /*Create widgets*/
  // lv_obj_t * label = lv_label_create(lv_screen_active());
  // lv_label_set_text(label, "Hello from LVGL!");
  // lv_obj_set_style_text_color(lv_screen_active(), lv_color_hex(0xffffff), LV_PART_MAIN);
  // lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

  return 0;
}
