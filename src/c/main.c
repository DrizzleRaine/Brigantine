// Brigantine Copyright 2017 nem0 (www.north-40.net)

// INCLUDES
#include "config.h"

ClaySettings settings;

// DEFAULT SETTINGS
static void config_default_settings() {
	// Step/Sleep Type Key: 0 = past day, 1 = avg of today's weekday, 2 = avg past week, 3  = avg past month, 4 = manual
	settings.enableSteps = true;
  settings.steps_type = 1;
  settings.steps_count = 10000;
	settings.enableSleep = 1;
  settings.sleep_type = 1;
  settings.sleep_count = 7;
	for (int s=0; s<24; s++) {
		settings.tides_storage[s] = 0;
	}
	settings.updated = false;
}

// SAVE PERSISTENT SETTINGS
static void config_save_settings() {
  persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
}

// READ PERSISTENT SETTINGS
static void config_load_settings() {
  config_default_settings();
  persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
}

// UPDATE TIME
static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);
  text_layer_set_text(s_time_layer, s_buffer);
	
	// Write day into buffer
	static char day_text[32];
	strftime(day_text, sizeof(day_text), "%A", tick_time);
  text_layer_set_text(s_day_layer, day_text);
	
	// Write current date to buffer
	static char date_text[20];
	strftime(date_text, sizeof(date_text), "%m-%d-%Y", tick_time);
	text_layer_set_text(s_date_layer, date_text);
	
	// Get current date formatted for NOAA to reduce polling to the API
	if (!settings.updated) {
		strftime(settings.last_poll, sizeof(settings.last_poll), "%Y%m%d", tick_time);
	}
	strftime(s_last_poll, sizeof(s_last_poll), "%Y%m%d", tick_time);	
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Last poll: %s",settings.last_poll);
}

// BATTERY UPDATE PROCESS
static void battery_update_proc() {
	static char s_buffer[] = "000";

	if (s_charging) {
		snprintf(s_buffer, sizeof(s_buffer)+1, "%s", "C");
		if (s_plugged && (s_battery_level == 100)) {
			snprintf(s_buffer, sizeof(s_buffer)+1, "%s", "F");
		}
	}
	else {
		snprintf(s_buffer, sizeof(s_buffer)+1, "%d", s_battery_level);
	}
	
	text_layer_set_text(s_battery_layer, s_buffer);
}

// BATTERY HANDLER
static void battery_callback(BatteryChargeState state) {
  s_battery_level = state.charge_percent;
	s_charging = state.is_charging;
	s_plugged = state.is_plugged;
}

// BLUETOOTH UPDATE PROCESS
static void bluetooth_update_proc() {
  if(!s_connected) {
		vibes_double_pulse();
	}
	layer_set_hidden(bitmap_layer_get_layer(s_bluetooth_bitlayer),!s_connected);
}

// BLUETOOTH HANDLER
static void bluetooth_callback(bool connected) {
	s_connected = connected;
}

// HEALTH UPDATE PROCESS
static void health_update_proc() {
	#if defined(PBL_HEALTH)
	static char s_bufferX[] = "00000";
	static char s_bufferN[] = "00000";
	
	snprintf(s_bufferX, sizeof(s_bufferX)+1, "%d",s_xp_level);
	if (s_head_level % 3600 / 60 < 10) {
		snprintf(s_bufferN, sizeof(s_bufferN)+1, "%d:0%d",s_head_level / 3600,s_head_level % 3600 / 60);		
	}
	else {
		snprintf(s_bufferN, sizeof(s_bufferN)+1, "%d:%d",s_head_level / 3600,s_head_level % 3600 / 60);
	}
	text_layer_set_text(s_xp_layer, s_bufferX);
	text_layer_set_text(s_nextLvl_layer, s_bufferN);
	#endif
}

