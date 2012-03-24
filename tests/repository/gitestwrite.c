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
#include "gitypelib-internal-xml.h"


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
  //  gboolean    needs_prefix=1;
  FILE *ofile;
  _GIWriteXml *xml;


  ofile = g_fopen (filename, "w");

  xml = _gi_xmlwrite_xml_open (ofile);
  xml->show_all = 1;
  _gi_xmlwrite_xml_printf (xml, "<?xml version=\"1.0\"?>\n");
  _gi_xmlwrite_xml_start_element (xml, "repository");
  _gi_xmlwrite_xml_start_element (xml, "namespace");
  _gi_xmlwrite_xml_printf (xml, " name=\"%s\" version=\"%s\"", namespace, "totally-experimental-0.00001");
  _gi_xmlwrite_xml_printf (xml, " version=\"1.0\"\n"
	      "            xmlns=\"http://www.gtk.org/introspection/core/1.0\"\n"
	      "            xmlns:c=\"http://www.gtk.org/introspection/c/1.0\"\n"
	      "            xmlns:glib=\"http://www.gtk.org/introspection/glib/1.0\"");
  _gi_xmlwrite_write_enum_info(namespace,(GIEnumInfo *)errorinfo,xml);
  _gi_xmlwrite_xml_end_element (xml, "namespace");
  _gi_xmlwrite_xml_end_element (xml, "repository");
  _gi_xmlwrite_xml_free (xml);
  exit(0);
}
