#include "ble_medical_plot.h"
#include "ble_medical_data.h"
#include "ble_medical_debug.h"
#include "config.h"
#include <math.h>
#include "ble_medical_bluetooth.h"
#include <simpleble_c/simpleble.h>

static GMutex producer_mutex;
static GtkChart *chart = NULL;
static ble_time_t _starting_time;
static GFileOutputStream* fstream;
static int initiatedDataReceiving = false;
static int isWriting = false;
static int isPlotting = false;

gint _data_compare_func (gconstpointer first, gconstpointer second, gpointer data)
{
        ble_time_t _first = ((t_pack*)first)->time;
        ble_time_t _second = ((t_pack*)second)->time;
        return (int)(_first - _second);
}

void data_writing(gpointer data)
{
        t_pack *t_pack_0 = (t_pack*)data;
        point_t points[10];
        pack_to_point(points, _starting_time, t_pack_0);
        gsize bytes_written;
#ifndef BLE_MEDICAL_PLOT_LOG
        for (size_t j = 0; j < 10; j++)
                g_output_stream_printf(G_OUTPUT_STREAM(fstream), &bytes_written, NULL, NULL, "%f :: %f\n", points[j].x, points[j].y);
#else
        for (size_t j = 0; j < 10; j++) {
                uint16_t* rvalue = ble_pack_get_rvalue(t_pack_0->data);
                uint16_t* irvalue = ble_pack_get_irvalue(t_pack_0->data);
                g_output_stream_printf(G_OUTPUT_STREAM(fstream), &bytes_written, NULL, NULL, "T1: %hhu\nT2: %hhu\nRed value: %hu %hu %hu %hu %hu %hu %hu %hu %hu %hu\nIR Value: %hu %hu %hu %hu %hu %hu %hu %hu %hu %hu\nBeat average: %d\nTime: %ld\n\n", 
                ble_pack_get_t1(t_pack_0->data),
                ble_pack_get_t2(t_pack_0->data),
                rvalue[0], rvalue[1], rvalue[2], rvalue[3], rvalue[4], rvalue[5], rvalue[6], rvalue[7], rvalue[8], rvalue[9],
                irvalue[0], irvalue[1], irvalue[2], irvalue[3], irvalue[4], irvalue[5], irvalue[6], irvalue[7], irvalue[8], irvalue[9],
                ble_pack_get_beat(t_pack_0->data),
                t_pack_0->time
                );
        }
#endif
        if (t_pack_0->plotWritten == false)
                t_pack_0->fileWritten = true;
        else
        {
                simpleble_free(t_pack_0->data);
                g_free(t_pack_0);
        }


}

void gui_chart_plot_thread(gpointer data)
{

        t_pack *t_pack_0 = (t_pack*) data;
        point_t points[10];
//        g_mutex_lock(&producer_mutex);
        pack_to_point(points, _starting_time, t_pack_0);
        for (size_t j = 0; j < 10; j++)
        {
                gtk_chart_plot_point(chart, points[j].x, points[j].y);
                g_print("%.2f  ::  %.2f\n", points[j].x, points[j].y);
        }
        if (points[0].x > 10) {
                // [TODO]: Clear the existing chart
        }

        if (t_pack_0->fileWritten == false)
                t_pack_0->plotWritten = true;
        else {
                simpleble_free(t_pack_0->data);
                g_free(data);
        }

//        g_mutex_unlock(&producer_mutex);
}

gpointer _producer_function(gpointer data)
{
        GtkWindow *window = (GtkWindow*)data;
        gint adapter_index = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(window), "adapter_index"));
        simpleble_peripheral_t *main_peripheral = (simpleble_peripheral_t*)g_object_get_data(G_OBJECT(window), "main_peripheral");

        double x = 0;
        double y;
        point_t point;

        simpleble_service_t service;
        int service_set = false;
        int char_set = false;
        size_t service_index, characteristic_index, services_count;
        simpleble_adapter_t adapter = simpleble_adapter_get_handle(adapter_index);
        simpleble_err_t err_code = simpleble_peripheral_connect(main_peripheral[0]);

        if (err_code != SIMPLEBLE_SUCCESS) {
                _debug_print("Plotting peripheral connection failed");
                exit(1);
        } else {
                _debug_print("Plotting peripheral connection success");
        }

        services_count = simpleble_peripheral_services_count(main_peripheral[0]);

        for (size_t i = 0; i < services_count; i++)
        {
                if (service_set && char_set)
                        break;
                err_code = simpleble_peripheral_services_get(main_peripheral[0], i, &service);
                if (err_code != SIMPLEBLE_SUCCESS)
                        exit(1);
                if (g_strcmp0(service.uuid.value, DEFAULT_SERVICE_UUID) == 0)
                {
                        service_index = i;
                        for (size_t j = 0; j < service.characteristic_count; j++)
                        {
                                if (g_strcmp0(service.uuid.value, DEFAULT_CHARACTERISTIC_UUID) == 0)
                                {
                                        characteristic_index = j;
                                        service_set = true;
                                        char_set = true;
                                }
                        }
                }
        }
