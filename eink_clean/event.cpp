#include <Arduino.h>
#include <HardwareSerial.h>
#include <stdint.h>
#include <stddef.h>
#include "event.h"

struct irq {
	bool active;
	bool(*condition)(void*);
	void *condition_ctx;
	void(*action)(void*);
	void *action_ctx;
};

struct irq ev_interrupt_stack[EV_MAX_INTERRUPT_DEPTH + 1];

// Returns the interrupt vector address between 0 and EV_MAX_INTERRUPT_DEPTH, 255 if stack full
uint8_t ev_register_interrupt(bool(*condition)(void*), void *condition_ctx, void(*action)(void*), void *action_ctx) {
	uint8_t index;
	for (index = 0; (index <= EV_MAX_INTERRUPT_DEPTH) && ev_interrupt_stack[index].active; index++);
	if (index == EV_MAX_INTERRUPT_DEPTH)
		return 255;
	ev_interrupt_stack[index] = { true, condition, condition_ctx, action, action_ctx };
	return index;
}

void ev_run_interrupts() {
	for (uint8_t i = 0; i < EV_MAX_INTERRUPT_DEPTH; i++) {
		struct irq *handler = ev_interrupt_stack + i;
		if (handler->active) {
			if (handler->condition(handler->condition_ctx)) {
				handler->action(handler->action_ctx);
	 			handler->active = false;
			}
		}
	}
}
