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
   int type;
   union
   {
      char * str;
      double dnum;
      int inum;
      dyn_property_t * child;
      struct pair_t
      {
         dyn_property_t * first, * second;
      } pair;
   } data;
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

static char * dp_strdup(const char * str, size_t n, const dyn_prop_allocator_t * alloc)
{
   size_t sz = strnlen(str, n);
   char * res = (char*)dp_malloc(sz+1, alloc);
   memcpy(res, str, sz);
   res[sz] = '\0';
   return res;
}
static void dyn_prop_clear(dyn_property_t * prop)
{
   switch(prop->type)
   {
   case DYN_PROP_ROOT:
      dyn_prop_free(prop->data.child);
      break;
   case DYN_PROP_STRING:
      dp_free(prop->data.str, &prop->alloc);
      break;
   case DYN_PROP_PAIR:
      dyn_prop_free(prop->data.pair.first);
      dyn_prop_free(prop->data.pair.second);
      break;
   case DYN_PROP_DOUBLE:
   case DYN_PROP_INTEGER:
   case DYN_PROP_EMPTY:
      break;
   }
   memset(&prop->data, 0, sizeof(prop->data));
   prop->type = DYN_PROP_EMPTY;
}
static void dyn_prop_free_one(dyn_property_t * prop)
{
   dyn_prop_clear(prop);
   dp_free(prop, &prop->alloc);
}

static void dyn_prop_clone_content_to(const dyn_property_t * prop, dyn_property_t * res)
{
   res->type = prop->type;
   switch(res->type)
   {
   case DYN_PROP_STRING:
      res->data.str = dp_strdup(prop->data.str, -1, &res->alloc);
      break;
   case DYN_PROP_DOUBLE:
      res->data.dnum = prop->data.dnum;
      break;
   case DYN_PROP_INTEGER:
      res->data.inum = prop->data.inum;
      break;
   case DYN_PROP_ROOT:
      res->data.child = dyn_prop_clone(prop->data.child);
      break;
   case DYN_PROP_PAIR:
      res->data.pair.first = dyn_prop_clone(prop->data.pair.first);
      res->data.pair.second = dyn_prop_clone(prop->data.pair.second);
      break;
   }
}

DYN_PROP_API void dyn_prop_clone_to(const dyn_property_t * prop, dyn_property_t * to)
{
   dyn_prop_clear(to);
   dyn_prop_clone_content_to(prop, to);
}

