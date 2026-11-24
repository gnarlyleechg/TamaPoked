import math
import unittest
from pokemon_motion import GAITS, paw_target, solve_joint

class MotionTests(unittest.TestCase):
    def test_bone_lengths(self):
        a,b,c=(0,0),(3,4),(0,8)
        for dex,(offsets,*_) in GAITS.items():
            for limb in range(len(offsets)):
                for i in range(101):
                    dx,dy,_=paw_target(dex,limb,i/100,10)
                    knee,end=solve_joint(a,b,c,a,(dx,8+dy))
                    self.assertAlmostEqual(math.dist(a,knee),5,places=6)
                    self.assertAlmostEqual(math.dist(knee,end),5,places=6)
        for target in [(0,0),(100,100),(-100,-100)]:
            k,e=solve_joint(a,b,c,a,target)
            self.assertAlmostEqual(math.dist(a,k),5,places=6)
            self.assertAlmostEqual(math.dist(k,e),5,places=6)
    def test_contact_and_loop(self):
        for dex,(offsets,*_) in GAITS.items():
            for limb in range(len(offsets)):
                self.assertEqual(paw_target(dex,limb,0,10),paw_target(dex,limb,1,10))
                for i in range(100):
                    x,y,stance=paw_target(dex,limb,i/100,10)
                    self.assertLessEqual(y,0)
                    if stance:self.assertEqual(y,0)
            for i in range(100):
                self.assertTrue(any(paw_target(dex,j,i/100,10)[2] for j in range(len(offsets))))
if __name__=="__main__":unittest.main()
