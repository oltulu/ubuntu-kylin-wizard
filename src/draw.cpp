// -*- Mode: C++; indent-tabs-mode: nil; tab-width: 2 -*-
/*
 * Copyright (C) 2015, National University of Defense Technology(NUDT) & Kylin Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: handsome_feng <jianfengli@ubuntukylin.com>
 */

#include "draw.h"

#include "style.h"
#include "blur.h"

#include <cairo.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <glib/gi18n.h>
#include <pango/pango.h>
#include <math.h>

const gchar* title_1 = _("Quick access to applications");
const gchar* subtitle_1 = _("Launcher");
                         // Provides you with quick access to applications, workspaces, removable devices and the Recycle Bin.
const gchar* details_1 = _("Open and switch all kinds of applications conveniently, add/delete applications in Launcher according to user's habit in the meantime");

const gchar* title_2 = _("Quick intelligent search");
const gchar* subtitle_2 = _("Dash");
                         // Allows you to search for applications, files, music, and videos, and shows you items that you have used recently.
const gchar* details_2 = _("Provide overall quick intelligent search function, local and on-line resources can be reached by clicking Dash, including applications, files, music, videos and pictures etc.");

const gchar* title_3 = _("Browse and manage files");
const gchar* subtitle_3 = _("Nautilus");
                        // Allows to browse directories, preview files and launch applications associated with them. It is also responsible for handling the icons on the desktop.
const gchar* details_3 = _("Can brwose and organize files, manage local storage device, file server, files on-line, set up, delete, browse, copy, move files or directories by cliking Launcher");

const gchar* title_4 = _("Check and modify system configuration");
const gchar* subtitle_4 = _("Youker assistant");
const gchar* details_4 = _("System management and configuration tools.Youker assistant can clear system trash,custom system,check system information etc");

const gchar* title_5 = _("Frequently used tools configuration");
const gchar* subtitle_5 = _("Unity control center");
const gchar* details_5 = _("Integrate frequently used configure tools. set personal preference by control panel, such as hardware configuration(internet/keyboard/mouse) and system information etc");

const gchar* title_6 = _("Check system state");
const gchar* subtitle_6 = _("Indicator");
                         // A comprehensive set of indicators provide convenient and powerful access to application features and system facilities such as power, sound, messaging, and the current session.
const gchar* details_6 = _("In this area,you can check system information like volumn/internet/time, user's manual, set screen-lock and logout/restart/shundown system etc.");
static gboolean on_close_pressed(GtkWidget *widget, GdkEventButton *event, GtkWidget *win)
{
  if (event->button == 1)
  {
    gtk_widget_destroy(win);
    gtk_main_quit();
  }
  return FALSE;
}

static gboolean on_arrow_left_pressed(GtkWidget *widget, GdkEventButton *event, Draw *draw)
{
  if (event->button == 1 && draw->page() != 0)
  {
    draw->pre_page();
    gtk_widget_queue_draw(WID(draw->Builer(), WIDGET, "window"));
  }
  return FALSE;
}

static gboolean enter_left_box(GtkWidget *widget, GdkEventButton *event, GtkWidget *img)
{
  gtk_image_set_from_file(GTK_IMAGE(img), PKGDATADIR"/arrow_left_hover.png");
  return FALSE;
}

static gboolean leave_left_box(GtkWidget *widget, GdkEventButton *event, GtkWidget *img)
{
  gtk_image_set_from_file(GTK_IMAGE(img), PKGDATADIR"/arrow_left.png");
  return FALSE;
}

static gboolean on_arrow_right_pressed(GtkWidget *widget, GdkEventButton *event, Draw *draw)
{
  if (event->button == 1 && draw->page() != PAGES_NUM - 1)
  {
    draw->next_page();
    gtk_widget_queue_draw(WID(draw->Builer(), WIDGET, "window"));
  }
  return FALSE;
}

static gboolean enter_right_box(GtkWidget *widget, GdkEventButton *event, GtkWidget *img)
{
  gtk_image_set_from_file(GTK_IMAGE(img), PKGDATADIR"/arrow_right_hover.png");
  return FALSE;
}

static gboolean leave_right_box(GtkWidget *widget, GdkEventButton *event, GtkWidget *img)
{
  gtk_image_set_from_file(GTK_IMAGE(img), PKGDATADIR"/arrow_right.png");
  return FALSE;
}

static gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, Draw *draw)
{
  switch(event->keyval)
  {
  case GDK_KEY_Escape:
    {
      gtk_widget_destroy(WID(draw->Builer(), WIDGET, "window"));
      gtk_main_quit();
    }
    break;
  case GDK_KEY_Left:
    if (draw->page() != 0)
    {
      draw->pre_page();
      gtk_widget_queue_draw(WID(draw->Builer(), WIDGET, "window"));
    }
    break;
  case GDK_KEY_Right:
    if (draw->page() != PAGES_NUM - 1)
    {
      draw->next_page();
      gtk_widget_queue_draw(WID(draw->Builer(), WIDGET, "window"));
    }
    break;
  default:
    break;
  }
  return FALSE;
}

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, Draw* draw)
{
  draw->do_drawing(cr);

  return FALSE;
}

Draw::Draw()
  : builder_(gtk_builder_new_from_file(PKGDATADIR"/wizard.ui"))
  , style_(new Style())
  , primary_screen_(0)
  , page_num_(0)
{
  GError *error = NULL;

  window_ = WID(builder_, WIDGET, "window");
  Setup();

  fixed_ = WID(builder_, WIDGET, "fixed");

  /*  css   */
  GtkCssProvider *provider;
  GdkDisplay *display;
  GdkScreen *screen;

  /* Get the thumbnail from current screen.
  GdkPixbuf *image_buf = gdk_pixbuf_new_from_file_at_size(PKGDATADIR"/computer.png", 422, 334, &error);
  if (!image_buf) {
    std::cerr << "error message: " << error->message << std::endl;
  }
  GdkPixbuf *preview = gdk_pixbuf_copy (image_buf);

  GdkPixbuf *tmp = gdk_pixbuf_scale_simple(root_pixbuf_, 385, 230, GDK_INTERP_NEAREST);

  if (tmp)
  {
    gdk_pixbuf_composite(tmp, preview,
                         17, 22, 385, 230, 17, 22, 1,1,GDK_INTERP_NEAREST, 255);
    g_object_unref(tmp);
  }
  base_img_ = WID(builder_, WIDGET, "base_img");
  gtk_image_set_from_pixbuf(GTK_IMAGE(base_img_), preview);
  gtk_fixed_move(GTK_FIXED(fixed_), base_img_, style_->get_base_pos().x, style_->get_base_pos().y);
  */

  base_img_ = WID(builder_, WIDGET, "base_img");
  GdkPixbuf *base_pixbuf = gdk_pixbuf_new_from_file(PKGDATADIR"/computer.png", &error);
  gtk_image_set_from_pixbuf(GTK_IMAGE(base_img_), base_pixbuf);
  gtk_fixed_move(GTK_FIXED(fixed_), base_img_, style_->get_base_pos().x, style_->get_base_pos().y);

  thumbnail_ = WID(builder_, WIDGET, "thumbnail");
  gtk_image_set_from_file(GTK_IMAGE(thumbnail_), PKGDATADIR"/thumbnail_1.png");
  gtk_fixed_move(GTK_FIXED(fixed_), thumbnail_, style_->get_base_pos().x + 17, style_->get_base_pos().y + 24);

  grid_ = WID(builder_, WIDGET, "grid");
  gtk_fixed_move(GTK_FIXED(fixed_), grid_, style_->get_title_pos().x, style_->get_title_pos().y);

  title_ = WID(builder_, WIDGET, "title");
  gtk_label_set_text(GTK_LABEL(title_), title_1);
  gtk_widget_set_name(GTK_WIDGET(title_),"title");   // name this so we can apply css to it later

  subtitle_ = WID(builder_, WIDGET, "subtitle");
  gtk_label_set_text(GTK_LABEL(subtitle_), _(subtitle_1));
  gtk_widget_set_name(GTK_WIDGET(subtitle_), "subtitle");
  gtk_widget_set_size_request(subtitle_, -1, 50);

  details_ = WID(builder_, WIDGET, "details");
  gtk_widget_set_name(GTK_WIDGET(details_), "details");
  gtk_label_set_text(GTK_LABEL(details_), details_1);
  gtk_widget_set_size_request(details_, 0.8 * (style_->get_right_arrow_pos().x - style_->get_spot_pos().x), -1);
  gtk_label_set_line_wrap(GTK_LABEL(details_), TRUE);

  left_box_ = WID(builder_, WIDGET, "left_box");
  arrow_left_img_ = WID(builder_, WIDGET, "arrow_left_img");
  gtk_image_set_from_file(GTK_IMAGE(arrow_left_img_), PKGDATADIR"/arrow_left.png");
  gtk_fixed_move(GTK_FIXED(fixed_), left_box_, style_->get_left_arrow_pos().x, style_->get_left_arrow_pos().y);
  gtk_widget_hide(left_box_);

  right_box_ = WID(builder_, WIDGET, "right_box");
  arrow_right_img_ = WID(builder_, WIDGET, "arrow_right_img");
  gtk_image_set_from_file(GTK_IMAGE(arrow_right_img_), PKGDATADIR"/arrow_right.png");
  gtk_fixed_move(GTK_FIXED(fixed_), right_box_, style_->get_right_arrow_pos().x, style_->get_right_arrow_pos().y);

  close_button_ = WID(builder_, WIDGET, "close_button");
  gtk_button_set_label(GTK_BUTTON(close_button_), _("Login System"));
  gtk_widget_set_name(GTK_WIDGET(close_button_), "close_button");
  gtk_fixed_move(GTK_FIXED(fixed_), close_button_, style_->get_close_pos().x, style_->get_close_pos().y);

  page_ind_ = WID(builder_, WIDGET, "page_ind");
  gtk_image_set_from_file(GTK_IMAGE(page_ind_), PKGDATADIR"/step_1.png");
  gtk_fixed_move(GTK_FIXED(fixed_), page_ind_, style_->get_page_ind_pos().x, style_->get_page_ind_pos().y);

  provider = gtk_css_provider_new();
  display = gdk_display_get_default();
  screen = gdk_display_get_default_screen(display);
  gtk_style_context_add_provider_for_screen(screen,
                                            GTK_STYLE_PROVIDER(provider),
                                            GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

  const gchar* css_file = PKGDATADIR"/wizard.css";
  gtk_css_provider_load_from_path(provider,
                                  css_file,//g_filename_to_utf8(css_file, strlen(css_file), &bytes_read, &bytes_written, &error),
                                  NULL);
  g_object_unref(provider);

  g_signal_connect(G_OBJECT(window_), "draw",
      G_CALLBACK(on_draw_event), this);
  g_signal_connect(G_OBJECT(window_), "key-press-event",
      G_CALLBACK(on_key_press), this);

  g_signal_connect(G_OBJECT(close_button_), "button_press_event",
      G_CALLBACK(on_close_pressed), window_);

  g_signal_connect(G_OBJECT(left_box_), "button_press_event",
      G_CALLBACK(on_arrow_left_pressed), this);
  g_signal_connect(G_OBJECT(left_box_), "enter_notify_event",
      G_CALLBACK(enter_left_box), arrow_left_img_);
  g_signal_connect(G_OBJECT(left_box_), "leave_notify_event",
      G_CALLBACK(leave_left_box), arrow_left_img_);

  g_signal_connect(G_OBJECT(right_box_), "button_press_event",
      G_CALLBACK(on_arrow_right_pressed), this);
  g_signal_connect(G_OBJECT(right_box_), "enter_notify_event",
      G_CALLBACK(enter_right_box), arrow_right_img_);
  g_signal_connect(G_OBJECT(right_box_), "leave_notify_event",
      G_CALLBACK(leave_right_box), arrow_right_img_);

  root_pixbuf_ = gdk_pixbuf_get_from_window(gdk_get_default_root_window(), 0, 0, style_->get_screen_width(), style_->get_screen_height());

  gint screen_num = gdk_screen_get_n_monitors(gdk_screen_get_default());
  if (screen_num > 1)
    draw_other(screen_num);
}

void Draw::Setup()
{
  gtk_widget_set_app_paintable(window_, TRUE);
//  gtk_window_set_decorated(GTK_WINDOW(window_), FALSE);
  gtk_window_set_type_hint(GTK_WINDOW(window_), GDK_WINDOW_TYPE_HINT_DOCK);
  gtk_window_set_keep_above(GTK_WINDOW(window_), TRUE);
//  gtk_window_fullscreen(GTK_WINDOW(window_));

  GdkScreen *screen = gdk_screen_get_default();
  gtk_widget_set_visual(window_, gdk_screen_get_rgba_visual(screen));

  gtk_widget_set_size_request(GTK_WIDGET(window_), style_->get_screen_width(), style_->get_screen_height());
}

void Draw::draw_other(gint num)
{
  GtkWidget *root_fixed = WID(builder_, WIDGET, "fixed");
  GdkPixbuf *bg_pixbuf;
  GtkWidget *other_bg;

  for (int i = 0; i < num; i++)
  {
    if (i == primary_screen_)
      continue;

    GdkRectangle geo;
    gdk_screen_get_monitor_geometry(gdk_screen_get_default(), i, &geo);
    if (geo.x == 0 && geo.y == 0)
      continue;

    GtkWidget *button = gtk_button_new_with_label(_("Login System"));
    gtk_widget_set_size_request(button, 115, 35);
    g_signal_connect(G_OBJECT(button), "button_press_event", G_CALLBACK(on_close_pressed), window_);

    bg_pixbuf = gdk_pixbuf_new_from_file_at_scale((style_->get_background_url()).c_str(), geo.width, geo.height, FALSE, NULL);
    other_bg = gtk_image_new_from_pixbuf(bg_pixbuf);
    gtk_fixed_put(GTK_FIXED(root_fixed), other_bg, geo.x, geo.y);
    gtk_fixed_put(GTK_FIXED(root_fixed), button, geo.x + (geo.width - CLOSE_BUTTON_WIDTH)/2, geo.y + (geo.height - CLOSE_BUTTON_HEIGHT)/2);
  }
}

void Draw::Run()
{
  gtk_widget_show_all(window_);
  gtk_window_present(GTK_WINDOW(window_));
}

GtkBuilder* Draw::Builer()
{
  return builder_;
}

void Draw::do_drawing(cairo_t *cr)
{
  draw_background(cr);
  draw_page(cr);
}

void Draw::draw_background(cairo_t *cr)
{
  cairo_surface_t *bg_sur = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, style_->get_screen_width(), style_->get_screen_height());
  cairo_t *bg_cr = cairo_create(bg_sur);
  gdk_cairo_set_source_pixbuf(bg_cr, root_pixbuf_, 0, 0);
  cairo_paint(bg_cr);

  blur(bg_sur, 8);
  cairo_set_source_surface(cr, bg_sur, 0, 0);
  cairo_paint(cr);

  cairo_destroy(bg_cr);
  cairo_surface_destroy(bg_sur);
}

