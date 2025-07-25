#include "appmodule.h"
#include "lvgl/src/core/lv_obj_scroll.h"
#include "lvgl/src/display/lv_display.h"
#include "lvgl/src/libs/tiny_ttf/lv_tiny_ttf.h"
#include "lvgl/src/misc/lv_area.h"
#include "lvgl/src/misc/lv_style_gen.h"
#include "lvgl/src/misc/lv_text.h"
#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

#include <stdio.h>
#include <libgen.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
// #include <windows.h>
#else
// #include <pthread.h>
// #include <unistd.h>
#endif

#include "lvgl/lvgl.h"
#include "lvgl/src/core/lv_obj_pos.h"
#include "lvgl/src/draw/lv_image_dsc.h"
#include "lvgl/src/lv_api_map_v9_0.h"
#include "lvgl/src/misc/lv_types.h"
#include "lvgl/src/draw/lv_image_decoder.h"
#include "lvgl/src/draw/lv_image_decoder_private.h"
#include "lvgl/src/others/xml/lv_xml.h"
#include "lvgl/src/widgets/image/lv_image.h"

#include "rxi/log.h"
#include "kgabis/parson.h"
#include "tidwall/buf.h"

#include "AppModule/appmodule.h"

#include "util/get_bin_path.h"
#include "util/fs.h"
#include "util/rng.h"

// #include "util/incbin.h"

lv_obj_t *screen_main;
// lv_subject_t subj_horizon_offset;

int game_state;

double runner_speed_current;
double runner_speed_start;
double runner_speed_max;

struct game_obj_drawn **clouds;
const lv_draw_buf_t *buf_spritesheet;
JSON_Object *obj_spritesheet;

int time_window = 10000;

int cloud_count   = 0;
int cloud_desired = 0;
int cloud_minY    = 0;
int cloud_maxY    = 0;
int cloud_sourceX = 0;
int cloud_sourceY = 0;
int cloud_width   = 0;
int cloud_height  = 0;
double cloud_speed   = 0;

struct game_obj_drawn **horizon_lines;
int horizon_line_sprite_count;
int horizon_line_count;
int horizon_line_desired;
int horizon_line_sourceX;
int horizon_line_sourceY;
int horizon_line_width;
int horizon_line_height;
int horizon_line_speed;
int horizon_line_yPos = 0;

struct game_obj_drawn *runner;

int runner_idle_sourceX;
int runner_idle_sourceY;
int runner_idle_width;
int runner_idle_height;
int runner_idle_count;

int runner_walk_sourceX;
int runner_walk_sourceY;
int runner_walk_width;
int runner_walk_height;
int runner_walk_count;

int runner_duck_sourceX;
int runner_duck_sourceY;
int runner_duck_width;
int runner_duck_height;
int runner_duck_count;

lv_obj_t *label_hiscore;
lv_obj_t *label_score;

// struct game_obj_drawn *horizon_lines;
// int horizon_lines_count;

// INCBIN(component_my_button, "AppModule/components/my_button.xml");
// INCBIN(screen_main        , "AppModule/screens/main.xml"        );
// INCBIN(screen_about       , "AppModule/screens/about.xml"       );

// INCBIN(img_cloud         , "AppModule/assets/2x-cloud.png");
// INCBIN(img_horizon       , "AppModule/assets/2x-horizon.png");
// INCBIN(img_obstacle_large, "AppModule/assets/2x-obstacle-large.png");
// INCBIN(img_obstacle_small, "AppModule/assets/2x-obstacle-small.png");
// INCBIN(img_restart       , "AppModule/assets/2x-restart.png");
// INCBIN(img_text          , "AppModule/assets/2x-text.png");
// INCBIN(img_trex          , "AppModule/assets/2x-trex.png");

// struct appmodule_asset *appmodule_assets = (struct appmodule_asset[]){
//   { .name = "assets/cloud.png"         , .start = img_cloud_start         , .end = img_cloud_end         , .sprites = 1, .dump = false },
//   { .name = "assets/horizon.png"       , .start = img_horizon_start       , .end = img_horizon_end       , .sprites = 1, .dump = false },
//   { .name = "assets/obstacle_large.png", .start = img_obstacle_large_start, .end = img_obstacle_large_end, .sprites = 3, .dump = false },
//   { .name = "assets/obstacle_small.png", .start = img_obstacle_small_start, .end = img_obstacle_small_end, .sprites = 6, .dump = false },
//   { .name = "assets/restart.png"       , .start = img_restart_start       , .end = img_restart_end       , .sprites = 1, .dump = false },
//   { .name = "assets/text.png"          , .start = img_text_start          , .end = img_text_end          , .sprites = 1, .dump = false },
//   { .name = "assets/trex.png"          , .start = img_trex_start          , .end = img_trex_end          , .sprites = 6, .dump = false },
//   { .name = NULL, },
// };

