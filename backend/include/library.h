#pragma once

#include <QObject>
#include <QWidget>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QListWidget>
#include <QMap>
#include <QScrollArea>
#include <QLabel>
#include <QMessageBox>
#include <QDockWidget>
#include <QPushButton>

#include <deque>

#include "database_manager.h"
#include "domain.h"
#include "bookmarks.h"
#include "cart.h"

class Library : public QMainWindow
{
    Q_OBJECT

public:
    explicit Library(QStackedWidget* stackedWidget, DatabaseManager& dbManager, QWidget* parent = nullptr);

    QWidget* CreateLibraryPage();
    void UpdateLibraryPage();

    void SetBookmarks(Bookmarks* bookmarks);
    void AddBookmark(const Book* book);
    void RemoveBookmark(const Book* book);
    QMap<const QString, const Book*> GetBookmarks();

    void SetCart(Cart* cart);
    void AddToCart(const Book* book);
    void RemoveFromCart(const Book* book);
    QMap<const QString, const Book*> GetCart();

    void LoadBooks();
    std::deque<Book> GetBooks() const;

    void DisplayBooks(const std::deque<Book>& books);
    void DisplayBooks(QSqlQuery& query);

    QFrame* CreateItemFrame(const Book* book, const QVector<QPair<QString, std::function<void()>>>& buttons);

private:
    QStackedWidget* stackedWidget_;
    DatabaseManager& db_manager_;
    QVBoxLayout* libraryLayout_;

    std::deque<Book> books_;
    Bookmarks* bookmarks_;
    Cart* cart_;
};
