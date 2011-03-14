/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
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
_cairo_round_rectangle (cairo_t *cr,
			gdouble	  x,
			gdouble	  y,
			gdouble	  w,
			gdouble	  h,
			gdouble	  radius)
{
	g_return_if_fail (cr != NULL);

	if (radius < 0.0001)
	{
		cairo_rectangle (cr, x, y, w, h);
		return;
	}

	cairo_move_to (cr, x+radius, y);
	cairo_arc (cr, x+w-radius, y+radius, radius, G_PI * 1.5, G_PI * 2);
	cairo_arc (cr, x+w-radius, y+h-radius, radius, 0, G_PI * 0.5);
	cairo_arc (cr, x+radius,   y+h-radius, radius, G_PI * 0.5, G_PI);
	cairo_arc (cr, x+radius,   y+radius,   radius, G_PI, G_PI * 1.5);
}

/* Set the appropriate matrix for
 * patterns coming from the style context
 */
static void
style_pattern_set_matrix (cairo_pattern_t *pattern,
                          gdouble          width,
                          gdouble          height)
{
  cairo_matrix_t matrix;
  gint w, h;

  if (cairo_pattern_get_type (pattern) == CAIRO_PATTERN_TYPE_SURFACE)
    {
      cairo_surface_t *surface;

      cairo_pattern_get_surface (pattern, &surface);
      w = cairo_image_surface_get_width (surface);
      h = cairo_image_surface_get_height (surface);
    }
  else
    w = h = 1;

  cairo_matrix_init_scale (&matrix, (gdouble) w / width, (gdouble) h / height);
  cairo_pattern_set_matrix (pattern, &matrix);
}

static void
adwaita_engine_render_arrow (GtkThemingEngine *engine,
			     cairo_t	      *cr,
			     gdouble	       angle,
			     gdouble	       x,
			     gdouble	       y,
			     gdouble	       size)
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
			     cairo_t	      *cr,
			     gdouble	       x,
			     gdouble	       y,
			     gdouble	       width,
			     gdouble	       height)
{
	GdkRGBA *fill_color, *border_color = NULL;
	cairo_pattern_t *pattern = NULL;
	GtkStateFlags state;
	gint line_width, focus_pad;
	gint border_radius;
	gboolean use_dashes;
	double dashes[2] = { 2.0, 0.2 };

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

	if (gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_NOTEBOOK) &&
	    gtk_theming_engine_has_region (engine, GTK_STYLE_REGION_TAB, NULL)) {
		/* as we render the tab smaller than the whole allocation, we need
		 * to recenter and resize the focus on the tab.
		 */
		y += 3.0;
		height -= 3.0;
	}

	cairo_save (cr);
	cairo_set_line_width (cr, line_width);

	if (line_width > 1) {
		_cairo_round_rectangle (cr, x, y,
					width, height, border_radius);
	} else {
		_cairo_round_rectangle (cr, x + 0.5, y + 0.5,
					width - 1, height - 1, border_radius);
	}

	/* if we have a fill color, draw the fill */
	if (fill_color != NULL) {
		gdk_cairo_set_source_rgba (cr, fill_color);
		cairo_fill_preserve (cr);
	}

	if (use_dashes) {
		cairo_set_dash (cr, dashes, 1, 0.0);
	}

	/* if we have a gradient, draw the gradient, otherwise
	 * draw the line if we have a color for it.
	 */
	if (pattern != NULL) {
		style_pattern_set_matrix (pattern, width, height);

		cairo_set_source (cr, pattern);
	} else if (border_color != NULL) {
		gdk_cairo_set_source_rgba (cr, border_color);
	}

	cairo_stroke (cr);
	cairo_restore (cr);

	if (pattern != NULL) {
		cairo_pattern_destroy (pattern);
	}

	if (border_color != NULL) {
		gdk_rgba_free (border_color);
	}

	if (fill_color != NULL) {
		gdk_rgba_free (fill_color);
	}
}

static gboolean
render_from_assets_common (GtkThemingEngine *engine,
			   cairo_t *cr,
			   gdouble x,
			   gdouble y,
			   gdouble width,
			   gdouble height)
{
	gboolean retval = FALSE;
	GtkStateFlags state;
	GValue value = { 0, };
	cairo_pattern_t *asset = NULL;
	cairo_surface_t *surface = NULL;

	state = gtk_theming_engine_get_state (engine);
	gtk_theming_engine_get_property (engine,
					 "background-image",
					 state,
					 &value);

	asset = g_value_dup_boxed (&value);
	g_value_unset (&value);

	if (asset != NULL) {
		cairo_pattern_get_surface (asset, &surface);
	}

	if (surface != NULL) {
		cairo_save (cr);

		cairo_set_source_surface (cr, surface, x, y);
		cairo_scale (cr,
			     width / cairo_image_surface_get_width (surface),
			     height / cairo_image_surface_get_height (surface));

		cairo_paint (cr);

		cairo_restore (cr);
		retval = TRUE;
	}

	if (asset != NULL) {
		cairo_pattern_destroy (asset);
	}

	return retval;
}

