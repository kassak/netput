package org.netput;

import org.netput.DynProperty;
import org.netput.Sink;

public class Netput
{
   static
   {
      System.loadLibrary("netput");
   }
   
   public static native Sink loadSink(DynProperty props);
}