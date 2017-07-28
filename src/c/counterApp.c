#include <pebble.h>
#include <stdlib.h>   //for calloc

#if defined(PBL_COLOR)
#define BGColor GColorJaegerGreen
#define FGColor GColorDarkGreen
#else
#define BGColor GColorWhite
#define FGColor GColorBlack
#endif

static Window * window;
static TextLayer * symbol;
static TextLayer * counter;
Layer * window_layer;
GRect bound;

char * toString();
int digits();
int pwrOf10();

int count;
char * str;

static void refresh_screen() {
  layer_mark_dirty(window_layer);
}

static void canvas_update_proc(Layer *layer, GContext *ctx) {
  GRect bg_bounds = GRect(bound.origin.x, bound.origin.y, bound.size.w, bound.size.h);
  graphics_context_set_stroke_color(ctx, BGColor);
  graphics_context_set_fill_color(ctx, BGColor);
  int corner_radius = 0;
  graphics_fill_rect(ctx, bg_bounds, corner_radius, GCornersAll);
  
  graphics_context_set_stroke_color(ctx, FGColor);
  graphics_context_set_fill_color(ctx, FGColor);
  
  GRect rect1_bounds = GRect(bound.origin.x + ((int) bound.size.w * .35), (bound.size.h / 2) - ((int) bound.size.h * .15), bound.size.w - ((int) bound.size.w * PBL_IF_RECT_ELSE(.675, .685)), bound.size.h - ((int) bound.size.h * PBL_IF_RECT_ELSE(.675, .685)));
  graphics_draw_rect(ctx, rect1_bounds);
  
  GRect rect2_bounds = GRect(bound.origin.x + ((int) bound.size.w * .3), (bound.size.h / 2) - ((int) bound.size.h * .2), bound.size.w - ((int) bound.size.w * PBL_IF_RECT_ELSE(.59, .6)), bound.size.h - ((int) bound.size.h * PBL_IF_RECT_ELSE(.59, .6)));
  graphics_draw_rect(ctx, rect2_bounds);
  
  GRect rect3_bounds = GRect(bound.origin.x + ((int) bound.size.w * PBL_IF_RECT_ELSE(.795, .825)), (bound.size.h / 2) - ((int) bound.size.h * .11), bound.size.w - ((int) bound.size.w * .1), bound.size.h - ((int) bound.size.h * .765));
  graphics_draw_rect(ctx, rect3_bounds);
  
  GFont font = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  
  // Set the color
  graphics_context_set_text_color(ctx, FGColor);
    
  // Determine a reduced bounding box
  GRect bounds = GRect(bound.origin.x, (bound.size.h / 2) - ((int) bound.size.h * .1), bound.size.w, bound.size.h);
  //Calculate the size of the text to be drawn, with restricted space
  GSize text_size = graphics_text_layout_get_content_size(str, font, bounds, GTextOverflowModeWordWrap, GTextAlignmentCenter);

  //Draw the text
  graphics_draw_text(ctx, str, font, bounds, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  graphics_draw_text(ctx, " +  ", font, bounds, GTextOverflowModeWordWrap,GTextAlignmentRight, NULL);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (count < 9999)
    count++;
  else
    count = 0;
  str = toString(count);
  //const char * string = str;
  //update text using string
  refresh_screen();
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

static void window_load(Window *window) {
  window_layer = window_get_root_layer(window);
  layer_set_update_proc(window_layer, canvas_update_proc);
  bound = layer_get_unobstructed_bounds(window_layer);
  refresh_screen();
}

static void window_unload(Window *window) {
  layer_remove_child_layers(window_get_root_layer(window));
  
  text_layer_destroy(symbol);
  text_layer_destroy(counter);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
  
  count = 0;
  str = toString(count, str);
}

static void deinit(void) {
  window_destroy(window);
}

char * toString(int value, char * result)
{
	int digit = digits(value);
	result = calloc(digit + 1, sizeof(char));
	result[digit] = '\0';
	int usedVal = 0;
	for (int i = digit; i > 0; i--)
	{
		int x = (value - usedVal) / pwrOf10(i - 1);
		result[digit - i] = (char) x + '0';
		usedVal = usedVal + (result[digit - i] - '0') * pwrOf10(i - 1);
	}
	return result;
}
int digits(int n) {
    if (n < 0) return digits((n == 0) ? 9999 : -n);
    if (n < 10) return 1;
    return 1 + digits(n / 10);
}
int pwrOf10(int power)
{
	int val = 1;
	int i = power;
	while (i > 0)
	{
		val *= 10;
		i--;
	}
	return val;
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}