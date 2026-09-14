#ifndef TOUHOU_SERIAL_H
#define TOUHOU_SERIAL_H

/* Must be called once before serial_write/serial_write_char. */
void serial_init(void);

void serial_write_char(char c);
void serial_write(const char *text);

#endif /* TOUHOU_SERIAL_H */
