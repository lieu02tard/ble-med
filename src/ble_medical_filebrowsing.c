#include "ble_medical_filebrowsing.h"

void _browsing_button_triggered(GtkButton       *self, 
                                gpointer        data)
{
        g_print("Browsing button triggered");
        gtk_window_present (GTK_WINDOW (data));
}

void _file_chooser_response    (GtkDialog       *self, 
                                qint            reponse_id, 
                                gpointer        data)
{

}

void load_file_browsing(GtkBuilder      *builder,
                        GtkWindow       *window)
{
        
        GtkWidget *dialog;
        GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER;
        dialog = gtk_file_chooser_dialog_new(_(u8"Open Folder"), GTK_WINDOW(window), action, _(u8"_Cancel"), GTK_RESPONSE_CANCEL, _(u8"_Open"), GTK_RESPONSE_ACCEPT, NULL);
        GObject *button = gtk_builder_get_object (builder, "button_browsing");
        GObject *list = gtk_builder_get_object(builder, "treeview_file")

        g_signal_connect(dialog, "response", G_CALLBACK(_file_chooser_response), dialog);

        g_signal_connect (button, "clicked", G_CALLBACK (_browsing_button_triggered), dialog);
}