// HEALTH HANDLER
static void health_callback(HealthEventType event, void *context) {	
	#if defined(PBL_HEALTH)
	HealthMetric step_metric = HealthMetricStepCount;
	HealthMetric sleep_metric = HealthMetricSleepSeconds;
	HealthServiceAccessibilityMask mask;
	HealthServiceTimeScope scope = HealthServiceTimeScopeWeekly;
	
	time_t now = time(NULL);
	time_t today = time_start_of_today();
	time_t day = today - SECONDS_PER_DAY;
	time_t week = today - (7 * SECONDS_PER_DAY);
	time_t month = today - (30 * SECONDS_PER_DAY);
	time_t start = today;
	
	s_xp_level = (int)health_service_sum_today(step_metric);
	s_next_level = settings.steps_count;

	mask = health_service_metric_accessible(step_metric, start, now);
	if(mask & HealthServiceAccessibilityMaskAvailable) {
		s_current_level = (int)health_service_aggregate_averaged(step_metric, start, now, HealthAggregationSum, scope);
	}
	else {
 		APP_LOG(APP_LOG_LEVEL_ERROR, "Step data unavailable for current level!");
	}
	
	switch (settings.steps_type) {
		case 0 :	start = day;
							scope = HealthServiceTimeScopeOnce;
		break;
		case 1 :	start = day;
							scope = HealthServiceTimeScopeWeekly;
		break;
		case 2 :	start = week;
							scope = HealthServiceTimeScopeDaily;
		break;
		case 3 :	start = month;
							scope = HealthServiceTimeScopeDaily;
		break;
	}
	
	if (settings.steps_type != 4) {
		mask = health_service_metric_accessible(step_metric, start, today);
		if(mask & HealthServiceAccessibilityMaskAvailable) {
			s_next_level = (int)health_service_aggregate_averaged(step_metric, start, today, HealthAggregationSum, scope);
		}
		else {
 			APP_LOG(APP_LOG_LEVEL_ERROR, "Step data unavailable!");
		}
	}
	
	s_head_level = (int)health_service_sum_today(sleep_metric);
	s_headmax_level = settings.sleep_count;

	switch (settings.sleep_type) {
		case 0 :	start = day;
							scope = HealthServiceTimeScopeOnce;
		break;
		case 1 :	start = day;
							scope = HealthServiceTimeScopeWeekly;
		break;
		case 2 :	start = week;
							scope = HealthServiceTimeScopeDaily;
		break;
		case 3 :	start = month;
							scope = HealthServiceTimeScopeDaily;
		break;
	}	
	
	if (settings.sleep_type != 4) {
		mask = health_service_metric_accessible(sleep_metric, start, today);
		if(mask & HealthServiceAccessibilityMaskAvailable) {
			s_head_level = (int)health_service_sum_today(sleep_metric);
			s_headmax_level = (int)health_service_aggregate_averaged(sleep_metric, start, today, HealthAggregationSum, scope);
		}
		else {
  		// No data recorded yet today
  		APP_LOG(APP_LOG_LEVEL_ERROR, "Sleep data unavailable!");
		}
	}
	#endif
}

// JAVA ERROR LOGGING : DROPPED MESSAGE
static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

// JAVA ERROR LOGGING : OUTBOX SEND FAILURE
static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

// JAVA ERROR LOGGING : OUTBOX SEND SUCCESS
static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Outbox send success!");
}

// JAVA OUTBOX SENT HANDLER
/*static void outbox_sent_handler(DictionaryIterator *iter, void *context) {
	Tuplet call_type[] = {
	  TupletCString(JS_CALL_TYPE, "Weather"),
	};
	
	uint8_t buffer[256];
	uint32_t size = sizeof(buffer);
	dict_serialize_tuplets_to_buffer(call_type, ARRAY_LENGTH(call_type), buffer, &size);
	
	int value = 3;
	
  if(app_message_outbox_begin(&iter) == APP_MSG_OK) {
    dict_write_int(iter, MESSAGE_KEY_JS_CALL_TYPE, &value, sizeof(int), true);
    app_message_outbox_send();
  }

  APP_LOG(APP_LOG_LEVEL_INFO, "All transmission complete!");
}*/

