#include <pebble.h>
#include "card_template.h"

int score;

static struct TemplateUi {
  Window *window;
  TextLayer *score_text;
} ui;


static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  static char buf[32];

  APP_LOG(APP_LOG_LEVEL_INFO, "Select click handler running...");

  score++;

  snprintf(buf, 32, "Score: %u", score);
  text_layer_set_text(ui.score_text, buf);
  APP_LOG(APP_LOG_LEVEL_INFO, "Select click handler complete, score: %d", score);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(ui.window);
  GRect bounds = layer_get_bounds(window_layer);

  ui.score_text = text_layer_create((GRect) {
        .origin = { 0, 72 },
        .size = { bounds.size.w, 20 }
      });
  text_layer_set_text_alignment(ui.score_text, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(ui.score_text));

  APP_LOG(APP_LOG_LEVEL_INFO, "Template window loaded");

  /*  ui.time_text = text_layer_create((GRect) {
        .origin = { 0, 0 },
        .size = { bounds.size.w, 64 }
      });
  text_layer_set_text_alignment(ui.time_text, GTextAlignmentCenter);
  text_layer_set_font(ui.time_text,
                      fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
		      layer_add_child(window_layer, text_layer_get_layer(ui.time_text));*/
}

static void window_appear(Window *window) {
  // When the game window appears, reset the game
  text_layer_set_text(ui.score_text, "Score: 0");
}

static void window_unload(Window *window) {
  /*  text_layer_destroy(ui.time_text);*/
  text_layer_destroy(ui.score_text);
}

void template_show(void) {
  const bool animated = true;
  window_stack_push(ui.window, animated);
  APP_LOG(APP_LOG_LEVEL_INFO, "Showing template window");
}

void template_init(void) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Initializing template card...");
  ui.window = window_create();

  window_set_click_config_provider(ui.window, click_config_provider);
  window_set_window_handlers(ui.window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
    .appear = window_appear
  });
  const bool animated = true;

  score = 0;

  APP_LOG(APP_LOG_LEVEL_INFO, "Template card initialized");
}

void template_deinit(void) {
  window_destroy(ui.window);
}
