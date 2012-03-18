#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <glib.h>
#include "girparser.h"
#include "girmodule.h"
#include "girnode.h"
#include "gitypelib-internal.h"
#include "config.h"

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
  enum_ = (GIrNodeEnum *) _g_ir_node_new (G_IR_NODE_ENUM,
                                          current_module);
 
  _g_ir_module_free (current_module);
  //enum_->methods = g_list_append (enum_->methods, function);

  exit(0);
}