static void
render_check_menuitem (GtkThemingEngine *engine,
		       cairo_t *cr,
		       gdouble x,
		       gdouble y,
		       gdouble width,
		       gdouble height)
{
	GdkRGBA color;
	GtkStateFlags state;

	state = gtk_theming_engine_get_state (engine);
	gtk_theming_engine_get_color (engine, state, &color);

	if (!(state & GTK_STATE_FLAG_ACTIVE))
		return;

	cairo_save (cr);

	cairo_translate (cr, x, y);

	cairo_set_line_width (cr, 2.0);
	cairo_set_line_cap (cr, CAIRO_LINE_CAP_ROUND);
	cairo_set_line_join (cr, CAIRO_LINE_JOIN_ROUND);

	cairo_move_to (cr, 0.5 + (width * 0.08), (height * 0.67));
	cairo_line_to (cr, 0.5 + (width * 0.32), (height * 0.90));
	cairo_line_to (cr, 0.5 + (width * 0.80), (height * 0.33));

	gdk_cairo_set_source_rgba (cr, &color);
	cairo_stroke (cr);

	cairo_restore (cr);
}

static void
adwaita_engine_render_check (GtkThemingEngine *engine,
			     cairo_t	      *cr,
			     gdouble	       x,
			     gdouble	       y,
			     gdouble	       width,
			     gdouble	       height)
{
	GdkRGBA *bg, *border, *fg;
	gboolean inconsistent = FALSE;
	gboolean draw_bullet;
	GtkStateFlags state;
	gint radius;

	if (gtk_theming_engine_has_class (engine,
					  GTK_STYLE_CLASS_MENUITEM))
	{
		render_check_menuitem (engine, cr,
				       x, y, width, height);

		return;
	}

	if (render_from_assets_common (engine, cr,
				       x, y + 2.0, width, height)) {
		return;
	}

	/* fallback to old code path */
	state = gtk_theming_engine_get_state (engine);
	inconsistent = (state & GTK_STATE_FLAG_INCONSISTENT) != 0;
	draw_bullet = (state & GTK_STATE_FLAG_ACTIVE);
	draw_bullet |= inconsistent;

	cairo_save (cr);

	gtk_theming_engine_get (engine, state,
				"border-radius", &radius,
				"background-color", &bg,
				"border-color", &border,
				"color", &fg,
				NULL);

	cairo_translate (cr, x, y);
	cairo_set_line_width (cr, 1);

	_cairo_round_rectangle (cr, 0.5, 0.5, width-1, height-1, 2);

	if ((state & GTK_STATE_FLAG_INSENSITIVE) == 0)
	{
		gdk_cairo_set_source_rgba (cr, bg);
		cairo_fill_preserve (cr);
	}

	gdk_cairo_set_source_rgba (cr, border);
	cairo_stroke (cr);

	if (draw_bullet)
	{
		if (inconsistent) /* Inconsistent */
		{
			cairo_set_line_width (cr, 2.0);
			cairo_move_to (cr, 3, height*0.5);
			cairo_line_to (cr, width-3, height*0.5);
		}
		else
		{
			cairo_set_line_width (cr, 1.7);
			cairo_move_to (cr, 0.5 + (width*0.2), (height*0.5));
			cairo_line_to (cr, 0.5 + (width*0.4), (height*0.7));

			cairo_curve_to (cr, 0.5 + (width*0.4), (height*0.7),
					    0.5 + (width*0.5), (height*0.4),
					    0.5 + (width*0.70), (height*0.25));

		}

		gdk_cairo_set_source_rgba (cr, fg);
		cairo_stroke (cr);
	}

	cairo_restore (cr);

	gdk_rgba_free (bg);
	gdk_rgba_free (fg);
	gdk_rgba_free (border);
}

static void
render_radio_menuitem (GtkThemingEngine *engine,
		       cairo_t *cr,
		       gdouble x,
		       gdouble y,
		       gdouble width,
		       gdouble height)
{
	GdkRGBA color;
	GtkStateFlags state;
	double radius;

	state = gtk_theming_engine_get_state (engine);

	if (!(state & GTK_STATE_FLAG_ACTIVE))
		return;

	gtk_theming_engine_get_color (engine, state, &color);

	radius = MAX (height / 2.0, width / 2.0) * 0.58;

	cairo_save (cr);

	cairo_translate (cr, x + width / 2.0, y + height * 0.67);
	cairo_arc (cr,
		   0, 0,
		   radius,
		   0, 4 * G_PI);

	gdk_cairo_set_source_rgba (cr, &color);
	cairo_fill (cr);

	cairo_restore (cr);
}