// JAVA INBOX HANDLER
static void inbox_received_handler(DictionaryIterator *iterator, void *context) {
	// Read tuples for data
	Tuple *temp_tuple = dict_find(iterator, MESSAGE_KEY_TEMPERATURE);
	Tuple *conditions_tuple = dict_find(iterator, MESSAGE_KEY_CONDITIONS);
	Tuple *location_tuple = dict_find(iterator, MESSAGE_KEY_LOCATION_NAME);
	
	Tuple *t_TIDES = dict_find(iterator, MESSAGE_KEY_TIDES);

	Tuple *t_ENABLE_STEPS = dict_find(iterator, MESSAGE_KEY_ENABLE_STEPS);
 	Tuple *t_STEPS_TYPE = dict_find(iterator, MESSAGE_KEY_STEPS_TYPE);
	Tuple *t_STEPS_COUNT = dict_find(iterator, MESSAGE_KEY_STEPS_COUNT);
	Tuple *t_ENABLE_SLEEP = dict_find(iterator, MESSAGE_KEY_ENABLE_SLEEP);
 	Tuple *t_SLEEP_TYPE = dict_find(iterator, MESSAGE_KEY_SLEEP_TYPE);
	Tuple *t_SLEEP_COUNT = dict_find(iterator, MESSAGE_KEY_SLEEP_COUNT);
	
	// If there's weather data, process it
	if(temp_tuple && conditions_tuple && location_tuple) {
		static char temperature_buffer[8];
		static char conditions_buffer[32];
		static char location_buffer[32];
		static char weather_layer_buffer[32];

  	snprintf(temperature_buffer, sizeof(temperature_buffer)+1, "%d F", (int)temp_tuple->value->int32);
  	snprintf(conditions_buffer, sizeof(conditions_buffer)+1, "%s", conditions_tuple->value->cstring);
  	snprintf(location_buffer, sizeof(location_buffer)+1, "%s", location_tuple->value->cstring);
		
		snprintf(weather_layer_buffer, sizeof(weather_layer_buffer)+1, "%s, %s", temperature_buffer, conditions_buffer);
		text_layer_set_text(s_lvl_layer, weather_layer_buffer);
		text_layer_set_text(s_loc_layer, location_buffer);
	}
	
	bool update = false;
	
	if (t_ENABLE_STEPS || t_STEPS_TYPE || t_STEPS_COUNT || t_ENABLE_SLEEP || t_SLEEP_TYPE || t_SLEEP_COUNT) {update = true;}

	// If there's config data, use it
	if (t_ENABLE_STEPS){
		settings.enableSteps = (bool)t_ENABLE_STEPS->value->int8;
	}
	if (t_STEPS_TYPE) {
		settings.steps_type = t_STEPS_TYPE->value->cstring[0] - '0';
	}
	if (t_STEPS_COUNT) {
		if (settings.steps_type == 4) {
			settings.steps_count = atoi(t_STEPS_COUNT->value->cstring);
		}
	} 
	if (t_ENABLE_SLEEP){
		settings.enableSleep = (bool)t_ENABLE_SLEEP->value->int8;
	}
	if (t_SLEEP_TYPE) {
		settings.sleep_type = (int)t_SLEEP_TYPE->value->cstring[0] - '0';
	}
	if (t_SLEEP_COUNT) {
		if (settings.sleep_type == 4) {
			settings.sleep_count = (int)t_SLEEP_COUNT->value->int32;
		}
	}

	if (t_TIDES && !settings.updated) {
		static char tides_buffer[49];
		
		snprintf(tides_buffer, sizeof(tides_buffer), "%s", t_TIDES->value->cstring);
		//APP_LOG(APP_LOG_LEVEL_DEBUG, "Tide Heights: %s",tides_buffer);
		
		for (int i=0; i<48; i+=2) {
			settings.tides_storage[i/2] = (tides_buffer[i]-'0')*10+(tides_buffer[i+1]-'0');
			settings.updated = true;
			update = true;
		}
	}
	else if (settings.updated && (settings.last_poll != s_last_poll)) {
		settings.updated = false;
	}
	
	if (update) {
		config_save_settings();
		s_connected = connection_service_peek_pebble_app_connection();
		battery_callback(battery_state_service_peek());
		battery_update_proc();
		bluetooth_update_proc();
		#if defined(PBL_HEALTH)
		health_callback(health_service_peek_current_activities(), NULL);
		health_update_proc();
		#endif
	}
}