//        g_mutex_lock(&producer_mutex);
        ble_pack_t pack;
        size_t pack_len;
        int hasFirstTime = false;

        GThreadPool *thread_plot = g_thread_pool_new(
                gui_chart_plot_thread,
                chart,
                4,
                false,
                NULL);
        GThreadPool *thread_write = g_thread_pool_new(
                data_writing,
                fstream,
                4,
                false,
                NULL
        );
        g_thread_pool_set_sort_function(thread_plot, _data_compare_func, NULL);
        g_thread_pool_set_sort_function(thread_write, _data_compare_func, NULL);
        g_mutex_init(&producer_mutex);
        GFile* file = g_file_new_for_path(DEFAULT_PATH);
         fstream = g_file_append_to(file, G_FILE_CREATE_REPLACE_DESTINATION, NULL, NULL);
        size_t j = 0;
        ble_time_t time_0 = 0;
        while (true) {
                err_code = simpleble_peripheral_read(main_peripheral[0], 
                service.uuid, 
                service.characteristics[characteristic_index].uuid, 
                &pack, 
                &pack_len);
                if (pack_len == 0)
                {
                        g_usleep((1000000/120)/10);
                        _debug_print("Sleep");
                        continue;
                }
                time_0 += 83300;
                if (err_code != SIMPLEBLE_SUCCESS)
                        exit(12);
                if (hasFirstTime == false) {
                        hasFirstTime = true;
                        _starting_time = time_0;
                }

                t_pack *t_pack_1 = (t_pack*)g_malloc0(sizeof(*t_pack_1));
                t_pack_1->data = (ble_pack_inf*)pack;
                t_pack_1->time = time_0;
                t_pack_1->fileWritten = false;
                t_pack_1->plotWritten = false;

                if (isPlotting == true)
                        g_thread_pool_push(thread_plot, t_pack_1, NULL);
                if (isWriting == true)
                        g_thread_pool_push(thread_write, t_pack_1, NULL);
//                simpleble_free(pack);
                j++;
        }

        g_output_stream_close(G_OUTPUT_STREAM(fstream), NULL, NULL);
        return NULL;
}

void _plotting_button_clicked(GtkButton *button, gpointer data)
{
//        g_mutex_lock(&producer_mutex);
        isPlotting = true;
        if (initiatedDataReceiving == false)
        {
                initiatedDataReceiving = true;
                GThread *_producer_thread = g_thread_new("producer_thread", _producer_function, data);
        }
}

void _start_button_clicked(GtkButton *button, gpointer data)
{
        isWriting = true;
        if (initiatedDataReceiving == false)
        {
                initiatedDataReceiving = true;
                GThread *_producer_thread = g_thread_new("producer_thread", _producer_function, data);
        }
}

void _stop_button_clicked(GtkButton *button, gpointer data)
{
        // Stop _producer_function()

}

void _new_record_button_clicked(GtkButton *button, gpointer data)
{
        // Save the temporary file as a new file with additional metadata
}

void chart_register_starting(GtkChart *chart, ble_time_t current_time)
{
        g_object_set_data(G_OBJECT(chart), "beginning_time", GINT_TO_POINTER(current_time));
}

void load_plotting(GtkBuilder *builder, GtkWindow *window)
{
        GObject *bled   = gtk_builder_get_object(builder, "bluetooth_dialog");
        GObject *plot_button = gtk_builder_get_object(builder, "button_plot");

        GObject *start_button = gtk_builder_get_object(builder, "button_start");
        GObject *new_record_button = gtk_builder_get_object(builder, "button_newrecord");
        GObject *stop_button = gtk_builder_get_object(builder, "button_stop");
        GObject *plot_box = gtk_builder_get_object(builder, "plot_box");

        chart = GTK_CHART(gtk_chart_new());
        gtk_chart_set_type(chart, GTK_CHART_TYPE_LINEAR_AUTOSCALE);
        gtk_chart_set_title(chart, "PPG Signal");
        gtk_chart_set_label(chart, "Random label");
        gtk_chart_set_x_label(chart, "Time [s]");
        gtk_chart_set_y_label(chart, "PPG signal");
        gtk_chart_set_x_interval(chart, 10.0);
        gtk_chart_set_y_upper(chart, 5000);
        gtk_chart_set_width(chart, 1000);
        gtk_widget_set_hexpand(GTK_WIDGET(chart), true);
        gtk_widget_set_vexpand(GTK_WIDGET(chart), true);
        gtk_widget_set_hexpand(GTK_WIDGET(plot_box), true);
        gtk_widget_set_vexpand(GTK_WIDGET(plot_box), true);
        gtk_box_append(GTK_BOX(plot_box), GTK_WIDGET(chart));
        g_object_set_data(G_OBJECT(window), "chart", chart);
        g_signal_connect(plot_button, "clicked", G_CALLBACK(_plotting_button_clicked), window);
        g_signal_connect(start_button, "clicked", G_CALLBACK(_start_button_clicked), window);
        g_signal_connect(new_record_button, "clicked", G_CALLBACK(_new_record_button_clicked), window);
        g_signal_connect(stop_button, "clicked", G_CALLBACK(_stop_button_clicked), window);

//        g_mutex_unlock(&producer_mutex);
}