static void
adwaita_engine_render_option (GtkThemingEngine *engine,
			      cairo_t	       *cr,
			      gdouble		x,
			      gdouble		y,
			      gdouble		width,
			      gdouble		height)
{
	GdkRGBA *bg, *border, *fg, highlight;
	GtkSymbolicColor *sym, *shade;
	gboolean inconsistent;
	gboolean draw_bullet;
	gdouble cx, cy, radius;
	GtkStateFlags state;

	if (gtk_theming_engine_has_class (engine,
					  GTK_STYLE_CLASS_MENUITEM))
	{
		render_radio_menuitem (engine, cr, x, y, width, height);
		return;
	}

	if (render_from_assets_common (engine, cr,
				       x, y + 2.0, width, height)) {
		return;
	}

	/* fallback to old code path */
	cx = width / 2.0;
	cy = height / 2.0;
	radius = MIN (width, height) / 2.0;

	cairo_save (cr);

	state = gtk_theming_engine_get_state (engine);

	gtk_theming_engine_get (engine, state,
				"background-color", &bg,
				"border-color", &border,
				"color", &fg,
				NULL);

	inconsistent = (state & GTK_STATE_FLAG_INCONSISTENT) != 0;
	draw_bullet = (state & GTK_STATE_FLAG_ACTIVE) != 0;
	draw_bullet |= inconsistent;

	sym = gtk_symbolic_color_new_literal (bg);
	shade = gtk_symbolic_color_new_shade (sym, 1.1);
	gtk_symbolic_color_resolve (shade, NULL, &highlight);

	cairo_translate (cr, x, y);
	cairo_set_line_width (cr, MAX (1.0, floor (radius/6)));

	cairo_new_sub_path (cr);
	cairo_arc (cr, ceil (cx), ceil (cy), MAX (1.0, ceil (radius) - 1.5), 0, G_PI*2);

	if ((state & GTK_STATE_FLAG_INSENSITIVE) == 0)
	{
		gdk_cairo_set_source_rgba (cr, bg);
		cairo_fill_preserve (cr);
	}

	gdk_cairo_set_source_rgba (cr, border);
	cairo_stroke (cr);

	if (draw_bullet)
	{
		if (inconsistent)
		{
			gdouble line_width = floor (radius * 2 / 3);

			cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
			cairo_set_line_width (cr, line_width);

			cairo_move_to (cr, ceil (cx - radius/3.0 - line_width) + line_width, ceil (cy - line_width) + line_width);
			cairo_line_to (cr, floor (cx + radius/3.0 + line_width) - line_width, ceil (cy - line_width) + line_width);

			gdk_cairo_set_source_rgba (cr, fg);
			cairo_stroke (cr);
		}
		else
		{
			cairo_new_sub_path (cr);
			cairo_arc (cr, ceil (cx), ceil (cy), floor (radius/2.0), 0, G_PI*2);
			gdk_cairo_set_source_rgba (cr, fg);
			cairo_fill (cr);

			cairo_arc (cr, floor (cx - radius/10.0), floor (cy - radius/10.0), floor (radius/6.0), 0, G_PI*2);
			cairo_set_source_rgba (cr, highlight.red, highlight.green, highlight.blue, 0.5);
			cairo_fill (cr);
		}
	}

	cairo_restore (cr);

	gdk_rgba_free (bg);
	gdk_rgba_free (fg);
	gdk_rgba_free (border);

	gtk_symbolic_color_unref (sym);
	gtk_symbolic_color_unref (shade);
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
			   cairo_t	  *cr,
			   gdouble	   x,
			   gdouble	   y,
			   gdouble	   width,
			   gdouble	   height,
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

	if (gap_side == GTK_POS_TOP) {
		angle = G_PI;
		cairo_translate (cr, width, height);
	}

	if (gap_side == GTK_POS_BOTTOM) {
		cairo_translate (cr,
				 x + 0.5,
				 (state & GTK_STATE_FLAG_ACTIVE) ?
				 y + 1.0 : y);
	} else if (gap_side == GTK_POS_TOP) {
		cairo_translate (cr,
				 x - 0.5,
				 (state & GTK_STATE_FLAG_ACTIVE) ?
				 y - 1.0 : y);
	}

	cairo_rotate (cr, angle);

	width -= 1.0;
	draw_tab_shape_active (cr, tab_curvature, 0, 0, width, height);

	if (background_pattern != NULL) {
		cairo_matrix_init_scale (&matrix,
					 1. / width,
					 1. / height);
		cairo_pattern_set_matrix (background_pattern, &matrix);
		cairo_set_source (cr, background_pattern);
	} else {
		gdk_cairo_set_source_rgba (cr, &background_color);
	}

	cairo_fill (cr);

	if (state & GTK_STATE_FLAG_ACTIVE) {
		draw_tab_shape_active (cr, tab_curvature, 0, 0, width, 6.0);
		gdk_cairo_set_source_rgba (cr, color);
		cairo_fill (cr);
	}

	draw_tab_shape_active (cr, tab_curvature, 0, 0, width, height);

	if (state & GTK_STATE_FLAG_ACTIVE) {
		style_pattern_set_matrix (pattern, width, height - 6.0);
		cairo_set_source (cr, pattern);
	} else {
		gdk_cairo_set_source_rgba (cr, &border_color);
	}

	cairo_stroke (cr);

	gdk_rgba_free (color);

	if (pattern != NULL) {
		cairo_pattern_destroy (pattern);
	}

	if (background_pattern != NULL) {
		cairo_pattern_destroy (background_pattern);
	}
	
	cairo_restore (cr);
}

