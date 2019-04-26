#include <gtk/gtk.h>

static void hello(GtkWidget *widget, gpointer data) {
    g_print("Execute text\n");
}

static gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer data) {
    g_print("delete event occurred\n");

    // return false to delete main window
    return false;
}

static void destroy(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}

int main(int argc, char *argv[]) {

    gtk_init(&argc, &argv);

    GdkScreen *screen = gdk_screen_get_default();
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_resource(provider, "/application/pinponi/style.css");
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider),
                                              GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 700);
    gtk_window_set_focus_on_map(GTK_WINDOW(window), TRUE);

    g_signal_connect (window, "delete-event", G_CALLBACK(delete_event), NULL);
    g_signal_connect (window, "destroy", G_CALLBACK(destroy), NULL);

    gtk_container_set_border_width(GTK_CONTAINER (window), 10);

    GtkWidget *box = gtk_vbox_new(false, 10);

    GtkTextBuffer *buffer = gtk_text_buffer_new(nullptr);
    GtkWidget *textView = gtk_text_view_new_with_buffer(buffer);
    GtkTextIter iter;
    gtk_text_buffer_get_iter_at_offset(buffer, &iter, 0);
    gtk_text_buffer_insert(buffer, &iter, "書いたコードの結果はここで表示します。\n", -1);
    gtk_box_pack_start(GTK_BOX (box), textView, true, true, 0);
    gtk_widget_show(textView);

    GtkWidget *text = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX (box), text, false, true, 0);
    gtk_widget_show(text);

    GtkWidget *button = gtk_button_new_with_label("実行");
    gtk_box_pack_start(GTK_BOX (box), button, false, true, 0);
    gtk_widget_show(button);

    g_signal_connect (button, "clicked", G_CALLBACK(hello), NULL);
//    g_signal_connect_swapped (button, "clicked", G_CALLBACK(gtk_widget_destroy), window);

    gtk_container_add(GTK_CONTAINER (window), box);
    gtk_widget_show(box);

    gtk_widget_show(window);
    gtk_main();

    return 0;
}
