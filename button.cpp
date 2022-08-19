#include "button.h"

#include <stddef.h>
#include <assert.h>
#include <string.h>


static button_t buttons[MAX_BUTTON_COUNT] = {0};
static size_t button_cnt = 0;


/****************************************************/
/* BUTTON CLASS 									*/
/****************************************************/

Button::Button(button_callback_t callback)
{
	this->callback = callback;
}

State Button::State()
{
	return this->State;
}

/****************************************************/
/* BUTTON DEBOUNCER CLASS 							*/
/****************************************************/

static void button_debouncer(button_t *btn, debouncer_event_t event)
{
	switch(btn->state)
	{
		case BUTTON_RELEASED:
		{
			if(event == PRESSED) {
				btn->debounce_cnt = btn->debounce_reload;
				btn->state = BUTTON_DEBOUNCING_PRESS;
				btn->callback(BUTTON_PRESSED);
			}
			break;
		}
		case BUTTON_PRESSED:
		{
			if(event == RELEASED) {
				btn->debounce_cnt = btn->debounce_reload;
				btn->state = BUTTON_DEBOUNCING_RELEASE;
				btn->callback(BUTTON_RELEASED);
			}
			break;
		}
		case BUTTON_DEBOUNCING_PRESS:
		{
			if(event == TICK) {
				if(btn->debounce_cnt-- == 0) {
					if(btn->pressed) {
						btn->state = BUTTON_PRESSED;
					}
					else {
						btn->state = BUTTON_RELEASED;
						btn->callback(BUTTON_RELEASED);
					}
				}
			}
			break;
		}
		case BUTTON_DEBOUNCING_RELEASE:
		{
			if(event == TICK) {
				if(btn->debounce_cnt-- == 0) {
					if(btn->pressed) {
						btn->state = BUTTON_PRESSED;
						btn->callback(BUTTON_PRESSED);
					}
					else {
						btn->state = BUTTON_RELEASED;
					}
				}
			}
			break;
		}
		default: break;
	}
}

button_t* button_create(button_config_t *config)
{
	assert(config != NULL);
	
	if(button_cnt >= MAX_BUTTON_COUNT)
		return NULL;
		
	memset(&buttons[button_cnt], 0, sizeof(struct button));
	
	if(config->callback == NULL) {
		buttons[button_cnt].callback = default_button_listener;
	} else {
		buttons[button_cnt].callback = config->callback;
	}

	buttons[button_cnt].debounce_reload = config->debounce_threshold;
	buttons[button_cnt].state = BUTTON_RELEASED;
	
	button_cnt++;
	return &buttons[button_cnt-1];
}

button_state_t button_state(button_t *btn)
{
	if(btn->state == BUTTON_PRESSED || btn->state == BUTTON_DEBOUNCING_PRESS) {
		return BUTTON_PRESSED;
	}
	
	return BUTTON_RELEASED;
}

bool button_process_pending(void)
{
	for(int i = 0 ; i < button_cnt ; i++)
	{
		if(buttons[i].state == BUTTON_DEBOUNCING_PRESS ||
		   buttons[i].state == BUTTON_DEBOUNCING_RELEASE)
		{
			return true;	   
		}
	}
	return false;
}

void button_irq_handler(button_t *btn, bool pressed)
{
	btn->pressed = pressed;
	
	if(pressed) {
		button_debouncer(btn, PRESSED);
	} else {
		button_debouncer(btn, RELEASED);
	}
}

void button_tick(void)
{
	for(int i = 0 ; i < button_cnt ; i++)
	{
		if(buttons[i].state == BUTTON_RELEASED ||
		   buttons[i].state == BUTTON_PRESSED) {
			
			/* If button is in pressed or released state,
			   then no need to feed ticks, as they are only used
			   for debouncing states.
			 */
			continue; 
		}
		
		button_debouncer(&buttons[i], TICK);
	}	
}