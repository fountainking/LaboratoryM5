#include "the_book.h"
#include "ui.h"
#include "settings.h"
#include "ascii_art.h"

// Global state
BookState bookState = BOOK_SEARCH;
String searchInput = "";
std::vector<SearchResult> searchResults;
int selectedResultIndex = 0;
int articleScrollOffset = 0;
String currentArticleTitle = "";
std::vector<String> articleLines;
int currentCategoryIndex = 0;

// Categories for The Book (all 15 categories)
BookCategory categories[] = {
  {"Wikipedia", "/the_book/wikipedia", TFT_BLUE},
  {"Science", "/the_book/science", TFT_CYAN},
  {"History", "/the_book/history", TFT_ORANGE},
  {"Literature", "/the_book/literature", TFT_MAGENTA},
  {"Religious Texts", "/the_book/religious", TFT_PURPLE},
  {"Medical", "/the_book/medical", TFT_RED},
  {"Survival", "/the_book/survival", TFT_YELLOW},
  {"Programming", "/the_book/programming", TFT_GREEN},
  {"Prog Languages", "/the_book/programming_languages", TFT_DARKGREEN},
  {"Edible Plants", "/the_book/edible_plants", TFT_GREENYELLOW},
  {"Poisonous Plants", "/the_book/poisonous_plants", TFT_MAROON},
  {"Agriculture", "/the_book/agriculture", TFT_OLIVE},
  {"DIY/Technical", "/the_book/diy_technical", TFT_NAVY},
  {"Reference", "/the_book/reference", TFT_DARKGREY},
  {"Tech", "/the_book/tech", TFT_LIGHTGREY}
};
const int totalCategories = 15;

// Display constants
const int RESULTS_PER_PAGE = 8;
const int LINES_PER_PAGE = 13;  // Lines of text that fit on screen

void enterTheBook() {
  bookState = BOOK_SEARCH;
  searchInput = "";
  searchResults.clear();
  selectedResultIndex = 0;
  articleScrollOffset = 0;
  currentCategoryIndex = 0;

  // Initialize SD card if not already done
  SPI.begin(SD_SPI_SCK_PIN, SD_SPI_MISO_PIN, SD_SPI_MOSI_PIN, SD_SPI_CS_PIN);
  if (!SD.begin(SD_SPI_CS_PIN, SPI, SD_SPI_FREQ)) {
    Serial.println("SD card mount failed for The Book");
  }

  // Check if data folder exists
  if (!SD.exists("/the_book")) {
    Serial.println("WARNING: /the_book folder not found on SD card!");
  }

  drawBookSearch();
}

void exitTheBook() {
  searchResults.clear();
  articleLines.clear();
}

void drawBookSearch() {
  M5Cardputer.Display.fillScreen(TFT_WHITE);

  // "Offline Knowledge Base" title (black, moved down)
  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(TFT_BLACK);
  M5Cardputer.Display.drawString("Offline Knowledge Base", 50, 15);

  // Large rounded search bar
  M5Cardputer.Display.drawRoundRect(15, 30, 210, 30, 15, TFT_BLACK);
  M5Cardputer.Display.drawRoundRect(16, 31, 208, 28, 14, TFT_BLACK);  // Double outline

  // "Search:" label - black text
  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(TFT_BLACK);
  M5Cardputer.Display.drawString("Search:", 25, 40);

  // Show search input with blinking cursor - black text
  String displayInput = searchInput;
  if (millis() % 1000 < 500) {
    displayInput += "_";
  }
  M5Cardputer.Display.drawString(displayInput, 75, 40);

  // Show live search results preview under search bar
  if (searchResults.size() > 0 && searchInput.length() >= 2) {
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setTextColor(TFT_BLACK);
    String preview = String(searchResults.size()) + " results - UP/DOWN to select";
    M5Cardputer.Display.drawString(preview, 20, 70);

    // Calculate scroll window to keep selected result visible
    const int maxVisible = 5;  // Max results visible at once
    int startIdx = selectedResultIndex - maxVisible / 2;
    startIdx = max(0, min(startIdx, (int)searchResults.size() - maxVisible));
    startIdx = max(0, startIdx);  // Ensure non-negative
    int endIdx = min((int)searchResults.size(), startIdx + maxVisible);

    // Show scrolling results window
    int yPos = 85;
    for (int i = startIdx; i < endIdx; i++) {
      // Yellow highlight for selected result
      if (i == selectedResultIndex) {
        M5Cardputer.Display.fillRect(18, yPos - 2, 204, 11, TFT_YELLOW);
      }

      M5Cardputer.Display.setTextColor(TFT_BLACK);
      String title = "> " + searchResults[i].title;
      if (title.length() > 35) {
        title = title.substring(0, 32) + "...";
      }
      M5Cardputer.Display.drawString(title, 20, yPos);
      yPos += 11;
    }
  }
}

