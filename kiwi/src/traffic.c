#include <pebble.h>
#include "traffic.h"

static Window* window;
static PropertyAnimation *prop_animation;
static GRect bounds;
static Layer *cardLayer;
static char* trafficText;
static int height = 84;

static void cardLayer_update_callback(Layer* me, GContext* ctx) {
  graphics_context_set_text_color(ctx, GColorBlack);
  graphics_draw_text(ctx,trafficText,fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21),
      GRect(0, height-30, bounds.size.w, 100),
      GTextOverflowModeWordWrap,
      GTextAlignmentCenter,
      NULL);
}

static void window_load(Window *window) {
  trafficText = "Accident on the I110";
  Layer *window_layer = window_get_root_layer(window);
  bounds = layer_get_bounds(window_layer);

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
  Layer *layer = cardLayer;

  GRect to_rect;




  destroy_property_animation(&prop_animation);

  prop_animation = property_animation_create_layer_frame(layer, NULL, &to_rect);
  animation_set_duration((Animation*) prop_animation, 400);
  switch (click_recognizer_get_button_id(recognizer)) {
    case BUTTON_ID_UP:

      break;

    case BUTTON_ID_DOWN:
      break;

    case BUTTON_ID_SELECT:
      //animation_set_curve((Animation*) prop_animation, AnimationCurveEaseInOut);
      break;
 default:
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
}
static void config_provider(Window *window) {
  window_single_click_subscribe(BUTTON_ID_UP, (ClickHandler) click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, (ClickHandler) click_handler);
}



static void window_unload(Window *window) {

}

void traffic_init(void) {
  window = window_create();
  Layer* window_layer = window_get_root_layer(window);
  bounds = layer_get_bounds(window_layer);
  cardLayer = layer_create(bounds);
  layer_set_update_proc(cardLayer, cardLayer_update_callback);
  layer_add_child(window_layer,cardLayer);

  window_set_click_config_provider(window, (ClickConfigProvider) config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
}

void traffic_deinit(void) {
    window_destroy(window);
      layer_destroy(cardLayer);
  destroy_property_animation(&prop_animation);

  window_stack_remove(window, false);
}

void traffic_show(void) {
	window_stack_push(window, false);
}

void set_traffic_text(char* trafficInput) {
  trafficText = trafficInput;
}

char* get_traffic_text() {
  if (trafficText != NULL)
    return trafficText;
  else
    return "How long will it take to get home?";
}

