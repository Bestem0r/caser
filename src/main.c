#include <adwaita.h>

static GtkWidget *text_view_input;
static AdwToastOverlay *global_toast_overlay;

static void
show_toast (const char *message)
{
  AdwToast *toast = adw_toast_new (message);
  adw_toast_set_timeout (toast, 2);
  adw_toast_overlay_add_toast (global_toast_overlay, toast);
}

static char *
get_buffer_text ()
{
  GtkTextIter start, end;
  GtkTextBuffer *buffer
      = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text_view_input));
  gtk_text_buffer_get_bounds (buffer, &start, &end);
  return gtk_text_buffer_get_text (buffer, &start, &end, FALSE);
}

static void
set_buffer_text (const char *text)
{
  gtk_text_buffer_set_text (
      gtk_text_view_get_buffer (GTK_TEXT_VIEW (text_view_input)), text, -1);
}

typedef char *(*ConvertFunc) (const char *, gssize);

static void
convert_text (const ConvertFunc convert_func, const char *message)
{
  char *text = get_buffer_text ();
  char *converted = convert_func (text, -1);

  set_buffer_text (converted);
  show_toast (message);

  g_free (text);
  g_free (converted);
}

static void
convert_to_uppercase ()
{
  convert_text (g_utf8_strup, "Content converted to uppercase");
}

static void
convert_to_lowercase ()
{
  convert_text (g_utf8_strdown, "Content converted to lowercase");
}

static char *
invert_case (const char *text)
{
  GString *result = g_string_new ("");

  for (const char *p = text; *p != '\0'; p = g_utf8_next_char (p))
    {
      const gunichar ch = g_utf8_get_char (p);
      g_string_append_unichar (
          result, g_unichar_islower (ch)   ? g_unichar_toupper (ch)
                  : g_unichar_isupper (ch) ? g_unichar_tolower (ch)
                                           : ch);
    }

  char *converted = g_strdup (result->str);
  g_string_free (result, TRUE);
  return converted;
}

static void
convert_to_invert ()
{
  char *text = get_buffer_text ();
  char *converted = invert_case (text);

  set_buffer_text (converted);
  show_toast ("Content converted to inverted case");

  g_free (text);
  g_free (converted);
}

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

  GtkWidget *menu_button = gtk_menu_button_new ();
  gtk_menu_button_set_label (GTK_MENU_BUTTON (menu_button), "Actions");

  GMenu *menu = g_menu_new ();
  g_menu_append (menu, "Uppercase", "app.uppercase");
  g_menu_append (menu, "Lowercase", "app.lowercase");
  g_menu_append (menu, "Invert", "app.invert");

  gtk_menu_button_set_popover (
      GTK_MENU_BUTTON (menu_button),
      gtk_popover_menu_new_from_model (G_MENU_MODEL (menu)));

  adw_header_bar_set_title_widget (ADW_HEADER_BAR (header_bar), menu_button);
  gtk_box_append (GTK_BOX (content_area), header_bar);

  GtkWidget *scrolled_window = gtk_scrolled_window_new ();
  gtk_widget_set_vexpand (scrolled_window, TRUE);

  text_view_input = gtk_text_view_new ();
  gtk_text_view_set_left_margin (GTK_TEXT_VIEW (text_view_input), 10);
  gtk_text_view_set_right_margin (GTK_TEXT_VIEW (text_view_input), 10);
  gtk_text_view_set_top_margin (GTK_TEXT_VIEW (text_view_input), 10);
  gtk_text_view_set_bottom_margin (GTK_TEXT_VIEW (text_view_input), 10);
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (text_view_input),
                               GTK_WRAP_WORD_CHAR);

  gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scrolled_window),
                                 text_view_input);
  gtk_box_append (GTK_BOX (content_area), scrolled_window);

  GSimpleAction *uppercase_action = g_simple_action_new ("uppercase", NULL);
  GSimpleAction *lowercase_action = g_simple_action_new ("lowercase", NULL);
  GSimpleAction *invert_action = g_simple_action_new ("invert", NULL);

  g_signal_connect (uppercase_action, "activate",
                    G_CALLBACK (convert_to_uppercase), NULL);
  g_signal_connect (lowercase_action, "activate",
                    G_CALLBACK (convert_to_lowercase), NULL);
  g_signal_connect (invert_action, "activate", G_CALLBACK (convert_to_invert),
                    NULL);

  g_action_map_add_action (G_ACTION_MAP (app), G_ACTION (uppercase_action));
  g_action_map_add_action (G_ACTION_MAP (app), G_ACTION (lowercase_action));
  g_action_map_add_action (G_ACTION_MAP (app), G_ACTION (invert_action));

  adw_application_window_set_content (ADW_APPLICATION_WINDOW (window),
                                      toast_overlay);

  gtk_window_present (GTK_WINDOW (window));

  gtk_widget_grab_focus (text_view_input);
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