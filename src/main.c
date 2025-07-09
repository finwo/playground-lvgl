#include "webview/api.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif

#include "kgabis/parson.h"
#include "lvgl/lvgl.h"
#include "rxi/log.h"
#include "webview/webview.h"
#include "tinycthread/tinycthread.h"

// Only used to suppress warnings caused by unused parameters.
#define UNUSED(x) (void)x

// typedef struct {
//   void *arg;
//   void (*next_fn)(void *);
//   void (*free_fn)(void *);
// } start_routine_wrapper_arg_t;

// #ifdef _WIN32
// DWORD __stdcall start_routine_wrapper(void *wrapper_arg) {
//   start_routine_wrapper_arg_t *arg = (start_routine_wrapper_arg_t *)wrapper_arg;
//   arg->next_fn(arg->arg);
//   arg->free_fn(wrapper_arg);
//   return 0;
// }
// #else
// void *start_routine_wrapper(void *wrapper_arg) {
//   start_routine_wrapper_arg_t *arg = (start_routine_wrapper_arg_t *)wrapper_arg;
//   arg->next_fn(arg->arg);
//   arg->free_fn(wrapper_arg);
//   return NULL;
// }
// #endif

// // Creates a thread with the given start routine and argument passed to
// // the start routine. Returns 0 on success and -1 on failure.
// int thread_create(void (*start_routine)(void *), void *arg) {
//   start_routine_wrapper_arg_t *wrapper_arg =
//       (start_routine_wrapper_arg_t *)malloc(
//           sizeof(start_routine_wrapper_arg_t));
//   wrapper_arg->arg = arg;
//   wrapper_arg->next_fn = start_routine;
//   wrapper_arg->free_fn = free;
// #ifdef _WIN32
//   HANDLE thread =
//       CreateThread(NULL, 0, start_routine_wrapper, wrapper_arg, 0, NULL);
//   if (thread) {
//     CloseHandle(thread);
//     return 0;
//   }
//   return -1;
// #else
//   pthread_t thread;
//   int error = pthread_create(&thread, NULL, start_routine_wrapper, wrapper_arg);
//   if (error == 0) {
//     pthread_detach(thread);
//     return 0;
//   }
//   return -1;
// #endif
// }

// // Make the current thread sleep for the given number of seconds.
// void thread_sleep(int seconds) {
// #ifdef _WIN32
//   Sleep(seconds * 1000);
// #else
//   sleep(seconds);
// #endif
// }

webview_t    *mainwindow  = NULL;
lv_display_t *maindisplay = NULL;
bool isWebviewReady = false;
bool stopLvgl       = false;

uint8_t *lvgl_buf = NULL;
mtx_t lvgl_buf_mtx;

int32_t currentWidth  = 960;
int32_t currentHeight = 640;


// typedef struct {
//   webview_t w;
// } context_t;

// static const char html[] = "\
// <div>\n\
//   <button id=\"increment\">+</button>\n\
//   <button id=\"decrement\">−</button>\n\
//   <span>Counter: <span id=\"counterResult\">0</span></span>\n\
// </div>\n\
// <hr />\n\
// <div>\n\
//   <button id=\"compute\">Compute</button>\n\
//   <span>Result: <span id=\"computeResult\">(not started)</span></span>\n\
// </div>\n\
// <script type=\"module\">\n\
//   const getElements = ids => Object.assign({}, ...ids.map(\n\
//     id => ({ [id]: document.getElementById(id) })));\n\
//   const ui = getElements([\n\
//     \"increment\", \"decrement\", \"counterResult\", \"compute\",\n\
//     \"computeResult\"\n\
//   ]);\n\
//   ui.increment.addEventListener(\"click\", async () => {\n\
//     ui.counterResult.textContent = await window.count(1);\n\
//   });\n\
//   ui.decrement.addEventListener(\"click\", async () => {\n\
//     ui.counterResult.textContent = await window.count(-1);\n\
//   });\n\
//   ui.compute.addEventListener(\"click\", async () => {\n\
//     ui.compute.disabled = true;\n\
//     ui.computeResult.textContent = \"(pending)\";\n\
//     ui.computeResult.textContent = await window.compute(6, 7);\n\
//     ui.compute.disabled = false;\n\
//   });\n\
// </script>";

