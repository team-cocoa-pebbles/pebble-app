#include <pebble.h>
#include "weather.h"

static TextLayer* weather_text;
static Window* window;
static PropertyAnimation *prop_animation;
static GRect bounds;

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  bounds = layer_get_bounds(window_layer);

  weather_text = text_layer_create((GRect) {
        .origin = { 0, bounds.size.h/2},
        .size = { bounds.size.w, 20 }
      });
  text_layer_set_text(weather_text, "#weather");
  text_layer_set_text_alignment(weather_text, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(weather_text));


  layer_add_child(window_layer, text_layer_get_layer(weather_text));
}

static void destroy_property_animation(PropertyAnimation **prop_animation) {
  if (*prop_animation == NULL) {
    return;
  }

  if (animation_is_scheduled((Animation*) *prop_animation)) {
    animation_unschedule((Animation*) *prop_animation);
  }

  property_animation_destroy(*prop_animation);
  *prop_animation = NULL;
}


static void click_handler(ClickRecognizerRef recognizer, Window *window) {
  Layer *layer = text_layer_get_layer(weather_text);

  GRect to_rect;




  destroy_property_animation(&prop_animation);

  prop_animation = property_animation_create_layer_frame(layer, NULL, &to_rect);
  animation_set_duration((Animation*) prop_animation, 400);
  switch (click_recognizer_get_button_id(recognizer)) {
    case BUTTON_ID_UP:
    to_rect = GRect(0, bounds.size.h/2, bounds.size.w, 60);
    prop_animation = property_animation_create_layer_frame(layer, NULL, &to_rect);
      animation_set_curve((Animation*) prop_animation, AnimationCurveEaseOut);
      break;

    case BUTTON_ID_DOWN:
    to_rect = GRect(0, bounds.size.h, bounds.size.w, 60);
    prop_animation = property_animation_create_layer_frame(layer, NULL, &to_rect);
      animation_set_curve((Animation*) prop_animation, AnimationCurveEaseIn);
      break;

    default:
    case BUTTON_ID_SELECT:
      //animation_set_curve((Animation*) prop_animation, AnimationCurveEaseInOut);
      break;
  }

  /*
   // Exmple animation parameters:

   // Duration defaults to 250 ms
   animation_set_duration(&prop_animation->animation, 1000);

   // Curve defaults to ease-in-out
   animation_set_curve(&prop_animation->animation, AnimationCurveEaseOut);

   // Delay defaults to 0 ms
   animation_set_delay(&prop_animation->animation, 1000);
   */

  /*animation_set_handlers((Animation*) prop_animation, (AnimationHandlers) {
    .started = (AnimationStartedHandler) animation_started,
    .stopped = (AnimationStoppedHandler) animation_stopped,
  }, NULL  callback data /);*/
  animation_schedule((Animation*) prop_animation);
}
static void config_provider(Window *window) {
  window_single_click_subscribe(BUTTON_ID_UP, (ClickHandler) click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, (ClickHandler) click_handler);
}



static void window_unload(Window *window) {
  text_layer_destroy(weather_text);
}

void weather_init(void) {
  window = window_create();

  window_set_click_config_provider(window, (ClickConfigProvider) config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

void weather_deinit(void) {
  window_destroy(window);
}

void weather_show(void) {
  window_stack_push(window, true);
}