void clip_rec(cairo_t *cr, int x, int y, int width, int height)
{
  cairo_save(cr);
  cairo_rectangle(cr, x, y, width, height);
  cairo_clip(cr);
  cairo_new_path(cr);
  cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 0.0);
  cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
  cairo_paint(cr);
  cairo_restore(cr);
}

void Draw::draw_page(cairo_t *cr)
{
  gint x = style_->trans_area_[page_num_].x;
  gint y = style_->trans_area_[page_num_].y;
  gint width = style_->trans_area_[page_num_].width;
  gint height = style_->trans_area_[page_num_].height;
  gboolean launcher_at_bottom = true;
  if (!g_strcmp0(style_->get_launcher_position(), "Left"))
    launcher_at_bottom = false;

  clip_rec(cr, x, y, width, height);
  draw_polyline(cr, launcher_at_bottom);

  switch (page_num_) {
  case 0:
    gtk_label_set_text(GTK_LABEL(title_), title_1);
    gtk_label_set_text(GTK_LABEL(subtitle_),_(subtitle_1));
    gtk_label_set_text(GTK_LABEL(details_), details_1);
    gtk_image_set_from_file(GTK_IMAGE(page_ind_), PKGDATADIR"/step_1.png");
    gtk_image_set_from_file(GTK_IMAGE(thumbnail_), PKGDATADIR"/thumbnail_1.png");
    gtk_widget_hide(left_box_);
    break;
 case 1:
    gtk_label_set_text(GTK_LABEL(title_), title_2);
    gtk_label_set_text(GTK_LABEL(subtitle_), _(subtitle_2));
    gtk_label_set_text(GTK_LABEL(details_), details_2);
    gtk_image_set_from_file(GTK_IMAGE(page_ind_), PKGDATADIR"/step_2.png");
    gtk_image_set_from_file(GTK_IMAGE(thumbnail_), PKGDATADIR"/thumbnail_2.png");
    gtk_widget_show(left_box_);
    break;
  case 2:
    gtk_label_set_text(GTK_LABEL(title_), title_3);
    gtk_label_set_text(GTK_LABEL(subtitle_), _(subtitle_3));
    gtk_label_set_text(GTK_LABEL(details_), details_3);
    gtk_image_set_from_file(GTK_IMAGE(page_ind_), PKGDATADIR"/step_3.png");
    gtk_image_set_from_file(GTK_IMAGE(thumbnail_), PKGDATADIR"/thumbnail_3.png");
    break;
  case 3:
    gtk_label_set_text(GTK_LABEL(title_), title_4);
    gtk_label_set_text(GTK_LABEL(subtitle_), _(subtitle_4));
    gtk_label_set_text(GTK_LABEL(details_), details_4);
    gtk_image_set_from_file(GTK_IMAGE(page_ind_), PKGDATADIR"/step_4.png");
    gtk_image_set_from_file(GTK_IMAGE(thumbnail_), PKGDATADIR"/thumbnail_4.png");
    break;
  case 4:
    gtk_label_set_text(GTK_LABEL(title_), title_5);
    gtk_label_set_text(GTK_LABEL(subtitle_), _(subtitle_5));
    gtk_label_set_text(GTK_LABEL(details_), details_5);
    gtk_image_set_from_file(GTK_IMAGE(page_ind_), PKGDATADIR"/step_5.png");
    gtk_image_set_from_file(GTK_IMAGE(thumbnail_), PKGDATADIR"/thumbnail_5.png");
    gtk_widget_show(right_box_);
    break;
  case 5:
    gtk_label_set_text(GTK_LABEL(title_), title_6);
    gtk_label_set_text(GTK_LABEL(subtitle_), _(subtitle_6));
    gtk_label_set_text(GTK_LABEL(details_), details_6);
    gtk_image_set_from_file(GTK_IMAGE(page_ind_), PKGDATADIR"/step_6.png");
    gtk_image_set_from_file(GTK_IMAGE(thumbnail_), PKGDATADIR"/thumbnail_6.png");
    gtk_widget_hide(right_box_);
    break;
  default:
    break;
  }
}

