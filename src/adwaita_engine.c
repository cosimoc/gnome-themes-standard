/* Adwaita - a GTK+ engine
 *
 * Copyright (C) 2011 Carlos Garnacho <carlosg@gnome.org>
 * Copyright (C) 2011 Red Hat, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Authors: Carlos Garnacho <carlosg@gnome.org>
 *          Cosimo Cecchi <cosimoc@gnome.org>
 *
 * Project contact: <gnome-themes-list@gnome.org>
 */

#include <gtk/gtk.h>
#include <gmodule.h>
#include <math.h>
#include <cairo-gobject.h>

#include "adwaita_utils.h"

#define ADWAITA_NAMESPACE "adwaita"

typedef struct _AdwaitaEngine AdwaitaEngine;
typedef struct _AdwaitaEngineClass AdwaitaEngineClass;

struct _AdwaitaEngine
{
  GtkThemingEngine parent_object;
};

struct _AdwaitaEngineClass
{
  GtkThemingEngineClass parent_class;
};

#define ADWAITA_TYPE_ENGINE		 (adwaita_engine_get_type ())
#define ADWAITA_ENGINE(object)		 (G_TYPE_CHECK_INSTANCE_CAST ((object), ADWAITA_TYPE_ENGINE, AdwaitaEngine))
#define ADWAITA_ENGINE_CLASS(klass)	 (G_TYPE_CHECK_CLASS_CAST ((klass), ADWAITA_TYPE_ENGINE, AdwaitaEngineClass))
#define ADWAITA_IS_ENGINE(object)	 (G_TYPE_CHECK_INSTANCE_TYPE ((object), ADWAITA_TYPE_ENGINE))
#define ADWAITA_IS_ENGINE_CLASS(klass)	 (G_TYPE_CHECK_CLASS_TYPE ((klass), ADWAITA_TYPE_ENGINE))
#define ADWAITA_ENGINE_GET_CLASS(obj)	 (G_TYPE_INSTANCE_GET_CLASS ((obj), ADWAITA_TYPE_ENGINE, AdwaitaEngineClass))

GType adwaita_engine_get_type	    (void) G_GNUC_CONST;
void  adwaita_engine_register_types (GTypeModule *module);

G_DEFINE_DYNAMIC_TYPE (AdwaitaEngine, adwaita_engine, GTK_TYPE_THEMING_ENGINE)

void
adwaita_engine_register_types (GTypeModule *module)
{
  adwaita_engine_register_type (module);
}

static void
adwaita_engine_init (AdwaitaEngine *self)
{
}

static void
adwaita_engine_render_arrow (GtkThemingEngine *engine,
                             cairo_t          *cr,
                             gdouble           angle,
                             gdouble           x,
                             gdouble           y,
                             gdouble           size)
{
  double line_width;
  GtkStateFlags state;
  GdkRGBA color;

  cairo_save (cr);

  line_width = size / 3.0 / sqrt (2);
  cairo_set_line_width (cr, line_width);
  cairo_set_line_join (cr, CAIRO_LINE_JOIN_ROUND);
  cairo_set_line_cap (cr, CAIRO_LINE_CAP_ROUND);

  cairo_translate (cr, x + size / 2.0, y + size / 2.0);
  cairo_rotate (cr, angle - G_PI_2);
  cairo_translate (cr, size / 4.0, 0);
 
  cairo_scale (cr,
               (size / (size + line_width)),
               (size / (size + line_width)));

  cairo_move_to (cr, -size / 2.0, -size / 2.0);
  cairo_rel_line_to (cr, size / 2.0, size / 2.0);
  cairo_rel_line_to (cr, - size / 2.0, size / 2.0);

  state = gtk_theming_engine_get_state (engine);
  gtk_theming_engine_get_color (engine, state, &color);

  if (gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_MENUITEM) &&
      !(state & GTK_STATE_FLAG_INSENSITIVE) && !(state & GTK_STATE_FLAG_PRELIGHT))
    {
      GdkRGBA *arrow_color;

      gtk_theming_engine_get (engine, state,
                              "-adwaita-menuitem-arrow-color", &arrow_color,
                              NULL);

      if (arrow_color != NULL)
        color = *arrow_color;

      gdk_rgba_free (arrow_color);
    }

  gdk_cairo_set_source_rgba (cr, &color);
  cairo_stroke (cr);

  cairo_restore (cr);
}

