#include "backupsd.h"
#include "save.h"
#include "sdmon.h"
#include <Arduino.h>
#include <FS.h>
#include <SD_MMC.h>

static const char *DIR_BK = "/backup";
static const char *F_NEW = "/backup/save.bin";
static const char *F_OLD = "/backup/save.old";
static const char *F_TMP = "/backup/save.tmp";

static uint8_t *bkAlloc(size_t n) {
  uint8_t *p = (uint8_t *)ps_malloc(n);       // PSRAM first
  if (!p) p = (uint8_t *)malloc(n);
  return p;
}

bool sdBackupWrite() {
  if (!sdReady) return false;
  size_t cap = saveExportSize();
  uint8_t *buf = bkAlloc(cap);
  if (!buf) return false;
  size_t n = saveExport(buf, cap);
  if (!n) { free(buf); return false; }

  if (!SD_MMC.exists(DIR_BK)) SD_MMC.mkdir(DIR_BK);
  if (SD_MMC.exists(F_TMP)) SD_MMC.remove(F_TMP);
  File f = SD_MMC.open(F_TMP, FILE_WRITE);
  if (!f) { free(buf); return false; }
  size_t w = f.write(buf, n);
  f.close();
  free(buf);
  if (w != n) { SD_MMC.remove(F_TMP); return false; }

  // verify what landed before it replaces anything
  File v = SD_MMC.open(F_TMP, FILE_READ);
  if (!v || v.size() != n) { if (v) v.close(); SD_MMC.remove(F_TMP); return false; }
  v.close();

  // rotate: tmp -> save.bin, previous save.bin -> save.old
  if (SD_MMC.exists(F_OLD)) SD_MMC.remove(F_OLD);
  if (SD_MMC.exists(F_NEW)) SD_MMC.rename(F_NEW, F_OLD);
  if (!SD_MMC.rename(F_TMP, F_NEW)) {
    if (SD_MMC.exists(F_OLD)) SD_MMC.rename(F_OLD, F_NEW);  // put the old one back
    return false;
  }
  Serial.printf("backup: wrote %u bytes to %s\n", (unsigned)n, F_NEW);
  return true;
}

static bool tryImport(const char *path) {
  File f = SD_MMC.open(path, FILE_READ);
  if (!f) return false;
  size_t n = f.size();
  if (n < 16 || n > 256 * 1024) { f.close(); return false; }
  uint8_t *buf = bkAlloc(n);
  if (!buf) { f.close(); return false; }
  size_t r = f.read(buf, n);
  f.close();
  bool ok = (r == n) && saveImport(buf, n);   // saveImport validates the whole blob first
  free(buf);
  return ok;
}

bool sdBackupRestoreFresh() {
  if (!sdReady) return false;
  if (tryImport(F_NEW)) { Serial.println("backup: restored from save.bin"); return true; }
  if (tryImport(F_OLD)) { Serial.println("backup: restored from save.old"); return true; }
  return false;
}

void sdBackupDelete() {
  if (!sdReady) return;
  if (SD_MMC.exists(F_NEW)) SD_MMC.remove(F_NEW);
  if (SD_MMC.exists(F_OLD)) SD_MMC.remove(F_OLD);
  if (SD_MMC.exists(F_TMP)) SD_MMC.remove(F_TMP);
}

void sdBackupTick(uint32_t epoch, uint32_t nowMs) {
  static uint32_t lastDay = 0;
  if (epoch == 0 || nowMs < 60000) return;     // no valid clock yet, or still booting
  uint32_t day = epoch / 86400UL;
  if (day == lastDay) return;
  lastDay = day;                                // one attempt per day; a failure waits for tomorrow
  sdBackupWrite();
}