DYN_PROP_API dyn_property_t * dyn_prop_clone(const dyn_property_t * prop)
{
   dyn_property_t * res, *last;
   if(!prop)
      return NULL;
   last = res = dyn_prop_create(&prop->alloc);
   dyn_prop_clone_content_to(prop, res);
   while(prop = prop->sibling)
   {
      last = last->sibling = dyn_prop_create(&prop->alloc);
      dyn_prop_clone_content_to(prop, last);
   }
   return res;
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

DYN_PROP_API dyn_property_t * dyn_prop_add_child(dyn_property_t * prop)
{
   dyn_prop_set_type(prop, DYN_PROP_ROOT);
   {
      dyn_property_t * c = dyn_prop_create(&prop->alloc);
      c->sibling = prop->data.child;
      prop->data.child = c;
      return c;
   }
}

DYN_PROP_API void dyn_prop_set_double(dyn_property_t * prop, double val)
{
   dyn_prop_clear(prop);
   prop->type = DYN_PROP_DOUBLE;
   prop->data.dnum = val;
}

DYN_PROP_API void dyn_prop_set_int(dyn_property_t * prop, int val)
{
   dyn_prop_clear(prop);
   prop->type = DYN_PROP_INTEGER;
   prop->data.inum = val;
}

DYN_PROP_API void dyn_prop_set_stringn(dyn_property_t * prop, const char * str, size_t n)
{
   dyn_prop_clear(prop);
   prop->type = DYN_PROP_STRING;
   prop->data.str = dp_strdup(str, n, &prop->alloc);
}

DYN_PROP_API void dyn_prop_set_string(dyn_property_t * prop, const char * str)
{
   dyn_prop_clear(prop);
   prop->type = DYN_PROP_STRING;
   prop->data.str = dp_strdup(str, -1, &prop->alloc);
}

DYN_PROP_API double dyn_prop_get_double(const dyn_property_t * prop)
{
   if(prop->type == DYN_PROP_DOUBLE)
      return prop->data.dnum;
   return 0;
}

DYN_PROP_API int dyn_prop_get_int(const dyn_property_t * prop)
{
   if(prop->type == DYN_PROP_INTEGER)
      return prop->data.inum;
   return 0;
}


DYN_PROP_API char const * dyn_prop_get_string(const dyn_property_t * prop)
{
   if(prop->type == DYN_PROP_STRING)
      return prop->data.str;
   return NULL;
}

DYN_PROP_API const dyn_property_t * dyn_prop_get_first_child(const dyn_property_t * prop)
{
   if(prop->type == DYN_PROP_ROOT)
      return prop->data.child;
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

DYN_PROP_API dyn_property_t * dyn_prop_append_sibling(dyn_property_t * prop)
{
   if(prop->sibling)
      return NULL;
   return prop->sibling = dyn_prop_create(&prop->alloc);
}

DYN_PROP_API void dyn_prop_set_type(dyn_property_t * prop, int type)
{
   if(prop->type == type)
      return;
   dyn_prop_clear(prop);
   prop->type = type;
   if(type == DYN_PROP_PAIR)
   {
      prop->data.pair.first = dyn_prop_create(&prop->alloc);
      prop->data.pair.second = dyn_prop_create(&prop->alloc);
   }
}

DYN_PROP_API const dyn_property_t * dyn_prop_get_pair(const dyn_property_t * prop, int first)
{
   if(prop->type != DYN_PROP_PAIR)
      return NULL;
   return first ? prop->data.pair.first : prop->data.pair.second;
}

DYN_PROP_API dyn_property_t * dyn_prop_access_pair(dyn_property_t * prop, int first)
{
   if(prop->type != DYN_PROP_PAIR)
      return NULL;
   return first ? prop->data.pair.first : prop->data.pair.second;
}

DYN_PROP_API const dyn_property_t * dyn_prop_mapping_find(const dyn_property_t * prop, const char * name)
{
   const dyn_property_t * tmp = dyn_prop_get_first_child(prop);
   for(; tmp; tmp = dyn_prop_get_next_sibling(tmp))
      if(dyn_prop_get_type(tmp) == DYN_PROP_PAIR)
      {
         const dyn_property_t * k = dyn_prop_get_pair(tmp, !!1);
         if(dyn_prop_get_type(k) == DYN_PROP_STRING)
            if(0 == strcmp(name, dyn_prop_get_string(k)))
               return dyn_prop_get_pair(tmp, !1);
      }
   return NULL;
}

DYN_PROP_API int dyn_prop_get_double_lexical(const dyn_property_t * prop, double * res)
{
   if(!prop)
      return !!0;
   switch(prop->type)
   {
   case DYN_PROP_DOUBLE:
      *res = prop->data.dnum;
      return !!1;
   case DYN_PROP_INTEGER:
      *res = prop->data.inum;
      return !!1;
   case DYN_PROP_STRING:
      {
         char * end;
         if(!prop->data.str)
            return !!0;
         *res = strtod(prop->data.str, &end);
         return end != prop->data.str;
      }
      return !!1;
   default:
      return !!0;
   }
}
DYN_PROP_API int dyn_prop_get_int_lexical(const dyn_property_t * prop, int * res)
{
   if(!prop)
      return !!0;
   switch(prop->type)
   {
   case DYN_PROP_DOUBLE:
      *res = (int)prop->data.dnum;
      return !!1;
   case DYN_PROP_INTEGER:
      *res = prop->data.inum;
      return !!1;
   case DYN_PROP_STRING:
      {
         char * end;
         if(!prop->data.str)
            return !!0;
         *res = strtol(prop->data.str, &end, 0);
         return end != prop->data.str;
      }
      return !!1;
   default:
      return !!0;
   }
}
