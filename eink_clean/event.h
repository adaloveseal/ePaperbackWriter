#ifndef __EV_EVENT_H
#define __EV_EVENT_H

#define EV_MAX_INTERRUPT_DEPTH 5

uint8_t ev_register_interrupt(bool(*condition)(void*), void *condition_ctx, void(*action)(void*), void *action_ctx);
void ev_run_interrupts();

#endif
