#include <netput/dyn_properties.h>
#include <stdlib.h>
#include <string.h>

static void * default_alloc_wrapper(void * ptr, size_t sz, void * unused)
{
   (void)unused;
   return realloc(ptr, sz);
}
static const dyn_prop_allocator_t * default_alloc()
{
   static dyn_prop_allocator_t res = {&default_alloc_wrapper, NULL};
   return &res;
}

struct dyn_property_t
{
   char * name;
   int type;
   union
   {
      char * str;
      double num;
      dyn_property_t * child;
   };
   dyn_property_t *sibling;
   dyn_prop_allocator_t alloc;
};

static void* dp_malloc(size_t sz, const dyn_prop_allocator_t * alloc)
{
   return alloc->realloc(NULL, sz, alloc->data);
}

static void dp_free(void * ptr, const dyn_prop_allocator_t * alloc)
{
   alloc->realloc(ptr, 0, alloc->data);
}

static char * dp_strdup(const char * str, const dyn_prop_allocator_t * alloc)
{
   size_t sz = strnlen(str, 1000000000);
   char * res = (char*)dp_malloc(sz+1, alloc);
   memcpy(res, str, sz+1);
   res[sz] = '\0';
   return res;
}
static void dyn_prop_clear(dyn_property_t * prop)
{
   switch(prop->type)
   {
   case DYN_PROP_ROOT:
      dyn_prop_free(prop->child);
      break;
   case DYN_PROP_STRING:
      dp_free(prop->str, &prop->alloc);
      break;
   case DYN_PROP_DOUBLE:
   case DYN_PROP_EMPTY:
      break;
   }
   prop->type = DYN_PROP_EMPTY;
}
static void dyn_prop_free_one(dyn_property_t * prop)
{
   dyn_prop_clear(prop);
   dp_free(prop->name, &prop->alloc);
   dp_free(prop, &prop->alloc);
}
DYN_PROP_API dyn_property_t * dyn_prop_create(const dyn_prop_allocator_t * alloc)
{
   if(!alloc)
      alloc = default_alloc();
   {
      dyn_property_t * c = (dyn_property_t *)dp_malloc(sizeof(dyn_property_t), alloc);
      memset(c, 0, sizeof(*c));
      c->alloc = *alloc;
      return c;
   }
}

DYN_PROP_API void dyn_prop_free(dyn_property_t * tmp)
{
   while(tmp)
   {
      dyn_property_t * tmp2 = tmp->sibling;
      dyn_prop_free_one(tmp);
      tmp = tmp2;
   }
}

DYN_PROP_API dyn_property_t * dyn_prop_add_child(dyn_property_t * prop, const char * name)
{
   if(prop->type != DYN_PROP_ROOT)
      dyn_prop_clear(prop);
   prop->type = DYN_PROP_ROOT;
   {
      dyn_property_t * c = dyn_prop_create(&prop->alloc);
      c->sibling = prop->child;
      prop->child = c;
      c->name = dp_strdup(name, &c->alloc);
      return c;
   }
}

DYN_PROP_API dyn_property_t * dyn_prop_find_or_add_child(dyn_property_t * prop, const char * name)
{
   if(prop->type == DYN_PROP_ROOT)
   {
      dyn_property_t * tmp = prop->child;
      while(tmp)
      {
         if(strcmp(tmp->name, name) == 0)
            return tmp;
         tmp = tmp->sibling;
      }
   }
   return dyn_prop_add_child(prop, name);
}

DYN_PROP_API const dyn_property_t * dyn_prop_find_child(const dyn_property_t * prop, const char * name)
{
   if(prop->type == DYN_PROP_ROOT)
   {
      dyn_property_t * tmp = prop->child;
      while(tmp)
      {
         if(strcmp(tmp->name, name) == 0)
            return tmp;
         tmp = tmp->sibling;
      }
   }
   return NULL;
}

DYN_PROP_API void dyn_prop_set_double(dyn_property_t * prop, double val)
{
   dyn_prop_clear(prop);
   prop->type = DYN_PROP_DOUBLE;
   prop->num = val;
}

DYN_PROP_API void dyn_prop_set_string(dyn_property_t * prop, const char * str)
{
   dyn_prop_clear(prop);
   prop->type = DYN_PROP_STRING;
   prop->str = dp_strdup(str, &prop->alloc);
}

DYN_PROP_API double dyn_prop_get_double(const dyn_property_t * prop)
{
   if(prop->type == DYN_PROP_DOUBLE)
      return prop->num;
   return 0;
}

DYN_PROP_API char const * dyn_prop_get_string(const dyn_property_t * prop)
{
   if(prop->type == DYN_PROP_STRING)
      return prop->str;
   return NULL;
}

DYN_PROP_API const dyn_property_t * dyn_prop_get_first_child(const dyn_property_t * prop)
{
   if(prop->type == DYN_PROP_ROOT)
      return prop->child;
   return NULL;
}

DYN_PROP_API const dyn_property_t * dyn_prop_get_next_sibling(const dyn_property_t * prop)
{
   return prop->sibling;
}

DYN_PROP_API int dyn_prop_get_type(const dyn_property_t * prop)
{
   return prop->type;
}

DYN_PROP_API char const * dyn_prop_get_name(const dyn_property_t * prop)
{
   return prop->name;
}

