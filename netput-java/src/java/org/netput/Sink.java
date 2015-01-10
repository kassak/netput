package org.netput;

import org.netput.ISink;

public class Sink implements ISink
{
   private Sink()
   {
      handle = -1;
   }
   
   public native void setDevice(long dev_id);
   public native void setKey(long key_id, long value);
   public native void flushDevice();

   protected native void finalize() throws Throwable;
   
   private long handle;
}