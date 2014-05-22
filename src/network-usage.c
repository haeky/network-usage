#include <pebble.h>

static Window *window;

static TextLayer *title_layer;
static TextLayer *subtitle_layer;
static TextLayer *data_layer;
static TextLayer *percentage_layer;
static TextLayer *days_layer;

static AppSync sync;
static uint8_t sync_buffer[128];

static void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: %d", app_message_error);
}

static void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) {
  switch (key) {
    case 0x0:
      text_layer_set_text(percentage_layer, new_tuple->value->cstring);
      break;
    case 0x1:
      text_layer_set_text(subtitle_layer, new_tuple->value->cstring);
      break;
    case 0x2:
      text_layer_set_text(data_layer, new_tuple->value->cstring);
      break;
    case 0x3:
      text_layer_set_text(days_layer, new_tuple->value->cstring);
      break;
  }
}

static void send_cmd(void) {
  Tuplet value = TupletInteger(1, 1);

  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  if (iter == NULL) {
    return;
  }

  dict_write_tuplet(iter, &value);
  dict_write_end(iter);

  app_message_outbox_send();
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  title_layer = text_layer_create(GRect(0, 0, 145, 70));
  text_layer_set_font(title_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text(title_layer, "Network!");
  layer_add_child(window_layer, text_layer_get_layer(title_layer));

  subtitle_layer = text_layer_create(GRect(0, 30, 145, 70));
  text_layer_set_font(subtitle_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  layer_add_child(window_layer, text_layer_get_layer(subtitle_layer));

  data_layer = text_layer_create(GRect(0, 75, 145, 70));
  text_layer_set_font(data_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(data_layer));

  percentage_layer = text_layer_create(GRect(0, 100, 145, 70));
  text_layer_set_font(percentage_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(percentage_layer));

  days_layer = text_layer_create(GRect(0, 125, 145, 70));
  text_layer_set_font(days_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(days_layer));

  Tuplet initial_values[] = {
    TupletCString(0x0, ""),
    TupletCString(0x1, ""),
    TupletCString(0x2, ""),
    TupletCString(0x3, "")
  };

  app_sync_init(&sync, sync_buffer, sizeof(sync_buffer), initial_values, ARRAY_LENGTH(initial_values),
    sync_tuple_changed_callback, sync_error_callback, NULL);

  send_cmd();
}

static void window_unload(Window *window) {
  text_layer_destroy(title_layer);
  text_layer_destroy(subtitle_layer);
  text_layer_destroy(data_layer);
  text_layer_destroy(percentage_layer);
  text_layer_destroy(days_layer);
}

static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  const int inbound_size = 128;
  const int outbound_size = 128;
  app_message_open(inbound_size, outbound_size);

  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  app_sync_deinit(&sync);
  window_destroy(window);
}

int main(void) {
  init();

  app_event_loop();
  deinit();
}
