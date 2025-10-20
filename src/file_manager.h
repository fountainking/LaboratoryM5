#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "config.h"
#include <SD.h>
#include <SPI.h>
#include <FS.h>
#include <AnimatedGIF.h>

// File Manager states
enum FileManagerState {
  FM_FOLDER_VIEW,
  FM_FILE_LIST,
  FM_FILE_VIEWER
};

// File types
enum FileType {
  TYPE_FOLDER,
  TYPE_PDF,
  TYPE_IMAGE,
  TYPE_GIF,
  TYPE_VIDEO,
  TYPE_AUDIO,
  TYPE_TEXT,
  TYPE_MODEL,
  TYPE_UNKNOWN
};

// File info structure
struct FileInfo {
  String name;
  FileType type;
  size_t size;
  time_t modified;
  bool isDirectory;
};

// File Manager functions
void enterFileManager();
void drawFolderView();
void drawFileList();
void drawFileViewer();
void drawTextViewer(const String& path);
void drawImageViewer(const String& path);
void drawGifViewer(const String& path);
void drawPDFViewer(const String& path);
void drawFileOperationsMenu();
void loadFolder(String path);
void loadFile(String path);
FileType getFileType(String filename);
void handleFileManagerNavigation(char key);
void updateGifPlayback();
void stopGifPlayback();
bool isGifPlaying();
String formatFileSize(size_t bytes);
void showDeleteConfirmation();
void deleteCurrentFile();
void showBatchDeleteConfirmation();
void batchDeleteFiles();
void cutFile();
void copyFile();
void pasteFile();
void renameFile();
void createFolder();
extern bool showOperationsMenu;

// File search globals
extern String searchQuery;
extern bool searchActive;

// Batch selection globals
extern bool fileSelected[50];
extern int selectedCount;

// Audio player functions
void drawAudioPlayer(const String& path);
void playAudioFile(const String& path);
void stopAudioPlayback();
bool isAudioPlaying();
void updateAudioPlayback();
void setAudioVolume(int vol); // 0-100
int getAudioVolume();
void audioVolumeUp();
void audioVolumeDown();

// GIF callback functions (used by both file_manager and main)
void * GIFOpenFile(const char *fname, int32_t *pSize);
void GIFCloseFile(void *pHandle);
int32_t GIFReadFile(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen);
int32_t GIFSeekFile(GIFFILE *pFile, int32_t iPosition);
void GIFDraw(GIFDRAW *pDraw);

// File Manager globals
extern FileManagerState fmState;
extern String currentPath;
extern FileInfo fileInfoList[50];
extern int fileCount;
extern int selectedFileIndex;
extern bool sdCardMounted;
extern int gifYOffset;
extern String currentAudioPath;

#endif