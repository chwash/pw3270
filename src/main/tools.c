/*
 * "Software pw3270, desenvolvido com base nos códigos fontes do WC3270  e X3270
 * (Paul Mattes Paul.Mattes@usa.net), de emulação de terminal 3270 para acesso a
 * aplicativos mainframe. Registro no INPI sob o nome G3270. Registro no INPI sob
 * o nome G3270.
 *
 * Copyright (C) <2008> <Banco do Brasil S.A.>
 *
 * Este programa é software livre. Você pode redistribuí-lo e/ou modificá-lo sob
 * os termos da GPL v.2 - Licença Pública Geral  GNU,  conforme  publicado  pela
 * Free Software Foundation.
 *
 * Este programa é distribuído na expectativa de  ser  útil,  mas  SEM  QUALQUER
 * GARANTIA; sem mesmo a garantia implícita de COMERCIALIZAÇÃO ou  de  ADEQUAÇÃO
 * A QUALQUER PROPÓSITO EM PARTICULAR. Consulte a Licença Pública Geral GNU para
 * obter mais detalhes.
 *
 * Você deve ter recebido uma cópia da Licença Pública Geral GNU junto com este
 * programa; se não, escreva para a Free Software Foundation, Inc., 51 Franklin
 * St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Este programa está nomeado como main.c e possui - linhas de código.
 *
 * Contatos:
 *
 * perry.werneck@gmail.com	(Alexandre Perry de Souza Werneck)
 * erico.mendonca@gmail.com	(Erico Mascarenhas Mendonça)
 *
 */

/**
 * @brief Misc tools for pw3270 application.
 *
 */

#include "private.h"
#include <pw3270.h>
#include <pw3270/application.h>
#include <pw3270/settings.h>
#include <pw3270/window.h>

/*---[ Implement ]----------------------------------------------------------------------------------*/

GtkBuilder * pw3270_application_get_builder(const gchar *name) {

#ifdef DEBUG
	g_autofree gchar * filename = g_build_filename("ui",name,NULL);
#else
	lib3270_autoptr(char) filename = lib3270_build_data_filename("ui",name,NULL);
#endif // DEBUG

	return gtk_builder_new_from_file(filename);
}

void gtk_container_remove_all(GtkContainer *container) {

	GList * children = gtk_container_get_children(container);
	GList * item;

	for(item = children; item; item = g_list_next(item)) {
		gtk_container_remove(container,GTK_WIDGET(item->data));
	}

	g_list_free(children);

}

void gtk_file_chooser_set_pw3270_filters(GtkFileChooser *chooser) {

	static const struct Filter {
		const gchar * name;
		const gchar * pattern;
	} filters[] = {
		{
			.name = N_("TN3270 Session Files"),
			.pattern = "*.3270"
		},
		{
			.name = N_("All files"),
			.pattern = "*.*"
		}
	};

	size_t ix;

	for(ix = 0; ix < G_N_ELEMENTS(filters); ix++) {
		GtkFileFilter *filter = gtk_file_filter_new();
		gtk_file_filter_add_pattern (filter, filters[ix].pattern);
		gtk_file_filter_set_name(filter, filters[ix].name);
		gtk_file_chooser_add_filter(chooser,filter);
	}

}

GtkWidget * pw3270_get_active_terminal() {

	GApplication *app = g_application_get_default();
	g_return_val_if_fail(GTK_IS_APPLICATION(app),NULL);

	GtkWindow * window = gtk_application_get_active_window(GTK_APPLICATION(app));

	return pw3270_application_window_get_active_terminal(GTK_WIDGET(window));

}

H3270 * pw3270_get_active_session() {

	GApplication *app = g_application_get_default();

	g_return_val_if_fail(GTK_IS_APPLICATION(app),NULL);

	GtkWindow * window = gtk_application_get_active_window(GTK_APPLICATION(app));

	return pw3270_window_get_session_handle(GTK_WIDGET(window));

}
