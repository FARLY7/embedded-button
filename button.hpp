#ifndef BUTTON_H
#define BUTTON_H

#include <stdbool.h>
#include <stdint.h>
#include <functional>

#ifndef MAX_BUTTON_COUNT
#define MAX_BUTTON_COUNT 2
#endif

/* WORK IN PROGRESS */

class Button {

public:
	enum class State {
		PRESSED,
		RELEASED,
		HELD
	};

	
	Button(button_callback_t callback);

	State state(void);

private:
	bool pressed;
	State state;
	uint8_t debounce_cnt;
	uint8_t debounce_reload;
	button_callback_t callback;
};


class ButtonDebouncer {

public:

	ButtonDebouncer();

	void add();

	void state_changed(Button *button, bool pressed);

	void tick();

private:

	enum class DebounceState {
		RELEASED,
		PRESSED,
		DEBOUNCING_PRESS,
		DEBOUNCING_RELEASE,
	};

	size_t debounce_threshold_ms;
};

#endif /* BUTTON_H */