// void count(const char *id, const char *req, void *arg) {
//   context_t *context = (context_t *)arg;
//   // Imagine that params->req is properly parsed or use your own JSON parser.
//   long direction = strtol(req + 1, NULL, 10);
//   char result[10] = {0};
//   (void)sprintf(result, "%ld", context->count += direction);
//   webview_return(context->w, id, 0, result);
// }

// typedef struct {
//   webview_t w;
//   char *id;
//   char *req;
// } compute_thread_params_t;

// compute_thread_params_t *
// compute_thread_params_create(webview_t w, const char *id, const char *req) {
//   compute_thread_params_t *params =
//       (compute_thread_params_t *)malloc(sizeof(compute_thread_params_t));
//   params->w = w;
//   params->id = (char *)malloc(strlen(id) + 1);
//   params->req = (char *)malloc(strlen(req) + 1);
//   strcpy(params->id, id);
//   strcpy(params->req, req);
//   return params;
// }

// void compute_thread_params_free(compute_thread_params_t *p) {
//   free(p->req);
//   free(p->id);
//   free(p);
// }

// void compute_thread_proc(void *arg) {
//   compute_thread_params_t *params = (compute_thread_params_t *)arg;
//   // Simulate load.
//   thread_sleep(1);
//   // Imagine that params->req is properly parsed or use your own JSON parser.
//   const char *result = "42";
//   webview_return(params->w, params->id, 0, result);
//   compute_thread_params_free(params);
// }

void webviewReady(const char *id, const char *req, void *arg) {
  isWebviewReady = true;
  webview_return(*mainwindow, id, 0, "");
}

void bound_updateResolution(const char *id, const char *req, void *arg) {
  mtx_lock(&lvgl_buf_mtx);
  JSON_Value *jreq  = json_parse_string(req);
  JSON_Array *jAreq = json_value_get_array(jreq);
  currentWidth  = (uint32_t)json_array_get_number(jAreq, 0);
  currentHeight = (uint32_t)json_array_get_number(jAreq, 1);
  // printf("Update resolution req: %s\n", req);
  // printf("Updated resolution: %dx%d\n", currentWidth, currentHeight);
  lv_display_set_resolution(maindisplay, currentWidth, currentHeight);
  lv_display_set_physical_resolution(maindisplay, currentWidth, currentHeight);

  free(lvgl_buf);
  lvgl_buf = malloc(currentWidth*3*100);
  lv_display_set_buffers(maindisplay, lvgl_buf, NULL, currentWidth*3*100, LV_DISPLAY_RENDER_MODE_PARTIAL);

  mtx_unlock(&lvgl_buf_mtx);

  webview_return(*mainwindow, id, 0, "");
}

int64_t starttime = 0;
uint32_t millis() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  int64_t now = (tv.tv_sec * ((int64_t)1000)) + (tv.tv_usec / 1000);
  if (!starttime) starttime = now;
  return (uint32_t)(now - starttime);
}

/*Copy the rendered image to the screen.
 *It needs to be implemented by the user*/
void my_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_buf) {
  // printf("Flushing %dx%d -- %dx%d -- %dx%d\n", currentWidth, currentHeight, area->x1,area->y1, area->x2, area->y2);
    /*Show the rendered image on the display*/
  char *myJs;

  int jsLength = asprintf(&myJs, "updateDisplay(%d,%d,%d,%d,[", area->x1, area->y1, area->x2, area->y2);
  int bufsize  = (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1) * 3;

  myJs = realloc(myJs, jsLength + (bufsize*4) + 4);
  for(int i = 0; i < bufsize; i++) {
    jsLength += sprintf(myJs+jsLength, "%u,", px_buf[i]);
  }
  jsLength += sprintf(myJs+jsLength, "]);");

  webview_eval(*mainwindow, myJs);
  free(myJs);

  // my_display_update(area, px_buf);
  /*Indicate that the buffer is available.
    *If DMA were used, call in the DMA complete interrupt*/
  lv_display_flush_ready(disp);
}

int thread_lvgl(void *arg) {
  // context_t *context = arg;

  while(!isWebviewReady) usleep(10000);
  webview_eval(*mainwindow, "updateBackground('#2255AA');");

  // Wait for the mainwindow to be ready
  usleep(1000000);

  /*Make LVGL periodically execute its tasks*/
  while(1) {
    if (stopLvgl) break;
    mtx_lock(&lvgl_buf_mtx);
    lv_timer_handler();
    mtx_unlock(&lvgl_buf_mtx);
    usleep(5000);
    // my_sleep(5);  /*Wait 5 milliseconds before processing LVGL timer again*/
  }

  return 0;
}