static void
adwaita_engine_render_extension (GtkThemingEngine *engine,
				 cairo_t	  *cr,
				 gdouble	   x,
				 gdouble	   y,
				 gdouble	   width,
				 gdouble	   height,
				 GtkPositionType   gap_side)
{
	GtkStateFlags state;

	if (gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_NOTEBOOK) &&
	    ((gap_side == GTK_POS_TOP) || (gap_side == GTK_POS_BOTTOM))) {
		render_notebook_extension (engine, cr, x, y, width, height, gap_side);

		return;
	}

	GTK_THEMING_ENGINE_CLASS (adwaita_engine_parent_class)->render_extension (engine, cr,
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
			_cairo_round_rectangle (cr, x + 1, y + 1, width - 2, 3, 0);
			break;
		case GTK_POS_TOP:
			_cairo_round_rectangle (cr, x + 1, y + height - 4, width - 2, 3, 0);
			break;
		case GTK_POS_RIGHT:
			_cairo_round_rectangle (cr, x + 1, y + 1, 3, height - 2, 0);
			break;
		case GTK_POS_LEFT:
			_cairo_round_rectangle (cr, x + width - 4, y + 1, 3, height - 2, 0);
			break;
		}

		gdk_cairo_set_source_rgba (cr, fill);
		cairo_fill (cr);

		gdk_rgba_free (fill);
	}
}

static void
draw_menu_bar_item_shape (cairo_t *cr,
			  gdouble radius,
			  gdouble x,
			  gdouble y,
			  gdouble w,
			  gdouble h,
			  gboolean for_fill)
{
	/* draw a round rectangle without the bottom side */
	cairo_move_to (cr, x+radius, y);
	cairo_arc (cr, x+w-radius, y+radius, radius, G_PI * 1.5, G_PI * 2);
	cairo_line_to (cr, x+w, y+h);

	if (for_fill) {
		cairo_line_to (cr, x, y+h);
	} else {
		cairo_move_to (cr, x, y+h);
	}

	cairo_arc (cr, x+radius, y+radius, radius, G_PI, G_PI * 1.5);
}

static void
render_menubar_active_frame (GtkThemingEngine *engine,
			     cairo_t          *cr,
			     gdouble           x,
			     gdouble           y,
			     gdouble           w,
			     gdouble           h)
{
	GtkStateFlags state;
	GdkRGBA color;
	gint radius, border_width;
	GtkBorder *border;

	state = gtk_theming_engine_get_state (engine);
	gtk_theming_engine_get_border_color (engine, state, &color);
	gtk_theming_engine_get (engine, state,
				"border-radius", &radius,
				"border-width", &border,
				NULL);

	border_width = MIN (MIN (border->top, border->bottom),
			    MIN (border->left, border->right));

	if (border_width > 1) {
		x += (gdouble) border_width / 2;
		y += (gdouble) border_width / 2;
		w -= border_width;
		h -= border_width;
        } else if (border_width == 1) {
		x += 0.5;
		y += 0.5;
		w -= 1;
		h -= 1;
        }

	cairo_save (cr);

	cairo_set_line_width (cr, border_width);
	draw_menu_bar_item_shape (cr, radius, x, y, w, h, FALSE);

	gdk_cairo_set_source_rgba (cr, &color);
	cairo_stroke (cr);

	cairo_restore (cr);

	gtk_border_free (border);
}

static void
render_frame_default (GtkThemingEngine *engine,
		      cairo_t *cr,
		      gdouble x,
		      gdouble y,
		      gdouble width,
		      gdouble height)
{
	cairo_pattern_t *pattern = NULL;
	GtkStateFlags state;
	GtkBorder *border;
	gint line_width, border_radius;
	GtkBorderStyle border_style;

	state = gtk_theming_engine_get_state (engine);

	gtk_theming_engine_get (engine, state,
				"-adwaita-border-gradient", &pattern,
				"border-style", &border_style,
				NULL);

	if (pattern == NULL || border_style == GTK_BORDER_STYLE_NONE) {
		GTK_THEMING_ENGINE_CLASS (adwaita_engine_parent_class)->render_frame
			(engine, cr,
			 x, y, width, height);

		return;
	}

	cairo_save (cr);

	gtk_theming_engine_get (engine, state,
				"border-radius", &border_radius,
				"border-width", &border,
				NULL);

	line_width = MIN (MIN (border->top, border->bottom),
			  MIN (border->left, border->right));
	style_pattern_set_matrix (pattern, width, height);

	cairo_set_line_width (cr, line_width);
	_cairo_round_rectangle (cr,
				x + line_width / 2.0,
				y + line_width / 2.0,
				width - line_width,
				height - line_width,
				border_radius);
	cairo_set_source (cr, pattern);

	cairo_stroke (cr);

	cairo_restore (cr);

	cairo_pattern_destroy (pattern);
	gtk_border_free (border);
}

