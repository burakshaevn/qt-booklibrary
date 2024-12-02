#pragma once

#include <QMainWindow>
#include <QVBoxLayout>
#include <QFrame>
#include <QLineEdit>
#include <QStackedWidget>
#include <QDockWidget>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QScrollArea>
#include <QDebug>
#include <QStackedWidget>
#include <QMessageBox>
#include <QGroupBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QFormLayout>
#include <QInputDialog>
#include <QSqlRecord>
#include <QComboBox>
#include <QSqlTableModel>
#include <QStandardItemModel>
#include <QTableWidget>
#include <QSqlQuery>
#include <QSqlError>
#include <QJsonArray>
#include <QJsonDocument>

#include "library.h"
#include <deque>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showLibraryPage();          // Отображение страницы библиотеки
    void showBookmarksPage();        // Отображение страницы закладок
    void showPaymentPage();          // Отображение страницы оплаты
    void showProfilePage();          // Отображение страницы профиля

private:
    Ui::MainWindow *ui;

    void setupGenreMenu();           // Настройка меню жанров
    void setupMainContent();         // Настройка главного содержимого
    void setupNavigationPanel();     // Настройка панели навигации
    void setupPages();               // Настройка QStackedWidget и страниц
    void setupLibraryPage();
    void setupBookmarksPage();
    void setupCartPage();
    void setupProfilePage();

    QFrame* createItemFrame(const Book* book, const QVector<QPair<QString, std::function<void()>>>& buttons);
    void clearLayout(QLayout *layout);

    void LoadBooks();
    void filterBooksByGenre(int genreId);
    void displayBooks(QSqlQuery& query);

    bool ConfigureDatabase(const bool need_notify, const QStringView host, int port, const QStringView db_name, const QStringView username, const QStringView password);

    QWidget* createLibraryPage();
    QWidget* createBookmarksPage();
    QWidget* createPaymentPage();
    QWidget* createProfilePage();
    bool verifyPassword(const QString &password, const QString &hash);
    QWidget* createAdminPage();
    QWidget* createUserPage();
    void clearStackedWidget();
    void logout();

    void updateGenreList(QListWidget* genreList, QMap<int, QString>& genreMap);

    QVBoxLayout *mainLayout;         // Основной макет
    QWidget *mainContentWidget;      // Главный виджет содержимого
    QStackedWidget *stackedWidget;   // Стек виджетов для переключения страниц
    QStackedWidget *stackedWidget_profile;   // Стек виджетов для переключения на странице Profile

    QFrame *searchPanel;             // Всплывающая панель поиска
    QLineEdit *searchLineEdit;       // Поле ввода для поиска
    std::unique_ptr<QDockWidget> dockWidget;

    DatabaseManager db_manager_;
    Library* library_;
    QString current_username;
    QString current_role;
    std::deque<Book> books_;
    std::map<const QString, Book*> bookmarks_;
    std::map<const QString, Book*> cart_;
    QListWidget* genreList_;         // Указатель на виджет со списком жанров
    QMap<int, QString> genreMap_;   // Отображение ID жанра на название

};
