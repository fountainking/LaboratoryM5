#ifndef THE_BOOK_H
#define THE_BOOK_H

#include "config.h"
#include <SD.h>
#include <SPI.h>
#include <FS.h>
#include <vector>

// The Book states
enum BookState {
  BOOK_SEARCH,        // Main search screen
  BOOK_TOC,           // Table of contents popup
  BOOK_RESULTS,       // Search results list
  BOOK_ARTICLE        // Article viewer
};

// Search result structure
struct SearchResult {
  String title;
  String filename;
  int lineNumber;
  String preview;
  String category;
};

// Category structure
struct BookCategory {
  String name;
  String path;
  uint16_t color;
};

// The Book globals
extern BookState bookState;
extern String searchInput;
extern std::vector<SearchResult> searchResults;
extern int selectedResultIndex;
extern int articleScrollOffset;
extern String currentArticleTitle;
extern std::vector<String> articleLines;
extern int currentCategoryIndex;
extern BookCategory categories[];
extern const int totalCategories;

// Search debouncing
extern unsigned long lastSearchInputTime;
extern const unsigned long SEARCH_DEBOUNCE_MS;
extern bool searchPending;

// Main functions
void enterTheBook();
void exitTheBook();
void drawBookSearch();
void drawTableOfContents();
void drawBookResults();
void drawBookArticle();
void handleBookNavigation(char key);

// Search and data functions
void searchTheBook(String query);
void loadArticle(SearchResult result);
void loadCategory(String categoryPath);
std::vector<SearchResult> searchIndex(String indexPath, String query);
void browseCategories();

// Utility functions
void scrollArticleUp();
void scrollArticleDown();
void clearSearch();

#endif
