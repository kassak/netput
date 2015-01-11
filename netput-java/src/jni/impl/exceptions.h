#pragma once

jint throw_NoClassDefError(JNIEnv *env, const char *message)
{
   const char * ncd_exc_name = "java/lang/NoClassDefFoundError";

   jclass exc_class = env->FindClass(ncd_exc_name);
   if(!exc_class)
   {
      env->FatalError(ncd_exc_name);
      return -1;
   }
   return env->ThrowNew(exc_class, message);
}

jint throw_exception_message(JNIEnv *env, const char * cname, const char *message)
{
    jclass exc_class = env->FindClass(cname);
    if(!exc_class)
        return throw_NoClassDefError(env, cname);
    return env->ThrowNew(exc_class, message);
}

jint throw_IncompatibleClassChangeError(JNIEnv *env, const char *message)
{
   return throw_exception_message(env, "java/lang/IncompatibleClassChangeError", message);
}

jobject create_objectv(JNIEnv *env, const char * class_name, const char * sig, va_list args)
{
   jclass cls = env->FindClass(class_name);
   if(!cls)
      return 0;
   jmethodID constr = env->GetMethodID(cls, "<init>", sig);
   if(!constr)
      return 0;
   jobject res = env->NewObjectV(cls, constr, args);
   return res;
}

jobject create_object(JNIEnv *env, const char * class_name, const char * sig, ...)
{
   va_list args;
   va_start(args, sig);
   jobject res = create_objectv(env, class_name, sig, args);
   va_end(args);
   return res;
}

jint throw_create_exception(JNIEnv *env, const char * class_name, const char * sig, ...)
{
   va_list args;
   va_start(args, sig);
   jobject res = create_objectv(env, class_name, sig, args);
   va_end(args);
   return env->Throw(static_cast<jthrowable>(res));
}

jstring create_string_ascii(JNIEnv * env, const char * str)
{
   return env->NewStringUTF(str);
}