void drawTableOfContents() {
  // Draw semi-transparent overlay (simulate by redrawing search screen first)
  drawBookSearch();

  // Draw popup window (taller to fit all categories)
  int popupX = 30;
  int popupY = 20;
  int popupW = 180;
  int popupH = 100;

  // Popup background (white with black border)
  M5Cardputer.Display.fillRoundRect(popupX, popupY, popupW, popupH, 8, TFT_WHITE);
  M5Cardputer.Display.drawRoundRect(popupX, popupY, popupW, popupH, 8, TFT_BLACK);
  M5Cardputer.Display.drawRoundRect(popupX + 1, popupY + 1, popupW - 2, popupH - 2, 7, TFT_BLACK);

  // Title
  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(TFT_ORANGE);
  M5Cardputer.Display.drawString("Table of Contents", popupX + 30, popupY + 8);

  // Draw categories list
  M5Cardputer.Display.setTextSize(1);
  int yPos = popupY + 22;
  for (int i = 0; i < totalCategories; i++) {
    M5Cardputer.Display.setTextColor(categories[i].color);
    String categoryText = "> " + categories[i].name;
    M5Cardputer.Display.drawString(categoryText, popupX + 10, yPos);
    yPos += 11;
  }
}

void drawBookResults() {
  M5Cardputer.Display.fillScreen(TFT_WHITE);
  M5Cardputer.Display.setTextColor(TFT_BLACK);
  M5Cardputer.Display.setTextSize(1);

  // Title
  M5Cardputer.Display.setTextColor(TFT_ORANGE);
  M5Cardputer.Display.drawString("Search Results", 10, 5);

  // Results count
  M5Cardputer.Display.setTextColor(TFT_DARKGREY);
  String countStr = "Found: " + String(searchResults.size()) + " results";
  M5Cardputer.Display.drawString(countStr, 10, 15);

  if (searchResults.size() == 0) {
    M5Cardputer.Display.setTextColor(TFT_ORANGE);
    M5Cardputer.Display.drawString("No results found", 10, 50);
    M5Cardputer.Display.setTextColor(TFT_DARKGREY);
    M5Cardputer.Display.drawString("Try a different search term", 10, 65);
    M5Cardputer.Display.drawString("` Back", 5, 120);
    return;
  }

  // Calculate scroll window
  int startIdx = max(0, selectedResultIndex - RESULTS_PER_PAGE / 2);
  int endIdx = min((int)searchResults.size(), startIdx + RESULTS_PER_PAGE);

  // Draw results
  int y = 30;
  for (int i = startIdx; i < endIdx; i++) {
    if (i == selectedResultIndex) {
      M5Cardputer.Display.fillRect(5, y - 2, 230, 13, TFT_LIGHTGREY);
    }

    // Truncate title if too long
    String title = searchResults[i].title;
    if (title.length() > 35) {
      title = title.substring(0, 32) + "...";
    }

    M5Cardputer.Display.setTextColor(TFT_BLUE);
    M5Cardputer.Display.drawString("> " + title, 8, y);

    // Category tag
    M5Cardputer.Display.setTextColor(TFT_DARKGREY);
    M5Cardputer.Display.drawString("[" + searchResults[i].category + "]", 200, y);

    y += 13;
  }

  // Instructions
  M5Cardputer.Display.setTextColor(TFT_DARKGREY);
  M5Cardputer.Display.drawString("UP/DOWN: Navigate | ENTER: Open", 5, 120);
  M5Cardputer.Display.drawString("` Back", 5, 130);
}