static void
adwaita_engine_render_focus (GtkThemingEngine *engine,
                             cairo_t          *cr,
                             gdouble           x,
                             gdouble           y,
                             gdouble           width,
                             gdouble           height)
{
  GdkRGBA *fill_color, *border_color = NULL;
  cairo_pattern_t *pattern = NULL;
  GtkStateFlags state;
  gint line_width, focus_pad;
  gint border_radius;
  gboolean use_dashes;
  double dashes[2] = { 2.0, 0.2 };
  const GtkWidgetPath *path;

  path = gtk_theming_engine_get_path (engine);
  state = gtk_theming_engine_get_state (engine);
  gtk_theming_engine_get (engine, state,
                          "-adwaita-focus-border-color", &border_color,
                          "-adwaita-focus-fill-color", &fill_color,
                          "-adwaita-focus-border-radius", &border_radius,
                          "-adwaita-focus-border-gradient", &pattern,
                          "-adwaita-focus-border-dashes", &use_dashes,
                          NULL);

  gtk_theming_engine_get_style (engine,
                                "focus-line-width", &line_width,
                                "focus-padding", &focus_pad,
                                NULL);

  /* as we render the tab smaller than the whole allocation, we need
   * to recenter and resize the focus on the tab.
   */
  if (gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_NOTEBOOK) &&
      gtk_theming_engine_has_region (engine, GTK_STYLE_REGION_TAB, NULL))
    {
      y += 3.0;
      height -= 3.0;
    }

  /* the treeview rows don't change allocation when modifying focus-padding,
   * so we have to move the focus ring inside the allocated area manually.
   */
  if (gtk_widget_path_is_type (path, GTK_TYPE_TREE_VIEW))
    {
      x += focus_pad;
      y += focus_pad;
      width -= 2 * focus_pad;
      height -= 2 * focus_pad;
    }

  cairo_save (cr);
  cairo_set_line_width (cr, line_width);

  if (line_width > 1)
    _cairo_round_rectangle_sides (cr, border_radius,
                                  x, y, width, height,
                                  SIDE_ALL, GTK_JUNCTION_NONE);
  else
    _cairo_round_rectangle_sides (cr, border_radius,
                                  x + 0.5, y + 0.5,
                                  width - 1, height - 1,
                                  SIDE_ALL, GTK_JUNCTION_NONE);

  /* if we have a fill color, draw the fill */
  if (fill_color != NULL)
    {
      gdk_cairo_set_source_rgba (cr, fill_color);
      cairo_fill_preserve (cr);
    }

  if (use_dashes)
    cairo_set_dash (cr, dashes, 1, 0.0);

  /* if we have a gradient, draw the gradient, otherwise
   * draw the line if we have a color for it.
   */
  if (pattern != NULL)
    {
      style_pattern_set_matrix (pattern, width, height, FALSE);
      cairo_set_source (cr, pattern);
    }
  else if (border_color != NULL)
    {
      gdk_cairo_set_source_rgba (cr, border_color);
    }

  cairo_stroke (cr);
  cairo_restore (cr);

  if (pattern != NULL)
    cairo_pattern_destroy (pattern);

  if (border_color != NULL)
    gdk_rgba_free (border_color);

  if (fill_color != NULL)
    gdk_rgba_free (fill_color);
}

static void
adwaita_engine_render_check (GtkThemingEngine *engine,
                             cairo_t          *cr,
                             gdouble           x,
                             gdouble           y,
                             gdouble           width,
                             gdouble           height)
{
  GTK_THEMING_ENGINE_CLASS (adwaita_engine_parent_class)->render_background
    (engine, cr, x, y, width, height);
}

