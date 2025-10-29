# Known Issues - LaboratoryM5

## Music Player Not Working

**Status:** BROKEN - Needs Investigation

**Symptom:** Music playback fails when attempting to play MP3 files

**Last Updated:** 2025-10-21

### What We Know:
- Music player UI works and displays MP3 files from `/mp3s` folder
- File selection and navigation functions properly
- Audio playback fails when pressing Enter to play

### Diagnostic Code Present:
The code has extensive logging in `src/audio_manager.cpp` (lines 124-180) that outputs:
- Memory status before MP3 creation
- SD card initialization status
- File open verification
- AudioGeneratorMP3_PSRAM initialization
- Detailed failure reasons

### Possible Causes:
1. **PSRAM Allocator Issue** - New PSRAM-based MP3 decoder may have allocation problems
2. **SD Card SPI Conflict** - Audio library might conflict with SD card SPI access
3. **MP3 File Format** - Files may be incompatible format/bitrate
4. **Memory Allocation** - Insufficient heap or PSRAM for MP3 buffer
5. **AudioOutputM5Speaker** - Custom speaker driver may have initialization issues

### Next Steps to Debug:
1. Connect to serial monitor and capture output when attempting playback
2. Check exact error messages from the diagnostic logging
3. Verify MP3 files are valid (standard MP3, 128-192kbps recommended)
4. Test with known-good MP3 file
5. Monitor heap/PSRAM usage during playback attempt

### Code Locations:
- Music player: `src/music_player.cpp`
- Audio manager: `src/audio_manager.cpp`
- PSRAM allocator: `src/psram_allocator.h`
- PSRAM MP3 decoder: `src/AudioGeneratorMP3_PSRAM.h`
- M5 speaker driver: `src/AudioOutputM5Speaker.h`

### Testing Required:
- [ ] Verify serial output during playback attempt
- [ ] Test with different MP3 files
- [ ] Check if radio streaming works (uses same audio stack)
- [ ] Try standard AudioGeneratorMP3 instead of PSRAM version
- [ ] Verify PSRAM is actually available and initialized

### Workarounds:
None currently - music feature is non-functional

---

## Other Issues

(None reported)
