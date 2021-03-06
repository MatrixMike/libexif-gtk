/* gtk-exif-tree-item.c
 *
 * Copyright © 2001 Lutz Müller <lutz@users.sourceforge.net>
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
#include "gtk-exif-tree-item.h"

#include <gtk/gtklabel.h>
#include <gtk/gtktree.h>
#include <gtk/gtklabel.h>
#include <gtk/gtksignal.h>

#include "gtk-exif-util.h"

struct _GtkExifTreeItemPrivate
{
};

#define PARENT_TYPE GTK_TYPE_TREE_ITEM
static GtkTreeItemClass *parent_class;

static void
gtk_exif_tree_item_destroy (GtkObject *object)
{
	GtkExifTreeItem *item = GTK_EXIF_TREE_ITEM (object);

	if (item->entry) {
		exif_entry_unref (item->entry);
		item->entry = NULL;
	}

	GTK_OBJECT_CLASS (parent_class)->destroy (object);
}

GTK_EXIF_FINALIZE (tree_item, TreeItem)

static void
gtk_exif_tree_item_class_init (gpointer g_class, gpointer class_data)
{
	GtkObjectClass *object_class;

	object_class = GTK_OBJECT_CLASS (klass);
	object_class->destroy  = gtk_exif_tree_item_destroy;
	object_class->finalize = gtk_exif_tree_item_finalize;

	parent_class = g_type_class_peek_parent (g_class);
}

static void
gtk_exif_tree_item_init (GtkExifTreeItem *item)
{
	item->priv = g_new0 (GtkExifTreeItemPrivate, 1);
}

GTK_EXIF_CLASS (tree_item, TreeItem, "TreeItem")

static void
fill_tree (GtkTree *tree, ExifContent *content)
{
	GtkWidget *item;
	guint i;
	ExifEntry *entry;

	for (i = 0; i < content->count; i++) {
		entry = content->entries[i];
		item = gtk_exif_tree_item_new ();
		gtk_widget_show (item);
		gtk_tree_append (tree, item);
		gtk_exif_tree_item_set_entry (GTK_EXIF_TREE_ITEM (item),
					      entry);
	}
}

GtkWidget *
gtk_exif_tree_item_new (void)
{
	GtkExifTreeItem *item;

	item = gtk_type_new (GTK_EXIF_TYPE_TREE_ITEM);

	return (GTK_WIDGET (item));
}

static void
on_select_child (GtkTree *tree, GtkWidget *item)
{
	GtkExifTreeItem *eitem = GTK_EXIF_TREE_ITEM (item);

	gtk_signal_emit_by_name (GTK_OBJECT (tree->root_tree),
				 "entry_selected", eitem->entry);
}

void
gtk_exif_tree_item_set_entry (GtkExifTreeItem *item, ExifEntry *entry)
{
	GtkWidget *tree, *label;

	g_return_if_fail (GTK_EXIF_IS_TREE_ITEM (item));
	g_return_if_fail (entry != NULL);

	/* Keep a reference to this entry */
	if (item->entry)
		exif_entry_unref (item->entry);
	item->entry = entry;
	exif_entry_ref (entry);

	label = gtk_label_new (
		  exif_tag_get_name_in_ifd (entry->tag, exif_content_get_ifd(entry->parent));
	gtk_widget_show (label);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment (GTK_MISC (label), 0, 0);
	gtk_container_add (GTK_CONTAINER (item), label);

	if (entry->content->count) {
		tree = gtk_tree_new ();
		gtk_widget_show (tree);
		gtk_tree_item_set_subtree (GTK_TREE_ITEM (item), tree);
		fill_tree (GTK_TREE (tree), entry->content);

		/* Bug in GtkTree? */
		gtk_signal_connect (GTK_OBJECT (tree), "select_child",
				    GTK_SIGNAL_FUNC (on_select_child), NULL);
	}
}
