#include <Arduino.h>
#include <HardwareSerial.h>
#include <stdint.h>
#include <stddef.h>
#include "event.h"

irq<void> ev_interrupt_stack[EV_MAX_INTERRUPT_DEPTH + 1];

// Returns the interrupt vector address between 0 and EV_MAX_INTERRUPT_DEPTH, 255 if stack full
uint8_t ev_register_interrupt(irq<bool> condition, irq<void> action) {
	uint8_t index;
	for (index = 0; (index <= EV_MAX_INTERRUPT_DEPTH) && ev_interrupt_stack[index].active; index++);
	if (index == EV_MAX_INTERRUPT_DEPTH)
		return 255;
	ev_interrupt_stack[index] = [&condition,&action,ev_interrupt_stack,index] {
		if (condition()) {
      Serial.println("Executing interrupt handler");
			action();
			ev_interrupt_stack[index].active = false;
		} else {
      Serial.println("Condition not verified, not running handler");
		}
	};
	return index;
}

void ev_run_interrupts() {
	for (uint8_t i = 0; i < EV_MAX_INTERRUPT_DEPTH; i++) {
    Serial.print("Interrupt vector ");
    Serial.print(i);
    Serial.print(" is ");
    Serial.println(ev_interrupt_stack[i].active);
    delay(2);
		if (ev_interrupt_stack[i].active) {
      Serial.print("Running interrupt ");
      Serial.println(i);
      delay(2);
			ev_interrupt_stack[i]();
		}
	}
}