static void
adwaita_engine_render_frame (GtkThemingEngine *engine,
			     cairo_t          *cr,
			     gdouble           x,
			     gdouble           y,
			     gdouble           width,
			     gdouble           height)
{
	const GtkWidgetPath *path;
	GtkRegionFlags flags = 0;
	gint len;
	GtkStateFlags state;	

	state = gtk_theming_engine_get_state (engine);

	if (gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_MENUITEM) &&
	    gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_MENUBAR)) {
		render_menubar_active_frame (engine, cr, x, y, width, height);

		return;
	}
	
	path = gtk_theming_engine_get_path (engine);
	len = gtk_widget_path_length (path);

	cairo_save (cr);

	if (gtk_widget_path_iter_has_region (path, len - 2,
					     GTK_STYLE_REGION_COLUMN_HEADER,
					     &flags))
	{
		GdkRGBA color;

		if ((flags & GTK_REGION_LAST) != 0)
                        goto out;

		/* Column header */
		if (gtk_theming_engine_get_direction (engine) == GTK_TEXT_DIR_RTL)
		{
			cairo_move_to (cr, x + 0.5, y + 2);
			cairo_line_to (cr, x + 0.5, y + height - 4);
		}
		else
		{
			cairo_move_to (cr, x + width - 0.5, y + 2);
			cairo_line_to (cr, x + width - 0.5, y + height - 4);
		}

		gtk_theming_engine_get_border_color (engine, state, &color);

		cairo_set_line_width (cr, 1);
		gdk_cairo_set_source_rgba (cr, &color);
		cairo_stroke (cr);
	}
	else
	{
		if (gtk_widget_path_is_type (path, GTK_TYPE_SCALE) &&
		    gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_TROUGH))
		{
			/* Render GtkScale trough thinner */
			if (!gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_VERTICAL))
			{
				y += height / 2.0 - 2.0;
				height = 4;
			}
			else
			{
				x += width / 2.0 - 2.0;
				width = 4;
			}
		}

		if (gtk_widget_path_is_type (path, GTK_TYPE_PROGRESS_BAR) &&
		    gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_TROUGH)) {
			GtkBorder *border_width;
			gint border_line;

			/* draw the border inside the trough itself, so it will
			 * be overdrawn by the fill.
			 */
			gtk_theming_engine_get (engine, state,
						"border-width", &border_width,
						NULL);

			border_line = MIN (MIN (border_width->top, border_width->bottom),
					   MIN (border_width->left, border_width->right));

			y += border_line;
			x += border_line;
			width -= 2 * border_line;
			height -= 2 * border_line;

			gtk_border_free (border_width);
		}

		render_frame_default (engine, cr, x, y, width, height);
	}

out:
	cairo_restore (cr);
}

static void
render_menubar_active_background (GtkThemingEngine *engine,
				  cairo_t          *cr,
				  gdouble           x,
				  gdouble           y,
				  gdouble           w,
				  gdouble           h)
{
	GtkStateFlags state;
	GdkRGBA color;
	gint radius;
	GtkBorder *border;

	state = gtk_theming_engine_get_state (engine);
	gtk_theming_engine_get_border_color (engine, state, &color);
	gtk_theming_engine_get (engine, state,
				"border-radius", &radius,
				"border-width", &border,
				NULL);

	gtk_theming_engine_get_background_color (engine, state, &color);

	/* omit all the border but the bottom line */
	x += border->left;
	y += border->top;
	w -= border->left + border->right;
	h -= border->top;

	cairo_save (cr);
	cairo_translate (cr, x, y);

	draw_menu_bar_item_shape (cr, radius, 0, 0, w, h, TRUE);

	gdk_cairo_set_source_rgba (cr, &color);
	cairo_fill (cr);

	cairo_restore (cr);

	gtk_border_free (border);
}

