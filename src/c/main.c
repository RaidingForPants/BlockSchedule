#include <pebble.h>
#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1
#define KEY_LOW_TEMP 2
#define KEY_HIGH_TEMP 3
#define KEY_SUNRISE 4
#define KEY_SUNSET 5

Window *window;
TextLayer *timeLayer;
TextLayer *dateLayer;
TextLayer *weatherLayer;
TextLayer *conditionsLayer;
TextLayer *schedule;
TextLayer *backgroundHalf;
TextLayer *block;
TextLayer *highAndLowTempLayer;
TextLayer *placeHolder;
Layer *windowLayer;
GBitmap *weatherImages[12];
BitmapLayer *weatherImageLayer;
time_t currentTime;
struct tm *tick_time;
bool timeToUpdate=true;
bool canUpdate=false;
bool specialDay = false;
static char specialDayIdentifier[12]={' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\0'};
static char block_schedule[7]={' ', ' ', ' ', ' ', ' ', ' ', ' '};
static char currentBlock[4]={' ', ' ', ' ', '\0'};
static char minMaxTemp[8]={' ', ' ', ' ', ' ', ' ', ' ', ' ', '\0'};
static int startTimes[6]={0,0,0,0,0,0};
static int endTimes[6]={0,0,0,0,0,0};
int index=0;
bool isSchool=false;

