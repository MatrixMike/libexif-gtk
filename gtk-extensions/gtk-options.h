/* gtk-options.h
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

#ifndef __GTK_OPTIONS_H__
#define __GTK_OPTIONS_H__

#include <gtk/gtktreemodel.h>

typedef struct _GtkOptions GtkOptions;
struct _GtkOptions {
	guint option;
	const gchar *name;
};

void          gtk_options_sort (GtkOptions *);

enum {
	GTK_OPTIONS_OPTION_COLUMN,
	GTK_OPTIONS_NAME_COLUMN,
	GTK_OPTIONS_N_COLUMNS
};

GtkTreeModel *gtk_tree_model_new_from_options     (GtkOptions *);
gboolean      gtk_tree_model_get_iter_from_option (GtkTreeModel *, guint,
						   GtkTreeIter *);

#endif /* __GTK_OPTIONS_H__ */

