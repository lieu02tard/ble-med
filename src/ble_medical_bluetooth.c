#include "ble_medical_bluetooth.h"
#include "ble_medical_debug.h"

enum {
        ID_COLUMN,
        NAME_COLUMN,
        CONNECTED_COLUMN,
        N_COLUMNS = 3
};

void _connect_button_clicked(GtkButton *self, gpointer data)
{

        // Initialize the Bluetooth connection dialog

        gtk_window_present(GTK_WINDOW(data));


}
void load_bluetooth (GtkBuilder *builder, GtkWindow *window)
{
        GObject *bled = gtk_builder_get_object(builder, "bluetooth_dialog");
        GObject *button = gtk_builder_get_object(builder, "button_connect");

        gtk_window_set_transient_for(GTK_WINDOW(bled), window);
        g_signal_connect(button, "clicked", G_CALLBACK(_connect_button_clicked), bled);
}