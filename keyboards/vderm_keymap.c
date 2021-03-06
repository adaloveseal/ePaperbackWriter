#include QMK_KEYBOARD_H

#define HOME 0
#define FN 1
#define FNCHAR 2

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
	[HOME] = LAYOUT(
		KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P,
		KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_ENT,
		MT(MOD_LCTL, KC_Z), MT(MOD_LALT, KC_X), LT(1, KC_C), LT(2, KC_V), MT(MOD_LSFT, KC_SPC), MT(MOD_RALT, KC_B), MT(MOD_LGUI, KC_N), MT(MOD_LCTL, KC_SCOLON)),

	[FN] = LAYOUT(
		KC_ESC, KC_2, KC_3, KC_4, KC_0, KC_HOME, KC_PGDN, KC_PGUP, KC_END, KC_TRNS,
		KC_TAB, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_LEFT, KC_DOWN, KC_UP, KC_RGHT, KC_DEL,
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, MT(MOD_LSFT, KC_SPC), KC_TRNS, KC_TRNS, KC_TRNS),

	[FNCHAR] = LAYOUT(
		KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0,
		KC_TAB,KC_5,KC_MINS,KC_EQL,KC_BSLS,KC_LBRC,KC_RBRC,KC_QUOT,KC_SLSH,KC_BSPC,
		KC_LCTL, KC_LALT, KC_TRNS, KC_TRNS, MT(MOD_LSFT, KC_SPC), KC_M, KC_COMMA, KC_DOT)
};

void matrix_init_user(void) {
}

void matrix_scan_user(void) {
}

