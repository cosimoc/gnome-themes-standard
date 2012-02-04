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
  GdkRGBA *border_color = NULL;
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
                          "-adwaita-focus-border-radius", &border_radius,
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

  if (use_dashes)
    cairo_set_dash (cr, dashes, 1, 0.0);

  if (border_color != NULL)
    gdk_cairo_set_source_rgba (cr, border_color);

  cairo_stroke (cr);
  cairo_restore (cr);

  if (border_color != NULL)
    gdk_rgba_free (border_color);
}

#define NOTEBOOK_TAB_TOP_MARGIN 3.0

static void
draw_tab_shape (cairo_t *cr,
                gdouble curve_width,
                gdouble x,
                gdouble y,
                gdouble width,
                gdouble height)
{
  cairo_move_to (cr, 0, height);

  cairo_arc (cr, 
             curve_width, y + 3.0,
             2.5,
             G_PI, G_PI + G_PI_2);

  cairo_arc (cr,
             width - curve_width, y + 3.0,
             2.5,
             G_PI + G_PI_2, 2 * G_PI);

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
  gint tab_curvature, border_width;
  GdkRGBA border_color, background_color;
  GtkStateFlags state;
  gdouble angle = 0;
  cairo_pattern_t *pattern = NULL;
  gboolean is_active;

  gtk_theming_engine_get_style (engine,
                                "tab-curvature", &tab_curvature,
                                NULL);
  state = gtk_theming_engine_get_state (engine);
  gtk_theming_engine_get_background_color (engine, state, &background_color);
  gtk_theming_engine_get_border_color (engine, state, &border_color);
  gtk_theming_engine_get (engine, state,
                          "-adwaita-border-gradient", &pattern,
                          NULL);

  is_active = (state & GTK_STATE_FLAG_ACTIVE);
  border_width = 1.0;

  cairo_save (cr);
  cairo_set_line_width (cr, border_width);

  if (gap_side == GTK_POS_TOP)
    {
      angle = G_PI;
      cairo_translate (cr, width, height);
    }

  if (gap_side == GTK_POS_BOTTOM)
    x += 0.5;
  else if (gap_side == GTK_POS_TOP)
    x -= 0.5;

  width -= 1.0;

  cairo_translate (cr, x, y);
  cairo_rotate (cr, angle);

  /* draw the tab shape and clip the background inside it */
  cairo_save (cr);
  draw_tab_shape (cr, tab_curvature, 
                  0, NOTEBOOK_TAB_TOP_MARGIN + border_width + 0.5,
                  width, is_active ? (height + 1.0) : (height));
  cairo_clip (cr);

  GTK_THEMING_ENGINE_CLASS (adwaita_engine_parent_class)->render_background
    (engine, cr, 0, NOTEBOOK_TAB_TOP_MARGIN + border_width + 0.5,
     width, is_active ? (height + 1.0) : (height));

  cairo_restore (cr);

  /* now draw the border */
  draw_tab_shape (cr, tab_curvature,
                  0, NOTEBOOK_TAB_TOP_MARGIN + border_width,
                  width, height);

  if (pattern && (state & GTK_STATE_FLAG_ACTIVE))
    {
      cairo_scale (cr, width, height - NOTEBOOK_TAB_TOP_MARGIN);
      cairo_set_source (cr, pattern);
      cairo_scale (cr, 1.0 / width, 1.0 / (height - NOTEBOOK_TAB_TOP_MARGIN));
    }
  else
    {
      gdk_cairo_set_source_rgba (cr, &border_color);
    }

  cairo_stroke (cr);

  if (pattern != NULL)
    cairo_pattern_destroy (pattern);

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
          cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
          cairo_scale (cr, 20, 20);
          cairo_set_source (cr, pattern);
          cairo_scale (cr, 1.0 / 20, 1.0 / 20);

          cairo_rectangle (cr, x, y, width, height);
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
  GdkRGBA lines_color;

  state = gtk_theming_engine_get_state (engine);

  if (state & GTK_STATE_FLAG_INSENSITIVE)
    return;

  gtk_theming_engine_get_color (engine, state, &lines_color);

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

  gdk_cairo_set_source_rgba (cr, &lines_color);
  cairo_stroke (cr);

  cairo_restore (cr);
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
  if (gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_PANE_SEPARATOR))
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