static void
adwaita_engine_render_option (GtkThemingEngine *engine,
                              cairo_t          *cr,
                              gdouble           x,
                              gdouble           y,
                              gdouble           width,
                              gdouble           height)
{
  GTK_THEMING_ENGINE_CLASS (adwaita_engine_parent_class)->render_background
      (engine, cr, x, y, width, height);
}

static void
draw_tab_arcs (cairo_t *cr,
               gdouble curve_width,
               gdouble x,
               gdouble y,
               gdouble width,
               gdouble height)
{
  cairo_arc (cr, 
             curve_width, 6.0,
             2.5,
             G_PI, G_PI + G_PI_2);

  cairo_arc (cr,
             width - curve_width, 6.0,
             2.5,
             G_PI + G_PI_2, 2 * G_PI);
}

static void
draw_tab_shape_active (cairo_t *cr,
                       gdouble curve_width,
                       gdouble x,
                       gdouble y,
                       gdouble width,
                       gdouble height)
{
  cairo_move_to (cr, 0, height);

  draw_tab_arcs (cr, curve_width, x, y, width, height);

  cairo_line_to (cr, width, height);
}

static void
render_notebook_extension (GtkThemingEngine *engine,
                           cairo_t          *cr,
                           gdouble           x,
                           gdouble           y,
                           gdouble           width,
                           gdouble           height,
                           GtkPositionType   gap_side)
{
  gint tab_curvature;
  GdkRGBA *color, border_color, background_color;
  GtkStateFlags state;
  gdouble angle = 0;
  cairo_pattern_t *pattern = NULL, *background_pattern = NULL;
  cairo_matrix_t matrix;

  gtk_theming_engine_get_style (engine,
                                "tab-curvature", &tab_curvature,
                                NULL);
  state = gtk_theming_engine_get_state (engine);
  gtk_theming_engine_get_background_color (engine, state, &background_color);
  gtk_theming_engine_get_border_color (engine, state, &border_color);
  gtk_theming_engine_get (engine, state,
                          "-adwaita-selected-tab-color", &color,
                          "-adwaita-border-gradient", &pattern,
                          "background-image", &background_pattern,
                          NULL);

  cairo_save (cr);
  cairo_set_line_width (cr, 1.0);

  if (gap_side == GTK_POS_TOP)
    {
      angle = G_PI;
      cairo_translate (cr, width, height);
    }

  if (gap_side == GTK_POS_BOTTOM)
    cairo_translate (cr,
                     x + 0.5,
                     (state & GTK_STATE_FLAG_ACTIVE) ?
                     y + 1.0 : y);
  else if (gap_side == GTK_POS_TOP)
    cairo_translate (cr,
                     x - 0.5,
                     (state & GTK_STATE_FLAG_ACTIVE) ?
                     y - 1.0 : y);

  cairo_rotate (cr, angle);

  width -= 1.0;
  draw_tab_shape_active (cr, tab_curvature, 0, 0, width, height);

  if (background_pattern != NULL)
    {
      cairo_matrix_init_scale (&matrix,
                               1. / width,
                               1. / height);
      cairo_pattern_set_matrix (background_pattern, &matrix);
      cairo_set_source (cr, background_pattern);
    }
  else
    {
      gdk_cairo_set_source_rgba (cr, &background_color);
    }

  cairo_fill (cr);

  if (state & GTK_STATE_FLAG_ACTIVE)
    {
      draw_tab_shape_active (cr, tab_curvature, 0, 0, width, 6.0);
      gdk_cairo_set_source_rgba (cr, color);
      cairo_fill (cr);
    }

  draw_tab_shape_active (cr, tab_curvature, 0, 0, width, height);

  if (state & GTK_STATE_FLAG_ACTIVE)
    {
      style_pattern_set_matrix (pattern, width, height - 6.0, FALSE);
      cairo_set_source (cr, pattern);
    }
  else
    {
      gdk_cairo_set_source_rgba (cr, &border_color);
    }

  cairo_stroke (cr);

  gdk_rgba_free (color);

  if (pattern != NULL)
    cairo_pattern_destroy (pattern);

  if (background_pattern != NULL)
    cairo_pattern_destroy (background_pattern);

  cairo_restore (cr);
}

