package com.intel.vmf;

public class FieldValue// extends Variant
{
	static
    {
        System.loadLibrary ("vmf");
    }
	
	protected final long nativeObj;
	
	protected FieldValue (long addr)
    {
	    //super (addr);
	    if (addr == 0)
            throw new java.lang.UnsupportedOperationException("Native object address is NULL");
        
        nativeObj = addr;
    }
	
	public FieldValue ()
	{
	    nativeObj = n_FieldValue ();
	}
	
	public FieldValue (String newName, Variant variant)
    { 
        //super (variant.nativeObj);
	    nativeObj = n_FieldValue (newName, variant.nativeObj);
    }
	
	public FieldValue (FieldValue other)
	{
	    nativeObj = n_FieldValue (other.nativeObj);
	}
	
	public String getName ()
	{
		return n_getName (nativeObj); 
	}
	
	public boolean equals (FieldValue other)
	{
		return n_equals (nativeObj, other.nativeObj);
	}
	
	public void setTo (FieldValue other)
	{
		n_setTo (nativeObj, other.nativeObj);
	}
	
	public void clear ()
	{
	    // TO DO: implementation
	}
	
	@Override
	protected void finalize () throws Throwable 
	{
		if (nativeObj != 0)
	        n_delete (nativeObj);
	    
        super.finalize();
	}
	
	private static native long n_FieldValue ();
	private static native long n_FieldValue (long other);
	private static native long n_FieldValue (String name, long variant);
	private static native String n_getName (long nativeObj); 
	private static native void n_setTo (long nativeObj, long other);
	private static native boolean n_equals (long nativeObj, long other);
	private static native void n_delete (long nativeObj);	
}
