#include <gtk/gtk.h>
#include <glib/gstdio.h>
#include <glib/gi18n.h>
#include "ble_medical.h"

static void activate (  GtkApplication  *app,
                        gpointer        user_data)
{
        GtkBuilder *builder = gtk_builder_new ();
        gtk_builder_add_from_file (builder, "builder.ui", NULL);

        GObject *window = gtk_builder_get_object (builder, "window");
        gtk_window_set_application (GTK_WINDOW (window), app);

        // Loading submodules
        load_file_browsing(builder, GTK_WINDOW(window));
        load_bluetooth(builder, GTK_WINDOW(window));

        gtk_widget_show (GTK_WIDGET (window));

        // Remember to free memory
        g_object_unref (builder);
}

int main (int argc,char *argv[])
{
#ifdef GTK_SRCDIR
        g_chdir (GTK_SRCDIR);
#endif

        GtkApplication *app = gtk_application_new ("org.gtk.ble-medical", G_APPLICATION_DEFAULT_FLAGS);
        g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);

        int status = g_application_run (G_APPLICATION (app), argc, argv);
        g_object_unref (app);

        return status;
}
