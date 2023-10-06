#include "ble_medical_filebrowsing.h"
#include "ble_medical_debug.h"
struct
{
        GObject *obj1;
        GObject *obj2;
} _twin_obj;
void _browsing_button_triggered(GtkButton       *self, 
                                gpointer        data)
{
        _debug_print("Browsing button triggered");
        gtk_window_present (GTK_WINDOW (data));
}
enum
{
        ID_COLUMN,
        NAME_COLUMN,
        FILE_NAME_COLUMN,
        N_COLUMNS = 3
};
void _file_chooser_response (GtkDialog       *self, 
                             gint            response_id, 
                             gpointer        data)
{
        _debug_print("File chooser button triggered");
        if (response_id == GTK_RESPONSE_ACCEPT)
        {
                GtkListStore *store = gtk_list_store_new(N_COLUMNS, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING);
                GtkTreeIter iter;
                

                gtk_tree_view_set_model(data, GTK_TREE_MODEL(store));
                GtkCellRenderer *renderer;
                GtkTreeViewColumn *column1, *column2, *column3;
                renderer = gtk_cell_renderer_text_new();
                column1 = gtk_tree_view_column_new_with_attributes("ID", renderer, "text", ID_COLUMN, NULL);
                column2 = gtk_tree_view_column_new_with_attributes("Name", renderer, "text", NAME_COLUMN, NULL);
                column3 = gtk_tree_view_column_new_with_attributes("Filename", renderer, "text", FILE_NAME_COLUMN, NULL);

                gtk_tree_view_append_column (GTK_TREE_VIEW(data), column1);
                gtk_tree_view_append_column (GTK_TREE_VIEW(data), column2);
                gtk_tree_view_append_column (GTK_TREE_VIEW(data), column3);

                GtkFileChooser *chooser = GTK_FILE_CHOOSER(self);
                g_autoptr(GFile) file = gtk_file_chooser_get_file(chooser);
                GFileInfo* inf = g_file_query_info(file, "standard::name", G_FILE_QUERY_INFO_NONE, NULL, NULL);
                _debug_print(g_file_info_get_name(inf)); // Directory name
                GFileEnumerator *fenum = g_file_enumerate_children(file, "standard::name", G_FILE_QUERY_INFO_NONE, NULL, NULL);
                
                gint i = 0;
                
                while (true)
                {
                        GFileInfo *info = g_file_enumerator_next_file(fenum, NULL, NULL);
                        if (info)
                        {
                                _debug_print(g_file_info_get_name(info)); // Each files' name

                                gtk_list_store_append(store, &iter);
                                gtk_list_store_set(store, &iter, ID_COLUMN, i, NAME_COLUMN, "ABC", FILE_NAME_COLUMN, g_file_info_get_name(info), -1);
                        }
                        else
                                break;

                }

        }

        gtk_window_destroy (GTK_WINDOW(self));
}
void _tree_selection_changed (GtkTreeSelection *selection, gpointer data)
{
        GtkTreeIter iter;
        GtkTreeModel *model;
        char *filename;

        if (gtk_tree_selection_get_selected(selection, &model, &iter))
        {
                gtk_tree_model_get(model, &iter, FILE_NAME_COLUMN, &filename, -1);
                GtkEntryBuffer *buffer = gtk_entry_buffer_new(_(filename), -1);
                gtk_entry_set_buffer(GTK_ENTRY(data), buffer);
                g_free(filename);
        }
}

void load_file_browsing(GtkBuilder      *builder,
                        GtkWindow       *window)
{
        
        GtkWidget *dialog;
        GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER;
        dialog = gtk_file_chooser_dialog_new(_(u8"Open Folder"), GTK_WINDOW(window), action, _(u8"_Cancel"), GTK_RESPONSE_CANCEL, _(u8"_Open"), GTK_RESPONSE_ACCEPT, NULL);
        GObject *button = gtk_builder_get_object (builder, "button_browsing");
        GObject *list = gtk_builder_get_object(builder, "treeview_file");
        GObject *text = gtk_builder_get_object(builder, "text_filename");
        GtkTreeSelection *select;

        select = gtk_tree_view_get_selection(GTK_TREE_VIEW(list));
        gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
        g_signal_connect(dialog, "response", G_CALLBACK(_file_chooser_response), list);
        g_signal_connect(G_OBJECT(select), "changed", G_CALLBACK(_tree_selection_changed), text);
        g_signal_connect (button, "clicked", G_CALLBACK (_browsing_button_triggered), dialog);
}