static void
adwaita_engine_render_extension (GtkThemingEngine *engine,
                                 cairo_t          *cr,
                                 gdouble           x,
                                 gdouble           y,
                                 gdouble           width,
                                 gdouble           height,
                                 GtkPositionType   gap_side)
{
  GtkStateFlags state;

  if (gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_NOTEBOOK) &&
      ((gap_side == GTK_POS_TOP) || (gap_side == GTK_POS_BOTTOM)))
    {
      render_notebook_extension (engine, cr, x, y, width, height, gap_side);
      return;
    }

  GTK_THEMING_ENGINE_CLASS (adwaita_engine_parent_class)->render_extension
    (engine, cr,
     x, y, width, height,
     gap_side);

  state = gtk_theming_engine_get_state (engine);

  if (state & GTK_STATE_FLAG_ACTIVE)
    {
      GdkRGBA *fill;

      gtk_theming_engine_get (engine, state,
                              "-adwaita-selected-tab-color", &fill,
                              NULL);

      switch (gap_side)
        {
        case GTK_POS_BOTTOM:
          cairo_rectangle (cr,
                           x + 1, y + 1,
                           width - 2, 3);
          break;
        case GTK_POS_TOP:
          cairo_rectangle (cr,
                           x + 1, y + height - 4,
                           width - 2, 3);
          break;
        case GTK_POS_RIGHT:
          cairo_rectangle (cr,
                           x + 1, y + 1,
                           3, height - 2);
          break;
        case GTK_POS_LEFT:
          cairo_rectangle (cr,
                           x + width - 4, y + 1,
                           3, height - 2);
          break;
        }

      gdk_cairo_set_source_rgba (cr, fill);
      cairo_fill (cr);

      gdk_rgba_free (fill);
    }
}

static void
adwaita_engine_render_frame (GtkThemingEngine *engine,
                             cairo_t          *cr,
                             gdouble           x,
                             gdouble           y,
                             gdouble           width,
                             gdouble           height)
{
  adwaita_trim_allocation_for_scale (engine,
                                     &x, &y,
                                     &width, &height);

  GTK_THEMING_ENGINE_CLASS (adwaita_engine_parent_class)->render_frame
    (engine, cr, x, y,
     width, height);
}

static void
adwaita_engine_render_background (GtkThemingEngine *engine,
                                  cairo_t          *cr,
                                  gdouble           x,
                                  gdouble           y,
                                  gdouble           width,
                                  gdouble           height)
{
  adwaita_trim_allocation_for_scale (engine,
                                     &x, &y,
                                     &width, &height);

  GTK_THEMING_ENGINE_CLASS (adwaita_engine_parent_class)->render_background
    (engine, cr, x, y,
     width, height);
}

static void
adwaita_engine_render_expander (GtkThemingEngine *engine,
                                cairo_t          *cr,
                                gdouble           x,
                                gdouble           y,
                                gdouble           width,
                                gdouble           height)
{
  GdkRGBA fg;
  GtkStateFlags state;
  gdouble side, offset;
  gint line_width;
  GtkBorder border;
  const GtkWidgetPath *path = gtk_theming_engine_get_path (engine);

  side = floor (MIN (width, height));

  if (gtk_widget_path_is_type (path, GTK_TYPE_TREE_VIEW) &&
      (side == 17))
    {
      /* HACK: draw the expander as if it was 11px instead of the allocated 17px,
       * so that we can have a bit of padding between the view edge and the
       * expander itself.
       */
      x += 3;
      y += 3;
      width -= 6;
      height -= 6;
      side -= 6;
    }

  x += width / 2 - side / 2;
  y += height / 2 - side / 2;

  x = floor (x);
  y = floor (y);

  /* make sure the rendered side length is always odd */
  if (((gint) side % 2) == 0)
    side -= 1.0;

  GTK_THEMING_ENGINE_CLASS (adwaita_engine_parent_class)->render_background
    (engine, cr, x, y, side, side);
  GTK_THEMING_ENGINE_CLASS (adwaita_engine_parent_class)->render_frame
    (engine, cr, x, y, side, side);

  state = gtk_theming_engine_get_state (engine);
  gtk_theming_engine_get_color (engine, state, &fg);
  gtk_theming_engine_get_border (engine, state, &border);

  line_width = 1;
  offset = (1 + line_width / 2.0);

  cairo_save (cr);

  cairo_set_line_width (cr, line_width);
  cairo_set_line_cap (cr, CAIRO_LINE_CAP_ROUND);
  gdk_cairo_set_source_rgba (cr, &fg);

  cairo_move_to (cr,
                 x + border.left + offset,
                 y + side / 2);
  cairo_line_to (cr,
                 x + side - (border.right + offset),
                 y + side / 2);

  if ((state & GTK_STATE_FLAG_ACTIVE) == 0)
    {
      cairo_move_to (cr,
                     x + side / 2,
                     y + border.top + offset);
      cairo_line_to (cr,
                     x + side / 2,
                     y + side - (border.bottom + offset));
    }

  cairo_stroke (cr);

  cairo_restore (cr);
}