static void
adwaita_engine_render_background (GtkThemingEngine *engine,
				  cairo_t          *cr,
				  gdouble           x,
				  gdouble           y,
				  gdouble           width,
				  gdouble           height)
{
	const GtkWidgetPath *path;
	GtkStateFlags state;

	path = gtk_theming_engine_get_path (engine);
	state = gtk_theming_engine_get_state (engine);

	if (gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_MENUITEM) &&
	    gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_MENUBAR)) {
		render_menubar_active_background (engine, cr, x, y, width, height);

		return;
	}

	if (gtk_widget_path_is_type (path, GTK_TYPE_SCALE) &&
	    gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_TROUGH))
	{
		/* Render GtkScale trough thinner */
		if (!gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_VERTICAL))
		{
			y += height / 2 - 2;
			height = 4;
		}
		else
		{
			x += width / 2 - 2;
			width = 4;
		}
	}

	if (gtk_widget_path_is_type (path, GTK_TYPE_PROGRESS_BAR) &&
	    gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_TROUGH)) {
		GtkBorder *border_width;
		gint border_line;

		/* draw the border inside the trough itself, so it will
		 * be overdrawn by the fill.
		 */
		gtk_theming_engine_get (engine, state,
					"border-width", &border_width,
					NULL);

		border_line = MIN (MIN (border_width->top, border_width->bottom),
				   MIN (border_width->left, border_width->right));

		y += border_line;
		x += border_line;
		width -= 2 * border_line;
		height -= 2 * border_line;

		gtk_border_free (border_width);
	}

	GTK_THEMING_ENGINE_CLASS (adwaita_engine_parent_class)->render_background (engine, cr, x, y,
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
	GdkRGBA border, bg, fg;
	GtkStateFlags state;
	gint side;

	side = MIN (width, height);

	x += ((int) width / 2) - (side / 2);
	y += ((int) height / 2) - (side / 2);

	state = gtk_theming_engine_get_state (engine);

	gtk_theming_engine_get_border_color (engine, state, &border);
	gtk_theming_engine_get_background_color (engine, state, &bg);
	gtk_theming_engine_get_color (engine, state, &fg);

	cairo_save (cr);

	cairo_set_line_width (cr, 1);

	_cairo_round_rectangle (cr, x + 0.5, y + 0.5, side, side, 2);
	gdk_cairo_set_source_rgba (cr, &bg);
	cairo_fill_preserve (cr);

	gdk_cairo_set_source_rgba (cr, &border);
	cairo_stroke (cr);

	cairo_set_line_width (cr, 1);
	cairo_set_line_cap (cr, CAIRO_LINE_CAP_ROUND);
	gdk_cairo_set_source_rgba (cr, &fg);

	cairo_move_to (cr, x + 3, y + side / 2 + 0.5);
	cairo_line_to (cr, x + side - 2, y + side / 2 + 0.5);

	if ((state & GTK_STATE_FLAG_ACTIVE) == 0)
	{
		cairo_move_to (cr, x + side / 2 + 0.5, y + 3);
		cairo_line_to (cr, x + side / 2 + 0.5, y + side - 2);
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
	const GtkWidgetPath *path;

	cairo_save (cr);
	path = gtk_theming_engine_get_path (engine);

	if (gtk_widget_path_is_type (path, GTK_TYPE_SCALE) &&
	    gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_PROGRESSBAR))
	{
		/* Render GtkScale fill level thinner */
		if (!gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_VERTICAL))
		{
			y += height / 2.0 - 2.0;
			height = 4;
		}
		else
		{
			x += width / 2.0 - 2.0;
			width = 4;
		}
	}

	GTK_THEMING_ENGINE_CLASS (adwaita_engine_parent_class)->render_activity (engine, cr,
										 x, y, width, height);

	if (gtk_widget_path_is_type (path, GTK_TYPE_PROGRESS_BAR) &&
	    gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_PROGRESSBAR))
	{
		cairo_pattern_t *pattern;

		pattern = cairo_pattern_create_linear (0, 0, 20, 20);
		cairo_pattern_add_color_stop_rgba (pattern, 0, 0, 0, 0, 0);
		cairo_pattern_add_color_stop_rgba (pattern, 0.49, 0, 0, 0, 0);
		cairo_pattern_add_color_stop_rgba (pattern, 0.5, 0, 0, 0, 0.1);
		cairo_pattern_add_color_stop_rgba (pattern, 0.99, 0, 0, 0, 0.1);

		cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
		cairo_rectangle (cr, x, y, width, height);
		cairo_set_source (cr, pattern);
		cairo_fill (cr);

		cairo_pattern_destroy (pattern);
	}

	cairo_restore (cr);
}

static void
draw_round_slider (cairo_t *cr,
		   gdouble  width,
		   gdouble  height)
{
	cairo_arc (cr, (width) / 2.0, (height) / 2.0,
		   MIN (height / 2.0, width / 2.0) - 0.5,
		   0, 2 * G_PI);
	cairo_close_path (cr);	
}

