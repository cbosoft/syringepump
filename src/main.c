#include <gtk/gtk.h>
#include <pthread.h>

static int THREAD_STOP = 0;

void *f(void * void_lbl)
{

  GObject *lbl = (GObject *)void_lbl;

  for (int i = 0; !THREAD_STOP; i++) {
    char s[100] = {0};
    sprintf(s, "time since start %ds", i);
    gtk_label_set_text(GTK_LABEL(lbl), s);
    sleep(1);
  }

  return NULL;
}

void close_threads_and_quit(void)
{

  gtk_main_quit();

  THREAD_STOP = 1;

}


int main (int argc, char **argv)
{
  GtkBuilder *builder;
  GObject *window;
  GObject *label;
  GError *error = NULL;

  gtk_init(&argc, &argv);

  builder = gtk_builder_new();
  if (gtk_builder_add_from_file(builder, "gui/main.ui", &error) == 0) {
    g_printerr("Error loading file: %s\n", error->message);
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
