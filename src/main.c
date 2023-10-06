#include <gtk/gtk.h>
#include <glib/gstdio.h>
#include <glib/gi18n.h>
#include "ble_medical.h"

static void activate (  GtkApplication  *app,
                        gpointer        user_data)
{
        /* Construct a GtkBuilder instance and load our UI description */
        GtkBuilder *builder = gtk_builder_new ();
        gtk_builder_add_from_file (builder, "builder.ui", NULL);

        /* Connect signal handlers to the constructed widgets. */
        GObject *window = gtk_builder_get_object (builder, "window");
        gtk_window_set_application (GTK_WINDOW (window), app);


        load_file_browsing(builder, GTK_WINDOW(window));
/*
  GtkWidget *dialog;
  GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER;
  dialog = gtk_file_chooser_dialog_new(_(u8"Open Folder"), GTK_WINDOW(window), action, _(u8"_Cancel"), GTK_RESPONSE_CANCEL, _(u8"_Open"), GTK_RESPONSE_ACCEPT, NULL);
  
  

  GObject *button = gtk_builder_get_object (builder, "button_browsing");
  g_signal_connect (button, "clicked", G_CALLBACK (browsing_button_triggered), dialog);
  */


        gtk_widget_show (GTK_WIDGET (window));

  /* We do not need the builder any more */
        g_object_unref (builder);
}

int main (int argc,char *argv[])
{
#ifdef GTK_SRCDIR
        g_chdir (GTK_SRCDIR);
#endif

        GtkApplication *app = gtk_application_new ("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
        g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);

        int status = g_application_run (G_APPLICATION (app), argc, argv);
        g_object_unref (app);

        return status;
}