static void
adwaita_engine_render_activity (GtkThemingEngine *engine,
                                cairo_t          *cr,
                                gdouble           x,
                                gdouble           y,
                                gdouble           width,
                                gdouble           height)
{
  GtkStateFlags state;

  cairo_save (cr);
  state = gtk_theming_engine_get_state (engine);

  adwaita_trim_allocation_for_scale (engine,
                                     &x, &y,
                                     &width, &height);

  GTK_THEMING_ENGINE_CLASS (adwaita_engine_parent_class)->render_activity
    (engine, cr,
     x, y, width, height);

  if (gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_PROGRESSBAR))
    {
      cairo_pattern_t *pattern = NULL;

      gtk_theming_engine_get (engine, state,
                              "-adwaita-progressbar-pattern", &pattern,
                              NULL);

      if (pattern != NULL)
        {
          style_pattern_set_matrix (pattern, 20, 20, TRUE);
          cairo_rectangle (cr, x, y, width, height);
          cairo_set_source (cr, pattern);
          cairo_fill (cr);

          cairo_pattern_destroy (pattern);
        }
    }

  cairo_restore (cr);
}

static void
render_switch_lines (GtkThemingEngine *engine,
                     cairo_t *cr,
                     gdouble x,
                     gdouble y,
                     gdouble width,
                     gdouble height,
                     GtkOrientation orientation)
{
  GtkStateFlags state;
  GdkRGBA *lines_color;

  state = gtk_theming_engine_get_state (engine);

  if (state & GTK_STATE_FLAG_INSENSITIVE)
    return;

  gtk_theming_engine_get (engine, state,
                          "-adwaita-switch-grip-color", &lines_color,
                          NULL);

  cairo_save (cr);

  cairo_translate (cr,
                   x + width / 2.0 - 4.0,
                   y + height / 2.0 - 3.0);

  cairo_move_to (cr, 0.0, 0.0);
  cairo_set_line_width (cr, 2.0);
  cairo_set_line_cap (cr, CAIRO_LINE_CAP_ROUND);

  cairo_line_to (cr, 0.0, 6.0);
  cairo_move_to (cr, 4.0, 0.0);
  cairo_line_to (cr, 4.0, 6.0);
  cairo_move_to (cr, 8.0, 0.0);
  cairo_line_to (cr, 8.0, 6.0);

  gdk_cairo_set_source_rgba (cr, lines_color);
  cairo_stroke (cr);

  cairo_restore (cr);

  gdk_rgba_free (lines_color);
}

static void
adwaita_engine_render_slider (GtkThemingEngine *engine,
                              cairo_t          *cr,
                              gdouble           x,
                              gdouble           y,
                              gdouble           width,
                              gdouble           height,
                              GtkOrientation    orientation)
{
  const GtkWidgetPath *path;

  GTK_THEMING_ENGINE_CLASS (adwaita_engine_parent_class)->render_slider
    (engine, cr,
     x, y, width, height,
     orientation);

  path = gtk_theming_engine_get_path (engine);

  if (gtk_widget_path_is_type (path, GTK_TYPE_SWITCH))
    render_switch_lines (engine, cr, x, y, width, height, orientation);
}