// JAVA INBOX LOADER
static void inbox_loader() {
	// Register callbacks
	app_message_register_inbox_received(inbox_received_handler);
	app_message_register_inbox_dropped(inbox_dropped_callback);
	app_message_register_outbox_failed(outbox_failed_callback);
	app_message_register_outbox_sent(outbox_sent_callback);
	
	// Open AppMessage
	const int inbox_size = 128;
	const int outbox_size = 132;
	app_message_open(inbox_size, outbox_size);
	//app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

// JAVA INBOX UPDATER
static void inbox_updater() {
  DictionaryIterator *iter;
 	app_message_outbox_begin(&iter);
  dict_write_uint8(iter, 0, 0);
  app_message_outbox_send();
}

// TICK HANDLER
static void tick_handler(struct tm* tick_time, TimeUnits units_changed) {
	bool charge_state_change = s_battery_charge_state.is_charging;
	bool connection_state_change = s_connected;

	// Update once per minute
	if(tick_time->tm_sec == 0) {
    update_time();
		#if defined(PBL_HEALTH)
		health_callback(health_service_peek_current_activities(), NULL);
		health_update_proc();
		#endif
  }
		
	// Update on change
	s_battery_charge_state = battery_state_service_peek();
	if (charge_state_change != s_battery_charge_state.is_charging) {
		battery_update_proc();
	}
	
	// Update on change, allegedly
	s_connected = connection_service_peek_pebble_app_connection();	
	if (connection_state_change != s_connected) {
		bluetooth_update_proc();
	}
	
	// Update every 30 minutes
	if(tick_time->tm_min % 30 == 0) {
 		inbox_updater();
	}
}

// DRAW BARS : GENERIC WRAPPER
static void draw_bar(int width, int current, int upper, Layer *layer, GContext *ctx){
	GRect bounds = layer_get_bounds(layer);

	if (current < upper) {
	  width = (current * width) / upper;
	}

  // Draw the background
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, GRect(1, 1, bounds.size.w, bounds.size.h), 0, GCornerNone);

  // Draw the bar
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, GRect(2, 2, width, bounds.size.h), 0, GCornerNone);
}

// DRAW INDIVIDUAL STAT BARS
static void draw_batterybar(Layer *layer, GContext *ctx){
	draw_bar(battery_bar_width,s_battery_level,100,layer,ctx);
}
static void draw_sleepbar(Layer *layer, GContext *ctx){
	draw_bar(sleep_bar_width,s_head_level,s_headmax_level,layer,ctx);
}
static void draw_stepsbar(Layer *layer, GContext *ctx){
	draw_bar(steps_bar_width,s_xp_level,s_next_level,layer,ctx);
}

