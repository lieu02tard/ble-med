#ifndef BLE_MEDICAL_DEBUG_H
#define BLE_MEDICAL_DEBUG_H

#ifdef DEBUG
#include <gtk/gtk.h>
#include <glib/gstdio.h>
#include <glib/gi18n.h>
#include <stdio.h>
#include <stdlib.h>

#define c_red           "\033[1;31m"
#define c_green         "\033[1;32m"
#define c_yellow        "\033[1;33m"
#define c_blue          "\033[1;34m"
#define c_magenta       "\033[1;35m"
#define c_cyan          "\033[1;36m"
#define c_reset         "\033[0m"
void _debug_print(const char* title) {
        fprintf(stderr, c_red);
        fprintf(stderr, "[DEBUG]: ");
        fprintf(stderr, c_reset);
        fprintf(stderr, c_green);
        fprintf(stderr, title);
        fprintf(stderr, c_reset);
        fprintf(stderr, "\n");
}
#else
void _debug_print(const char* title) {
}
#endif

#endif