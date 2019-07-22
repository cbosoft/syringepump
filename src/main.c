#include <gtk/gtk.h>
#include <pthread.h>

#include "error.h"

int main (int argc, char **argv)
{
  GtkBuilder *builder;
  GError *error = NULL;

  gtk_init(&argc, &argv);

  builder = gtk_builder_new();
  if (gtk_builder_add_from_file(builder, "gui/main.ui", &error) == 0) {
    timestamp("Error loading file: %s\n", error->message);
    g_clear_error(&error);
    return 1;
  }

  window = gtk_builder_get_object(builder, "window");
  g_signal_connect(window, "destroy", G_CALLBACK(close_threads_and_quit), NULL);

  label = gtk_builder_get_object(builder, "label");

  pthread_t thread;

  pthread_create(&thread, NULL, f, label);

  gtk_main();

  pthread_join(thread, NULL);

  return 0;
}