void Draw::draw_polyline(cairo_t *cr, gboolean at_bottom)
{
  cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.9);
  cairo_set_line_width(cr, 2);

  Point p;
  p.x = style_->focus_pos_[page_num_].x;
  p.y = style_->focus_pos_[page_num_].y;

  cairo_move_to(cr, p.x, p.y);

  if (at_bottom)
  {
    gint inflexion_y = 0;
    if (page_num_ != PAGES_NUM - 1)
      inflexion_y = style_->get_inflexion_pos().y;
    else
      inflexion_y = (style_->get_spot_pos().y + style_->get_panel_height()) / 2;

    cairo_line_to(cr, p.x, inflexion_y);
    cairo_line_to(cr, style_->get_inflexion_pos().x, inflexion_y);
    cairo_line_to(cr, style_->get_spot_pos().x, style_->get_spot_pos().y);
    cairo_stroke(cr);
  }
  else
  {
    if (page_num_ == PAGES_NUM - 1)
    {
      cairo_line_to(cr, p.x, style_->get_inflexion_pos().y);
      cairo_line_to(cr, style_->get_spot_pos().x, style_->get_inflexion_pos().y);
    }
    else
    {
      cairo_line_to(cr, style_->get_inflexion_pos().x, p.y);
      cairo_line_to(cr, style_->get_inflexion_pos().x, style_->get_spot_pos().y);
    }
    cairo_line_to(cr, style_->get_spot_pos().x, style_->get_spot_pos().y);
    cairo_stroke(cr);
  }
  draw_ring(cr,  p.x, p.y);
  draw_ring(cr, style_->get_spot_pos().x, style_->get_spot_pos().y);
}

void Draw::draw_ring(cairo_t *cr, gint x, gint y)
{
  cairo_set_source_rgba(cr, 1., 1., 1., 0.1);
  cairo_arc(cr, x, y, 10, 0., 2 * M_PI);
  cairo_fill(cr);

  cairo_set_source_rgba(cr, 1., 1., 1., 0.4);
  cairo_arc(cr, x, y, 7, 0., 2 * M_PI);
  cairo_fill(cr);

  cairo_set_source_rgba(cr, 1., 1., 1., 0.9);
  cairo_arc(cr, x, y, 4, 0., 2 * M_PI);
  cairo_fill(cr);
}

gint Draw::page()
{
  return page_num_;
}

void Draw::next_page()
{
  if (page_num_ != PAGES_NUM - 1)
    page_num_++;
}

void Draw::pre_page()
{
  if (page_num_ != 0)
    page_num_--;
}