static void mainWindowLoad(Window *w){
	currentTime = time(NULL);
  windowLayer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(windowLayer);
  window_set_background_color(window, GColorCeleste);
  
	 //weatherImage = gbitmap_create_with_resource(resource ID);
	 //weatherImageLayer = bitmap_layer_create(GRect());
	 //bitmap_layer_set_compositing_mode(weatherImageLayer, GCompOpSet);
	 //bitmap_layer_set_bitmap(weatherImageLayer, weatherInage);
	
  timeLayer = text_layer_create(GRect(0, 18, bounds.size.w, 36));
  text_layer_set_background_color(timeLayer, GColorClear);
  text_layer_set_text_color(timeLayer, GColorWhite);
	text_layer_set_font(timeLayer, fonts_get_system_font(FONT_KEY_LECO_36_BOLD_NUMBERS));
  text_layer_set_text_alignment(timeLayer, GTextAlignmentCenter);
	
	backgroundHalf=text_layer_create(GRect(0, 0, bounds.size.w, bounds.size.h/2));
	text_layer_set_background_color(backgroundHalf, GColorBlue);
	
	placeHolder=text_layer_create(GRect(8, 93, 64, 64));
	text_layer_set_background_color(placeHolder, GColorBlue);
	text_layer_set_text(placeHolder, "Image");
	text_layer_set_font(placeHolder, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_text_color(placeHolder, GColorWhite);
	text_layer_set_text_alignment(placeHolder, GTextAlignmentCenter);
	
	block = text_layer_create(GRect(6, -4, bounds.size.w, 24));
	text_layer_set_background_color(block, GColorClear);
	text_layer_set_font(block, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_text_color(block, GColorLightGray);
	text_layer_set_text_alignment(block, GTextAlignmentLeft);
	text_layer_set_text(block, "Block:");
	
	schedule = text_layer_create(GRect(53, -4, bounds.size.w, 24));
	text_layer_set_background_color(schedule, GColorClear);
	text_layer_set_text_color(schedule, GColorLightGray);
  text_layer_set_font(schedule, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(schedule, GTextAlignmentLeft);
  
  dateLayer = text_layer_create(GRect(-4, 52, bounds.size.w, 24));
  text_layer_set_background_color(dateLayer, GColorClear);
  text_layer_set_text_color(dateLayer, GColorLightGray);
  text_layer_set_font(dateLayer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(dateLayer, GTextAlignmentRight);
  
  weatherLayer = text_layer_create(GRect(75, 90, bounds.size.w-83, 40));
  text_layer_set_background_color(weatherLayer, GColorClear);
  text_layer_set_text_color(weatherLayer, GColorBlack);
  text_layer_set_font(weatherLayer, fonts_get_system_font(FONT_KEY_LECO_36_BOLD_NUMBERS));
  text_layer_set_text_alignment(weatherLayer, GTextAlignmentCenter);
	text_layer_set_text(weatherLayer, "...");
	
	highAndLowTempLayer = text_layer_create(GRect(77, 134, bounds.size.w-75, 30));
	text_layer_set_background_color(highAndLowTempLayer, GColorClear);
  text_layer_set_text_color(highAndLowTempLayer, GColorDarkGray);
  text_layer_set_font(highAndLowTempLayer, fonts_get_system_font(FONT_KEY_LECO_20_BOLD_NUMBERS));
  text_layer_set_text_alignment(highAndLowTempLayer, GTextAlignmentCenter);
	text_layer_set_text(highAndLowTempLayer, "...-...");
  
	layer_add_child(windowLayer, text_layer_get_layer(backgroundHalf));
  layer_add_child(windowLayer, text_layer_get_layer(timeLayer));
  layer_add_child(windowLayer, text_layer_get_layer(dateLayer));
  layer_add_child(windowLayer, text_layer_get_layer(weatherLayer));
	layer_add_child(windowLayer, text_layer_get_layer(schedule));
	layer_add_child(windowLayer, text_layer_get_layer(block));
	layer_add_child(windowLayer, text_layer_get_layer(highAndLowTempLayer));
	layer_add_child(windowLayer, text_layer_get_layer(placeHolder));
	 //layer_add_child(windowLayer, bitmap_layer_get_layer(weatherImageLayer));
}

static void mainWindowUnload(Window *w){
  text_layer_destroy(timeLayer);
  text_layer_destroy(dateLayer);
  text_layer_destroy(weatherLayer);
	text_layer_destroy(schedule);
	text_layer_destroy(backgroundHalf);
	text_layer_destroy(block);
	text_layer_destroy(highAndLowTempLayer);
	text_layer_destroy(placeHolder);
	 //gbitmap_destroy(weatherImage);
	 //bitmap_layer_destroy(weatherImageLayer);
}

static int convertTimeToMin(char min[]){
	int h=(min[0]-'0')*10+(min[1]-'0');
	int	m=(min[2]-'0')*10+(min[3]-'0');
	return h*60+m;
}

static bool isTimeGreaterThanMin(struct tm *time, int minutes){
	if(minutes<1){
		return false;
	}
	if(time->tm_hour>minutes/60) return true;
	if(time->tm_hour==minutes/60){
		if(time->tm_min>minutes%60) return true;
		else return false;
	}
	return false;
}

static void updateTime(){
	currentTime = time(NULL);
	tick_time = localtime(&currentTime);
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H:%M" : "%I:%M", tick_time);
    if((tick_time->tm_hour<10&&tick_time->tm_hour>0)||(tick_time->tm_hour>12&&tick_time->tm_hour<22&&!clock_is_24h_style())){
			for(int i=1;i<(int)strlen(s_buffer);i++){
				s_buffer[i-1]=s_buffer[i];
				if(i==(int)strlen(s_buffer)-1){
					s_buffer[i]='\0';
				}
			}
		}
	 text_layer_set_text(timeLayer, s_buffer);
  
  static char date_buffer[16];
  strftime(date_buffer, sizeof(date_buffer), "%a %b %d", tick_time);
  text_layer_set_text(dateLayer, date_buffer);
}

static void setSingleBlock(int idx){
	if(isSchool){
	currentBlock[0]=block_schedule[idx];
	currentBlock[1]='\0';
	text_layer_set_text(block, "Block:");
	text_layer_set_text(schedule, currentBlock);
	}else{
		text_layer_set_text(block, "No School");
		text_layer_set_text(schedule, "");
	}
}

static void setTransitionBlock(int idx){
	if(isSchool){
	currentBlock[0]=block_schedule[idx];
	currentBlock[1]='>';
	currentBlock[2]=block_schedule[idx+1];
	text_layer_set_text(block, "Block:");
	text_layer_set_text(schedule, currentBlock);
	}else{
		text_layer_set_text(block, "No School");
		text_layer_set_text(schedule, "");
	}
}

static void updateSchedule(){
	static char date_buffer[12];
	struct tm *le_time = localtime(&currentTime);
	strftime(date_buffer, sizeof(date_buffer), "%Y-%m-%d", le_time);
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);
	dict_write_cstring(iter, 0, date_buffer);
	app_message_outbox_send();
}

static void tickHandler(struct tm *tickTime, TimeUnits units_changed) {
  updateTime();
  // Get weather update every 30 minutes
  if(tickTime->tm_min % 30 == 0) {
		
    // Begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
  
    // Add a key-value pair
    dict_write_cstring(iter, 0, "weather");
  	
    // Send the message!
    app_message_outbox_send();
  }
	if(tickTime->tm_hour==7&&tickTime->tm_min==0){
		updateSchedule();
	}
	if(tickTime->tm_min % 60 == 0){
		vibes_double_pulse();
	}
	if(tickTime->tm_hour%24==0){
		timeToUpdate=true;
	}
	if(startTimes[0]==0){
		text_layer_set_text(block, "No School");
		text_layer_set_text(schedule, "");
	}else{
		text_layer_set_text(block, "");
		text_layer_set_text(schedule, "");
	}
	if(specialDay){
		text_layer_set_text(block, specialDayIdentifier);
		text_layer_set_text(schedule, "");
	}
	if(isTimeGreaterThanMin(tickTime, startTimes[0]-11)){
		setSingleBlock(0);
	}
	if(isTimeGreaterThanMin(tickTime, endTimes[0]-4)){
		if(!(startTimes[1]==0)){
			setTransitionBlock(0);
		}
	}
  if(isTimeGreaterThanMin(tickTime, endTimes[0])&&startTimes[1]==0){
		text_layer_set_text(block, "School Over");
		text_layer_set_text(schedule, "");
	}
	if(isTimeGreaterThanMin(tickTime, startTimes[1]-1)){
		setSingleBlock(1);
	}
	if(isTimeGreaterThanMin(tickTime, endTimes[1]-4)){
		if(!(startTimes[2]==0)){
			setTransitionBlock(1);
		}
	}
	if(isTimeGreaterThanMin(tickTime, endTimes[1])&&startTimes[2]==0){
		text_layer_set_text(block, "School Over");
		text_layer_set_text(schedule, "");
	}
	if(isTimeGreaterThanMin(tickTime, startTimes[2]-1)){
		setSingleBlock(2);
	}
	if(isTimeGreaterThanMin(tickTime, endTimes[2]-4)){
		if(!(startTimes[3]==0)){
			setTransitionBlock(2);
		}
	}
	if(isTimeGreaterThanMin(tickTime, endTimes[2])&&startTimes[3]==0){
		text_layer_set_text(block, "School Over");
		text_layer_set_text(schedule, "");
	}
	if(isTimeGreaterThanMin(tickTime, startTimes[3]-1)){
		setSingleBlock(3);
	}
	if(isTimeGreaterThanMin(tickTime, endTimes[3]-4)){
		if(!(startTimes[4]==0)){
			setTransitionBlock(3);
		}
	}
	if(isTimeGreaterThanMin(tickTime, endTimes[3])&&startTimes[4]==0){
		text_layer_set_text(block, "School Over");
		text_layer_set_text(schedule, "");
	}
	if(isTimeGreaterThanMin(tickTime, startTimes[4]-1)){
		setSingleBlock(4);
	}
	if(isTimeGreaterThanMin(tickTime, endTimes[4]-4)){
	 if(!(startTimes[5]==0)){
			setTransitionBlock(4);
		}
	}
	if(isTimeGreaterThanMin(tickTime, endTimes[4])&&startTimes[5]==0){
		text_layer_set_text(block, "School Over");
		text_layer_set_text(schedule, "");
	}
	if(isTimeGreaterThanMin(tickTime, startTimes[5]-1)){
		setSingleBlock(5);
	}
	if(isTimeGreaterThanMin(tickTime, endTimes[5]-1)){
		text_layer_set_text(block, "School Over");
		text_layer_set_text(schedule, "");
	}
	if(isTimeGreaterThanMin(tickTime, 960)){
		text_layer_set_text(block, "");
		text_layer_set_text(schedule, "");
	}
	
	if(timeToUpdate){
		updateSchedule();
		timeToUpdate=false;
	}
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  static char temperature_buffer[8];
  static char conditions_buffer[32];
	static char temp_low_buffer[6];
	static char temp_high_buffer[6];
	static char block_buffer[70];
  // Read tuples for data
  Tuple *temp_tuple = dict_find(iterator, KEY_TEMPERATURE);
  Tuple *conditions_tuple = dict_find(iterator, KEY_CONDITIONS);
	Tuple *temp_low_tuple = dict_find(iterator, KEY_LOW_TEMP);
	Tuple *temp_high_tuple = dict_find(iterator, KEY_HIGH_TEMP);
	Tuple *sunrise_tuple = dict_find(iterator, KEY_SUNRISE);
	Tuple *sunset_tuple = dict_find(iterator, KEY_SUNSET);
	
// If all data is available, use it
  if((temp_tuple && conditions_tuple)&&(temp_low_tuple&&temp_high_tuple)&&(sunrise_tuple&&sunset_tuple)){
    snprintf(temperature_buffer, sizeof(temperature_buffer), "%d", (int)temp_tuple->value->int32);
    snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", conditions_tuple->value->cstring);
		snprintf(temp_low_buffer, sizeof(temp_low_buffer), "%d", (int)temp_low_tuple->value->int32);
		snprintf(temp_high_buffer, sizeof(temp_high_buffer), "%d", (int)temp_high_tuple->value->int32);
		time_t sunrise = (int)sunrise_tuple->value->int32;
		time_t sunset = (int)sunset_tuple->value->int32;
		currentTime = time(NULL);
		int i = 0;
		for(int j = 0;j<(int)strlen(temp_high_buffer);j++){
			minMaxTemp[i]=temp_high_buffer[i];
			i++;
		}
		minMaxTemp[i]='-';
		i++;
		for(int j = 0;j<(int)strlen(temp_low_buffer);j++){
			minMaxTemp[i]=temp_low_buffer[j];
			i++;
		}
		text_layer_set_text(highAndLowTempLayer, minMaxTemp);
		  if(!strcmp("clear sky", conditions_tuple->value->cstring)){
				if(currentTime<sunrise){
					
				}else if(currentTime>sunrise && currentTime<sunset){
					
				}else if(currentTime>sunset){
					
				}
			}else if(!strcmp("few clouds", conditions_tuple->value->cstring)){
				if(currentTime<sunrise){
					
				}else if(currentTime>sunrise && currentTime<sunset){
					
				}else if(currentTime>sunset){
					
				}
			}else if(!strcmp("scattered clouds", conditions_tuple->value->cstring)){
				
			}else if(!strcmp("broken clouds", conditions_tuple->value->cstring)){
				
			}else if(!strcmp("shower rain", conditions_tuple->value->cstring)){
				
			}else if(!strcmp("rain", conditions_tuple->value->cstring)){
				if(currentTime<sunrise){
					
				}else if(currentTime>sunrise && currentTime<sunset){
					
				}else if(currentTime>sunset){
					
				}
			}else if(!strcmp("thunderstorm", conditions_tuple->value->cstring)){
				
			}else if(!strcmp("snow", conditions_tuple->value->cstring)){
				
			}else if(!strcmp("mist", conditions_tuple->value->cstring)){
				
			}
  	text_layer_set_text(weatherLayer, temperature_buffer);
		
  }else{
		if(!strcmp("No School", temp_tuple->value->cstring)){
			isSchool=false;
			APP_LOG(APP_LOG_LEVEL_ERROR, "No School");
			specialDay=false;
		}else if(!strcmp("Exam", temp_tuple->value->cstring)){
			isSchool=false;
			specialDay=true;
			snprintf(specialDayIdentifier, sizeof(specialDayIdentifier), "%s", temp_tuple->value->cstring);
		}else{
			isSchool=true;
			specialDay=false;
		}
		
		snprintf(block_buffer, sizeof(block_buffer), "%s", temp_tuple->value->cstring);
		for(int i=0;i<6;i++){
			startTimes[i]=0;
			endTimes[i]=0;
		}
		for(int i=0;i<((int)strlen(block_buffer));i+=11){
			block_schedule[i/11]=block_buffer[i];
		}
		for(int i=1;i<((int)strlen(block_buffer));i+=11){
			char a[5]={block_buffer[i], block_buffer[i+1], block_buffer[i+3], block_buffer[i+4], '\0'};
			startTimes[i/11]=convertTimeToMin(a);
		}
		for(int i=6;i<((int)strlen(block_buffer));i+=11){
			char a[5]={block_buffer[i], block_buffer[i+1], block_buffer[i+3], block_buffer[i+4], '\0'};
			endTimes[i/11]=convertTimeToMin(a);
		}
		currentTime = time(NULL);
		tick_time = localtime(&currentTime);
		tickHandler(tick_time, 0);
		//text_layer_set_text(schedule, block_schedule);
	}
	//shows the block
	
  // Assemble full string and display
  
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

void deinit(void) {
  window_destroy(window);
}

void init(void) {
  window =  window_create();
  //set up the window
  window_set_window_handlers(window, (WindowHandlers) {
    .load = mainWindowLoad,
    .unload = mainWindowUnload
  });
  window_stack_push(window, true);
  tick_timer_service_subscribe(MINUTE_UNIT, tickHandler);
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  updateTime();
	currentTime=time(NULL);
	tick_time=localtime(&currentTime);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
