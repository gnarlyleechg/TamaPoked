import unittest
from local_palette import allocate
class PaletteTest(unittest.TestCase):
    def test_reclaim(self):
        old=list(range(255)); new=[0xf800,0x07e0,0x001f]
        p,o,n=allocate(old,new,[bytes([0,1,255])],[bytes([0,1,2,255])])
        self.assertEqual(len(p),5)
        self.assertEqual([p[o[i]] for i in (0,1)],[0,1])
        self.assertEqual([p[i] for i in n],new)
    def test_full(self):
        old=list(range(255)); p,o,n=allocate(old,[65535],[bytes(range(255))],[bytes([0])])
        self.assertEqual(len(p),255)
        self.assertEqual([p[i] for i in o],old)
    def test_frequency(self):
        old=list(range(254)); p,o,n=allocate(old,[65535,63488],[bytes(range(254))],[bytes([0,1,1,1])])
        self.assertEqual(p[-1],63488)
    def test_pack_roundtrip(self):
        import struct,tempfile
        from pathlib import Path
        from local_palette import merge_local,equivalent
        from merge_walk_pilot import read
        def pack(pal,acts):
            return b"TPK2"+struct.pack("<BH",len(acts),len(pal))+struct.pack("<"+"H"*len(pal),*pal)+b"".join(struct.pack("<4BH",k,len(data),1,1,80)+bytes(data) for k,data in acts.items())
        with tempfile.TemporaryDirectory() as d:
            a,b,c=[Path(d)/n for n in ("old","new","merged")]
            a.write_bytes(pack(list(range(255)),{0:[0,254,255],1:[2],2:[3],5:[42,255]}))
            b.write_bytes(pack([63488,2016,31],{1:[0,1,255],2:[2,0,255]}))
            c.write_bytes(merge_local(a,b))
            p,x=read(a);q,y=read(c);r,z=read(b)
            self.assertEqual(set(y),set(x))
            for k in (0,5): self.assertTrue(equivalent(p,x[k],q,y[k]))
            for k in (1,2): self.assertTrue(equivalent(r,z[k],q,y[k]))
            self.assertEqual(len(q),6)
if __name__=='__main__': unittest.main()
