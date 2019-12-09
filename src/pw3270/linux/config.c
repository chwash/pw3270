/*
 * "Software pw3270, desenvolvido com base nos códigos fontes do WC3270  e X3270
 * (Paul Mattes Paul.Mattes@usa.net), de emulação de terminal 3270 para acesso a
 * aplicativos mainframe. Registro no INPI sob o nome G3270.
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
 * Este programa está nomeado como config.c e possui - linhas de código.
 *
 * Contatos:
 *
 * perry.werneck@gmail.com	(Alexandre Perry de Souza Werneck)
 * erico.mendonca@gmail.com	(Erico Mascarenhas Mendonça)
 * licinio@bb.com.br		(Licínio Luis Branco)
 * kraucer@bb.com.br		(Kraucer Fernandes Mazuco)
 *
 */

#ifdef _WIN32
	#include <windows.h>
#endif // _WIN32

 #include "../pw3270/private.h"

 #include "../common.h"
 #include <stdarg.h>
 #include <glib/gstdio.h>

/*--[ Globals ]--------------------------------------------------------------------------------------*/

 static GKeyFile	* keyfile 		= NULL;
 static gchar		* keyfilename	= NULL;

/*--[ Implement ]------------------------------------------------------------------------------------*/

 static gchar * search_for_ini(void)
 {
 	static const gchar * (*dir[])(void) =
 	{
 		g_get_user_config_dir,
 		g_get_user_data_dir,
 		g_get_home_dir,
	};

	size_t f;
	g_autofree gchar * name = g_strconcat(g_get_application_name(),".conf",NULL);

	//
	// First search the user data
	//

 	for(f=0;f<G_N_ELEMENTS(dir);f++)
	{
		gchar *filename = g_build_filename(dir[f](),name,NULL);

		trace("Checking for %s",filename);

		if(g_file_test(filename,G_FILE_TEST_IS_REGULAR))
			return filename;
		g_free(filename);

	}

#ifdef DATADIR
	//
	// Search the application DATADIR
	//
	{
		gchar *filename = g_build_filename(DATAROOTDIR,G_STRINGIFY(PRODUCT_NAME),name,NULL);

		trace("Checking for default config \"%s\"",filename);

		if(g_file_test(filename,G_FILE_TEST_IS_REGULAR))
			return filename;

		g_message("Can't find default config (%s)",filename);

		g_free(filename);

	}
#endif // DATADIR

	//
	// Search the system config folders
	//
	const gchar * const * sysconfig = g_get_system_config_dirs();
 	for(f=0;sysconfig[f];f++)
	{
		gchar *filename = g_build_filename(sysconfig[f],name,NULL);
		trace("Checking for %s",filename);
		if(g_file_test(filename,G_FILE_TEST_IS_REGULAR))
			return filename;
		g_free(filename);
	}

	//
	// Search the system data folders
	//
	const gchar * const * sysdata = g_get_system_data_dirs();
 	for(f=0;sysdata[f];f++)
	{
		gchar *filename;

		// Check for product dir
		filename = g_build_filename(sysdata[f],G_STRINGIFY(PRODUCT_NAME),name,NULL);
		trace("Checking for system data \"%s\"",filename);
		if(g_file_test(filename,G_FILE_TEST_IS_REGULAR))
			return filename;
		g_free(filename);

		// Check for file
		filename = g_build_filename(sysdata[f],name,NULL);
		trace("Checking for system data \"%s\"",filename);
		if(g_file_test(filename,G_FILE_TEST_IS_REGULAR))
			return filename;
		g_free(filename);



	}

	//
	// Can't find, use user config dir
	//
	g_message("No config, defaulting to %s/%s",g_get_user_config_dir(),name);
 	return g_build_filename(g_get_user_config_dir(),name,NULL);

 }

 gboolean get_boolean_from_config(const gchar *group, const gchar *key, gboolean def)
 {

	if(keyfile)
	{
		GError		* err = NULL;
		gboolean	  val = g_key_file_get_boolean(keyfile,group,key,&err);
		if(err)
			g_error_free(err);
		else
			return val;
	}

	return def;
 }

 gint get_integer_from_config(const gchar *group, const gchar *key, gint def)
 {

	if(keyfile)
	{
		GError	* err = NULL;
		gint	  val = g_key_file_get_integer(keyfile,group,key,&err);
		if(err)
			g_error_free(err);
		else
			return val;
	}

	return def;
 }


 gchar * get_string_from_config(const gchar *group, const gchar *key, const gchar *def)
 {

	if(keyfile)
	{
		gchar *ret = g_key_file_get_string(keyfile,group,key,NULL);
		if(ret)
			return ret;
	}

	if(def)
		return g_strdup(def);

	return NULL;

 }

void pw3270_session_config_load(const gchar *filename)
{
	if(keyfilename)
	{
		g_free(keyfilename);
		keyfilename = NULL;
	}

	if(keyfile)
	{
		g_key_file_free(keyfile);
		keyfile = NULL;
	}

	keyfile = g_key_file_new();

	if(filename)
	{
		g_message(_("Loading %s"),filename);
		g_key_file_load_from_file(keyfile,filename,G_KEY_FILE_NONE,NULL);
		keyfilename = g_strdup(filename);
	}
	else
	{
		// Using default file.
		g_autofree gchar *name = search_for_ini();
		if(name)
		{
			g_message(_("Loading %s"),name);
			g_key_file_load_from_file(keyfile,name,G_KEY_FILE_NONE,NULL);
		}
	}

}

static void set_string(const gchar *group, const gchar *key, const gchar *fmt, va_list args)
{
	gchar * value = g_strdup_vprintf(fmt,args);

	g_key_file_set_string(keyfile,group,key,value);

	g_free(value);
}

