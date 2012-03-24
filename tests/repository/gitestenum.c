#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <glib.h>
#include "girparser.h"
#include "girmodule.h"
#include "girnode.h"
#include "gitypelib-internal.h"
#include "config.h"

#include <errno.h>
#include <string.h>

#include <glib.h>
#include <glib/gstdio.h>
#include <gio/gio.h>

#ifdef G_OS_WIN32
#include <io.h>
#include <fcntl.h>
#endif

#include "girmodule.h"
#include "girnode.h"
#include "girparser.h"
#include "gitypelib-internal.h"

GIrNodeEnum *
start_enum (
            GIrModule *current_module,
            const gchar  * name,       // ResolverError
            const gchar  * typename,   // GResolverError
            const gchar *typeinit,     // g_resolver_error_get_type
            const gchar *error_domain, // g-resolver-error-quark
            int   deprecated // is it deprecated?
            );



GIrNodeEnum *
start_enum (
            GIrModule *current_module,
            const gchar  * name,       // ResolverError
            const gchar  * typename,   // GResolverError
            const gchar *typeinit,     // g_resolver_error_get_type
            const gchar *error_domain, // g-resolver-error-quark
            int   deprecated // is it deprecated?
            )
{
  GIrNodeEnum *enum_;
  enum_ = (GIrNodeEnum *) _g_ir_node_new (G_IR_NODE_ENUM,
                                          current_module);

  ((GIrNode *)enum_)->name = g_strdup (name);
  enum_->gtype_name = g_strdup (typename);
  enum_->gtype_init = g_strdup (typeinit);
  enum_->error_domain = g_strdup (error_domain);

  if (deprecated)
    enum_->deprecated = TRUE;
  else
    enum_->deprecated = FALSE;

  current_module->entries =
    g_list_append (current_module->entries, enum_);

  return enum_;
}


void add_enum_field (GIrModule *current_module,
                     GIrNodeEnum * _enum, 
                     const gchar  * name,
                     const gchar  * c_identifier,
                     int  enumvalue,
                     int deprecated
                     );

void add_enum_field (GIrModule *current_module,
                     GIrNodeEnum * _enum, 
                     const gchar  * name,
                     const gchar  * c_identifier,
                     int  enumvalue,
                     int deprecated
                     )
{
  GIrNodeValue *value_;

  value_ = (GIrNodeValue *) _g_ir_node_new (G_IR_NODE_VALUE,
					   current_module);

  ((GIrNode *)value_)->name = g_strdup (name);

  value_->value = enumvalue;

  if (deprecated)
    value_->deprecated = TRUE;
  else
    value_->deprecated = FALSE;

  g_hash_table_insert (((GIrNode *)value_)->attributes,
                       g_strdup ("c:identifier"),
                       g_strdup (c_identifier));

  _enum->values = g_list_append (_enum->methods, value_);
}

static gboolean
write_out_typelib (gchar *prefix,
		   GITypelib *typelib)
{
  FILE *file;
  gsize written;
  GFile *file_obj;
  gchar *filename;
  GFile *tmp_file_obj;
  gchar *tmp_filename;
  GError *error = NULL;
  gboolean success = FALSE;

  filename = g_strdup_printf ("/tmp/%s-%s", "test", "enum.typelib");  

  g_fprintf (stderr, "goign to open '%s':\n",
             filename);

  file_obj = g_file_new_for_path (filename);

  tmp_filename = g_strdup_printf ("%s.tmp", filename);
  g_fprintf (stderr, "goign to open '%s':\n",
             tmp_filename);

  tmp_file_obj = g_file_new_for_path (tmp_filename);

  file = g_fopen (tmp_filename, "wb");

  g_fprintf (stderr, "goign to open '%s':\n",
             tmp_filename);

  if (file == NULL)
    {
      g_fprintf (stderr, "failed to open '%s': %s\n",
                 tmp_filename, g_strerror (errno));
      goto out;
    }


  written = fwrite (typelib->data, 1, typelib->len, file);
  if (written < typelib->len) {
    g_fprintf (stderr, "ERROR: Could not write the whole output: %s",
	       strerror(errno));
    goto out;
  }

                              fclose (file);

  if (tmp_filename != NULL)
    {
      if (!g_file_move (tmp_file_obj, file_obj, G_FILE_COPY_OVERWRITE, NULL, NULL, NULL, &error))
        {
	  g_fprintf (stderr, "ERROR: failed to rename %s to %s: %s", tmp_filename, filename, error->message);
          g_clear_error (&error);
	  goto out;
        }
    }
  success = TRUE;
out:
  g_free (filename);
  g_free (tmp_filename);

  return success;
}

int build_typelib (GIrModule *module);
int build_typelib (GIrModule *module)
{
  GITypelib *typelib;
  GError *error = NULL;
  typelib = _g_ir_module_build_typelib (module);
  if (typelib == NULL)
    g_error ("Failed to build typelib for module '%s'\n", module->name);

  if (!g_typelib_validate (typelib, &error))
    g_error ("Invalid typelib for module '%s': %s",
             module->name, error->message);

  if (!write_out_typelib (NULL, typelib))
    return 1;
  g_typelib_free (typelib);
  typelib = NULL;
}



int
main(int argc, char **argv)
{
  GIrNodeEnum *enum_;
  GIrModule *current_module;
  const gchar *module_name="Test";
  const gchar *module_version="fake-0.1";
  const gchar *shared_library="nothing.so";
  const gchar *cprefix="something";

  current_module = _g_ir_module_new (module_name,
                                     module_version, shared_library, cprefix);
  enum_ =  start_enum (current_module,
                       "ResolverError",
                       "GResolverError",
                       "g_resolver_error_get_type",
                       "g-resolver-error-quark",
                       0);


  add_enum_field (current_module,enum_, "Funky1","funky1",1,0);
  add_enum_field (current_module,enum_, "foo","foo",2,0);
  g_type_init() ;
  build_typelib (current_module);

  _g_ir_module_free (current_module);


  exit(0);
}
