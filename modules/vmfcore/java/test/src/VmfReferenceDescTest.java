import junit.framework.*;
import com.intel.vmf.MetadataStream;
import com.intel.vmf.Metadata;
import com.intel.vmf.MetadataSet;
import com.intel.vmf.MetadataDesc;
import com.intel.vmf.FieldDesc;
import com.intel.vmf.ReferenceDesc;
import com.intel.vmf.Variant;
import com.intel.vmf.Vmf;
import com.intel.vmf.MetadataSchema;

import static org.junit.Assert.*;
import org.junit.Test;

public class VmfReferenceDescTest 
{
    @BeforeClass
    public static void init()
    {
        Vmf.initialize();
    }
    
    @AfterClass
    public static void terminate()
    {
        Vmf.terminate();
    }
    
    protected ReferenceDesc ref1;
    protected ReferenceDesc ref2;
    protected ReferenceDesc ref3;
    protected ReferenceDesc ref4;
    
    @Before
    public static void setUp ()
    {
       ref1 = new ReferenceDesc ("friend");
       ref2 = new ReferenceDesc ("colleague", false, true);
       ref3 = new ReferenceDesc ("spouse", true);
       ref4 = new ReferenceDesc ();
    }
    
    @Test
    public void testReferenceDesc ()
    {
        System.out.println("Inside VmfReferenceDescTest.testReferenceDesc()");
        
        assertEquals("friend", ref1.getName ());
        assertEquals("colleague", ref2.getName());
        assertEquals("spouse", ref3.getName());
        assertEquals("", ref4.getName());
        
        assertEquals(false, ref1.isUnique());
        assertEquals(false, ref2.isUnique());
        assertEquals(true, ref3.isUnique());
        assertEquals(false, ref4.isUnique());
        
        assertEquals(false, ref1.isCustom());
        assertEquals(true, ref2.isCustom());
        assertEquals(false, ref3.isCustom());
        assertEquals(false, ref4.isCustom());
    }
}