// GRAPHICS LOADER
static void graphics_loader(GRect frame) {
	// Draw Bluetooth icon
	s_bluetooth_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BLUETOOTH_ICON);
	s_bluetooth_bitlayer = bitmap_layer_create(GRect(-9, 45, frame.size.w, 12));
  bitmap_layer_set_bitmap(s_bluetooth_bitlayer, s_bluetooth_bitmap);
  bitmap_layer_set_alignment(s_bluetooth_bitlayer, GAlignRight);
	
	// Create time child layer
	s_time_layer = text_layer_create(GRect(0, -6, frame.size.w , 36));
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_LECO_36_BOLD_NUMBERS));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentLeft);
	text_layer_set_text(s_time_layer, "00:00");
	
	// Create day child layer
	s_day_layer = text_layer_create(GRect(2, 27, frame.size.w, 34));
  text_layer_set_text_color(s_day_layer, GColorWhite);
  text_layer_set_background_color(s_day_layer, GColorClear);
  text_layer_set_font(s_day_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_day_layer, GTextAlignmentLeft);
  text_layer_set_text(s_day_layer, "TODAY");
	
	// Create date child layer
	s_date_layer = text_layer_create(GRect(2, 38, frame.size.w, 34));
  text_layer_set_text_color(s_date_layer, GColorWhite);
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_LECO_20_BOLD_NUMBERS));
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentLeft);
  text_layer_set_text(s_date_layer, "00-00-0000");
	
	// Create location child layer
	s_loc_layer = text_layer_create(GRect(2, 55, frame.size.w, 34));
  text_layer_set_text_color(s_loc_layer, GColorWhite);
  text_layer_set_background_color(s_loc_layer, GColorClear);
  text_layer_set_font(s_loc_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_loc_layer, GTextAlignmentLeft);
	text_layer_set_text(s_loc_layer, "Location");
	
	// Create weather child layer
	s_lvl_layer = text_layer_create(GRect(2, 67, frame.size.w, 34));
  text_layer_set_text_color(s_lvl_layer, GColorWhite);
  text_layer_set_background_color(s_lvl_layer, GColorClear);
  text_layer_set_font(s_lvl_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_lvl_layer, GTextAlignmentLeft);
	text_layer_set_text(s_lvl_layer, "Weather");

	// Create high tide child layer
	s_hitide_layer = text_layer_create(GRect(0, 55, frame.size.w, 34));
  text_layer_set_text_color(s_hitide_layer, GColorWhite);
  text_layer_set_background_color(s_hitide_layer, GColorClear);
  text_layer_set_font(s_hitide_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_hitide_layer, GTextAlignmentRight);
	text_layer_set_text(s_hitide_layer, "H: 00:00");
	
	// Create low tide child layer
	s_lotide_layer = text_layer_create(GRect(0, 67, frame.size.w, 34));
  text_layer_set_text_color(s_lotide_layer, GColorWhite);
  text_layer_set_background_color(s_lotide_layer, GColorClear);
  text_layer_set_font(s_lotide_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_lotide_layer, GTextAlignmentRight);
	text_layer_set_text(s_lotide_layer, "L: 00:00");

	// Create battery text child layer
	s_battery_layer = text_layer_create(GRect((battery_bar_width+2)*-1, 0, frame.size.w, 34));
  text_layer_set_text_color(s_battery_layer, GColorWhite);
  text_layer_set_background_color(s_battery_layer, GColorClear);
  text_layer_set_font(s_battery_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_battery_layer, GTextAlignmentRight);
	text_layer_set_text(s_battery_layer, "000");

	// Create steps text layer
	s_xp_layer = text_layer_create(GRect((steps_bar_width+2)*-1, 13, frame.size.w, 34));
  text_layer_set_text_color(s_xp_layer, GColorWhite);
  text_layer_set_background_color(s_xp_layer, GColorClear);
  text_layer_set_font(s_xp_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_xp_layer, GTextAlignmentRight);
  text_layer_set_text(s_xp_layer, "00000");
	
	// Create sleep text layer
	s_nextLvl_layer = text_layer_create(GRect((sleep_bar_width+2)*-1, 26, frame.size.w, 34));
  text_layer_set_text_color(s_nextLvl_layer, GColorWhite);
  text_layer_set_background_color(s_nextLvl_layer, GColorClear);
  text_layer_set_font(s_nextLvl_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_nextLvl_layer, GTextAlignmentRight);
  text_layer_set_text(s_nextLvl_layer, "00000");
		
	// Create bar layers
	s_batterybar_layer = layer_create(GRect(frame.size.w-battery_bar_width, 5, battery_bar_width, 9));
	s_stepsbar_layer = layer_create(GRect(frame.size.w-steps_bar_width, 18, steps_bar_width, 9));
	s_sleepbar_layer = layer_create(GRect(frame.size.w-sleep_bar_width, 31, sleep_bar_width, 9));
}

// CANVAS UPDATE PROCESS
static void canvas_update_proc(Layer *layer, GContext *ctx) {
	GRect frame = layer_get_bounds(layer);
	int bottom_buffer = 2;
	int tide_bar_height = 0;
	int max = settings.tides_storage[0];
	
	time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), "%H", tick_time);
	int hour_now = ((s_buffer[0]-'0')*10)+(s_buffer[1]-'0');

	for (int m=0; m<24; m++) {
		if (max < settings.tides_storage[m]) {
			max = settings.tides_storage[m];
			s_tides_hi = m;
		}
	}
	
	graphics_context_set_stroke_color(ctx, GColorWhite);
	graphics_draw_rect(ctx, GRect(0, tide_box_height, frame.size.w, frame.size.h-tide_box_height-bottom_buffer));
	
	for (int x=0; x<24; x++) {
		tide_bar_height = ((settings.tides_storage[x] * tide_box_height) / max);
  	graphics_context_set_fill_color(ctx, GColorWhite);		
		graphics_fill_rect(ctx, GRect(x*(tide_bar_width+1)+1, frame.size.h-tide_bar_height-bottom_buffer, tide_bar_width, tide_bar_height),0,GCornerNone);
		if (x == hour_now) {
			GPoint start = GPoint(x*(tide_bar_width+1)+1, tide_box_height);
			GPoint end = GPoint(x*(tide_bar_width+1)+1, frame.size.h-bottom_buffer-2);
			graphics_draw_line(ctx, start, end);
			start = GPoint(x*(tide_bar_width+1)+tide_bar_width, tide_box_height);
			end = GPoint(x*(tide_bar_width+1)+tide_bar_width, frame.size.h-bottom_buffer-2);
			graphics_draw_line(ctx, start, end);
		}
	}
}