static void
draw_mark_slider (cairo_t *cr,
		  gdouble  width,
		  gdouble  height,
		  gboolean marks_below,
		  GtkOrientation orientation)
{
	cairo_set_line_join (cr, CAIRO_LINE_JOIN_ROUND);

	if (marks_below) {
		if (orientation == GTK_ORIENTATION_HORIZONTAL) {
			cairo_move_to (cr, 0, height / 2.0 + 3.0);
			cairo_arc (cr, 2.5, 3.5,
				   2.0,
				   G_PI, G_PI + G_PI_2);
			
			cairo_arc (cr, width - 2.5, 3.5,
				   2.0,
				   G_PI + G_PI_2, 2 * G_PI);

			cairo_line_to (cr, width, height / 2.0 + 3.0);
			cairo_line_to (cr, width / 2.0, height);
			cairo_line_to (cr, 0, height / 2.0 + 3.0);

			cairo_close_path (cr);
		} else {
			cairo_move_to (cr, width / 2.0, 0);
			cairo_arc (cr, width - 2.5, 2.5,
				   2.0,
				   G_PI + G_PI_2, 2 * G_PI);

			cairo_arc (cr, width - 2.5, height - 2.5,
				   2.0,
				   0, G_PI_2);

			cairo_line_to (cr, width / 2.0, height);
			cairo_line_to (cr, 0, height / 2.0);
			cairo_line_to (cr, width / 2.0, 0);

			cairo_close_path (cr);
		}
	} else {
		if (orientation == GTK_ORIENTATION_HORIZONTAL) {
			cairo_move_to (cr, width, height / 2.0 - 3.0);
			cairo_arc (cr, width - 2.5, height - 3.5,
				   2.0,
				   0, G_PI_2);

			cairo_arc (cr, 2.5, height - 3.5,
				   2.0,
				   G_PI_2, G_PI);

			cairo_line_to (cr, 0, height / 2.0 - 3.0);
			cairo_line_to (cr, width / 2.0, 0);
			cairo_line_to (cr, width, height / 2.0 - 3.0);

			cairo_close_path (cr);
		} else {
			cairo_move_to (cr, width / 2.0, height);
			cairo_arc (cr, 2.5, height - 2.5,
				   2.0,
				   G_PI_2, G_PI);

			cairo_arc (cr, 2.5, 2.5,
				   2.0,
				   G_PI, G_PI + G_PI_2);

			cairo_line_to (cr, width / 2.0, 0);
			cairo_line_to (cr, width, height / 2.0);
			cairo_line_to (cr, width / 2.0, height);

			cairo_close_path (cr);
		}
	}
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

	path = gtk_theming_engine_get_path (engine);
	cairo_save (cr);

	if (gtk_widget_path_is_type (path, GTK_TYPE_SCALE))
	{
		cairo_pattern_t *pattern, *border_pattern;
		cairo_matrix_t matrix;
		GtkStateFlags state;
		GdkRGBA color;
		gdouble shade_factor = 2.0 / 3.0;
		gboolean marks_above = FALSE, marks_below = FALSE;

		if (gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_SCALE_HAS_MARKS_ABOVE)) {
			marks_above = TRUE;
		} else if (gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_SCALE_HAS_MARKS_BELOW)) {
			marks_below = TRUE;
		}

		cairo_translate (cr, x, y);

		if ((marks_above && marks_below) || (!marks_above && !marks_below)) {
			draw_round_slider (cr, width, height);
		} else {
			draw_mark_slider (cr, width, height, marks_below, orientation);
		}

		state = gtk_theming_engine_get_state (engine);
		cairo_set_line_width (cr, 1.0);

		gtk_theming_engine_get (engine, state,
					"background-image", &pattern,
					NULL);

		cairo_matrix_init_scale (&matrix, 1 / width, 1 / height);
		cairo_pattern_set_matrix (pattern, &matrix);
		cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);

		cairo_set_source (cr, pattern);
		cairo_fill_preserve (cr);

		gtk_theming_engine_get_border_color (engine, state, &color);

		if (orientation == GTK_ORIENTATION_HORIZONTAL) {
			border_pattern = cairo_pattern_create_linear (0, 0, 0, height);
		} else {
			border_pattern = cairo_pattern_create_linear (width, 0, 0, 0);
		}

		cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);

		cairo_pattern_add_color_stop_rgba (border_pattern,
						   marks_above ? 0.0 : 1.0,
						   color.red / shade_factor,
						   color.green / shade_factor,
						   color.blue / shade_factor,
						   color.alpha);

		cairo_pattern_add_color_stop_rgba (border_pattern,
						   marks_above ? 1.0 : 0.0,
						   color.red,
						   color.green,
						   color.blue,
						   color.alpha);

		cairo_set_source (cr, border_pattern);
		cairo_stroke (cr);

		cairo_pattern_destroy (pattern);
		cairo_pattern_destroy (border_pattern);
	}
	else
	{
		GTK_THEMING_ENGINE_CLASS (adwaita_engine_parent_class)->render_slider (engine, cr,
										       x, y, width, height,
										       orientation);
	}

	cairo_restore (cr);
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
	} else if (gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_PANE_SEPARATOR)) {
		GdkRGBA bg, fg;
		GtkStateFlags state;
		gdouble xx, yy;

		state = gtk_theming_engine_get_state (engine);
		gtk_theming_engine_get_background_color (engine, state, &bg);
		gtk_theming_engine_get_color (engine, state, &fg);

		cairo_save (cr);

		cairo_rectangle (cr, x, y,
				 width, height);
		gdk_cairo_set_source_rgba (cr, &bg);
		cairo_fill (cr);

		gdk_cairo_set_source_rgba (cr, &fg);
		cairo_set_line_width (cr, 2.0);
		cairo_line_to (cr, x + width, y + height);
		cairo_stroke (cr);

		if (width > height) {
			for (xx = x + width / 2 - 12; xx <= x + width / 2 + 12; xx += 6) {
				cairo_arc (cr, xx, y + height / 2 - 1,
					   1.5,
					   0, G_PI * 2.0);
				cairo_fill (cr);
			}
		} else {
			for (yy = y + height / 2 - 12; yy <= y + height / 2 + 12; yy += 6) {
				cairo_arc (cr, x + width / 2.0, yy,
					   1.0,
					   0, G_PI * 2.0);
				cairo_fill (cr);
			}
		}

		cairo_restore (cr);
	} else {
		GTK_THEMING_ENGINE_CLASS (adwaita_engine_parent_class)->render_handle (engine, cr,
										       x, y, width, height);
	}
}

