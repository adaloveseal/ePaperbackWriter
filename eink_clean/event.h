#ifndef __EV_EVENT_H
#define __EV_EVENT_H

#define EV_MAX_INTERRUPT_DEPTH 5

template<typename T> struct irq {
public:
  bool active;
	irq() {
    active = false;
	};
	template<typename F> irq(F&& f): _holder(new holder<F>(static_cast<F>(f))){
    active = true;
	};
	T operator () () {
		return _holder->call();
	};
private:
	struct holder_base {
		virtual T call() = 0;
	};
	template<typename F> struct holder: holder_base {
		holder(F&& f): _f(static_cast<F>(f)) { }
		T call() { return _f(); }
		F _f;
	};
	holder_base *_holder;
};

uint8_t ev_register_interrupt(irq<bool> condition, irq<void> action);
void ev_run_interrupts();

#endif
