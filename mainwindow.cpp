#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDockWidget>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QScrollArea>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setupGenreMenu();         // Настройка меню жанров
    setupMainContent();       // Настройка главного прокручиваемого содержимого
    setupNavigationPanel();   // Настройка панели навигации
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Настройка меню жанров слева
void MainWindow::setupGenreMenu() {
    auto *dockWidget = new QDockWidget("Жанры", this);
    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    auto *genreList = new QListWidget(dockWidget);
    genreList->addItems({"Фантастика", "Детектив", "Роман", "Научная литература"});
    dockWidget->setWidget(genreList);
    addDockWidget(Qt::LeftDockWidgetArea, dockWidget);
}

// Настройка центрального прокручиваемого содержимого
void MainWindow::setupMainContent() {
    mainContentWidget = new QWidget(this);
    setCentralWidget(mainContentWidget);

    mainLayout = new QVBoxLayout(mainContentWidget);
    auto *scrollArea = new QScrollArea(mainContentWidget);
    scrollArea->setWidgetResizable(true);

    auto *booksContainer = new QWidget(scrollArea);
    auto *booksLayout = new QVBoxLayout(booksContainer);

    for (int i = 0; i < 50; ++i) {
        auto *bookLabel = new QLabel(QString("Книга %1: Информация о книге").arg(i + 1), booksContainer);
        booksLayout->addWidget(bookLabel);
    }

    booksContainer->setLayout(booksLayout);
    scrollArea->setWidget(booksContainer);
    mainLayout->addWidget(scrollArea);
}

// Настройка нижней панели навигации
void MainWindow::setupNavigationPanel() {
    auto *navigationPanel = new QFrame(this);
    navigationPanel->setFrameShape(QFrame::StyledPanel);
    navigationPanel->setFixedHeight(50);

    auto *navLayout = new QHBoxLayout(navigationPanel);
    auto *libraryButton = new QPushButton("Библиотека", navigationPanel);
    auto *bookmarksButton = new QPushButton("Закладки", navigationPanel);
    auto *searchButton = new QPushButton("Поиск", navigationPanel);
    auto *paymentButton = new QPushButton("Оплата", navigationPanel);
    auto *profileButton = new QPushButton("Профиль", navigationPanel);

    navLayout->addWidget(libraryButton);
    navLayout->addWidget(bookmarksButton);
    navLayout->addWidget(searchButton);
    navLayout->addWidget(paymentButton);
    navLayout->addWidget(profileButton);

    mainLayout->addWidget(navigationPanel);

    // Подключаем кнопки навигации
    connect(libraryButton, &QPushButton::clicked, this, &MainWindow::showLibraryPage);
    connect(bookmarksButton, &QPushButton::clicked, this, &MainWindow::showBookmarksPage);
    connect(searchButton, &QPushButton::clicked, this, &MainWindow::showSearchPanel);
    connect(paymentButton, &QPushButton::clicked, this, &MainWindow::showPaymentPage);
    connect(profileButton, &QPushButton::clicked, this, &MainWindow::showProfilePage);

    // Настраиваем всплывающую панель поиска
    searchPanel = new QFrame(this);
    searchPanel->setFrameShape(QFrame::StyledPanel);
    searchPanel->setFixedHeight(50);
    searchPanel->setFixedWidth(300);
    searchPanel->setWindowFlags(Qt::Popup);

    auto *searchLayout = new QHBoxLayout(searchPanel);
    searchLineEdit = new QLineEdit(searchPanel);
    auto *searchActionButton = new QPushButton("Найти", searchPanel);

    searchLayout->addWidget(searchLineEdit);
    searchLayout->addWidget(searchActionButton);
    connect(searchActionButton, &QPushButton::clicked, this, &MainWindow::performSearch);
}

// Метод поиска
void MainWindow::performSearch() {
    QString query = searchLineEdit->text();
    qDebug() << "Поиск по запросу:" << query;
    searchPanel->hide();
}

// Показ панели поиска
void MainWindow::showSearchPanel() {
    searchPanel->move(mapToGlobal(QPoint(width() / 2 - searchPanel->width() / 2, height() / 2 - searchPanel->height() / 2)));
    searchPanel->show();
    searchLineEdit->setFocus();
}

// Методы для отображения страниц
void MainWindow::showLibraryPage() {
    // Логика для отображения основной страницы библиотеки
    qDebug() << "Переход на страницу библиотеки";
}

void MainWindow::showBookmarksPage() {
    // Логика для отображения страницы с закладками
    qDebug() << "Переход на страницу закладок";
}

void MainWindow::showPaymentPage() {
    // Логика для отображения страницы оплаты
    qDebug() << "Переход на страницу оплаты";
}

void MainWindow::showProfilePage() {
    // Логика для отображения страницы профиля
    qDebug() << "Переход на страницу профиля";
}