/* taken from gtkthemingengine.c */
static GdkPixbuf *
scale_or_ref (GdkPixbuf *src,
              gint       width,
              gint       height)
{
	if (width == gdk_pixbuf_get_width (src) &&
	    height == gdk_pixbuf_get_height (src)) {
		return g_object_ref (src);
	} else {
		return gdk_pixbuf_scale_simple (src,
						width, height,
						GDK_INTERP_BILINEAR);
	}
}

static gboolean
lookup_icon_size (GtkThemingEngine *engine,
		  GtkIconSize       size,
		  gint             *width,
		  gint             *height)
{
	GdkScreen *screen;
	GtkSettings *settings;

	screen = gtk_theming_engine_get_screen (engine);
	settings = gtk_settings_get_for_screen (screen);

	return gtk_icon_size_lookup_for_settings (settings, size, width, height);
}

/* Kudos to the gnome-panel guys. */
static void
colorshift_pixbuf (GdkPixbuf *src,
                   GdkPixbuf *dest,
                   gint       shift)
{
	gint i, j;
	gint width, height, has_alpha, src_rowstride, dest_rowstride;
	guchar *target_pixels;
	guchar *original_pixels;
	guchar *pix_src;
	guchar *pix_dest;
	int val;
	guchar r, g, b;

	has_alpha       = gdk_pixbuf_get_has_alpha (src);
	width           = gdk_pixbuf_get_width (src);
	height          = gdk_pixbuf_get_height (src);
	src_rowstride   = gdk_pixbuf_get_rowstride (src);
	dest_rowstride  = gdk_pixbuf_get_rowstride (dest);
	original_pixels = gdk_pixbuf_get_pixels (src);
	target_pixels   = gdk_pixbuf_get_pixels (dest);

	for (i = 0; i < height; i++) {
		pix_dest = target_pixels   + i * dest_rowstride;
		pix_src  = original_pixels + i * src_rowstride;

		for (j = 0; j < width; j++) {
			r = *(pix_src++);
			g = *(pix_src++);
			b = *(pix_src++);

			val = r + shift;
			*(pix_dest++) = CLAMP (val, 0, 255);

			val = g + shift;
			*(pix_dest++) = CLAMP (val, 0, 255);

			val = b + shift;
			*(pix_dest++) = CLAMP (val, 0, 255);

			if (has_alpha) {
				*(pix_dest++) = *(pix_src++);
			}
		}
	}
}

static GdkPixbuf *
adwaita_engine_render_icon_pixbuf (GtkThemingEngine    *engine,
				   const GtkIconSource *source,
				   GtkIconSize          size)
{
	GdkPixbuf *base_pixbuf;
	GdkPixbuf *scaled;
	GdkPixbuf *stated;
	GtkStateFlags state;
	gint width = 1;
	gint height = 1;

	cairo_surface_t *stated_surface;
	cairo_t *cr;

	base_pixbuf = gtk_icon_source_get_pixbuf (source);
	state = gtk_theming_engine_get_state (engine);

	g_return_val_if_fail (base_pixbuf != NULL, NULL);

	if (size != (GtkIconSize) -1 &&
	    !lookup_icon_size (engine, size, &width, &height)) {
		g_warning (G_STRLOC ": invalid icon size '%d'", size);
		return NULL;
	}

	/* If the size was wildcarded, and we're allowed to scale, then scale; otherwise,
	 * leave it alone.
	 */
	if (size != (GtkIconSize) -1 &&
	    gtk_icon_source_get_size_wildcarded (source)) {
		scaled = scale_or_ref (base_pixbuf, width, height);
	} else {
		scaled = g_object_ref (base_pixbuf);
	}

	/* If the state was wildcarded, then generate a state. */
	if (gtk_icon_source_get_state_wildcarded (source)) {
		if (state & GTK_STATE_FLAG_INSENSITIVE) {
			/* dim the pixbuf with a 0.5 alpha black layer */
			stated_surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32,
								     gdk_pixbuf_get_width (scaled),
								     gdk_pixbuf_get_height (scaled));
			cr = cairo_create (stated_surface);

			gdk_cairo_set_source_pixbuf (cr, scaled, 0, 0);
			cairo_paint (cr);

			cairo_set_source_rgba (cr, 0.0, 0.0, 0.0, 0.5);
			cairo_set_operator (cr, CAIRO_OPERATOR_DEST_IN);
			cairo_paint (cr);

			stated = gdk_pixbuf_get_from_surface (stated_surface,
							      0, 0,
							      cairo_image_surface_get_width (stated_surface),
							      cairo_image_surface_get_height (stated_surface));

			g_object_unref (scaled);
			cairo_destroy (cr);
			cairo_surface_destroy (stated_surface);
		} else if (state & GTK_STATE_FLAG_PRELIGHT) {
			stated = gdk_pixbuf_copy (scaled);
			colorshift_pixbuf (scaled, stated, 30);
			g_object_unref (scaled);
		} else {
			stated = scaled;
		}
	} else {
		stated = scaled;
	}

	return stated;
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
	engine_class->render_icon_pixbuf = adwaita_engine_render_icon_pixbuf;

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
