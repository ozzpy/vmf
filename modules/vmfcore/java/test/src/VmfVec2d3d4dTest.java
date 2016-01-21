import junit.framework.*;
import com.intel.vmf.MetadataStream;
import com.intel.vmf.Metadata;
import com.intel.vmf.MetadataSet;
import com.intel.vmf.MetadataDesc;
import com.intel.vmf.FieldDesc;
import com.intel.vmf.ReferenceDesc;
import com.intel.vmf.Reference;
import com.intel.vmf.Variant;
import com.intel.vmf.Vmf;
import com.intel.vmf.MetadataSchema;

import static org.junit.Assert.*;
import org.junit.Test;

public class VmfVec2d3d4dTest 
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
    
    protected vmf_vec2d v2d;
    protected vmf_vec3d v3d;
    protected vmf_vec4d v4d;
    
    @Test
    public void testVmfVec2d3d4d ()
    {
        v2d = new vmf_vec2d (1, 2);
        v3d = new vmf_vec3d (1, 2, 3);
        v4d = new vmf_vec4d (1, 2, 3, 4);
        
        v2d1 = new vmf_vec2d ();
        v3d1 = new vmf_vec3d ();
        v4d1 = new vmf_vec4d ();
        
        assertEquals(1, v2d.getX());
        assertEquals(2, v2d.getY());
        
        assertEquals(1, v3d.getX());
        assertEquals(2, v3d.getY());
        assertEquals(3, v3d.getZ());
        
        assertEquals(1, v4d.getX());
        assertEquals(2, v4d.getY());
        assertEquals(3, v4d.getZ());
        assertEquals(4, v4d.getW());
        
        assertEquals(0, v2d1.getX());
        assertEquals(0, v2d1.getY());
        
        assertEquals(0, v3d1.getX());
        assertEquals(0, v3d1.getY());
        assertEquals(0, v3d1.getZ());
        
        assertEquals(0, v4d1.getX());
        assertEquals(0, v4d1.getY());
        assertEquals(0, v4d1.getZ());
        assertEquals(0, v4d1.getW());
    }
}