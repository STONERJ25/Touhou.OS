#ifndef TOUHOU_STRING_H
#define TOUHOU_STRING_H

/* No standard library exists here, so these are ours -- small and
 * deliberately minimal, not a general-purpose string library. */

static inline int str_equals(const char *a, const char *b) {
    while (*a != 0 && *b != 0) {
        if (*a != *b) {
            return 0;
        }
        a++;
        b++;
    }
    return *a == *b; /* both must hit their null terminator at the same time */
}

static inline int str_starts_with(const char *text, const char *prefix) {
    while (*prefix != 0) {
        if (*text != *prefix) {
            return 0;
        }
        text++;
        prefix++;
    }
    return 1;
}

/* The font only has uppercase glyphs -- use this before displaying any
 * externally-sourced text (CPUID strings, eventually ACPI tables, ...)
 * that might contain lowercase, rather than let it render as blank gaps. */
static inline void str_to_upper(char *text) {
    while (*text != 0) {
        if (*text >= 'a' && *text <= 'z') {
            *text = (char)(*text - 'a' + 'A');
        }
        text++;
    }
}

#endif /* TOUHOU_STRING_H */