#ifndef WINTERM
#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) {
  UNUSED(hInst);
  UNUSED(hPrevInst);
  UNUSED(lpCmdLine);
  UNUSED(nCmdShow);
#else
int main() {
#endif
#else // no WINTERM
int main() {
#endif
  const char *loglevel = "trace";
  int i = 0;

  mtx_init(&lvgl_buf_mtx, 0);

//   // TODO: add argparse here if needed

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

  log_debug("Creating webview window");
  webview_t w = webview_create(1, NULL);
  mainwindow = &w;

  webview_set_title(w, "LVGL Playground");
  webview_set_size(w, currentWidth, currentHeight, WEBVIEW_HINT_NONE);

  lv_init();
  lv_tick_set_cb(millis);

  lv_display_t * display = lv_display_create(currentWidth, currentHeight);
  maindisplay = display;

  /*LVGL will render to this 1/10 screen sized buffer for 3 bytes/pixel*/
  lvgl_buf = malloc(currentWidth*3*100);
  lv_display_set_buffers(display, lvgl_buf, NULL, currentWidth*3*100, LV_DISPLAY_RENDER_MODE_PARTIAL);

  /*This callback will display the rendered image*/
  lv_display_set_flush_cb(display, my_flush_cb);

  lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xAA5522), LV_PART_MAIN);

  /*Create widgets*/
  lv_obj_t * label = lv_label_create(lv_screen_active());
  lv_label_set_text(label, "Hello from LVGL!");
  lv_obj_set_style_text_color(lv_screen_active(), lv_color_hex(0xffffff), LV_PART_MAIN);
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);



  // int i;
  // context_t context = {
    // .port          = 38475,
    // .settings_file = calloc(snprintf(NULL, 0, settings_file_template, homedir()) + 1, 1),
  // };
  thrd_t threads[2];

  // thrd_create(&threads[0], thread_fnet  , NULL    );
  thrd_create(&threads[1], thread_lvgl, NULL);

//   // A binding that counts up or down and immediately returns the new value.
//   webview_bind(w, "count", count, &context);

//   // A binding that creates a new thread and returns the result at a later time.
//   webview_bind(w, "compute", compute, &context);

  // A binding that counts up or down and immediately returns the new value.
  webview_bind(w, "_ready"           , webviewReady, &w);
  webview_bind(w, "_updateResolution", bound_updateResolution, &w);

  webview_set_html(w,
    #include "view.h"
  );

  // Blocks
  webview_run(w);

  // Cleanup
  webview_destroy(w);
  stopLvgl = true;

  // Wait for other threads to die out
  // TODO: send kill signal?
  for(i = 1; i < 2 ; i++) {
    // printf("Joining thread %d\n", i);
    thrd_join(threads[i], NULL);
  }

  // --------------------------------------------------------

  // const char *settings_file_template =
  //   "%s"
  //   DIRECTORY_SEPARATOR
  //   ".config"
  //   DIRECTORY_SEPARATOR
  //   "finwo"
  //   DIRECTORY_SEPARATOR
  //   "stream-companion.json"
  //   ;

  // int i;
  // context_t context = {
  //   .port          = 38475,
  //   .settings_file = calloc(snprintf(NULL, 0, settings_file_template, homedir()) + 1, 1),
  // };
  // thrd_t threads[2];

  // printf("main:context %p\n", &context);

  // sprintf(context.settings_file, settings_file_template, homedir());
  // if (!file_exists(context.settings_file, "rw")) {
  //   file_put_contents(context.settings_file, &(struct buf){
  //     .cap  = 3,
  //     .data = "{}\n",
  //     .len  = 3
  //   }, 1);
  // }

  // /* thrd_create(&threads[0], thread_fnet  , NULL    ); */
  // thrd_create(&threads[1], thread_http  , &context);

  // // Launch the window on the main thread
  // thread_window(&context);

  // for(i = 1; i < 2 ; i++) {
  //   printf("Joining thread %d\n", i);
  //   thrd_join(threads[i], NULL);
  // }

  // printf("Main fn finished\n");
  // return 0;

  return 0;
}

#ifdef WINTERM
#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) {
  UNUSED(hInst);
  UNUSED(hPrevInst);
  UNUSED(lpCmdLine);
  UNUSED(nCmdShow);
  return main();
}
#endif
#endif
