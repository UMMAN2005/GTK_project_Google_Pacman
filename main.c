#include "app.h"

GtkApplication* app;

int main(int argc, char **argv) {
    int status;

    // Create a new GTK application
    app = gtk_application_new("com.example.google_pacman", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    // Run the application
    status = g_application_run(G_APPLICATION(app), argc, argv);

    // Release resources
    g_object_unref(app);

    return status;
}
