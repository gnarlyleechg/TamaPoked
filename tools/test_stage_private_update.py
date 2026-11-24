import hashlib,struct,unittest
from stage_private_update import partitions,plan

def entry(t,s,o,n,label):return struct.pack('<HBBII16sI',0x50aa,t,s,o,n,label.encode(),0)
def table(*entries):
    b=b''.join(entries);return b+b'\xeb\xeb'+b'\xff'*14+hashlib.md5(b).digest()+b'\xff'*32
NVS=entry(1,2,0x9000,0x5000,'nvs')
APP=entry(0,0x10,0x10000,0x300000,'app0')
FAT=entry(1,0x81,0x610000,0x900000,'ffat')
T=table(NVS,APP,FAT)
B=bytearray(10000);B[0]=0xe9;B[1]=1;B[12]=9
class Test(unittest.TestCase):
 def test_valid(self):self.assertEqual(plan(B,T,T)['app_offset'],0x10000)
 def test_bad_digest(self):
  b=bytearray(T);b[96+16]^=1
  with self.assertRaises(ValueError):partitions(b)
 def test_layout_change(self):
  with self.assertRaises(ValueError):plan(B,T,table(NVS,APP,entry(1,0x81,0x620000,0x800000,'ffat')))
 def test_overlap(self):
  with self.assertRaises(ValueError):partitions(table(NVS,APP,entry(1,0x81,0x20000,0x1000,'ffat')))
 def test_wrong_chip(self):
  b=bytearray(B);b[12]=0
  with self.assertRaises(ValueError):plan(b,T,T)
 def test_oversized(self):
  b=B+bytearray(0x300000)
  with self.assertRaises(ValueError):plan(b,T,T)
 def test_missing_nvs(self):
  t=table(APP,FAT)
  with self.assertRaises(ValueError):plan(B,t,t)
 def test_invalid_padding(self):
  with self.assertRaises(ValueError):partitions(T[:-1]+b'\0')
 def test_wrong_slot(self):
  t=table(NVS,entry(0,0x11,0x10000,0x300000,'app1'),FAT)
  with self.assertRaises(ValueError):plan(B,t,t)
if __name__=='__main__':unittest.main()
