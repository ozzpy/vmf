//package com.intel.vmf;

public class MetadataSet extends IQuery
{
    static
    {
        System.loadLibrary("vmf");
    }
    
    protected final long nativeObj;
    
    public static final int FRAME_COUNT_ALL = long.MAX_VALUE;
    
    public MetadataSet ()
    {
        nativeObj = n_MetadataSet ();
    }
    
    public void copy (MetadataSet other)
    {
        n_copy (nativeObj, other.nativeObj);
    }
    
    public void setTo (MetadataSet other)
    {
        n_setTo (nativeObj, other.nativeObj);
    }
    
    /*TO DO:
    public MetadataSet query (filter){}
    public MetadataSet queryByReference (Metadata md, filter){}
    */
    
    public MetadataSet queryByFrameIndex (long index)
    {
        return new MetadataSet (n_queryByFrameIndex (nativeObj, index));
    }
    
    public MetadataSet queryBySchema (String schemaName)
    {
        return new MetadataSet (n_queryBySchema (nativeObj, schemaName));
    }
    
    public MetadataSet queryByName (String name)
    {
        return new MetadataSet (n_queryByName (nativeObj, name));
    }
    
    public MetadataSet queryByNameAndValue (String mdName, FieldValue value)
    {
        return new MetadataSet (n_queryByNameAndValue (nativeObj, mdName, value.nativeObj));
    }
    
    public MetadataSet queryByNameAndFields (String mdName, FieldValue fields[])
    {
        int nativeFieldValueObjs[] = new int [fields.length];
        for (int i = 0; i < fields.length; i++)
        {
            nativeFieldValueObjs[i] = fields[i].nativeObj;
        }
        
        return new MetadataSet (n_queryByNameAndFields (nativeObj, mdName, nativeFieldValueObjs));
    }
    
    public MetadataSet queryByReference (String refName)
    {
        return new MetadataSet (n_queryByReference (nativeObj, refName));
    }
    
    public MetadataSet queryByReference (String refName, FieldValue value);
    {
        return new MetadataSet (n_queryByReference (nativeObj, refName, value.nativeObj));
    }
    
    public MetadataSet queryByReference (String refName, FieldValue[] fields)
    {
        int nativeFieldValueObjs[] = new int [fields.length];
        for (int i = 0; i < fields.length; i++)
        {
            nativeFieldValueObjs[i] = fields[i].nativeObj;
        }
        
        return new MetadataSet (n_queryByReference (nativeObj, refName, nativeFieldValueObjs));
    }
    
    public long shift (long nTarFrameIndex, long nSrcFrameIndex)
    {
        long nNumOfFrames = FRAME_COUNT_ALL;
        return n_shift (nTarFrameIndex, nSrcFrameIndex, nNumOfFrames, 0);
    }
    
    public long shift (long nTarFrameIndex, long nSrcFrameIndex, long nNumOfFrames, MetadataSet setFailure)
    {
        return n_shift (nTarFrameIndex, nSrcFrameIndex, nNumOfFrames, setFailure.nativeObj);
    }
    
    @Override
    protected void finalize () throws Throwable 
    {
        if (nativeObj != 0)
            n_delete (nativeObj);
        
        super.finalize();
    }
    
    private native long n_MetadataSet ();
    private native static void n_copy (long nativeObj, long other);
    private native static void n_setTo (long nativeObj, long other);
    private native static MetadataSet n_queryByFrameIndex (long nativeObj, long index);
    private native static MetadataSet n_queryBySchema (long nativeObj, String schemaName);
    private native static MetadataSet n_queryByName (long nativeObj, String name);
    private native static MetadataSet n_queryByNameAndValue (long nativeObj, String mdName, long value);
    private native static MetadataSet n_queryByNameAndFields (long nativeObj, String mdName, long nativeFieldValueObjs[]);
    private native static MetadataSet n_queryByReference (long nativeObj, String refName);
    private native static MetadataSet n_queryByReference (long nativeObj, String refName, long value);
    private native static MetadataSet n_queryByReference (long nativeObj, String refName, long nativeFieldValueObjs[]);
    private native static long n_shift (long nTarFrameIndex, long nSrcFrameIndex, long nNumOfFrames, long setFailure);
    private native static void n_delete (long nativeObj);
}