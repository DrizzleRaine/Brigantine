#pragma once

#include <pebble.h>

#define SETTINGS_KEY 1

// SETTINGS STRUCTURE
typedef struct ClaySettings {
	bool crippled_status;
	int battery_breakpoint;
	int dead_battery_breakpoint;
	int steps_breakpoint;
	int sleep_breakpoint;
	bool enableSteps;
  int steps_type;
  int steps_count;
	bool enableSleep;
  int sleep_type;
  int sleep_count;
	bool enableHR;
} __attribute__((__packed__)) ClaySettings;

// VARIABLES
static Window *s_main_window;

static Layer *s_canvas_layer;

static int s_battery_level;
static int s_xp_level;
static int s_current_level;
static int s_next_level;
static int s_head_level;
static int s_headmax_level;

static bool s_charging;
static bool s_connected;
static bool s_plugged;

static BatteryChargeState s_battery_charge_state;

static TextLayer *s_time_layer;
static TextLayer *s_battery_layer;
static TextLayer *s_date_layer;
static TextLayer *s_xp_layer;
static TextLayer *s_nextLvl_layer;
static TextLayer *s_lvl_layer;
static TextLayer *s_loc_layer;

static BitmapLayer *s_background_bitlayer;
static BitmapLayer *s_bluetooth_bitlayer;

static GBitmap *s_bluetooth_bitmap;

static Layer *s_batterybar_layer;
static Layer *s_sleepbar_layer;
static Layer *s_stepsbar_layer;

const int battery_bar_width = 12;
const int sleep_bar_width = 12;
const int steps_bar_width = 12;

static void main_window_load(Window *window);
static void battery_update_proc();