#include "ble_medical_bluetooth.h"
#include "ble_medical_debug.h"

#include <glib/gi18n.h>
#include <glib/gstdio.h>
#include <simpleble_c/simpleble.h>

#define PERIPHERAL_LIST_MAX 10 // Default size for peripherals list

enum {
        ADDRESS_COL,
        IDENTIFIER_COL,
        CONNECTION_COL,
        N_COLUMNS = 3
}; // Defines 3 columns from Adapter Tree

enum {
        ADDRESS_P_COL,
        IDENTIFIER_P_COL,
        CONNECTION_P_COL,
        NP_COLUMNS = 3
}; // Defines 3 columns from Peripherals Tree
typedef struct _twin_list {
        size_t *peripheral_list_len;
        simpleble_peripheral_t *list;
} _twin_list; // 

typedef struct _twin_list_1 {
        simpleble_peripheral_t peripheral;
        simpleble_adapter_t adapter;
} _twin_list_1;

// Update information about a peripheral 's services
void _load_peripherals_info(gpointer data)
{

}

void _adapter_on_scan_found(    simpleble_adapter_t adapter, 
                                simpleble_peripheral_t peripheral, 
                                void *data)
{
        _twin_list      *tlist = (_twin_list*)data;

        char            *adapter_identifier = simpleble_adapter_identifier(adapter);
        char            *peripheral_identifier = simpleble_peripheral_identifier(peripheral);
        char            *peripheral_address = simpleble_peripheral_address(peripheral);

        if (    adapter_identifier == NULL || 
                peripheral_identifier == NULL || 
                peripheral_address == NULL )
                exit(1);

        if (*(tlist->peripheral_list_len) < PERIPHERAL_LIST_MAX)
                tlist->list[*(tlist->peripheral_list_len)++] = peripheral;
        else
                simpleble_peripheral_release_handle(peripheral);
        
        simpleble_free(peripheral_identifier);
        simpleble_free(peripheral_address);
}

void _adapter_on_scan_stop(simpleble_adapter_t adapter, void *data)
{
}

void _adapter_on_scan_start(simpleble_adapter_t adapter, void *data)
{
}
void _load_peripherals(gpointer data)
{
        GtkTreeView     *adapters_tree = (GtkTreeView*)data;
        GObject         *bled_obj = (GObject*) g_object_get_data(G_OBJECT(data), "bled");
        simpleble_adapter_t     adapter = simpleble_adapter_get_handle(GPOINTER_TO_INT(g_object_get_data(G_OBJECT(data), "adapter")));


        size_t                  peripheral_list_len = 0;
        simpleble_peripheral_t  peripheral_list[PERIPHERAL_LIST_MAX] = {0};
        _twin_list              _tmp_0 = {&peripheral_list_len, peripheral_list};

        simpleble_adapter_set_callback_on_scan_start(adapter, _adapter_on_scan_start, &_tmp_0);
        simpleble_adapter_set_callback_on_scan_stop(adapter, _adapter_on_scan_stop, &_tmp_0);
        simpleble_adapter_set_callback_on_scan_found(adapter, _adapter_on_scan_found, &_tmp_0);

        simpleble_adapter_scan_for(adapter, 5000);

        for (size_t i = 0; i < peripheral_list_len; i++)
        {
                simpleble_peripheral_t  peripheral = peripheral_list[i];
                char    *peripheral_identifier = simpleble_peripheral_identifier(peripheral);
                char    *peripheral_address = simpleble_peripheral_address(peripheral);
        }

}
void _peripherals_tree_selected(GtkTreeView     *self,
                                GtkTreePath     *path,
                                GtkTreeViewColumn       *column,
                                gpointer        data)
{
        GtkTreeSelection        *select = (GtkTreeSelection*)data;
        GtkTreeIter             iter;
        GtkTreeModel            *model;
        char                    *address;

        if (gtk_tree_selection_get_selected(select, &model, &iter))
        {
                gtk_tree_model_get(model, &iter, ADDRESS_P_COL, &address, -1);

                // Scan the received list of peripherals for one that have the same address as the list
        }
}
void _adapters_tree_selected(   GtkTreeView       *self, 
                                GtkTreePath       *path, 
                                GtkTreeViewColumn *column, 
                                gpointer          data)
{
        GtkTreeSelection        *select = (GtkTreeSelection*)data;
        GtkTreeIter             iter;
        GtkTreeModel            *model;
        char                    *address;

        if (gtk_tree_selection_get_selected(select, &model, &iter))
        {
                gtk_tree_model_get(model, &iter, ADDRESS_COL, &address, -1);

                size_t adapter_count = simpleble_adapter_get_count();

                for (size_t i = 0; i < adapter_count; i++)
                {
                        simpleble_adapter_t _tmp_adapter = simpleble_adapter_get_handle(i);
                        char *_tmp_address      = simpleble_adapter_address(_tmp_adapter);
                        char *_tmp_identifier   = simpleble_adapter_identifier(_tmp_adapter);

                        if (g_strcmp0(_tmp_address, address) == 0)
                        {
                                g_object_set_data(G_OBJECT(self), "adapter", GINT_TO_POINTER(i));
                                gtk_list_store_set(GTK_LIST_STORE(model), &iter, 
                                ADDRESS_COL, _tmp_address, 
                                IDENTIFIER_COL, _tmp_identifier, 
                                CONNECTION_COL, "Connected", -1);

                                // Triggering peripherals scanning
                                GThread *thread_1= g_thread_new("bluetooth_scanning", _load_peripherals, self);
                        }

                        simpleble_free(_tmp_address);
                        simpleble_free(_tmp_identifier);
                        simpleble_adapter_release_handle(_tmp_adapter);
                        
                }
        }
}