static void
adwaita_engine_render_handle (GtkThemingEngine *engine,
                              cairo_t          *cr,
                              gdouble           x,
                              gdouble           y,
                              gdouble           width,
                              gdouble           height)
{
  if (gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_GRIP))
    {
      GdkRGBA bg;
      GtkJunctionSides sides;
      GtkStateFlags state;
      int lx, ly;
      int x_down;
      int y_down;
      int dots;

      state = gtk_theming_engine_get_state (engine);
      gtk_theming_engine_get_background_color (engine, state, &bg);

      /* The number of dots fitting into the area. Just hardcoded to 3 right now. */
      /* dots = MIN (width - 2, height - 2) / 3; */
      dots = 3;

      cairo_save (cr);

      sides = gtk_theming_engine_get_junction_sides (engine);

      switch (sides)
        {
        case GTK_JUNCTION_CORNER_TOPRIGHT:
          x_down = 0;
          y_down = 0;
          cairo_translate (cr, x + width - 4*dots, y + 1);
          break;
        case GTK_JUNCTION_CORNER_BOTTOMRIGHT:
          x_down = 0;
          y_down = 1;
          cairo_translate (cr, x + width - 4*dots, y + height + 1 - 4*dots);
          break;
        case GTK_JUNCTION_CORNER_BOTTOMLEFT:
          x_down = 1;
          y_down = 1;
          cairo_translate (cr, x + 2, y + height + 1 - 4*dots);
          break;
        case GTK_JUNCTION_CORNER_TOPLEFT:
          x_down = 1;
          y_down = 0;
          cairo_translate (cr, x + 2, y + 1);
          break;
        default:
          /* Not implemented. */
          cairo_restore (cr);
          return;
        }

      for (lx = 0; lx < dots; lx++) /* horizontally */
        {
          for (ly = 0; ly <= lx; ly++) /* vertically */
            {
              int mx, my;
              mx = x_down * dots + (1 - x_down * 2) * lx - x_down;
              my = y_down * dots + (1 - y_down * 2) * ly - y_down;

              gdk_cairo_set_source_rgba (cr, &bg);
              cairo_arc (cr,
                         mx * 4 - 1 + 1.5,
                         my * 4 - 1 + 1.5,
                         1.5,
                         0, G_PI * 2.0);

              cairo_fill (cr);
            }
        }

      cairo_restore (cr);
    }
  else if (gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_PANE_SEPARATOR))
    {
      GdkRGBA fg;
      GtkStateFlags state;
      gdouble xx, yy;

      state = gtk_theming_engine_get_state (engine);
      gtk_theming_engine_get_color (engine, state, &fg);

      GTK_THEMING_ENGINE_CLASS (adwaita_engine_parent_class)->render_background
        (engine, cr, x, y, width, height);
      GTK_THEMING_ENGINE_CLASS (adwaita_engine_parent_class)->render_frame
        (engine, cr, x, y, width, height);

      cairo_save (cr);
      cairo_set_line_width (cr, 2.0);
      gdk_cairo_set_source_rgba (cr, &fg);

      if (width > height)
        {
          for (xx = x + width / 2 - 12; xx <= x + width / 2 + 12; xx += 6)
            {
              cairo_arc (cr, xx, y + height / 2.0,
                         1.0,
                         0, G_PI * 2.0);
              cairo_fill (cr);
            }
        }
      else
        {
          for (yy = y + height / 2 - 12; yy <= y + height / 2 + 12; yy += 6)
            {
              cairo_arc (cr, x + width / 2.0, yy,
                         1.0,
                         0, G_PI * 2.0);
              cairo_fill (cr);
            }
        }

      cairo_restore (cr);
    }
  else
    {
      GTK_THEMING_ENGINE_CLASS (adwaita_engine_parent_class)->render_handle
        (engine, cr,
         x, y, width, height);
    }
}