// static void appmodule_switch_screen(lv_event_t * e) {
//   printf("switch_screen called\n");
// }

// static void appmodule_main_screen_events(lv_event_t * e) {
//   lv_event_code_t event_code = lv_event_get_code(e);
//   printf("Event! %d\n", event_code);
// }

int appmodule_setup(JSON_Object *obj_config_root) {
  const char *loglevel = "trace";
  char *appDir = dirname(get_bin_path());

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

  log_info("Logging driver initialized: %s\n", loglevel);

  // Disable scrolling, we're a game, not an app
  screen_main = lv_screen_active();
  lv_obj_set_scrollbar_mode(screen_main, LV_SCROLLBAR_MODE_OFF);
  lv_obj_set_size(screen_main, display_width, display_height);
  lv_obj_remove_flag(screen_main, LV_OBJ_FLAG_SCROLLABLE);

  // Basic validation
  if (!json_object_has_value_of_type(obj_config_root, "config", JSONObject)) {
    log_fatal("config missing in globals.json");
    exit(1);
  }
  if (!json_object_has_value_of_type(obj_config_root, "spritesheets", JSONObject)) {
    log_fatal("spritesheets missing in globals.json");
    exit(1);
  }

  // Fetch global configuration
  JSON_Object *obj__config = json_object_get_object(obj_config_root, "config");
  time_window = (int)json_object_get_number(obj__config, "timeWindow");
  time_window = time_window ? time_window : 10000;

  if (!json_object_has_value_of_type(obj__config, "speedStart", JSONNumber)) {
    log_fatal("'speedStart' configuration missing");
    exit(1);
  }
  if (!json_object_has_value_of_type(obj__config, "speedMax", JSONNumber)) {
    log_fatal("'speedMax' configuration missing");
    exit(1);
  }
  runner_speed_start = json_object_get_number(obj__config, "speedStart");
  runner_speed_max   = json_object_get_number(obj__config, "speedMax");

  // Check for font config existing
  if (!json_object_has_value_of_type(obj__config, "fontFile", JSONString)) {
    log_fatal("'fontFile' configuration missing");
    exit(1);
  }
  if (!json_object_has_value_of_type(obj__config, "fontSize", JSONNumber)) {
    log_fatal("'fontSize' configuration missing");
    exit(1);
  }

  // Check if we can load the font
  char *font_file;
  asprintf(&font_file, "%s/assets/%s", appDir, json_object_get_string(obj__config, "fontFile"));
  log_debug("Target font file: %s\n", font_file);
  if (!file_exists(font_file, "r")) {
    log_fatal("Font file could not be read");
    exit(1);
  }

  // Actually load the font
  int font_size = json_object_get_number(obj__config, "fontSize") * display_scaling;
  struct buf *font_data = file_get_contents(font_file);
  lv_font_t *customFont = lv_tiny_ttf_create_data(font_data->data, font_data->len, font_size);
  lv_style_t *fontStyle = calloc(1, sizeof(lv_style_t));
  lv_style_init(fontStyle);
  lv_style_set_text_font(fontStyle, customFont);
  lv_obj_add_style(lv_screen_active(), fontStyle, 0);

  // Build the scoring labels
  label_hiscore = lv_label_create(lv_screen_active());
  label_score = lv_label_create(lv_screen_active());
  lv_obj_set_align(label_hiscore, LV_ALIGN_TOP_RIGHT);
  lv_obj_set_align(label_score, LV_ALIGN_TOP_RIGHT);
  lv_obj_set_pos(label_hiscore, 0, font_size);
  lv_obj_set_pos(label_score, 0, 0);
  lv_label_set_text(label_score, "00000");
  lv_label_set_text(label_hiscore, "HI 00000");

  static lv_style_t hiscoreStyle;
  lv_style_init(&hiscoreStyle);
  lv_style_set_text_color(&hiscoreStyle, lv_color_hex(0x999999));
  lv_obj_add_style(label_hiscore, &hiscoreStyle, 0);

  // Get the spritesheet config
  char *target_spritesheet = NULL;
  asprintf(&target_spritesheet, "%dx", display_scaling);
  log_debug("Target spritesheet: %s\n", target_spritesheet);
  JSON_Object *obj_spritesheets = json_object_get_object(obj_config_root, "spritesheets");
  if (!json_object_has_value_of_type(obj_spritesheets, target_spritesheet, JSONObject)) {
    log_fatal("Target spritesheet missing in config");
    exit(1);
  }
  obj_spritesheet = json_object_get_object(obj_spritesheets, target_spritesheet);

  // Load the spritesheet texture
  if (!json_object_has_value_of_type(obj_spritesheet, "texture", JSONString)) {
    log_fatal("Spritesheet '%s' is missing 'texture' field", target_spritesheet);
    exit(1);
  }
  char *texturePath = NULL;
  asprintf(&texturePath, "%s/assets/%s", appDir, json_object_get_string(obj_spritesheet, "texture"));
  struct buf *encodedData = file_get_contents(texturePath);
  if (!encodedData) {
    log_fatal("Could not read spritesheet: %s\n", texturePath);
    exit(1);
  }
  lv_image_decoder_dsc_t dsc;
  lv_image_dsc_t loader_dsc = {
    .data      = (const uint8_t *)encodedData->data,
    .data_size = encodedData->len,
  };
  lv_image_decoder_open(&dsc, &loader_dsc, NULL);
  buf_spritesheet = dsc.decoded;
  lv_xml_register_image(NULL, "spritesheet", dsc.decoded);

  // Load sprite info
  if (json_object_has_value_of_type(obj_spritesheet, "spriteset", JSONObject)) {
    JSON_Object *obj_spriteset = json_object_get_object(obj_spritesheet, "spriteset");
    printf("Loading cloud spriteset\n");

    if (json_object_has_value_of_type(obj_spriteset, "cloud", JSONObject)) {
      printf("Loading cloud sprite\n");
      JSON_Object *obj_cloud = json_object_get_object(obj_spriteset, "cloud");
      cloud_sourceX = json_object_get_number(obj_cloud, "x");
      cloud_sourceY = json_object_get_number(obj_cloud, "y");
      cloud_width   = json_object_get_number(obj_cloud, "w");
      cloud_height  = json_object_get_number(obj_cloud, "h");
    }

    if (json_object_has_value_of_type(obj_spriteset, "horizon", JSONObject)) {
      printf("Loading horizon sprite\n");
      JSON_Object *obj_horizon = json_object_get_object(obj_spriteset, "horizon");
      horizon_line_sourceX      = json_object_get_number(obj_horizon, "x");
      horizon_line_sourceY      = json_object_get_number(obj_horizon, "y");
      horizon_line_width        = json_object_get_number(obj_horizon, "w");
      horizon_line_height       = json_object_get_number(obj_horizon, "h");
      horizon_line_sprite_count = json_object_get_number(obj_horizon, "c");
    }

    if (json_object_has_value_of_type(obj_spriteset, "runner_idle", JSONObject)) {
      printf("Loading runner idle sprite\n");
      JSON_Object *obj_runner_idle = json_object_get_object(obj_spriteset, "runner_idle");
      runner_idle_sourceX = json_object_get_number(obj_runner_idle, "x");
      runner_idle_sourceY = json_object_get_number(obj_runner_idle, "y");
      runner_idle_width   = json_object_get_number(obj_runner_idle, "w");
      runner_idle_height  = json_object_get_number(obj_runner_idle, "h");
      runner_idle_count   = json_object_get_number(obj_runner_idle, "c");
    }

    if (json_object_has_value_of_type(obj_spriteset, "runner_walk", JSONObject)) {
      printf("Loading runner walk sprite\n");
      JSON_Object *obj_runner_walk = json_object_get_object(obj_spriteset, "runner_walk");
      runner_walk_sourceX = json_object_get_number(obj_runner_walk, "x");
      runner_walk_sourceY = json_object_get_number(obj_runner_walk, "y");
      runner_walk_width   = json_object_get_number(obj_runner_walk, "w");
      runner_walk_height  = json_object_get_number(obj_runner_walk, "h");
      runner_walk_count   = json_object_get_number(obj_runner_walk, "c");
    }

    if (json_object_has_value_of_type(obj_spriteset, "runner_duck", JSONObject)) {
      printf("Loading runner_duck sprite\n");
      JSON_Object *obj_runner_duck = json_object_get_object(obj_spriteset, "runner_duck");
      runner_duck_sourceX = json_object_get_number(obj_runner_duck, "x");
      runner_duck_sourceY = json_object_get_number(obj_runner_duck, "y");
      runner_duck_width   = json_object_get_number(obj_runner_duck, "w");
      runner_duck_height  = json_object_get_number(obj_runner_duck, "h");
      runner_duck_count   = json_object_get_number(obj_runner_duck, "c");
    }

  }

  // Load background elements
  if (json_object_has_value_of_type(obj_config_root, "backgroundEl", JSONObject)) {
    JSON_Object *obj_backgroundElements = json_object_get_object(obj_config_root, "backgroundEl");
    if (json_object_has_value_of_type(obj_backgroundElements, "cloud", JSONObject)) {
      JSON_Object *obj_backgroundCloud = json_object_get_object(obj_backgroundElements, "cloud");

      cloud_count   = 0;
      cloud_desired = json_object_get_number(obj_backgroundCloud, "maxEls");
      cloud_minY    = json_object_get_number(obj_backgroundCloud, "minY");
      cloud_maxY    = json_object_get_number(obj_backgroundCloud, "maxY");
      cloud_speed   = json_object_get_number(obj_backgroundCloud, "speed");
    }
  }

  // Load ground information
  if (json_object_has_value_of_type(obj_config_root, "lines", JSONObject)) {
    JSON_Object *obj_lines = json_object_get_object(obj_config_root, "lines");
    if (json_object_has_value_of_type(obj_lines, "yPos", JSONNumber)) {
      horizon_line_yPos = (int)json_object_get_number(obj_lines, "yPos");
    }
  }

  // Create initial clouds
  clouds = calloc(cloud_desired, sizeof(void*));
  while (cloud_count < cloud_desired) {
    struct game_obj_drawn *_cloud = clouds[cloud_count] = (struct game_obj_drawn *)calloc(1, sizeof(struct game_obj_drawn));

    _cloud->base.pos.x   = rand_between(0, display_width);
    _cloud->base.pos.y   = rand_between(cloud_minY, cloud_maxY);
    _cloud->base.speed.x = cloud_speed;
    _cloud->el           = lv_image_create(lv_screen_active());

    lv_obj_t *img = _cloud->el;
    lv_image_set_src(img, buf_spritesheet);
    lv_image_set_inner_align(img, LV_IMAGE_ALIGN_TOP_LEFT);
    lv_image_set_offset_x(img, -cloud_sourceX);
    lv_image_set_offset_y(img, -cloud_sourceY);
    lv_obj_set_x(img, _cloud->base.pos.x * display_scaling);
    lv_obj_set_y(img, _cloud->base.pos.y * display_scaling);
    lv_obj_set_size(img, cloud_width, cloud_height);

    cloud_count++;
  }


  // Create ground lines
  horizon_line_desired = display_width / (horizon_line_width/display_scaling) + 2;
  horizon_lines = calloc(horizon_line_desired, sizeof(void*));
  while(horizon_line_count < horizon_line_desired) {
    struct game_obj_drawn *_horizon_line = horizon_lines[horizon_line_count] = (struct game_obj_drawn *)calloc(1, sizeof(struct game_obj_drawn));

    _horizon_line->base.pos.x   = horizon_line_count * (horizon_line_width / display_scaling);
    _horizon_line->base.pos.y   = horizon_line_yPos;
    _horizon_line->base.speed.x = -1000;
    _horizon_line->el           = lv_image_create(lv_screen_active());

    lv_obj_t *img = _horizon_line->el;
    lv_image_set_src(img, buf_spritesheet);
    lv_image_set_inner_align(img, LV_IMAGE_ALIGN_TOP_LEFT);
    lv_image_set_offset_x(img, -horizon_line_sourceX);
    lv_image_set_offset_y(img, -horizon_line_sourceY);
    lv_obj_set_x(img, _horizon_line->base.pos.x * display_scaling);
    lv_obj_set_y(img, _horizon_line->base.pos.y * display_scaling);
    lv_obj_set_size(img, horizon_line_width, horizon_line_height);

    horizon_line_count++;
    printf("Created ground line # %d\n", horizon_line_count);
  }

  // Create runner
  runner = calloc(1, sizeof(struct game_obj_drawn));

  // Why does this base.pos.y work to put trex's feet halfway the ground??
  runner->base.pos.x = 0;
  runner->base.pos.y = horizon_line_yPos + (horizon_line_height/display_scaling) - (runner_idle_height/display_scaling);
  runner->el         = lv_image_create(lv_screen_active());

  lv_obj_t *img = runner->el;
  lv_image_set_src(img, buf_spritesheet);
  lv_image_set_inner_align(img, LV_IMAGE_ALIGN_TOP_LEFT);
  lv_image_set_offset_x(img, -runner_idle_sourceX);
  lv_image_set_offset_y(img, -runner_idle_sourceY);
  lv_obj_set_x(img, runner->base.pos.x * display_scaling);
  lv_obj_set_y(img, runner->base.pos.y * display_scaling);
  lv_obj_set_size(img, runner_idle_width, runner_idle_height);

  // Set the game_state so the main loop knows what to do
  game_state = GAME_STATE_WAITING;

  // struct xml_document* globalXmlDocument = xml_parse_document(globalXmlContents->data, globalXmlContents->len);
  // struct xml_node* globalXmlRoot = xml_document_root(globalXmlDocument);

  // struct xml_string *root_tagname = xml_node_name(globalXmlRoot);

  // uint8_t *root_tagname_str = calloc(xml_string_length(root_tagname) + 1, sizeof(uint8_t));
  // xml_string_copy(root_tagname, root_tagname_str, xml_string_length(root_tagname));

  // printf("root tag: %s\n", root_tagname_str);

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

  // // Decompress png images into dsc structs
  // lv_image_dsc_t loader_dsc = {};
  // lv_image_decoder_dsc_t dsc;
  // for(int i = 0; appmodule_assets[i].name; i++) {
  //   loader_dsc.data      = appmodule_assets[i].start;
  //   loader_dsc.data_size = appmodule_assets[i].end - appmodule_assets[i].start;
  //   lv_image_decoder_open(&dsc, &loader_dsc, NULL);
  //   appmodule_assets[i].dsc = dsc.decoded;
  //   lv_xml_register_image(NULL, appmodule_assets[i].name, appmodule_assets[i].dsc);
  //   if (appmodule_assets[i].dump) {
  //     for(int j = 0; j < appmodule_assets[i].dsc->data_size; j++) {
  //       printf("%.2x ", appmodule_assets[i].dsc->data[j]);
  //       if (!(j%8)) printf("   ");
  //       if (!(j%16)) printf("\n");
  //     }
  //   }
  // }

  // // DEBUG: print sizes
  // for(int i = 0; assets[i].name; i++) {
  //   printf("[%*s] %*d / %d = %*d\n", 25, assets[i].name, 4, assets[i].dsc->header.w, assets[i].sprites, 4, assets[i].dsc->header.w / assets[i].sprites);
  // }

  // t-rex white = #f7f7f7
  // t-rex grey  = #535353

  // Build the ground



  // // lv_subject_init_int(&subj_horizon_offset, 0);
  // char *main_xml_path = NULL;
  // asprintf(&main_xml_path, "%s%s", appDir, "/screens/main.xml");
  // struct buf *main_xml = file_get_contents(main_xml_path);

  // // // Setup the display
  // // // lv_xml_register_event_cb(NULL, "my_callback_1", appmodule_switch_screen);
  // lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x2255AA), LV_PART_MAIN);
  // // // lv_xml_component_register_from_data("my_button"   , component_my_button_start);
  // // // lv_xml_component_register_from_data("screen_about", screen_about_start       );
  // lv_xml_component_register_from_data("screen_main" , main_xml->data);
  // screen_main = lv_xml_create(NULL, "screen_main", NULL);
  // // // lv_obj_add_event_cb(screen_main, appmodule_main_screen_events, LV_EVENT_CLICKED | LV_EVENT_PRESSED, NULL);
  // lv_scr_load(screen_main);

  // // Auto-detect background color from horizon (top-left pixel)
  // uint32_t global_bg_color = 0xff00ff;
  // for(int i = 0; appmodule_assets[i].name; i++) {
  //   if (strcmp("assets/horizon.png", appmodule_assets[i].name)) continue;
  //   global_bg_color =
  //     (appmodule_assets[i].dsc->data[0] << 16) +
  //     (appmodule_assets[i].dsc->data[1] <<  8) +
  //     (appmodule_assets[i].dsc->data[2] <<  0) ;
  //   break;
  // }
  // lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(global_bg_color), LV_PART_MAIN);

  // // Limit trex to 1 sprite
  // struct appmodule_asset asset_trex = appmodule_assets[6]; // TODO: use search here as well
  // lv_obj_t *img_trex = lv_obj_find_by_name(NULL, "trex");
  // lv_obj_set_width(img_trex, asset_trex.dsc->header.w / asset_trex.sprites);
  // // lv_image_set_inner_align(img_trex, LV_IMAGE_ALIGN_TOP_LEFT);
  //

  free(appDir);
  return 0;
}

#ifdef __cplusplus
} // extern "C"
#endif
