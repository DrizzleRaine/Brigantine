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
static int s_heart_level;

static bool s_charging;
static bool s_connected;
static bool s_plugged;

static BatteryChargeState s_battery_charge_state;

static bool crippledAL = false;
static bool crippledAR = false;
static bool crippledLL = false;
static bool crippledLR = false;
static bool crippledH1 = false;
static bool crippledH2 = false;
static bool dead = false;

static TextLayer *s_time_layer;
static TextLayer *s_battery_layer;
static TextLayer *s_date_layer;
static TextLayer *s_xp_layer;
static TextLayer *s_nextLvl_layer;
static TextLayer *s_lvl_layer;
static TextLayer *s_heart_layer;

static BitmapLayer *s_background_bitlayer;
static BitmapLayer *s_vaultBoy_bitlayer;
static BitmapLayer *s_crippledAL_bitlayer;
static BitmapLayer *s_crippledAR_bitlayer;
static BitmapLayer *s_crippledLL_bitlayer;
static BitmapLayer *s_crippledLR_bitlayer;
static BitmapLayer *s_crippledH1_bitlayer;
static BitmapLayer *s_crippledH2_bitlayer;
static BitmapLayer *s_dead_bitlayer;
static BitmapLayer *s_bluetooth_bitlayer;

static GBitmap *s_background_bitmap;
static GBitmap *s_vaultBoy_bitmap;
static GBitmap *s_crippledAL_bitmap;
static GBitmap *s_crippledAR_bitmap;
static GBitmap *s_crippledLL_bitmap;
static GBitmap *s_crippledLR_bitmap;
static GBitmap *s_crippledH1_bitmap;
static GBitmap *s_crippledH2_bitmap;
static GBitmap *s_dead_bitmap;
static GBitmap *s_bluetooth_bitmap;

static Layer *s_batterybar_layer;
static Layer *s_sleepbar_layer;
static Layer *s_stepsbar1_layer;
static Layer *s_stepsbar2_layer;

const int c_bar_width = 24;

static void main_window_load(Window *window);
static void battery_update_proc();