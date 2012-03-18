/*
  fork of gitestrepo.c for testing the writing
*/
#include "girepository.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <gio/gio.h>

#include <errno.h>
#include <string.h>
#include <stdio.h>

#include <glib.h>
#include <glib-object.h>
#include <glib/gstdio.h>

#include "girwriter.h"
#include "girepository.h"
#include "gitypelib-internal.h"

typedef struct {
  FILE *file;
  GSList *stack;
  gboolean show_all;
} Xml;

typedef struct {
  char *name;
  guint has_children : 1;
} XmlElement;


static XmlElement *
xml_element_new (const char *name)
{
  XmlElement *elem;

  elem = g_slice_new (XmlElement);
  elem->name = g_strdup (name);
  elem->has_children = FALSE;
  return elem;
}

static void
xml_element_free (XmlElement *elem)
{
  g_free (elem->name);
  g_slice_free (XmlElement, elem);
}

static void
xml_printf (Xml *xml, const char *fmt, ...)
{
  va_list ap;
  char *s;

  va_start (ap, fmt);
  s = g_markup_vprintf_escaped (fmt, ap);
  fputs (s, xml->file);
  g_free (s);
  va_end (ap);
}

static void
xml_start_element (Xml *xml, const char *element_name)
{
  XmlElement *parent = NULL;

  if (xml->stack)
    {
      parent = xml->stack->data;

      if (!parent->has_children)
        xml_printf (xml, ">\n");

      parent->has_children = TRUE;
    }

  xml_printf (xml, "%*s<%s", g_slist_length(xml->stack)*2, "", element_name);

  xml->stack = g_slist_prepend (xml->stack, xml_element_new (element_name));
}

static void
xml_end_element (Xml *xml, const char *name)
{
  XmlElement *elem;

  g_assert (xml->stack != NULL);

  elem = xml->stack->data;
  xml->stack = g_slist_delete_link (xml->stack, xml->stack);

  if (name != NULL)
    g_assert_cmpstr (name, ==, elem->name);

  if (elem->has_children)
    xml_printf (xml, "%*s</%s>\n", g_slist_length (xml->stack)*2, "", elem->name);
  else
    xml_printf (xml, "/>\n");

  xml_element_free (elem);
}

static void
xml_end_element_unchecked (Xml *xml)
{
  xml_end_element (xml, NULL);
}

static Xml *
xml_open (FILE *file)
{
  Xml *xml;

  xml = g_slice_new (Xml);
  xml->file = file;
  xml->stack = NULL;

  return xml;
}

static void
xml_close (Xml *xml)
{
  g_assert (xml->stack == NULL);
  if (xml->file != NULL)
    {
      fflush (xml->file);
      if (xml->file != stdout)
        fclose (xml->file);
      xml->file = NULL;
    }
}

static void
write_attributes (Xml *file,
                  GIBaseInfo *info)
{
  GIAttributeIter iter = { 0, };
  char *name, *value;

  while (g_base_info_iterate_attributes (info, &iter, &name, &value))
    {
      xml_start_element (file, "attribute");
      xml_printf (file, " name=\"%s\" value=\"%s\"", name, value);
      xml_end_element (file, "attribute");
    }
}

static void
xml_free (Xml *xml)
{
  xml_close (xml);
  g_slice_free (Xml, xml);
}


static void
write_value_info (const gchar *namespace,
		  GIValueInfo *info,
		  Xml         *file)
{
  const gchar *name;
  gint64 value;
  gchar *value_str;
  gboolean deprecated;

  name = g_base_info_get_name ((GIBaseInfo *)info);
  value = g_value_info_get_value (info);
  deprecated = g_base_info_is_deprecated ((GIBaseInfo *)info);

  xml_start_element (file, "member");
  value_str = g_strdup_printf ("%" G_GINT64_FORMAT, value);
  xml_printf (file, " name=\"%s\" value=\"%s\"", name, value_str);
  g_free (value_str);

  if (deprecated)
    xml_printf (file, " deprecated=\"1\"");

  write_attributes (file, (GIBaseInfo*) info);

  xml_end_element (file, "member");
}

static void
write_enum_info (const gchar *namespace,
		 GIEnumInfo *info,
		 Xml         *file)
{
  const gchar *name;
  const gchar *type_name;
  const gchar *type_init;
  const gchar *error_domain;
  gboolean deprecated;
  gint i;

  name = g_base_info_get_name ((GIBaseInfo *)info);
  deprecated = g_base_info_is_deprecated ((GIBaseInfo *)info);

  type_name = g_registered_type_info_get_type_name ((GIRegisteredTypeInfo*)info);
  type_init = g_registered_type_info_get_type_init ((GIRegisteredTypeInfo*)info);
  error_domain = g_enum_info_get_error_domain (info);

  if (g_base_info_get_type ((GIBaseInfo *)info) == GI_INFO_TYPE_ENUM)
    xml_start_element (file, "enumeration");
  else
    xml_start_element (file, "bitfield");
  xml_printf (file, " name=\"%s\"", name);

  if (type_init)
    xml_printf (file, " glib:type-name=\"%s\" glib:get-type=\"%s\"", type_name, type_init);
  if (error_domain)
    xml_printf (file, " glib:error-domain=\"%s\"", error_domain);

  if (deprecated)
    xml_printf (file, " deprecated=\"1\"");

  write_attributes (file, (GIBaseInfo*) info);

  for (i = 0; i < g_enum_info_get_n_values (info); i++)
    {
      GIValueInfo *value = g_enum_info_get_value (info, i);
      write_value_info (namespace, value, file);
      g_base_info_unref ((GIBaseInfo *)value);
    }

  xml_end_element_unchecked (file);
}




