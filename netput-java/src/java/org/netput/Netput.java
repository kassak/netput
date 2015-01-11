package org.netput;

import org.netput.DynProperty;
import org.netput.Sink;

public class Netput
{
   static
   {
      System.loadLibrary("netput-jni");
   }

   public static class SinkLoadFailedException extends Exception
   {
      public SinkLoadFailedException(String m)
      {
         super(m);
      }
   }
   public static class ConfigParseFailedException extends Exception
   {
      public ConfigParseFailedException(String m)
      {
         super(m);
      }
   }

   public static native Sink loadSink(DynProperty props);
   public static native DynProperty parseConfigFile(byte[] path);
   public static native DynProperty parseConfigString(byte[] content);
   public static DynProperty parseConfigFile(String path)
   {
      try
      {
         System.out.println(path);
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