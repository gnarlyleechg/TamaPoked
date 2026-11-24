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
if __name__=='__main__': unittest.main()
