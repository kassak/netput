package org.netput;

public interface ISink
{
   public void setDevice(long dev_id);
   public void setKey(long key_id, long value);
   public void flushDevice();
}