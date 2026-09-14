#ifndef TOUHOU_KEYBOARD_H
#define TOUHOU_KEYBOARD_H

/* Blocks until a key with a known ASCII mapping is pressed, then returns
 * that character. Key releases and unmapped keys (arrows, Shift, etc.)
 * are silently skipped. */
char keyboard_read_char(void);

#endif /* TOUHOU_KEYBOARD_H */
