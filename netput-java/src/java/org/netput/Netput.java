package org.netput;

import org.netput.DynProperty;
import org.netput.Sink;

public class Netput
{
   static
   {
      System.loadLibrary("netput-jni");
   }

   public static native Sink loadSink(DynProperty props);
   public static native DynProperty parseConfigFile(byte[] path);
   public static native DynProperty parseConfigString(byte[] content);
   public static DynProperty parseConfigFile(String path)
   {
      try
      {
         return parseConfigFile(path.getBytes("UTF-8"));
      }
      catch(java.io.UnsupportedEncodingException e)
      {
         throw new RuntimeException(e);
      }
   }
   public static DynProperty parseConfigString(String content)
   {
      try
      {
         return parseConfigString(content.getBytes("UTF-8"));
      }
      catch(java.io.UnsupportedEncodingException e)
      {
         throw new RuntimeException(e);
      }
   }
}