// WINDOW : LOAD
static void main_window_load(Window *window) {
	// Paint it black
	window_set_background_color(window, GColorBlack);
	
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);

	// Create window
  GRect frame = layer_get_bounds(window_layer);
	s_canvas_layer = layer_create(frame);
	
	// Load graphics
	graphics_loader(frame);
	
	layer_add_child(s_canvas_layer, s_batterybar_layer);
	layer_add_child(s_canvas_layer, s_sleepbar_layer);
	layer_add_child(s_canvas_layer, s_stepsbar_layer);
	
	layer_set_update_proc(s_canvas_layer, canvas_update_proc);
	layer_set_update_proc(s_batterybar_layer, draw_batterybar);
	layer_set_update_proc(s_sleepbar_layer, draw_sleepbar);
	layer_set_update_proc(s_stepsbar_layer, draw_stepsbar);
	
	layer_add_child(s_canvas_layer, bitmap_layer_get_layer(s_bluetooth_bitlayer));

	layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_battery_layer));
	layer_add_child(window_layer, text_layer_get_layer(s_day_layer));
	layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
	layer_add_child(window_layer, text_layer_get_layer(s_xp_layer)); // Current steps
	layer_add_child(window_layer, text_layer_get_layer(s_nextLvl_layer)); // Weekly step average
	layer_add_child(window_layer, text_layer_get_layer(s_lvl_layer)); // Location
	layer_add_child(window_layer, text_layer_get_layer(s_loc_layer)); // Weather
	
	// Add to Window
	layer_add_child(window_get_root_layer(window), s_canvas_layer);
	
	layer_mark_dirty(s_canvas_layer);
}

// WINDOW : UNLOAD
static void main_window_unload(Window *window) {
	gbitmap_destroy(s_bluetooth_bitmap);
	
	bitmap_layer_destroy(s_background_bitlayer);
	bitmap_layer_destroy(s_bluetooth_bitlayer);

  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_battery_layer);
	text_layer_destroy(s_day_layer);
	text_layer_destroy(s_date_layer);
	text_layer_destroy(s_xp_layer);
	text_layer_destroy(s_nextLvl_layer);
	text_layer_destroy(s_lvl_layer);
	text_layer_destroy(s_loc_layer);
	text_layer_destroy(s_hitide_layer);
	text_layer_destroy(s_lotide_layer);
	
	layer_destroy(s_batterybar_layer);
	layer_destroy(s_sleepbar_layer);
	layer_destroy(s_stepsbar_layer);
	
	layer_destroy(s_canvas_layer);
}

// INITIALIZE
static void init(void) {
	// Get data from Java agents
	inbox_updater();
	inbox_loader();
	
	// Load settings
	config_load_settings();

	// Init stat variables
	s_xp_level = 0;
	s_current_level = 0;
	s_next_level = settings.steps_count;
	s_head_level = 0;
	s_headmax_level = settings.sleep_count;
	s_tides_hi = 0;
	
	// Subscribe to services
  battery_state_service_subscribe(battery_callback);
	connection_service_subscribe((ConnectionHandlers) {.pebble_app_connection_handler = bluetooth_callback});
	#if defined(PBL_HEALTH)
	health_service_events_subscribe(health_callback, NULL);
	#endif
	tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
	
	// Peek at services
	s_connected = connection_service_peek_pebble_app_connection();
	battery_callback(battery_state_service_peek());
	#if defined(PBL_HEALTH)
	health_callback(health_service_peek_current_activities(), NULL);
	#endif
	
	// Create main window element and assign to pointer
  s_main_window = window_create();
	
	// Set handlers to manage elements within the window
  window_set_window_handlers(s_main_window, (WindowHandlers) {.load = main_window_load,.unload = main_window_unload});

	// Show the Window on the watch, no animation
  window_stack_push(s_main_window, false);
	
	// Update dynamic fields
	update_time();
	battery_update_proc();
	bluetooth_update_proc();
	#if defined(PBL_HEALTH)
	health_update_proc();
	#endif
}

// DEINITIALIZE
static void deinit(void) {
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
	#if defined(PBL_HEALTH)
	health_service_events_unsubscribe();
	#endif
  window_destroy(s_main_window);
}

// RUN APPLICATION
int main(void) {
  init();
  app_event_loop();
  deinit();
}