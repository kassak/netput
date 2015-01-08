#include <netput/config_parser.h>
#include <yaml.h>
#include <assert.h>
#include "tricks.h"

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
static int do_parse_node(yaml_parser_t * parser, yaml_event_t * event, dyn_property_t * node);

static int do_parse_scalar(yaml_parser_t * parser, yaml_event_t * event, dyn_property_t * sc)
{
   if(!verify_event(event, YAML_SCALAR_EVENT))
      return 0;
   dyn_prop_set_stringn(sc, event->data.scalar.value, event->data.scalar.length);
   if(!next_event(parser, event))
      return 0;
   return !0;
}

static int do_parse_alias(yaml_parser_t * parser, yaml_event_t * event, dyn_property_t * node)
{
   if(!verify_event(event, YAML_ALIAS_EVENT))
      return 0;
   if(!next_event(parser, event))
      return 0;
   return !0;
}

static int do_parse_sequence(yaml_parser_t * parser, yaml_event_t * event, dyn_property_t * seq)
{
   if(!verify_event(event, YAML_SEQUENCE_START_EVENT))
      return 0;
   while(true)
   {
      if(!next_event(parser, event))
         return 0;
      if(event->type == YAML_SEQUENCE_END_EVENT)
         break;
      dyn_property_t * el = dyn_prop_add_child(seq);
      if(!do_parse_node(parser, event, el))
         return 0;
   }
   if(!next_event(parser, event))
      return 0;
   return !0;
}

static int do_parse_mapping(yaml_parser_t * parser, yaml_event_t * event, dyn_property_t * map)
{
   if(!verify_event(event, YAML_MAPPING_START_EVENT))
      return 0;
   while(true)
   {
      if(!next_event(parser, event))
         return 0;
      if(event->type == YAML_MAPPING_END_EVENT)
         break;
      dyn_property_t * el = dyn_prop_add_child(map);
      dyn_prop_set_type(el, DYN_PROP_PAIR);
      if(!do_parse_node(parser, event, dyn_prop_pair_access(el, true)))
         return 0;
      if(!do_parse_node(parser, event, dyn_prop_pair_access(el, false)))
         return 0;
   }
   if(!next_event(parser, event))
      return 0;
   return !0;
}


static int do_parse_node(yaml_parser_t * parser, yaml_event_t * event, dyn_property_t * node)
{
   int pres = 0;
   switch(event->type)
   {
   case YAML_ALIAS_EVENT:
      pres = do_parse_alias(parser, event, node);
      break;
   case YAML_SCALAR_EVENT:
      pres = do_parse_scalar(parser, event, node);
      break;
   case YAML_SEQUENCE_START_EVENT:
      pres = do_parse_sequence(parser, event, node);
      break;
   case YAML_MAPPING_START_EVENT:
      pres = do_parse_mapping(parser, event, node);
      break;
   default:
      pres = 0;
      yaml_event_delete(event);
   }
   return pres;
}

static int do_parse_document(yaml_parser_t * parser, yaml_event_t * event, dyn_property_t * doc)
{
   int pres;
   if(!verify_event(event, YAML_DOCUMENT_START_EVENT))
      return 0;
   if(!next_event(parser, event))
      return 0;
   pres = do_parse_node(parser, event, doc);
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
   int done = 0;
   if (!yaml_parser_parse(parser, &event))
      return NULL;
   if(!verify_event(&event, YAML_STREAM_START_EVENT))
      return NULL;
   if(!next_event(parser, &event))
      return NULL;
   while(event.type == YAML_DOCUMENT_START_EVENT)
   {
      dyn_property_t * doc = dyn_prop_create(NULL);
      if(!res)
         res = doc;
      dyn_prop_link(last, doc);
      last = doc;
      if(!do_parse_document(parser, &event, doc))
         goto error;
   }
   if(!verify_event(&event, YAML_STREAM_END_EVENT))
      goto error;
   yaml_event_delete(&event);
   return res;
error:
   dyn_prop_free(res);
   return NULL;
}

DYN_PROP_API dyn_property_t * parse_config_file(const char * path)
{
   FILE* input;
   dyn_property_t * res;
   yaml_parser_t parser;
   yaml_parser_initialize(&parser);

   input = fopen_trick(path, "rb");
   yaml_parser_set_input_file(&parser, input);

   res = do_parse(&parser);

   yaml_parser_delete(&parser);
   return res;
}

DYN_PROP_API dyn_property_t * parse_config_string(const char * config_str)
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

