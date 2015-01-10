#include <netput/config_parser.h>
#include <yaml.h>
#include <assert.h>
#include "tricks.h"

typedef struct alias_t
{
   char * name;
   const dyn_property_t * prop;
   struct alias_t * next;
} alias_t;
typedef struct parser_data_t
{
   alias_t * aliases;
} parser_data_t;

static alias_t * find_alias(const parser_data_t * data, const char * name)
{
   alias_t * res = data->aliases;
   while(res)
   {
      if(0 == strcmp(name, res->name))
         return res;
      res = res->next;
   }
   return NULL;
}
static void add_alias(parser_data_t * data, const char * name, const dyn_property_t * prop)
{
   alias_t * res = (alias_t *)malloc(sizeof(alias_t));
   res->name = strdup(name);
   res->prop = prop;
   res->next = data->aliases;
   data->aliases = res;
}

static void init_parser_data(parser_data_t * data)
{
   data->aliases = NULL;
}
static void deinit_parser_data(parser_data_t * data)
{
   while(data->aliases)
   {
      alias_t * a = data->aliases;
      data->aliases = a->next;
      free(a);
   }
}

static int verify_event(yaml_event_t * event, int type)
{
   if(event->type != type)
   {
      yaml_event_delete(event);
      //LOG: enexpected event
      return 0;
   }
   return !0;
}

static int next_event(yaml_parser_t * parser, yaml_event_t * event)
{
   yaml_event_delete(event);
   if(!yaml_parser_parse(parser, event))
   {
      //LOG: parser error
      return 0;
   }
   return !0;
}
static int do_parse_node(yaml_parser_t * parser, yaml_event_t * event, dyn_property_t * node, parser_data_t * pdata);

static int do_parse_scalar(yaml_parser_t * parser, yaml_event_t * event, dyn_property_t * sc, parser_data_t * pdata)
{
   if(!verify_event(event, YAML_SCALAR_EVENT))
      return 0;
   dyn_prop_set_stringn(sc, event->data.scalar.value, event->data.scalar.length);
   if(event->data.scalar.anchor)
      add_alias(pdata, event->data.scalar.anchor, sc);
   if(!next_event(parser, event))
      return 0;
   return !0;
}

static int do_parse_alias(yaml_parser_t * parser, yaml_event_t * event, dyn_property_t * node, parser_data_t * pdata)
{
   alias_t * al;
   if(!verify_event(event, YAML_ALIAS_EVENT))
      return 0;
   al = find_alias(pdata, event->data.alias.anchor);
   if(!al)
      return 0;
   dyn_prop_clone_to(al->prop, node);
   if(!next_event(parser, event))
      return 0;
   return !0;
}

static int do_parse_sequence(yaml_parser_t * parser, yaml_event_t * event, dyn_property_t * seq, parser_data_t * pdata)
{
   if(!verify_event(event, YAML_SEQUENCE_START_EVENT))
      return 0;
   if(event->data.sequence_start.anchor)
      add_alias(pdata, event->data.sequence_start.anchor, seq);
   if(!next_event(parser, event))
      return 0;
   while(1)
   {
      if(event->type == YAML_SEQUENCE_END_EVENT)
         break;
      {
         dyn_property_t * el = dyn_prop_add_child(seq);
         if(!do_parse_node(parser, event, el, pdata))
            return 0;
      }
   }
   if(!next_event(parser, event))
      return 0;
   return !0;
}

static int do_parse_mapping(yaml_parser_t * parser, yaml_event_t * event, dyn_property_t * map, parser_data_t * pdata)
{
   if(!verify_event(event, YAML_MAPPING_START_EVENT))
      return 0;
   if(event->data.mapping_start.anchor)
      add_alias(pdata, event->data.mapping_start.anchor, map);
   if(!next_event(parser, event))
      return 0;
   while(1)
   {
      if(event->type == YAML_MAPPING_END_EVENT)
         break;
      {
         dyn_property_t * el = dyn_prop_add_child(map);
         dyn_prop_set_type(el, DYN_PROP_PAIR);
         if(!do_parse_node(parser, event, dyn_prop_access_pair(el, !!1), pdata))
            return 0;
         if(!do_parse_node(parser, event, dyn_prop_access_pair(el, !!0), pdata))
            return 0;
      }
   }
   if(!next_event(parser, event))
      return 0;
   return !0;
}


static int do_parse_node(yaml_parser_t * parser, yaml_event_t * event, dyn_property_t * node, parser_data_t * pdata)
{
   int pres = 0;
   switch(event->type)
   {
   case YAML_ALIAS_EVENT:
      pres = do_parse_alias(parser, event, node, pdata);
      break;
   case YAML_SCALAR_EVENT:
      pres = do_parse_scalar(parser, event, node, pdata);
      break;
   case YAML_SEQUENCE_START_EVENT:
      pres = do_parse_sequence(parser, event, node, pdata);
      break;
   case YAML_MAPPING_START_EVENT:
      pres = do_parse_mapping(parser, event, node, pdata);
      break;
   default:
      pres = 0;
      yaml_event_delete(event);
   }
   return pres;
}

static int do_parse_document(yaml_parser_t * parser, yaml_event_t * event, dyn_property_t * doc, parser_data_t * pdata)
{
   int pres;
   if(!verify_event(event, YAML_DOCUMENT_START_EVENT))
      return 0;
   if(!next_event(parser, event))
      return 0;
   pres = do_parse_node(parser, event, doc, pdata);
   if(!pres)
      return 0;
   if(!verify_event(event, YAML_DOCUMENT_END_EVENT))
      return 0;
   if(!next_event(parser, event))
      return 0;
   return !0;
}


static dyn_property_t * do_parse(yaml_parser_t * parser)
{
   yaml_event_t event;
   dyn_property_t * res = NULL;
   dyn_property_t * last = NULL;
   parser_data_t pdata;
   int done = 0;
   if (!yaml_parser_parse(parser, &event))
      return NULL;
   if(!verify_event(&event, YAML_STREAM_START_EVENT))
      return NULL;
   if(!next_event(parser, &event))
      return NULL;
   init_parser_data(&pdata);
   while(event.type == YAML_DOCUMENT_START_EVENT)
   {
      dyn_property_t * doc = last ? dyn_prop_append_sibling(last) : dyn_prop_create(NULL);
      if(!res)
         res = doc;
      last = doc;
      if(!do_parse_document(parser, &event, doc, &pdata))
         goto error;
   }
   if(!verify_event(&event, YAML_STREAM_END_EVENT))
      goto error;
   yaml_event_delete(&event);
   deinit_parser_data(&pdata);
   return res;
error:
   dyn_prop_free(res);
   deinit_parser_data(&pdata);
   return NULL;
}

DYN_PROP_API dyn_property_t * netput_parse_config_file(const char * path)
{
   FILE* input;
   dyn_property_t * res;
   yaml_parser_t parser;
   yaml_parser_initialize(&parser);

   input = fopen_trick(path, "rb");
   if(!input)
      return NULL;
   yaml_parser_set_input_file(&parser, input);

   res = do_parse(&parser);

   yaml_parser_delete(&parser);
   return res;
}

DYN_PROP_API dyn_property_t * netput_parse_config_string(const char * config_str)
{
   dyn_property_t * res;
   size_t length = strlen(config_str);
   yaml_parser_t parser;
   yaml_parser_initialize(&parser);

   yaml_parser_set_input_string(&parser, (const unsigned char *)config_str, length);

   res = do_parse(&parser);

   yaml_parser_delete(&parser);
   return res;
}

