#ifndef BUTTON_H
#define BUTTON_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_BUTTON_COUNT 3

typedef enum {
	BUTTON_RELEASED,
	BUTTON_PRESSED,
	BUTTON_DEBOUNCING_PRESS,
	BUTTON_DEBOUNCING_RELEASE,
} button_state_t;

/**
 * @brief Button callback executed on button state changes.
 * @param[in] state The state of the button.
 * @return none
 */
typedef void (*button_callback_t)(button_state_t state);

typedef struct {
	uint8_t debounce_threshold;
	button_callback_t callback;
} button_config_t;

typedef struct button button_t;



button_t* button_create(button_config_t *config);

button_state_t button_state(button_t *btn);

bool button_process_pending(void);

void button_irq_handler(button_t *btn, bool pressed);

void button_tick(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* BUTTON_H */