void _peripherals_tree_changed( GtkTreeSelection        *selection,
                                gpointer        data)
{
        GtkTreeIter     iter;
        GtkTreeModel    *model;
        char            *address;

        if (gtk_tree_selection_get_selected(selection, &model, &iter))
        {
                gtk_tree_model_get(model, &iter, ADDRESS_P_COL, &address, -1);
                GtkEntryBuffer *buffer = gtk_entry_buffer_new(_(address), -1);
                gtk_entry_set_buffer(GTK_ENTRY(data), buffer);
                g_free(address);
        }
}
void _adapters_tree_changed(    GtkTreeSelection *selection, 
                                gpointer         data)
{
        GtkTreeIter     iter;
        GtkTreeModel    *model;
        char            *address;

        if (gtk_tree_selection_get_selected(selection, &model, &iter))
        {
                gtk_tree_model_get(model, &iter, ADDRESS_COL, &address, -1);
                GtkEntryBuffer *buffer = gtk_entry_buffer_new(_(address), -1);
                gtk_entry_set_buffer(GTK_ENTRY(data), buffer);
                g_free(address);
        }
}
void _load_adapters_on_exit()
{

}

void _load_adapters(gpointer data)
{
        simpleble_err_t err_code = SIMPLEBLE_SUCCESS;
        size_t          adapter_count = simpleble_adapter_get_count();
        GtkTreeIter     iter;
        GtkListStore    *store = (GtkListStore*)data;
        GtkEntry        *text = GTK_ENTRY(g_object_get_data(G_OBJECT(data), "text"));
        GtkTreeView     *tree = g_object_get_data(G_OBJECT(data), "tree");

        atexit(_load_adapters_on_exit);
        if (adapter_count == 0)
        {
                // [TODO]: Reload after 5 seconds. If not, print a message
        }
        else
        {
                GObject         *store_obj = G_OBJECT(store);
                gpointer        __tmp_ptr = GINT_TO_POINTER(adapter_count);

                g_object_set_data(store_obj, "adapter_count", __tmp_ptr);
                for (size_t i = 0; i < adapter_count; i++)
                {
                        simpleble_adapter_t _tmp_adapter = simpleble_adapter_get_handle(i);
                        char    *identifier = simpleble_adapter_identifier(_tmp_adapter);
                        char    *address = simpleble_adapter_address(_tmp_adapter);

                        gtk_list_store_append(store, &iter);
                        gtk_list_store_set(store, &iter, 
                        ADDRESS_COL, address,
                        IDENTIFIER_COL, identifier,
                        CONNECTION_COL, "A", -1);      
                }
        }
}

