#include <pebble.h>
#include "sports.h"
#include "hashtag.h"
#include "traffic.h"
#include "weather.h"

#define NUM_APPS 4


static Window *window;
static MenuLayer *menu_layer;
static InverterLayer *inv_layer;

static char* names[NUM_APPS];
static char* descriptions[NUM_APPS];
static int top;
static int bottom;
static bool onTop;


static AppSync sync;
static uint8_t sync_buffer[32];

enum MessageKey {
  WEATHER_KEY = 0x0,         // TUPLE_INT
  TEMPERATURE_KEY = 0x1,         // TUPLE_INT
};


//-----
//AppSync

static void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: %d", app_message_error);
}


static void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) {
  switch (key) {
    case WEATHER_KEY:
      APP_LOG(APP_LOG_LEVEL_INFO, "GOT WEATHER: %d", new_tuple->value->int8);
      break;
    case TEMPERATURE_KEY:
      APP_LOG(APP_LOG_LEVEL_INFO, "GOT TEMPERATURE: %d", new_tuple->value->int8);
      break;
    default:
      APP_LOG(APP_LOG_LEVEL_INFO, "Unknown Key Received: %d", key);
  }
}


//-----

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return 1;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return 2;
}

static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  // This is a define provided in pebble.h that you may use for the default height
  return 0;
}

static int16_t menu_get_cell_height_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  // This is a define provided in pebble.h that you may use for the default height
  return 75;
}

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  // Determine which section we're working with
  return;
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  // Determine which section we're going to draw in
   // Use the row to specify which item we'll draw
  switch (cell_index->row) {
    case 0:          // This is a basic menu item with a title and subtitle
      menu_cell_basic_draw(ctx, cell_layer, names[top], descriptions[top], NULL);
       break;

    case 1:
      // This is a basic menu icon with a cycling icon
      menu_cell_basic_draw(ctx, cell_layer, names[bottom], descriptions[bottom], NULL);          
      break;

  }
}

void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  // Use the row to specify which item will receive the select action
/*  switch (cell_index->row) {
    // This is the menu item with the cycling icon


    case 1:
      // Cycle the icon
      current_icon = (current_icon + 1) % NUM_MENU_ICONS;
      // After changing the icon, mark the layer to have it updated
      layer_mark_dirty(menu_layer_get_layer(menu_layer));
      break;
  }
*/
    switch (cell_index->row)
    {
      case 0:
	APP_LOG(APP_LOG_LEVEL_INFO, "Hashtag");
        vibes_short_pulse();
        hashtag_show();
        break;
      case 1:
	APP_LOG(APP_LOG_LEVEL_INFO, "Sports");
        sports_show();
        break;
      case 2:
	APP_LOG(APP_LOG_LEVEL_INFO, "Weather");
        weather_show();
        break;
    }
}



static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  window_set_fullscreen(window,true);
  GRect bounds = layer_get_bounds(window_layer);
  inv_layer = inverter_layer_create(GRect(0,0,144,168));

  top = 0;
  bottom = 1;
  onTop = true;

  names[0] = "Weather";
  names[1] = "Traffic";
  names[2] = "Twitter";
  names[3] = "Sports";

  descriptions[0] = "Wut dat weather be doe?";
  descriptions[1] = "Move out my way foo!";
  descriptions[2] = "#hashtag";
  descriptions[3] = "I go HARD in the paint";

  //-------
  //AppSync
  Tuplet initial_values[] = {
    TupletInteger(WEATHER_KEY, (int8_t) 1),
    TupletInteger(TEMPERATURE_KEY, (int8_t) 1)
  };
  app_sync_init(&sync, sync_buffer, sizeof(sync_buffer), initial_values, ARRAY_LENGTH(initial_values),
      sync_tuple_changed_callback, sync_error_callback, NULL);
  //-------

  menu_layer = menu_layer_create(bounds);

  menu_layer_set_callbacks(menu_layer,NULL, (MenuLayerCallbacks) {
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .get_cell_height = menu_get_cell_height_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_callback,
  });

   // menu_layer_set_click_config_onto_window(menu_layer, window);

  // Add it to the window for display
  layer_add_child(window_layer, menu_layer_get_layer(menu_layer));
  layer_add_child(window_layer, (Layer*) inv_layer);
  layer_set_hidden((Layer*)inv_layer,true);
}

static void window_unload(Window *window) {
  app_sync_deinit(&sync);
  menu_layer_destroy(menu_layer);
}


static void click_handler(ClickRecognizerRef recognizer, Window *window) {
  switch (click_recognizer_get_button_id(recognizer)) {
    case BUTTON_ID_UP:
      if (!onTop)
      {
        layer_set_hidden((Layer*)inv_layer,true);
        onTop = true;
      }
      else
        top = (top + 1) % NUM_APPS;
      layer_mark_dirty(menu_layer_get_layer(menu_layer));
      break;

    case BUTTON_ID_DOWN:
      if (onTop)
      {
        layer_set_hidden((Layer*)inv_layer,false);
        onTop = false;
      }
      else
        bottom = (bottom + 1) % NUM_APPS;
      layer_mark_dirty(menu_layer_get_layer(menu_layer));
      break;

    case BUTTON_ID_SELECT:
        if (onTop)
        {
          switch(top)
          {
            case 1:
              traffic_show();
              break;
            default:
              break;
          }
        }
      break;

    default:
      break;
  }
}

static void config_provider(Window *window) {
  window_single_click_subscribe(BUTTON_ID_UP, (ClickHandler) click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, (ClickHandler) click_handler);
}



/*/---------
//Messaging
void out_sent_handler(DictionaryIterator *sent, void *context) {
  // outgoing message was delivered
}


void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
  // outgoing message failed
}


void in_received_handler(DictionaryIterator *received, void *context) {
  // incoming message received
}


void in_dropped_handler(AppMessageResult reason, void *context) {
  // incoming message dropped
}
//---------*/

static void init(void) {
  hashtag_init();
  sports_init();
  weather_init();
  traffic_init();

  /*app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_register_outbox_sent(out_sent_handler);
  app_message_register_outbox_failed(out_failed_handler);*/

  const uint32_t inbound_size = 64;
  const uint32_t outbound_size = 64;
  app_message_open(inbound_size, outbound_size);


  window = window_create();
  window_set_click_config_provider(window, (ClickConfigProvider) config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
  inverter_layer_destroy(inv_layer);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);
  app_event_loop();
  deinit();
}