static void
adwaita_engine_class_init (AdwaitaEngineClass *klass)
{
  GtkThemingEngineClass *engine_class = GTK_THEMING_ENGINE_CLASS (klass);

  engine_class->render_arrow = adwaita_engine_render_arrow;
  engine_class->render_focus = adwaita_engine_render_focus;
  engine_class->render_check = adwaita_engine_render_check;
  engine_class->render_option = adwaita_engine_render_option;
  engine_class->render_extension = adwaita_engine_render_extension;
  engine_class->render_frame = adwaita_engine_render_frame;
  engine_class->render_background = adwaita_engine_render_background;
  engine_class->render_expander = adwaita_engine_render_expander;
  engine_class->render_activity = adwaita_engine_render_activity;
  engine_class->render_slider = adwaita_engine_render_slider;
  engine_class->render_handle = adwaita_engine_render_handle;

  gtk_theming_engine_register_property (ADWAITA_NAMESPACE, NULL,
                                        g_param_spec_boxed ("focus-border-color",
                                                            "Focus border color",
                                                            "Focus border color",
                                                            GDK_TYPE_RGBA, 0));
  gtk_theming_engine_register_property (ADWAITA_NAMESPACE, NULL,
                                        g_param_spec_int ("focus-border-radius",
                                                          "Focus border radius",
                                                          "Focus border radius",
                                                          0, G_MAXINT, 0,
                                                          0));
  gtk_theming_engine_register_property (ADWAITA_NAMESPACE, NULL,
                                        g_param_spec_boxed ("focus-border-gradient",
                                                            "Focus border gradient",
                                                            "Focus border gradient",
                                                            CAIRO_GOBJECT_TYPE_PATTERN, 0));
  gtk_theming_engine_register_property (ADWAITA_NAMESPACE, NULL,
                                        g_param_spec_boxed ("focus-fill-color",
                                                            "Focus fill color",
                                                            "Focus fill color",
                                                            GDK_TYPE_RGBA, 0));
  gtk_theming_engine_register_property (ADWAITA_NAMESPACE, NULL,
                                        g_param_spec_boxed ("selected-tab-color",
                                                            "Selected tab color",
                                                            "Selected tab color",
                                                            GDK_TYPE_RGBA, 0));
  gtk_theming_engine_register_property (ADWAITA_NAMESPACE, NULL,
                                        g_param_spec_boxed ("border-gradient",
                                                            "Border gradient",
                                                            "Border gradient",
                                                            CAIRO_GOBJECT_TYPE_PATTERN, 0));
  gtk_theming_engine_register_property (ADWAITA_NAMESPACE, NULL,
                                        g_param_spec_boolean ("focus-border-dashes",
                                                              "Focus border uses dashes",
                                                              "Focus border uses dashes",
                                                              FALSE, 0));
  gtk_theming_engine_register_property (ADWAITA_NAMESPACE, NULL,
                                        g_param_spec_boxed ("menuitem-arrow-color",
                                                            "Menuitem arrow color",
                                                            "Menuitem arrow color",
                                                            GDK_TYPE_RGBA, 0));
  gtk_theming_engine_register_property (ADWAITA_NAMESPACE, NULL,
                                        g_param_spec_boxed ("switch-grip-color",
                                                            "Switch grip color",
                                                            "Switch grip color",
                                                            GDK_TYPE_RGBA, 0));
  gtk_theming_engine_register_property (ADWAITA_NAMESPACE, NULL,
                                        g_param_spec_boxed ("progressbar-pattern",
                                                            "Progressbar pattern",
                                                            "Progressbar pattern",
                                                            CAIRO_GOBJECT_TYPE_PATTERN, 0));
}

static void
adwaita_engine_class_finalize (AdwaitaEngineClass *klass)
{
}

G_MODULE_EXPORT void
theme_init (GTypeModule *module)
{
  adwaita_engine_register_types (module);
}

G_MODULE_EXPORT void
theme_exit (void)
{
}

G_MODULE_EXPORT GtkThemingEngine *
create_engine (void)
{
  return GTK_THEMING_ENGINE (g_object_new (ADWAITA_TYPE_ENGINE,
                                           "name", "adwaita",
                                           NULL));
}
