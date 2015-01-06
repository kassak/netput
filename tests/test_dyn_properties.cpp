#include <gtest/gtest.h>
#include <netput/dyn_properties.h>
#include <cstdlib>

struct leak_check_allocator_t
{
   void * realloc(void * ptr, size_t sz)
   {
      chunks.erase(ptr);
      void * res = std::realloc(ptr, sz);
      if(sz != 0 && res)
         chunks.insert(ptr);
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
   leak_check_allocator_t alloc;
   dyn_prop_allocator_t dp_alloc = {&leak_check_allocator_t::srealloc, &alloc};

   dyn_property_t * prop = dyn_prop_create(&dp_alloc);
   EXPECT_EQ(dyn_prop_get_type(prop), DYN_PROP_EMPTY);
   dyn_property_t * aaa = dyn_prop_add_child(prop, "aaa");
   EXPECT_EQ(dyn_prop_get_type(prop), DYN_PROP_ROOT);
   EXPECT_EQ(dyn_prop_get_first_child(prop), aaa);
   dyn_property_t * bbb = dyn_prop_add_child(prop, "bbb");

   EXPECT_EQ(dyn_prop_get_type(aaa), DYN_PROP_EMPTY);
   EXPECT_EQ(dyn_prop_get_type(bbb), DYN_PROP_EMPTY);

   EXPECT_STRCASEEQ(dyn_prop_get_name(aaa), "aaa");
   EXPECT_STRCASEEQ(dyn_prop_get_name(bbb), "bbb");

   EXPECT_EQ(dyn_prop_get_first_child(prop), bbb);
   EXPECT_EQ(dyn_prop_get_next_sibling(bbb), aaa);
   EXPECT_EQ(dyn_prop_get_next_sibling(aaa), (dyn_property_t *)NULL);

   dyn_prop_set_string(aaa, "value");
   EXPECT_STRCASEEQ(dyn_prop_get_string(aaa), "value");
   EXPECT_EQ(dyn_prop_get_type(aaa), DYN_PROP_STRING);
   
   dyn_prop_set_double(aaa, 666);
   EXPECT_EQ(dyn_prop_get_double(aaa), 666);
   EXPECT_EQ(dyn_prop_get_type(aaa), DYN_PROP_DOUBLE);

   dyn_prop_free(prop);

   EXPECT_TRUE(alloc.chunks.empty());
}