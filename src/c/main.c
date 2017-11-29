#include <pebble.h>

#define FIRE_TYPE 0

//Hub Vars
static Window *s_hub_window;
static BitmapLayer *s_background_layer;
static TextLayer *s_status_text;

void javaMessage(int fireType){
  int value;
  switch(fireType){
    case 0:
    // A dummy value for high_fire, dummy
    value = 0;
    break;
    case 1:
    // A dummy value for mid_fire, dummy
    value = 1;
    break;
    case 2:
    // A dummy value for low_fire, dummy
    value = 2;
    break;
  }
  
  vibes_short_pulse();

    // Declare the dictionary's iterator
  DictionaryIterator *out_iter;
  
    // Prepare the outbox buffer for this message
  AppMessageResult result = app_message_outbox_begin(&out_iter);
  
  if(result == APP_MSG_OK) {
  // Construct the message
      // Add an item to ask for weather data
      dict_write_int(out_iter, FIRE_TYPE, &value, sizeof(int), true);
    
      // Send this message
      result = app_message_outbox_send();
      if(result != APP_MSG_OK) {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Error sending the outbox: %d", (int)result);
      } 
    
  } else {
  // The outbox cannot be used right now
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error preparing the outbox: %d", (int)result);
  }
}

static void accel_raw_handler(AccelData *data, uint32_t num_samples) {

  static char buffer[] = "XYZ: 9999 / 9999 / 9999";
  snprintf(buffer, sizeof("XYZ: 9999 / 9999 / 9999"), "XYZ: %d / %d / %d", data[0].x, data[0].y, data[0].z);
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, buffer);
  
  //I have no idea how I'm going to organize this
  if((data[0].x < -600)){
    javaMessage(0);
  }
  if((data[0].x > 600)){
    javaMessage(2);
  }
  
}

void hub_select_single_click_handler(ClickRecognizerRef recognizer, void *context) {

    window_set_background_color(s_hub_window, GColorGreen);
  
}

void hub_config_provider(Window *window) {
// window_single_click_subscribe(BUTTON_ID_UP, up_single_click_handler);
 window_single_click_subscribe(BUTTON_ID_SELECT, hub_select_single_click_handler);
// window_single_click_subscribe(BUTTON_ID_DOWN, hub_down_single_click_handler);
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
 /* // Store incoming information
  static char conditions_buffer[32];
  static char weather_layer_buffer[32];
  
  static char episode_buffer[32];
  static char episode_layer_buffer[32];
  
  static char hosts_buffer[88];
  static char hosts_layer_buffer[88];

  // Read tuples for data
  Tuple *conditions_tuple = dict_find(iterator, KEY_TITLE);
  Tuple *episode_tuple = dict_find(iterator, KEY_EPISODE);
  Tuple *hosts_tuple = dict_find(iterator, KEY_HOSTS);

  // If all data is available, use it
  if(conditions_tuple && episode_tuple) {
    snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", conditions_tuple->value->cstring);
    snprintf(episode_buffer, sizeof(episode_buffer), "%s", episode_tuple->value->cstring);
    snprintf(hosts_buffer, sizeof(hosts_buffer), "%s", hosts_tuple->value->cstring);

    // Assemble full string and display
    snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s", conditions_buffer);
    text_layer_set_text(text_layer, weather_layer_buffer);
    snprintf(episode_layer_buffer, sizeof(episode_layer_buffer), "%s", episode_buffer);
    text_layer_set_text(s_episode_layer, episode_layer_buffer);
    snprintf(s_message, 32, "%s - %s", conditions_buffer, episode_buffer);
    snprintf(hosts_layer_buffer, sizeof(hosts_layer_buffer), "%s", hosts_buffer);
    text_layer_set_text(s_hosts_textlayer, hosts_layer_buffer);
  }*/
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  window_set_background_color(s_hub_window, GColorRed);
  
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_status_text);
  bitmap_layer_destroy(s_background_layer);
}

static void init() {
  // Create main Window element and assign to pointer
  s_hub_window = window_create();
  
  window_set_click_config_provider(s_hub_window, (ClickConfigProvider) hub_config_provider);
  
  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_hub_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  accel_data_service_subscribe(25, accel_raw_handler);

  window_stack_push(s_hub_window, true);
  
  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  // Open AppMessage
  const int inbox_size = 128;
  const int outbox_size = 128;
  app_message_open(inbox_size, outbox_size);
}

static void deinit() {
  // Destroy Window
  accel_data_service_unsubscribe();
  window_destroy(s_hub_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}