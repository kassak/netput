#include <gtest/gtest.h>
#include <netput/dyn_properties.h>
#include <netput/config_parser.h>
#include <cstdlib>

struct leak_check_allocator_t
{
   void * realloc(void * ptr, size_t sz)
   {
      chunks.erase(ptr);
      void * res = std::realloc(ptr, sz);
      if(sz != 0 && res)
         chunks.insert(res);
      return res;
   }

   static void * srealloc(void * ptr, size_t sz, void * data)
   {
      return static_cast<leak_check_allocator_t*>(data)->realloc(ptr, sz);
   }

   std::set<void*> chunks;
};

TEST(test_dyn_properties, check_structure)
{
   const dyn_property_t * null_prop = NULL;

   leak_check_allocator_t alloc;
   dyn_prop_allocator_t dp_alloc = {&leak_check_allocator_t::srealloc, &alloc};

   dyn_property_t * prop = dyn_prop_create(&dp_alloc);
   EXPECT_EQ(dyn_prop_get_type(prop), DYN_PROP_EMPTY);
   dyn_property_t * aaa = dyn_prop_add_child(prop);
   EXPECT_EQ(dyn_prop_get_type(prop), DYN_PROP_ROOT);
   EXPECT_EQ(dyn_prop_get_first_child(prop), aaa);
   dyn_property_t * bbb = dyn_prop_add_child(prop);

   EXPECT_EQ(dyn_prop_get_type(aaa), DYN_PROP_EMPTY);
   EXPECT_EQ(dyn_prop_get_type(bbb), DYN_PROP_EMPTY);

   dyn_prop_set_type(aaa, DYN_PROP_PAIR);
   EXPECT_NE(dyn_prop_get_pair(aaa, !!1), null_prop);
   EXPECT_NE(dyn_prop_get_pair(aaa, !!0), null_prop);

   dyn_prop_set_string(dyn_prop_access_pair(aaa, !!1), "aaa");

   EXPECT_STRCASEEQ(dyn_prop_get_string(dyn_prop_get_pair(aaa, !!1)), "aaa");

   EXPECT_EQ(dyn_prop_get_first_child(prop), bbb);
   EXPECT_EQ(dyn_prop_get_next_sibling(bbb), aaa);
   EXPECT_EQ(dyn_prop_get_next_sibling(aaa), null_prop);

   dyn_prop_set_string(aaa, "value");
   EXPECT_STRCASEEQ(dyn_prop_get_string(aaa), "value");
   EXPECT_EQ(dyn_prop_get_type(aaa), DYN_PROP_STRING);
   
   dyn_prop_set_double(aaa, 666);
   EXPECT_EQ(dyn_prop_get_double(aaa), 666);
   EXPECT_EQ(dyn_prop_get_type(aaa), DYN_PROP_DOUBLE);

   dyn_prop_free(prop);

   EXPECT_EQ(alloc.chunks.size(), 0);
}

TEST(test_config_parser, check_structure)
{
   const dyn_property_t * null_prop = NULL;
   const char * config_str =
      "---\n"
      "test1: &lst\n"
      " - one\n"
      " - two\n"
      " - one\n"
      "test2:\n"
      " uno: *lst\n"
      " dos: tres\n"
      ;
   dyn_property_t * conf;
   const dyn_property_t * test1, * test2, *uno;
   conf = netput_parse_config_string(config_str);
   ASSERT_NE(conf, null_prop);
   test1 = dyn_prop_mapping_find(conf, "test1");
   test2 = dyn_prop_mapping_find(conf, "test2");
   ASSERT_NE(test1, null_prop);
   ASSERT_NE(test2, null_prop);
   uno = dyn_prop_mapping_find(test2, "uno");
   ASSERT_EQ(dyn_prop_get_type(uno), DYN_PROP_ROOT);
   if(conf)
      dyn_prop_free(conf);
}