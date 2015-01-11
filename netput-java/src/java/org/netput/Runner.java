package org.netput;

import org.netput.DynProperty;
import org.netput.Netput;
import org.netput.Sink;

import java.util.Scanner;

public class Runner
{
   public static void main(String[] argv)
   {
      if(argv.length != 1)
      {
         System.out.println("too few args");
         return;
      }
      Scanner in = new Scanner(System.in);
      System.out.println("press enter to start...");
      in.nextLine();
      DynProperty prop = Netput.parseConfigFile(argv[0]);
      if(prop == null)
      {
         System.out.println("config parse failed");
         return;
      }
      Sink sink = Netput.loadSink(prop);
      if(sink == null)
      {
         System.out.println("sink load failed");
         return;
      }
      while(true)
      {
         String cmd = in.next();
         if(cmd.equals("d"))
            sink.setDevice(in.nextLong());
         else if(cmd.equals("f"))
            sink.flushDevice();
         else if(cmd.equals("k"))
         {
            long key = in.nextLong();
            long val = in.nextLong();
            sink.setKey(key, val);
         }
         else if(cmd.equals("q"))
         {
            break;
         }
         else
            System.out.println("Ignored");
      }
   }
}