void _connect_button_clicked(   GtkButton       *self, 
                                gpointer        data)
{

        gtk_window_present(GTK_WINDOW(data));

        GObject *adapters_tree      = (GObject*) g_object_get_data(data, "adapters_tree");
        GObject *adapters_text      = (GObject*) g_object_get_data(data, "adapters_text");
        GObject *peripherals_text   = (GObject*) g_object_get_data(data, "peripherals_text");
        GObject *peripherals_tree   = (GObject*) g_object_get_data(data, "peripherals_tree");
        GObject *close_button       = (GObject*) g_object_get_data(data, "close_button");
        GObject *connect_button     = (GObject*) g_object_get_data(data, "connect_button");

        gtk_tree_view_set_activate_on_single_click(GTK_TREE_VIEW(adapters_tree), false);
        
        // Load adapters tree view
        GtkListStore *adapters_store = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
        GtkListStore *peripherals_store = gtk_list_store_new(NP_COLUMNS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

        gtk_tree_view_set_model(GTK_TREE_VIEW(adapters_tree), GTK_TREE_MODEL(adapters_store));
        gtk_tree_view_set_model(GTK_TREE_VIEW(peripherals_tree), GTK_TREE_MODEL(peripherals_store));

        GtkTreeIter             adapters_iter, peripherals_iter;
        GtkCellRenderer         *renderer;
        GtkTreeViewColumn       *column_1, *column_2, *column_3, *column_4, *column_5, *column_6;

        renderer = gtk_cell_renderer_text_new();
        column_1 = gtk_tree_view_column_new_with_attributes("Address", renderer, "text", ADDRESS_COL, NULL);
        column_2 = gtk_tree_view_column_new_with_attributes("Identifier", renderer, "text", IDENTIFIER_COL, NULL);
        column_3 = gtk_tree_view_column_new_with_attributes("Connection", renderer, "text", CONNECTION_COL, NULL);

        column_4 = gtk_tree_view_column_new_with_attributes("Address", renderer, "text", ADDRESS_P_COL, NULL);
        column_5 = gtk_tree_view_column_new_with_attributes("Identifier", renderer, "text", IDENTIFIER_P_COL, NULL);
        column_6 = gtk_tree_view_column_new_with_attributes("Connection", renderer, "text", CONNECTION_P_COL, NULL);

        GtkTreeSelection *select_1 = gtk_tree_view_get_selection(GTK_TREE_VIEW(adapters_tree));
        GtkTreeSelection *select_2 = gtk_tree_view_get_selection(GTK_TREE_VIEW(peripherals_tree));

        gtk_tree_selection_set_mode(select_1, GTK_SELECTION_SINGLE);
        gtk_tree_selection_set_mode(select_2, GTK_SELECTION_SINGLE);

        g_signal_connect(G_OBJECT(select_1), "changed", G_CALLBACK(_adapters_tree_changed), adapters_text);
        g_signal_connect(G_OBJECT(adapters_tree), "row-activated", G_CALLBACK(_adapters_tree_selected), select_1);
        g_signal_connect(G_OBJECT(select_2), "changed", G_CALLBACK(_peripherals_tree_changed), peripherals_text);
        g_signal_connect(G_OBJECT(peripherals_tree), "row-activated", G_CALLBACK(_peripherals_tree_selected), select_2);

        g_object_set_data(G_OBJECT(adapters_tree), "bled", data);

        gtk_tree_view_append_column(GTK_TREE_VIEW(adapters_tree), column_1);
        gtk_tree_view_append_column(GTK_TREE_VIEW(adapters_tree), column_2);
        gtk_tree_view_append_column(GTK_TREE_VIEW(adapters_tree), column_3);
        gtk_tree_view_append_column(GTK_TREE_VIEW(peripherals_tree), column_4);
        gtk_tree_view_append_column(GTK_TREE_VIEW(peripherals_tree), column_5);
        gtk_tree_view_append_column(GTK_TREE_VIEW(peripherals_tree), column_6);


        g_object_set_data(G_OBJECT(adapters_store), "iter", &adapters_iter);
        g_object_set_data(G_OBJECT(adapters_store), "text", adapters_text);
        g_object_set_data(G_OBJECT(adapters_store), "tree", adapters_tree);

        GThread* thread_1 = g_thread_new("bluetooth_connection", _load_adapters, adapters_store);
}

void _bluetooth_dialog_activate(GtkWindow       *self, 
                                gpointer        data)
{
}
void load_bluetooth (   GtkBuilder      *builder, 
                        GtkWindow       *window)
{
        GObject *bled   = gtk_builder_get_object(builder, "bluetooth_dialog");
        GObject *button = gtk_builder_get_object(builder, "button_connect");

        gtk_window_set_transient_for(GTK_WINDOW(bled), window);
        g_object_set_data(bled, "builder", builder);


        GObject *adapters_text      = gtk_builder_get_object(builder, "adapters_text");
        GObject *adapters_tree      = gtk_builder_get_object(builder, "adapters_tree");

        GObject *peripherals_text   = gtk_builder_get_object(builder, "peripherals_text");
        GObject *peripherals_tree   = gtk_builder_get_object(builder, "peripherals_tree");

        GObject *connect_button     = gtk_builder_get_object(builder, "bluetooth_connect_button");
        GObject *close_button       = gtk_builder_get_object(builder, "bluetooth_close_button");

        g_object_set_data(bled, "adapters_text", adapters_text);
        g_object_set_data(bled, "adapters_tree", adapters_tree);
        g_object_set_data(bled, "peripherals_text", peripherals_text);
        g_object_set_data(bled, "peripherals_tree", peripherals_tree);
        g_object_set_data(bled, "connect_button", connect_button);
        g_object_set_data(bled, "close_button", close_button);
        
        g_signal_connect(button, "clicked", G_CALLBACK(_connect_button_clicked), bled);
}