void set_string_to_config(const gchar *group, const gchar *key, const gchar *fmt, ...)
{
	if(fmt)
	{
		va_list args;
		va_start(args, fmt);
		set_string(group,key,fmt,args);
		va_end(args);
	}
	else if(g_key_file_has_key(keyfile,group,key,NULL))
	{
		g_key_file_remove_key(keyfile,group,key,NULL);
	}

}

void set_boolean_to_config(const gchar *group, const gchar *key, gboolean val)
{
	if(keyfile)
		g_key_file_set_boolean(keyfile,group,key,val);

}

void set_integer_to_config(const gchar *group, const gchar *key, gint val)
{

	if(keyfile)
		g_key_file_set_integer(keyfile,group,key,val);

}

void pw3270_session_config_free(void)
{

	if(!keyfile)
		return;

	g_autofree gchar * text = g_key_file_to_data(keyfile,NULL,NULL);

	if(text)
	{
		GError * error = NULL;

		if(keyfilename)
		{
			g_message( _("Saving %s"), keyfilename);

			g_file_set_contents(keyfilename,text,-1,&error);
			g_free(keyfilename);
			keyfilename = 0;
		}
		else
		{
			g_autofree gchar * name = g_strconcat(g_get_application_name(),".conf",NULL);
			g_autofree gchar * filename = g_build_filename(g_get_user_config_dir(),name,NULL);
			g_mkdir_with_parents(g_get_user_config_dir(),S_IRUSR|S_IWUSR);
			g_file_set_contents(filename,text,-1,&error);
		}


		if(error) {
			g_message( _( "Can't save session settings: %s" ), error->message);
			g_error_free(error);
		}

	}

	g_key_file_free(keyfile);
	keyfile = NULL;

}

gchar * build_data_filename(const gchar *first_element, ...)
{
	va_list args;
	gchar	*path;

	va_start(args, first_element);
	path = filename_from_va(first_element,args);
	va_end(args);
	return path;
}

gchar * filename_from_va(const gchar *first_element, va_list args)
{
	const gchar * appname[]	= { g_get_application_name(), PACKAGE_NAME };
	size_t p,f;

	// Make base path
	const gchar *element;
	GString	* result = g_string_new("");
	for(element = first_element;element;element = va_arg(args, gchar *))
    {
    	g_string_append_c(result,G_DIR_SEPARATOR);
    	g_string_append(result,element);
    }

	g_autofree gchar * suffix = g_string_free(result, FALSE);

	// Check system data dirs
	const gchar * const * system_data_dirs = g_get_system_data_dirs();
	for(p=0;p<G_N_ELEMENTS(appname);p++)
	{
		for(f=0;system_data_dirs[f];f++)
		{
			gchar * path = g_build_filename(system_data_dirs[f],appname[p],suffix,NULL);
			trace("searching \"%s\"",path);
			if(g_file_test(path,G_FILE_TEST_EXISTS))
				return path;
			g_free(path);
		}

	}

	// Check current dir
	g_autofree gchar *dir = g_get_current_dir();
	gchar * path = g_build_filename(dir,suffix,NULL);
	trace("searching \"%s\"",path);
	if(g_file_test(path,G_FILE_TEST_EXISTS))
		return path;
	g_free(path);

	trace("Can't find \"%s\"",suffix);

	return g_build_filename(".",suffix,NULL);
}

GKeyFile * pw3270_session_config_get(void)
{
	if(!keyfile)
		pw3270_session_config_load(NULL);
	return keyfile;
}

 static const struct _WindowState
 {
        const char *name;
        GdkWindowState flag;
        void (*activate)(GtkWindow *);
 } WindowState[] =
 {
        { "Maximized",	GDK_WINDOW_STATE_MAXIMIZED,	gtk_window_maximize             },
        { "Iconified",	GDK_WINDOW_STATE_ICONIFIED,	gtk_window_iconify              },
        { "Sticky",		GDK_WINDOW_STATE_STICKY,	gtk_window_stick                }
 };

void save_window_state_to_config(const gchar *group, const gchar *key, GdkWindowState CurrentState)
{
	int			  f;
	GKeyFile	* conf 	= pw3270_session_config_get();
	gchar		* id	= g_strconcat(group,".",key,NULL);

	for(f=0;f<G_N_ELEMENTS(WindowState);f++)
		g_key_file_set_boolean(conf,id,WindowState[f].name,CurrentState & WindowState[f].flag);

	g_free(id);

}

void save_window_size_to_config(const gchar *group, const gchar *key, GtkWidget *hwnd)
{
	int 		  pos[2];
	GKeyFile	* conf 	= pw3270_session_config_get();
	gchar		* id	= g_strconcat(group,".",key,NULL);

	gtk_window_get_size(GTK_WINDOW(hwnd),&pos[0],&pos[1]);
	g_key_file_set_integer_list(conf,id,"size",pos,2);

	g_free(id);

}

void restore_window_from_config(const gchar *group, const gchar *key, GtkWidget *hwnd)
{
	gchar		* id	= g_strconcat(group,".",key,NULL);
	GKeyFile	* conf 	= pw3270_session_config_get();

	if(g_key_file_has_key(conf,id,"size",NULL))
	{
		gsize	  sz	= 2;
		gint	* vlr	=  g_key_file_get_integer_list(conf,id,"size",&sz,NULL);
		int		  f;

		if(vlr)
		{
			gtk_window_resize(GTK_WINDOW(hwnd),vlr[0],vlr[1]);
			g_free(vlr);
		}

		for(f=0;f<G_N_ELEMENTS(WindowState);f++)
		{
			if(g_key_file_get_boolean(conf,id,WindowState[f].name,NULL))
				WindowState[f].activate(GTK_WINDOW(hwnd));
		}

	}

	g_free(id);

}

