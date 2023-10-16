#ifndef BLE_MEDICAL_PLOT_H
#define BLE_MEDICAL_PLOT_H

#include "ble_medical_data.h"
#include <gtk/gtk.h>
#include "gtkchart.h"


void start_plotting();
void end_plotting(GtkChart*);
void chart_plot_thread(GtkChart*, ble_pack_t);
void chart_register_starting(GtkChart*, ble_time_t);
void load_plotting(GtkBuilder*, GtkWindow*);

#endif