void drawBookArticle() {
  M5Cardputer.Display.fillScreen(TFT_WHITE);
  M5Cardputer.Display.setTextColor(TFT_BLACK);
  M5Cardputer.Display.setTextSize(1);

  // Title bar
  M5Cardputer.Display.fillRect(0, 0, 240, 18, TFT_LIGHTGREY);
  M5Cardputer.Display.setTextColor(TFT_ORANGE);
  String truncTitle = currentArticleTitle;
  if (truncTitle.length() > 35) {
    truncTitle = truncTitle.substring(0, 32) + "...";
  }
  M5Cardputer.Display.drawString(truncTitle, 5, 5);

  // Article content
  int y = 22;
  int startLine = articleScrollOffset;
  int endLine = min((int)articleLines.size(), startLine + LINES_PER_PAGE);

  M5Cardputer.Display.setTextColor(TFT_BLACK);
  for (int i = startLine; i < endLine; i++) {
    String line = articleLines[i];

    // Word wrap for long lines
    if (line.length() > 39) {
      line = line.substring(0, 39);
    }

    M5Cardputer.Display.drawString(line, 5, y);
    y += 9;
  }

  // Scroll indicator
  if (articleLines.size() > LINES_PER_PAGE) {
    M5Cardputer.Display.setTextColor(TFT_DARKGREY);
    String scrollInfo = String(startLine + 1) + "-" + String(endLine) + "/" + String(articleLines.size());
    M5Cardputer.Display.drawString(scrollInfo, 190, 5);
  }

  // Instructions
  M5Cardputer.Display.setTextColor(TFT_DARKGREY);
  M5Cardputer.Display.drawString("UP/DOWN: Scroll | ` Back", 5, 128);
}

void handleBookNavigation(char key) {
  if (bookState == BOOK_TOC) {
    // Any key closes the table of contents popup
    bookState = BOOK_SEARCH;
    if (settings.soundEnabled) M5Cardputer.Speaker.tone(800, 50);
    drawBookSearch();
  } else if (bookState == BOOK_RESULTS) {
    if (key == ';' || key == ',') {
      // Navigate results up
      if (selectedResultIndex > 0) {
        selectedResultIndex--;
        if (settings.soundEnabled) M5Cardputer.Speaker.tone(800, 50);
        drawBookResults();
      }
    } else if (key == '.' || key == '/') {
      // Navigate results down
      if (selectedResultIndex < (int)searchResults.size() - 1) {
        selectedResultIndex++;
        if (settings.soundEnabled) M5Cardputer.Speaker.tone(1000, 50);
        drawBookResults();
      }
    }
  } else if (bookState == BOOK_ARTICLE) {
    if (key == ';' || key == ',') {
      scrollArticleUp();
    } else if (key == '.' || key == '/') {
      scrollArticleDown();
    }
  }
}

std::vector<SearchResult> searchIndex(String indexPath, String query) {
  std::vector<SearchResult> results;

  // Check if index file exists
  if (!SD.exists(indexPath)) {
    Serial.println("Index file not found: " + indexPath);
    return results;
  }

  File indexFile = SD.open(indexPath, FILE_READ);
  if (!indexFile) {
    Serial.println("Failed to open index: " + indexPath);
    return results;
  }

  query.toLowerCase();

  // Read index line by line
  while (indexFile.available() && results.size() < 50) {  // Limit to 50 results
    String line = indexFile.readStringUntil('\n');
    line.trim();

    // Skip empty lines
    if (line.length() == 0) continue;

    // Check if query matches (case-insensitive)
    String lowerLine = line;
    lowerLine.toLowerCase();
    if (lowerLine.indexOf(query) == -1) continue;

    // Parse: KEYWORD|FILENAME|LINE|PREVIEW
    int pipe1 = line.indexOf('|');
    int pipe2 = line.indexOf('|', pipe1 + 1);
    int pipe3 = line.indexOf('|', pipe2 + 1);

    if (pipe1 > 0 && pipe2 > pipe1 && pipe3 > pipe2) {
      SearchResult result;
      result.title = line.substring(0, pipe1);
      result.filename = line.substring(pipe1 + 1, pipe2);
      result.lineNumber = line.substring(pipe2 + 1, pipe3).toInt();
      result.preview = line.substring(pipe3 + 1);

      // Extract category from path
      if (indexPath.indexOf("wikipedia") >= 0) {
        result.category = "Wiki";
      } else if (indexPath.indexOf("programming") >= 0) {
        result.category = "Code";
      } else if (indexPath.indexOf("survival") >= 0) {
        result.category = "Survival";
      } else if (indexPath.indexOf("religious") >= 0) {
        result.category = "Religious";
      } else if (indexPath.indexOf("edible") >= 0) {
        result.category = "Plants";
      } else {
        result.category = "Other";
      }

      results.push_back(result);
    }
  }

  indexFile.close();
  return results;
}

