#pragma once
#include <stdint.h>

// Kids build: daily whole-save backup to the SD card, and an automatic restore
// after a flash that erased the save. The save format is save.cpp's (validated,
// checksummed). Nothing here ever formats or deletes anything except its own
// two files in /backup.
//
//   /backup/save.bin  newest backup
//   /backup/save.old  the one before it (kept in case the newest is bad)

bool sdBackupWrite();          // writes a backup now; false if no SD or any step failed
bool sdBackupRestoreFresh();   // true if a valid backup was imported into an EMPTY save (caller restarts)
void sdBackupDelete();         // removes our two files (called by the WIPE command only)
void sdBackupTick(uint32_t epoch, uint32_t nowMs);  // call from loop(); writes once per real day
