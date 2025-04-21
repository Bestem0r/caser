#include <adwaita.h>

static AdwToastOverlay *global_toast_overlay;

static void
on_activate (GtkApplication *app)
{
  GtkWidget *window = GTK_WIDGET (adw_application_window_new (app));
  gtk_window_set_title (GTK_WINDOW (window), "Caser");
  gtk_window_set_default_size (GTK_WINDOW (window), 600, 400);
  GtkWidget *header_bar = adw_header_bar_new ();
  gtk_widget_add_css_class (header_bar, "flat");

  GtkWidget *content_area = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  GtkWidget *toast_overlay = adw_toast_overlay_new ();
  global_toast_overlay = ADW_TOAST_OVERLAY (toast_overlay);
  adw_toast_overlay_set_child (ADW_TOAST_OVERLAY (toast_overlay),
                               content_area);

  gtk_box_append (GTK_BOX (content_area), header_bar);

  adw_application_window_set_content (ADW_APPLICATION_WINDOW (window),
                                      toast_overlay);

  gtk_window_present (GTK_WINDOW (window));
}

int
main (const int argc, char *argv[])
{
  adw_init ();
  AdwApplication *app = adw_application_new ("net.bestemor.caser",
                                             G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (on_activate), NULL);
  const int status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);
  return status;
}