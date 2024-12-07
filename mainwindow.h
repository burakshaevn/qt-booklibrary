#pragma once

#include <QMainWindow>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QLineEdit>
#include <QDockWidget>
#include <QPushButton>
#include <QFrame>

#include "library.h"
#include "bookmarks.h"
#include "cart.h"
#include "profile.h"
#include "database_manager.h"

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
    void showLibraryPage();
    void showBookmarksPage();
    void showCartPage();
    void showProfilePage();
    void OnUserLoggedIn();
    void OnUserLogOut();

private:
    void setupMainContent();      // Инициализация страниц
    void setupNavigationPanel();  // Настройка нижней панели навигации
    void setupLibraryPage();      // Отображение страницы библиотека
    void setupBookmarksPage();    // Отображение страницы закладки
    void setupCartPage();         // Отображение страницы корзина
    void setupGenreMenu();        // Настройка меню жанров

    void FilterBooksByGenre(const QStringView selectedGenreName);
    void UpdateGenreList(QListWidget* genreList, QMap<int, QString>& genreMap);

    // Конфигурирует подключение к базе данных
    bool ConfigureDatabase(const bool need_notify, const QStringView host, int port, const QStringView db_name, const QStringView username, const QStringView password);

    Ui::MainWindow* ui;
    QStackedWidget* stackedWidget_;  // Стек виджетов для страниц

    std::unique_ptr<QDockWidget> genreWidget;
    QListWidget* genreList_;
    QMap<int, QString> genreMap_;

    DatabaseManager db_manager_;     // Менеджер базы данных
    Library* library_;               // Объект библиотеки
    Bookmarks* bookmarks_;           // Закладки
    Cart* cart_;                     // Корзина
    Profile* profile_;               // Профиль
};
