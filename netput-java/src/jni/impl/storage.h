#pragma once
#include <vector>
#include <assert.h>
#include <stdlib.h>

template<class T>
struct storage_t
{
   struct holder_t
   {
      T obj;
      jlong id;
      void (*deleter)(T &);
   };
   
   storage_t(){}

   holder_t * get_object(jlong id)
   {
      assert(id != -1);
      if(id >= objects_.size() || objects_[id].id == -1)
         return NULL;
      return &objects_[id];
   }
   
   holder_t * allocate()
   {
      holder_t * res;
      if(!free_.empty())
      {
         jlong id = free_.back();
         free_.pop_back();
         res = &objects_[id];
         res->id = id;
      }
      else
      {
         objects_.resize(objects_.size() + 1);
         res = &objects_.back();
         res->id = objects_.size() - 1;
      }
      memset(&res->obj, 0, sizeof(res->obj));
      res->deleter = NULL;
      return res;
   }
   
   void deallocate(jlong id)
   {
      assert(id != -1);
      holder_t * s = get_object(id);
      if(!s)
         return;
      free_.push_back(id);
      s->id = -1;
      s->deleter(s->obj);
      s->deleter = NULL;
   }
private:
   storage_t(storage_t<T> const &);
private:
   std::vector<holder_t> objects_;
   std::vector<jlong> free_;
};

template<class T>
inline storage_t<T> & storage()
{
   static storage_t<T> inst;
   return inst;      
}
template<class T>
jfieldID get_handle_field(JNIEnv * env, jobject obj)
{
   jclass c = env->GetObjectClass(obj);
   return env->GetFieldID(c, "handle", "J");
}

template<class T>
jfieldID get_handle_field_cached(JNIEnv * env, jobject obj)
{
   static jfieldID id = get_handle_field<T>(env, obj);
   return id;
}

template<class T>
typename storage_t<T>::holder_t * get_native_holder_ptr(JNIEnv * env, jobject obj)
{
   jlong id = env->GetLongField(obj, get_handle_field_cached<T>(env, obj));
   if(id == -1) // already deleted
      return NULL;
   return storage<T>().get_object(id);
}

template<class T>
T * get_native_ptr(JNIEnv * env, jobject obj)
{
   typename storage_t<T>::holder_t * res = get_native_holder_ptr<T>(env, obj);
   return res ? &res->obj : NULL;
}

template<class T>
T * new_native_ptr(JNIEnv * env, jobject obj)
{
   typename storage_t<T>::holder_t * res = storage<T>().allocate();
   env->SetLongField(obj, get_handle_field_cached<T>(env, obj), res->id);
   return &res->obj;
}

template<class T>
void delete_native_ptr(JNIEnv * env, jobject obj)
{
   jlong id = env->GetLongField(obj, get_handle_field_cached<T>(env, obj));
   if(id == -1) //already deleted
      return;
   storage<T>().deallocate(id);
   env->SetLongField(obj, get_handle_field_cached<T>(env, obj), -1);
}