void test_constructor_return_type(GIBaseInfo* object_info);

void
test_constructor_return_type(GIBaseInfo* object_info)
{
  GIFunctionInfo* constructor;
  GITypeInfo* return_type;
  GIBaseInfo *return_info;
  const gchar* class_name;
  const gchar* return_name;

  class_name = g_registered_type_info_get_type_name ((GIRegisteredTypeInfo*) object_info);
  g_assert (class_name);

  constructor = g_object_info_find_method((GIObjectInfo*)object_info, "new");
  g_assert (constructor);

  return_type = g_callable_info_get_return_type ((GICallableInfo*)constructor);
  g_assert (return_type);
  g_assert (g_type_info_get_tag (return_type) == GI_TYPE_TAG_INTERFACE);

  return_info = g_type_info_get_interface (return_type);
  g_assert (return_info);

  return_name = g_registered_type_info_get_type_name ((GIRegisteredTypeInfo*) return_info);
  g_assert (strcmp (class_name, return_name) == 0);
}


int
main(int argc, char **argv)
{
  GIRepository *repo;
  GITypelib *ret;
  GError *error = NULL;
  GIBaseInfo *info;
  GIBaseInfo *siginfo;
  GIEnumInfo *errorinfo;
  GType gtype;
  const char *prefix;

  g_type_init ();

  repo = g_irepository_get_default ();

  ret = g_irepository_require (repo, "Gio", NULL, 0, &error);
  if (!ret)
    g_error ("%s", error->message);

  prefix = g_irepository_get_c_prefix (repo, "Gio");
  g_assert (prefix != NULL);
  g_assert_cmpstr (prefix, ==, "G");

  info = g_irepository_find_by_name (repo, "Gio", "Cancellable");
  g_assert (info != NULL);
  g_assert (g_base_info_get_type (info) == GI_INFO_TYPE_OBJECT);

  gtype = g_registered_type_info_get_g_type ((GIRegisteredTypeInfo *)info);
  g_assert (g_type_is_a (gtype, G_TYPE_OBJECT));

  info = g_irepository_find_by_gtype (repo, g_type_from_name ("GCancellable"));
  g_assert (info != NULL);

  g_print ("Successfully found GCancellable\n");

  test_constructor_return_type (info);

  info = g_irepository_find_by_name (repo, "Gio", "ThisDoesNotExist");
  g_assert (info == NULL);

  info = g_irepository_find_by_name (repo, "Gio", "FileMonitor");
  g_assert (info != NULL);
  siginfo = g_object_info_find_signal ((GIObjectInfo*) info, "changed");
  g_assert (siginfo != NULL);
  g_base_info_unref (siginfo);

  /* Test notify on gobject */

  info = g_irepository_find_by_name (repo, "GObject", "Object");
  g_assert (info != NULL);
  siginfo = g_object_info_find_signal (info, "notify");
  g_assert (siginfo != NULL);
  g_base_info_unref (siginfo);
  g_base_info_unref (info);

  /* enum tests */
  errorinfo = g_irepository_find_by_error_domain (repo, G_RESOLVER_ERROR);
  g_assert (errorinfo != NULL);
  g_assert (g_base_info_get_type ((GIBaseInfo *)errorinfo) == GI_INFO_TYPE_ENUM);
  g_assert (strcmp (g_base_info_get_name ((GIBaseInfo*)errorinfo), "ResolverError") == 0);

  const char *filename = "testenum.gir";
  const char *namespace="test";
  gboolean    needs_prefix=1;
  FILE *ofile;
  Xml *xml;


  ofile = g_fopen (filename, "w");

  xml = xml_open (ofile);
  xml->show_all = 1;
  xml_printf (xml, "<?xml version=\"1.0\"?>\n");
  xml_start_element (xml, "repository");
  xml_start_element (xml, "namespace");
  xml_printf (xml, " name=\"%s\" version=\"%s\"", namespace, "totally-experimental-0.00001");
  xml_printf (xml, " version=\"1.0\"\n"
	      "            xmlns=\"http://www.gtk.org/introspection/core/1.0\"\n"
	      "            xmlns:c=\"http://www.gtk.org/introspection/c/1.0\"\n"
	      "            xmlns:glib=\"http://www.gtk.org/introspection/glib/1.0\"");
  write_enum_info(namespace,(GIEnumInfo *)errorinfo,xml);
  xml_end_element (xml, "namespace");
  xml_end_element (xml, "repository");
  xml_free (xml);
  exit(0);
}
