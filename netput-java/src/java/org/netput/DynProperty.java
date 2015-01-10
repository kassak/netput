package org.netput;

public class DynProperty
{
   public DynProperty()
   {
      handle = -1;
   }

   protected native void finalize() throws Throwable;
  
   private long handle;
}