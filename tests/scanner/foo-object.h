#ifndef __FOO_OBJECT_H__
#define __FOO_OBJECT_H__

#include <glib-object.h>
#include "utility.h"

#define FOO_TYPE_INTERFACE           (foo_interface_get_type ())
#define FOO_INTERFACE(object)        (G_TYPE_CHECK_INSTANCE_CAST ((object), FOO_TYPE_INTERFACE, FooInterface))
#define FOO_IS_INTERFACE(object)     (G_TYPE_CHECK_INSTANCE_TYPE ((object), FOO_TYPE_INTERFACE))

#define FOO_TYPE_OBJECT              (foo_object_get_type ())
#define FOO_OBJECT(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), FOO_TYPE_OBJECT, FooObject))
#define FOO_IS_OBJECT(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), FOO_TYPE_OBJECT))

#define FOO_TYPE_SUBOBJECT           (foo_subobject_get_type ())
#define FOO_SUBOBJECT(subobject)     (G_TYPE_CHECK_INSTANCE_CAST ((subobject), FOO_TYPE_SUBOBJECT, FooSubobject))
#define FOO_IS_SUBOBJECT(subobject)  (G_TYPE_CHECK_INSTANCE_TYPE ((subobject), FOO_TYPE_SUBOBJECT))

typedef struct _FooInterface       FooInterface;
typedef struct _FooInterfaceIface  FooInterfaceIface;
typedef struct _FooObject          FooObject;
typedef struct _FooObjectClass     FooObjectClass;
typedef struct _FooSubobject       FooSubobject;
typedef struct _FooSubobjectClass  FooSubobjectClass;

struct _FooInterfaceIface
{
  GTypeInterface parent_iface;
};

GType                 foo_interface_get_type       (void) G_GNUC_CONST;

struct _FooObject
{
  GObject parent_instance;
};

struct _FooObjectClass
{
  GObjectClass parent_class;

  gboolean (* virtual_method) (FooObject *object, int first_param);
};

gint                  foo_init                     (void);

GType                 foo_object_get_type          (void) G_GNUC_CONST;
FooObject*            foo_object_new               ();
gint                  foo_object_method            (FooObject *object);
UtilityObject*        foo_object_external_type     (FooObject *object);
gint                  foo_object_out               (FooObject *object,
						    int       *outarg);
GObject*              foo_object_create_object     (FooObject *object);
GObject*              foo_object_allow_none        (FooObject *object, gchar *allow_none);
gint                  foo_object_inout             (FooObject *object, int *inoutarg);
gint                  foo_object_inout2            (FooObject *object, int *inoutarg);
gint                  foo_object_inout3            (FooObject *object, int *inoutarg);
gint                  foo_object_in                (FooObject *object, int *inarg);
gint                  foo_object_calleeowns        (FooObject *object, GObject *toown);
gint                  foo_object_calleesowns       (FooObject *object, GObject *toown1, GObject *toown2);
GList*                foo_object_get_strings       (FooObject *object);
GSList*               foo_object_get_objects       (FooObject *object);

void                  foo_object_various           (FooObject *object, void *data, GType some_type);

/* A random typedef */
typedef GSList FooList;

void                  foo_object_with_tdef         (FooObject *object, FooList *blah);


struct _FooSubobject
{
  FooObject parent_instance;
};

struct _FooSubobjectClass
{
  FooObjectClass parent_class;
};

GType                 foo_subobject_get_type       (void) G_GNUC_CONST;
FooSubobject*         foo_subobject_new            ();

typedef enum
{
  FOO_ENUM_ALPHA,
  FOO_ENUM_BETA,
  FOO_ENUM_DELTA
} FooEnumType;

GType foo_enum_type_get_type (void);

int                   foo_enum_type_method         (FooEnumType foo_enum);

FooEnumType           foo_enum_type_returnv        (int x);

typedef enum
{
  FOO_FLAGS_FIRST  = 1 << 0,
  FOO_FLAGS_SECOND = 1 << 1,
  FOO_FLAGS_THIRD  = 1 << 2
} FooFlagsType;

typedef enum
{
  FOO_ENUM_UN = 1,
  FOO_ENUM_DEUX = 2,
  FOO_ENUM_TROIS = 3,
} FooEnumNoType;

GType foo_flags_type_get_type (void);

typedef struct _FooBoxed FooBoxed;

GType                 foo_boxed_get_type       (void) G_GNUC_CONST;
FooBoxed*             foo_boxed_new            (void);
void                  foo_boxed_method         (FooBoxed* boxed);

/* FIXME: Scanner does not support this yet
const char *FOO_CONSTANT_STR = "foo-constant-str";
const int FOO_CONSTANT_INT = 10;
const float FOO_CONSTANT_FLOAT = 10;
*/

/* Callback */
typedef gboolean (* FooCallback) (FooObject *foo, gboolean b, gpointer data);

/* Invalid comments, should be ignored */

/* @ */
/* @: */

typedef struct _FooStruct           FooStruct; 
typedef struct _FooStructPrivate    FooStructPrivate; 

struct _FooStruct
{
  FooStructPrivate *priv;
  int member;
};

typedef struct _FooRectangle          FooRectangle;

struct _FooRectangle
{
  gint x;
  gint y;
  gint width;
  gint height;
};

#endif /* __FOO_OBJECT_H__ */