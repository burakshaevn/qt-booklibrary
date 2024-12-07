#include "bookmarks.h"

Bookmarks::Bookmarks(QStackedWidget* stackedWidget, QWidget* parent)
    : QMainWindow(parent)
    , stackedWidget_(stackedWidget)
{}

QWidget *Bookmarks::CreateBookmarksPage() {
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    layout->addWidget(new QLabel("Bookmarks page.", page));
    return page;
}

void Bookmarks::AddBookmark(const Book* book){
    bookmarks_[book->title] = book;
}
void Bookmarks::RemoveBookmark(const Book* book){
    auto it = bookmarks_.find(book->title);
    if (it != bookmarks_.end()){
        bookmarks_.erase(it);
    }
}

QMap<const QString, const Book*> Bookmarks::GetBookmarks(){
    return bookmarks_;
}
