#ifndef BOOKMARKS_H
#define BOOKMARKS_H

#include <QWidget>
#include <QMainWindow>
#include <QStackedWidget>
#include <QMap>
#include <QVBoxLayout>
#include <QLabel>
#include "domain.h"

class Bookmarks : public QMainWindow
{
public:
    explicit Bookmarks(QStackedWidget* stackedWidget, QWidget* parent = nullptr);

    QWidget* CreateBookmarksPage();

    void AddBookmark(const Book* book);
    void RemoveBookmark(const Book* book);
    QMap<const QString, const Book*> GetBookmarks();

private:
    QStackedWidget* stackedWidget_;
    QMap<const QString, const Book*> bookmarks_;

    QFrame* CreateBookmarkItem(const Book* book);
};

#endif // BOOKMARKS_H
