/* gtk-exif-entry-resolution.c
 *
 * Copyright � 2001 Lutz M�ller <lutz@users.sourceforge.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details. 
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "config.h"
#include "gtk-exif-entry-resolution.h"

#include <string.h>

#include <gtk/gtkspinbutton.h>
#include <gtk/gtkradiobutton.h>
#include <gtk/gtkvbox.h>
#include <gtk/gtkhbox.h>
#include <gtk/gtksignal.h>
#include <gtk/gtkframe.h>
#include <gtk/gtkoptionmenu.h>
#include <gtk/gtkmenuitem.h>
#include <gtk/gtkmenu.h>
#include <gtk/gtklabel.h>

#include <libexif/exif-utils.h>

#include "gtk-exif-util.h"

#ifdef ENABLE_NLS
#  include <libintl.h>
#  undef _
#  define _(String) dgettext (PACKAGE, String)
#  ifdef gettext_noop
#    define N_(String) gettext_noop (String)
#  else
#    define N_(String) (String)
#  endif
#else
#  define textdomain(String) (String)
#  define gettext(String) (String)
#  define dgettext(Domain,Message) (Message)
#  define dcgettext(Domain,Message,Type) (Message)
#  define bindtextdomain(Domain,Directory) (Domain)
#  define _(String) (String)
#  define N_(String) (String)
#endif

typedef struct _ResolutionObjects ResolutionObjects;
struct _ResolutionObjects
{
	GtkToggleButton *check;
	GtkWidget *sp, *sq;
	GtkAdjustment *ap, *aq;
};

typedef struct _ResolutionUnitObjects ResolutionUnitObjects;
struct _ResolutionUnitObjects
{
	GtkToggleButton *check;
	GtkOptionMenu *menu;
};

struct _GtkExifEntryResolutionPrivate
{
	ExifContent *content;

	GtkToggleButton *check;
	ResolutionObjects ox, oy;
	ResolutionUnitObjects u;

	ExifTag tag_x, tag_y, tag_u;
};

#define PARENT_TYPE GTK_EXIF_TYPE_ENTRY
static GtkExifEntryClass *parent_class;

static void
gtk_exif_entry_resolution_destroy (GtkObject *object)
{
	GtkExifEntryResolution *entry = GTK_EXIF_ENTRY_RESOLUTION (object);

	if (entry->priv->content) {
		exif_content_unref (entry->priv->content);
		entry->priv->content = NULL;
	}

	GTK_OBJECT_CLASS (parent_class)->destroy (object);
}

GTK_EXIF_FINALIZE (entry_resolution, EntryResolution)

static void
gtk_exif_entry_resolution_class_init (gpointer g_class, gpointer class_data)
{
	GtkObjectClass *object_class;
	GObjectClass *gobject_class;

	object_class = GTK_OBJECT_CLASS (g_class);
	object_class->destroy  = gtk_exif_entry_resolution_destroy;

	gobject_class = G_OBJECT_CLASS (g_class);
	gobject_class->finalize = gtk_exif_entry_resolution_finalize;

	parent_class = g_type_class_peek_parent (g_class);
}

static void
gtk_exif_entry_resolution_init (GTypeInstance *instance, gpointer g_class)
{
	GtkExifEntryResolution *entry = GTK_EXIF_ENTRY_RESOLUTION (instance);

	entry->priv = g_new0 (GtkExifEntryResolutionPrivate, 1);
}

GTK_EXIF_CLASS (entry_resolution, EntryResolution, "EntryResolution")

static void
on_inch_activate (GtkMenuItem *item, GtkExifEntryResolution *entry)
{
	ExifEntry *e;
	ExifByteOrder o;

	e = exif_content_get_entry (entry->priv->content,
				    entry->priv->tag_u);
	g_return_if_fail (e != NULL);
	o = exif_data_get_byte_order (e->parent->parent);
	exif_set_short (e->data, o, 2);
	gtk_exif_entry_changed (GTK_EXIF_ENTRY (entry), e);
}

static void
on_centimeter_activate (GtkMenuItem *item, GtkExifEntryResolution *entry)
{
	ExifEntry *e;
	ExifByteOrder o;

	e = exif_content_get_entry (entry->priv->content,
				    entry->priv->tag_u);
	g_return_if_fail (e != NULL);
	o = exif_data_get_byte_order (e->parent->parent);
	exif_set_short (e->data, o, 3);
	gtk_exif_entry_changed (GTK_EXIF_ENTRY (entry), e);
}

static void
on_w_value_changed (GtkAdjustment *a, GtkExifEntryResolution *entry)
{
	ExifEntry *e;
	ExifRational r;
	ExifSRational sr;
	ExifByteOrder o;

	e = exif_content_get_entry (entry->priv->content,
				    entry->priv->tag_x);
	g_return_if_fail (e != NULL);
	o = exif_data_get_byte_order (e->parent->parent);
	switch (e->format) {
	case EXIF_FORMAT_RATIONAL:
		r.numerator   = entry->priv->ox.ap->value;
		r.denominator = entry->priv->ox.aq->value;
		exif_set_rational (e->data, o, r);
		break;
	case EXIF_FORMAT_SRATIONAL:
		sr.numerator   = entry->priv->ox.ap->value;
		sr.denominator = entry->priv->ox.aq->value;
		exif_set_srational (e->data, o, sr);
		break;
	default:
		g_warning ("Invalid format!");
		return;
	}
	gtk_exif_entry_changed (GTK_EXIF_ENTRY (entry), e);
}

static void
on_h_value_changed (GtkAdjustment *a, GtkExifEntryResolution *entry)
{
	ExifEntry *e;
	ExifRational r;
	ExifSRational sr;
	ExifByteOrder o;

	e = exif_content_get_entry (entry->priv->content,
				    entry->priv->tag_y);
	g_return_if_fail (e != NULL);
	o = exif_data_get_byte_order (e->parent->parent);
	switch (e->format) {
	case EXIF_FORMAT_RATIONAL:
		r.numerator   = entry->priv->oy.ap->value;
		r.denominator = entry->priv->oy.aq->value;
		exif_set_rational (e->data, o, r);
		break;
	case EXIF_FORMAT_SRATIONAL:
		sr.numerator   = entry->priv->oy.ap->value;
		sr.denominator = entry->priv->oy.aq->value;
		exif_set_srational (e->data, o, sr);
		break;
	default:
		g_warning ("Invalid format!");
		return;
	}
	gtk_exif_entry_changed (GTK_EXIF_ENTRY (entry), e);
}

static void
gtk_exif_entry_resolution_load_unit (GtkExifEntryResolution *entry,
				     ExifEntry *e)
{
	ExifByteOrder o;

	o = exif_data_get_byte_order (e->parent->parent);
	switch (e->format) {
	case EXIF_FORMAT_SHORT:
		switch (exif_get_short (e->data, o)) {
		case 2:
			gtk_option_menu_set_history (entry->priv->u.menu, 1);
			break;
		case 3:
			gtk_option_menu_set_history (entry->priv->u.menu, 0);
			break;
		default:
			g_warning ("Invalid unit!");
		}
		break;
	default:
		g_warning ("Invalid format!");
	}
}

static void
gtk_exif_entry_resolution_load (GtkExifEntryResolution *entry, ExifEntry *e)
{
	ExifRational  r;
	ExifSRational sr;
	ResolutionObjects o;
	ExifByteOrder order;

	g_return_if_fail (GTK_EXIF_IS_ENTRY_RESOLUTION (entry));
	g_return_if_fail (e != NULL);

	switch (e->tag) {
	case EXIF_TAG_X_RESOLUTION:
	case EXIF_TAG_FOCAL_PLANE_X_RESOLUTION:
		o = entry->priv->ox;
		break;
	case EXIF_TAG_Y_RESOLUTION:
	case EXIF_TAG_FOCAL_PLANE_Y_RESOLUTION:
		o = entry->priv->oy;
		break;
	default:
		g_warning ("Invalid tag!");
		return;
	}

	g_signal_handlers_block_matched (G_OBJECT (o.ap),
			G_SIGNAL_MATCH_DATA, 0, 0, NULL, NULL, entry);
	g_signal_handlers_block_matched (G_OBJECT (o.aq),
			G_SIGNAL_MATCH_DATA, 0, 0, NULL, NULL, entry);
	order = exif_data_get_byte_order (e->parent->parent);
	switch (e->format) {
	case EXIF_FORMAT_RATIONAL:
		r = exif_get_rational (e->data, order);
		gtk_adjustment_set_value (o.ap, r.numerator);
		gtk_adjustment_set_value (o.aq, r.denominator);
		break;
	case EXIF_FORMAT_SRATIONAL:
		sr = exif_get_srational (e->data, order);
		gtk_adjustment_set_value (o.ap, sr.numerator);
		gtk_adjustment_set_value (o.aq, sr.denominator);
		break;
	default:
		g_warning ("Invalid format!");
		break;
	}
	g_signal_handlers_unblock_matched (G_OBJECT (o.ap),
			G_SIGNAL_MATCH_DATA, 0, 0, NULL, NULL, entry);
	g_signal_handlers_unblock_matched (G_OBJECT (o.aq),
			G_SIGNAL_MATCH_DATA, 0, 0, NULL, NULL, entry);
}

static void
on_cw_toggled (GtkToggleButton *toggle, GtkExifEntryResolution *entry)
{
	ExifEntry *e;

	e = exif_content_get_entry (entry->priv->content,
				    entry->priv->tag_x);
	gtk_widget_set_sensitive (entry->priv->ox.sp, toggle->active);
	gtk_widget_set_sensitive (entry->priv->ox.sq, toggle->active);
	if (toggle->active && !e) { 
		e = exif_entry_new ();
		exif_content_add_entry (entry->priv->content, e);
		exif_entry_initialize (e, entry->priv->tag_x);
		gtk_exif_entry_resolution_load (entry, e);
		exif_entry_unref (e);
		gtk_exif_entry_added (GTK_EXIF_ENTRY (entry), e);
	} else if (!toggle->active && e) {
		g_object_ref (entry);
		gtk_exif_entry_removed (GTK_EXIF_ENTRY (entry), e);
		exif_content_remove_entry (entry->priv->content, e);
		g_object_unref (entry);
	}
}

static void
on_ch_toggled (GtkToggleButton *toggle, GtkExifEntryResolution *entry)
{
        ExifEntry *e;

        e = exif_content_get_entry (entry->priv->content,
                                    entry->priv->tag_y);
	gtk_widget_set_sensitive (entry->priv->oy.sp, toggle->active);
	gtk_widget_set_sensitive (entry->priv->oy.sq, toggle->active);
        if (toggle->active && !e) {
		e = exif_entry_new ();
		exif_content_add_entry (entry->priv->content, e);
		exif_entry_initialize (e, entry->priv->tag_y);
		gtk_exif_entry_resolution_load (entry, e);
		exif_entry_unref (e);
		gtk_exif_entry_added (GTK_EXIF_ENTRY (entry), e);
        } else if (!toggle->active && e) {
		g_object_ref (entry);
		gtk_exif_entry_removed (GTK_EXIF_ENTRY (entry), e);
		exif_content_remove_entry (entry->priv->content, e);
		g_object_unref (entry);
        }
}

static void
on_unit_toggled (GtkToggleButton *toggle, GtkExifEntryResolution *entry)
{
	ExifEntry *e;

	e = exif_content_get_entry (entry->priv->content,
				    entry->priv->tag_u);
	gtk_widget_set_sensitive (GTK_WIDGET (entry->priv->u.menu),
				  toggle->active);
	if (toggle->active && !e) {
		e = exif_entry_new ();
		exif_content_add_entry (entry->priv->content, e);
		exif_entry_initialize (e, entry->priv->tag_u);
		gtk_exif_entry_resolution_load_unit (entry, e);
		exif_entry_unref (e);
		gtk_exif_entry_added (GTK_EXIF_ENTRY (entry), e);
	} else if (!toggle->active && e) {
		g_object_ref (entry);
		gtk_exif_entry_removed (GTK_EXIF_ENTRY (entry), e);
		exif_content_remove_entry (entry->priv->content, e);
		g_object_unref (entry);
	}
}

GtkWidget *
gtk_exif_entry_resolution_new (ExifContent *content, gboolean focal_plane)
{
	GtkExifEntryResolution *entry;
	GtkWidget *hbox, *sp, *sq, *label, *menu, *item, *o, *c;
	GtkObject *ap, *aq;
	ExifEntry *e;

	g_return_val_if_fail (content != NULL, NULL);

	entry = g_object_new (GTK_EXIF_TYPE_ENTRY_RESOLUTION, NULL);
	entry->priv->content = content;
	exif_content_ref (content);

	if (focal_plane) {
		gtk_exif_entry_construct (GTK_EXIF_ENTRY (entry), 
			_("Focal Plane Resolution"),
			_("The number of pixels on the camera focal plane."));
		entry->priv->tag_x = EXIF_TAG_FOCAL_PLANE_X_RESOLUTION;
		entry->priv->tag_y = EXIF_TAG_FOCAL_PLANE_Y_RESOLUTION;
		entry->priv->tag_u = EXIF_TAG_FOCAL_PLANE_RESOLUTION_UNIT;
	} else {
		gtk_exif_entry_construct (GTK_EXIF_ENTRY (entry),
			_("Resolution"),
			_("The number of pixels per unit."));
		entry->priv->tag_x = EXIF_TAG_X_RESOLUTION;
		entry->priv->tag_y = EXIF_TAG_Y_RESOLUTION;
		entry->priv->tag_u = EXIF_TAG_RESOLUTION_UNIT;
	}

	/* Width */
	e = exif_content_get_entry (content, entry->priv->tag_x);
	hbox = gtk_hbox_new (FALSE, 5);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (entry), hbox, TRUE, FALSE, 0);
	c = gtk_check_button_new_with_label (_("Image width direction:"));
	gtk_widget_show (c);
	gtk_box_pack_start (GTK_BOX (hbox), c, FALSE, FALSE, 0);
	entry->priv->ox.check = GTK_TOGGLE_BUTTON (c);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (c), (e != NULL));
	g_signal_connect (GTK_OBJECT (c), "toggled",
			    G_CALLBACK (on_cw_toggled), entry);
	ap = gtk_adjustment_new (0, 0, 0xffffffff, 1, 0xffff, 0);
	entry->priv->ox.ap = GTK_ADJUSTMENT (ap);
	sp = gtk_spin_button_new (GTK_ADJUSTMENT (ap), 0, 0);
	gtk_widget_show (sp);
	gtk_box_pack_start (GTK_BOX (hbox), sp, TRUE, TRUE, 0);
	gtk_widget_set_sensitive (sp, (e != NULL));
	entry->priv->ox.sp = sp;
	g_signal_connect (ap, "value_changed",
			    G_CALLBACK (on_w_value_changed), entry);
	label = gtk_label_new ("/");
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
	aq = gtk_adjustment_new (0, 0, 0xffffffff, 1, 0xffff, 0);
	entry->priv->ox.aq = GTK_ADJUSTMENT (aq);
	sq = gtk_spin_button_new (GTK_ADJUSTMENT (aq), 0, 0);
	gtk_widget_show (sq);
	gtk_box_pack_start (GTK_BOX (hbox), sq, TRUE, TRUE, 0);
	gtk_widget_set_sensitive (sq, (e != NULL));
	entry->priv->ox.sq = sq;
	g_signal_connect (aq, "value_changed",
			    G_CALLBACK (on_w_value_changed), entry);
	if (e)
		gtk_exif_entry_resolution_load (entry, e);

	/* Height */
	e = exif_content_get_entry (content, entry->priv->tag_y);
	hbox = gtk_hbox_new (FALSE, 5);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (entry), hbox, TRUE, FALSE, 0);
	c = gtk_check_button_new_with_label (_("Image height direction:"));
	gtk_widget_show (c);
	gtk_box_pack_start (GTK_BOX (hbox), c, FALSE, FALSE, 0);
	entry->priv->oy.check = GTK_TOGGLE_BUTTON (c);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (c), (e != NULL));
	g_signal_connect (GTK_OBJECT (c), "toggled",
			    G_CALLBACK (on_ch_toggled), entry);
	ap = gtk_adjustment_new (0, 0, 0xffffffff, 1, 0xffff, 0);
	entry->priv->oy.ap = GTK_ADJUSTMENT (ap);
	sp = gtk_spin_button_new (GTK_ADJUSTMENT (ap), 0, 0);
	gtk_widget_show (sp);
	gtk_box_pack_start (GTK_BOX (hbox), sp, TRUE, TRUE, 0);
	entry->priv->oy.sp = sp;
	gtk_widget_set_sensitive (sp, (e != NULL));
	g_signal_connect (ap, "value_changed",
			    G_CALLBACK (on_h_value_changed), entry);
	label = gtk_label_new ("/");
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
	aq = gtk_adjustment_new (0, 0, 0xffffffff, 1, 0xffff, 0);
	entry->priv->oy.aq = GTK_ADJUSTMENT (aq);
	sq = gtk_spin_button_new (GTK_ADJUSTMENT (aq), 0, 0);
	gtk_widget_show (sq);
	gtk_box_pack_start (GTK_BOX (hbox), sq, TRUE, TRUE, 0);
	entry->priv->oy.sq = sq;
	gtk_widget_set_sensitive (sq, (e != NULL));
	g_signal_connect (aq, "value_changed",
			    G_CALLBACK (on_h_value_changed), entry);
	if (e)
		gtk_exif_entry_resolution_load (entry, e);

	/* Unit */
	e = exif_content_get_entry (content, entry->priv->tag_u);
	hbox = gtk_hbox_new (FALSE, 5);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (entry), hbox, TRUE, FALSE, 0);
	c = gtk_check_button_new_with_label (_("Unit:"));
	gtk_widget_show (c);
	gtk_box_pack_start (GTK_BOX (hbox), c, FALSE, FALSE, 0);
	entry->priv->check = GTK_TOGGLE_BUTTON (c);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (c), (e != NULL));
	g_signal_connect (GTK_OBJECT (c), "toggled",
			  G_CALLBACK (on_unit_toggled), entry);
	o = gtk_option_menu_new ();
	gtk_widget_show (o);
	gtk_box_pack_start (GTK_BOX (hbox), o, TRUE, TRUE, 0);
	entry->priv->u.menu = GTK_OPTION_MENU (o);
	menu = gtk_menu_new ();
	gtk_widget_show (menu);
	item = gtk_menu_item_new_with_label (_("Centimeter"));
	gtk_widget_show (item);
	gtk_container_add (GTK_CONTAINER (menu), item);
	g_signal_connect (GTK_OBJECT (item), "activate",
			  G_CALLBACK (on_centimeter_activate), entry);
	item = gtk_menu_item_new_with_label (_("Inch"));
	gtk_widget_show (item);
	gtk_container_add (GTK_CONTAINER (menu), item);
	g_signal_connect (GTK_OBJECT (item), "activate",
			  G_CALLBACK (on_inch_activate), entry);
	gtk_option_menu_set_menu (GTK_OPTION_MENU (o), menu);
	if (e)
		gtk_exif_entry_resolution_load_unit (entry, e);

	return (GTK_WIDGET (entry));
}