void searchTheBook(String query) {
  if (query.length() < 2) {
    searchResults.clear();
    return;
  }

  searchResults.clear();
  selectedResultIndex = 0;

  // Search all category indexes
  for (int i = 0; i < totalCategories; i++) {
    String indexPath = categories[i].path + "/index.txt";
    std::vector<SearchResult> catResults = searchIndex(indexPath, query);

    // Add results from this category
    for (size_t j = 0; j < catResults.size(); j++) {
      searchResults.push_back(catResults[j]);
    }
  }

  Serial.println("Search found " + String(searchResults.size()) + " results");

  bookState = BOOK_RESULTS;
  drawBookResults();
}

void loadArticle(SearchResult result) {
  articleLines.clear();
  articleScrollOffset = 0;
  currentArticleTitle = result.title;

  // Build full path
  String fullPath = "";
  if (result.category == "Wiki") {
    fullPath = "/the_book/wikipedia/" + result.filename;
  } else if (result.category == "Code") {
    fullPath = "/the_book/programming_languages/" + result.filename;
  } else if (result.category == "Survival") {
    fullPath = "/the_book/survival/" + result.filename;
  } else if (result.category == "Religious") {
    fullPath = "/the_book/religious/" + result.filename;
  } else if (result.category == "Plants") {
    fullPath = "/the_book/edible_plants/" + result.filename;
  }

  // Check if file exists
  if (!SD.exists(fullPath)) {
    articleLines.push_back("Error: Article file not found");
    articleLines.push_back("");
    articleLines.push_back("Path: " + fullPath);
    bookState = BOOK_ARTICLE;
    drawBookArticle();
    return;
  }

  File articleFile = SD.open(fullPath, FILE_READ);
  if (!articleFile) {
    articleLines.push_back("Error: Could not open article");
    bookState = BOOK_ARTICLE;
    drawBookArticle();
    return;
  }

  // Seek to the line number if specified
  if (result.lineNumber > 0) {
    for (int i = 0; i < result.lineNumber && articleFile.available(); i++) {
      articleFile.readStringUntil('\n');
    }
  }

  // Read article content (limit to 500 lines to avoid memory issues)
  int linesRead = 0;
  while (articleFile.available() && linesRead < 500) {
    String line = articleFile.readStringUntil('\n');
    line.trim();

    // Handle long lines - split into multiple display lines
    while (line.length() > 39) {
      articleLines.push_back(line.substring(0, 39));
      line = line.substring(39);
      linesRead++;
    }

    if (line.length() > 0 || articleLines.size() > 0) {
      articleLines.push_back(line);
      linesRead++;
    }
  }

  articleFile.close();

  if (articleLines.size() == 0) {
    articleLines.push_back("(Empty article)");
  }

  bookState = BOOK_ARTICLE;
  drawBookArticle();
}

void scrollArticleUp() {
  if (articleScrollOffset > 0) {
    articleScrollOffset--;
    if (settings.soundEnabled) M5Cardputer.Speaker.tone(800, 30);
    drawBookArticle();
  }
}

void scrollArticleDown() {
  if (articleScrollOffset < (int)articleLines.size() - LINES_PER_PAGE) {
    articleScrollOffset++;
    if (settings.soundEnabled) M5Cardputer.Speaker.tone(1000, 30);
    drawBookArticle();
  }
}

void clearSearch() {
  searchInput = "";
  searchResults.clear();
  selectedResultIndex = 0;
}
