#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , genreList_(nullptr)
{
    ui->setupUi(this);

    QString host = "localhost";
    int port = 5432;
    QString db_name = "booklibrary";
    QString login = "postgres";
    QString password = "89274800234Nn";
    ConfigureDatabase(false, host, port, db_name, login, password);

    setCentralWidget(new QWidget(this));
    stackedWidget_ = new QStackedWidget(this);
    setupMainContent();
    setupNavigationPanel();
    connect(profile_, &Profile::UserLoggedIn, this, &MainWindow::OnUserLoggedIn);
    connect(profile_, &Profile::UserLogout, this, &MainWindow::OnUserLogOut);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showLibraryPage() {
    stackedWidget_->setCurrentIndex(0); // Переход на страницу библиотеки
    library_->UpdateLibraryPage();
}

void MainWindow::showBookmarksPage() {
    stackedWidget_->setCurrentIndex(1); // Переход на страницу закладок
}

void MainWindow::showCartPage() {
    stackedWidget_->setCurrentIndex(2); // Переход на страницу оплаты
}

void MainWindow::showProfilePage() {
    stackedWidget_->setCurrentIndex(3); // Переход на страницу профиля
}

void MainWindow::OnUserLoggedIn(){
    setupGenreMenu();
}

void MainWindow::OnUserLogOut(){
    genreWidget.reset();
}

// Создаём страницы для навигационной панели
void MainWindow::setupMainContent() {
    setCentralWidget(stackedWidget_);

    library_ = new Library(stackedWidget_, db_manager_, this);
    bookmarks_ = new Bookmarks(stackedWidget_, this);
    cart_ = new Cart(stackedWidget_, this);
    profile_ = new Profile(library_, this);

    library_->SetBookmarks(bookmarks_);
    library_->SetCart(cart_);

    stackedWidget_->addWidget(library_);
    stackedWidget_->addWidget(bookmarks_);
    stackedWidget_->addWidget(cart_);
    stackedWidget_->addWidget(profile_);

    stackedWidget_->setCurrentWidget(profile_);
}

// Настройка нижней панели навигации
void MainWindow::setupNavigationPanel() {
    auto *navigationPanel = new QFrame(this);
    navigationPanel->setFrameShape(QFrame::StyledPanel);
    navigationPanel->setFixedHeight(50);

    auto *navLayout = new QHBoxLayout(navigationPanel);
    auto *libraryButton = new QPushButton("Library", navigationPanel);
    auto *bookmarksButton = new QPushButton("Bookmarks", navigationPanel);
    auto *paymentButton = new QPushButton("Cart", navigationPanel);
    auto *profileButton = new QPushButton("Profile", navigationPanel);

    navLayout->addWidget(libraryButton);
    navLayout->addWidget(bookmarksButton);
    navLayout->addWidget(paymentButton);
    navLayout->addWidget(profileButton);

    auto *centralWidget = new QWidget(this);
    auto *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addWidget(stackedWidget_);
    mainLayout->addWidget(navigationPanel);

    setCentralWidget(centralWidget);

    connect(libraryButton, &QPushButton::clicked, this, [this]() {
        if (db_manager_.Open()) {
            if (profile_->GetUserRole() == UserRoles::User){
                setupLibraryPage();
            }
            else {
                QMessageBox::warning(this, "Error", "Log in as a USER.");
            }
        }
        else {
            QMessageBox::warning(this, "Error", "Need connect to database.");
        }
    });
    connect(bookmarksButton, &QPushButton::clicked, this, [this]() {
        if (db_manager_.Open()){
            if (profile_->GetUserRole() == UserRoles::User){
                setupBookmarksPage();
            }
            else {
                QMessageBox::warning(this, "Error", "Log in as a USER.");
            }
        }
        else {
            QMessageBox::warning(this, "Error", "Need connect to database.");
        }
    });
    connect(paymentButton, &QPushButton::clicked, this, [this]() {
        if (db_manager_.Open()) {
            if (profile_->GetUserRole() == UserRoles::User){
                setupCartPage();
            }
            else {
                QMessageBox::warning(this, "Error", "Log in as a USER.");
            }
        }
        else {
            QMessageBox::warning(this, "Error", "Need connect to database.");
        }
    });
    connect(profileButton, &QPushButton::clicked, this, [this]() {
        stackedWidget_->setCurrentWidget(profile_);
    });
}

void MainWindow::setupLibraryPage() {
    auto books = library_->GetBooks();
    if (books.empty()) {
        qWarning() << "No books to display.";
        return;
    }

    QWidget* booksContainer = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(booksContainer);

    for (auto& book : books) {
        QFrame* bookFrame = library_->CreateItemFrame(&book, {
               {"Add to Cart", [this, book]() {
                    library_->AddToCart(&book);
                    QMessageBox::information(this, "Cart", "The book has been added to the cart.");
                }},
               {"Add to Bookmarks", [this, book]() {
                 library_->AddBookmark(&book);
                    QMessageBox::information(this, "Bookmarks", "The book has been added to bookmarks.");
                }}
           });
        layout->addWidget(bookFrame);
    }

    QScrollArea* scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(booksContainer);

    stackedWidget_->addWidget(scrollArea);
    stackedWidget_->setCurrentWidget(scrollArea);
}

void MainWindow::setupBookmarksPage() {
    stackedWidget_->setCurrentIndex(1);

    // Контейнер для закладок
    QWidget* bookmarksContainer = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(bookmarksContainer);

    auto bookmarks = library_->GetBookmarks();

    if (bookmarks.empty()) {
        QLabel* emptyLabel = new QLabel("Bookmarks is empty.");
        emptyLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(emptyLabel);
    } else {
        for (const auto& book : bookmarks) {
            QFrame* bookmarkFrame = library_->CreateItemFrame(book,{
              {"Add to Cart", [this, book]() {
               if (book->price < 0) {
                   QMessageBox::warning(this, "Error", "Uncnown price.");
                   return;
               }
                library_->AddToCart(book);
                QMessageBox::information(this, "Cart", "Added to cart.");
               }},
              {"Delete from bookmarks", [this, book]() {
                    bookmarks_->RemoveBookmark(book);
                   setupBookmarksPage();
               }}
             });
            layout->addWidget(bookmarkFrame);
        }
    }

    // Устанавливаем контейнер в область прокрутки
    QScrollArea* scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(bookmarksContainer);

    // Добавляем в стек
    stackedWidget_->addWidget(scrollArea);
    stackedWidget_->setCurrentWidget(scrollArea);
}

void MainWindow::setupCartPage() {
    stackedWidget_->setCurrentIndex(2);

    // Контейнер для корзины
    QWidget* cartContainer = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(cartContainer);

    auto cart = library_->GetCart();

    if (cart.empty()) {
        QLabel* emptyLabel = new QLabel("Cart is empty.");
        emptyLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(emptyLabel);
    }
    else {
        double totalCost = 0.0;

        for (const auto& book : cart) {
            double price = book->price;
            // Создаём карточку товара
            QFrame* cartItemFrame = library_->CreateItemFrame(book, {
                     {"Delete from cart", [this, book]() {
                        library_->RemoveFromCart(book);
                          setupCartPage();
                      }}
                 });

            layout->addWidget(cartItemFrame);
            totalCost += price;
        }

        QLabel* totalLabel = new QLabel(QString("Total cost: $%1").arg(totalCost, 0, 'f', 2));
        totalLabel->setAlignment(Qt::AlignRight);
        layout->addWidget(totalLabel);

        QPushButton* payButton = new QPushButton("To pay");
        connect(payButton, &QPushButton::clicked, this, [this, cart]() {
            QSqlQuery query;

            // Получение ID текущего пользователя
            query.prepare("SELECT id FROM users WHERE username = :username");
            query.bindValue(":username", profile_->GetUsername());

            if (!query.exec() || !query.next()) {
                QMessageBox::warning(this, "Error", "The user ID could not be retrieved.");
                return;
            }

            int userId = query.value(0).toInt();

            // Обновление данных в БД
            for (const auto& book : cart) {
                int bookId = book->id;
                double price = book->price;

                // Добавляем запись в таблицу payments
                query.prepare("INSERT INTO payments (user_id, book_id, amount) VALUES (:user_id, :book_id, :amount)");
                query.bindValue(":user_id", userId);
                query.bindValue(":book_id", bookId);
                query.bindValue(":amount", price);

                if (!query.exec()) {
                    QMessageBox::warning(this, "Error", "Payment recording error.");
                    return;
                }

                // Обновляем массив purchased_books
                query.prepare("UPDATE users SET purchased_books = array_append(purchased_books, :book_id) WHERE id = :user_id");
                query.bindValue(":book_id", bookId);
                query.bindValue(":user_id", userId);

                if (!query.exec()) {
                    QMessageBox::warning(this, "Error", "Error updating the list of purchased books.");
                    return;
                }
            }

            QMessageBox::information(this, "Payment", "The payment was successful.");
            cart_->Clear();
            setupCartPage();
        });

        layout->addWidget(payButton);
    }

    QScrollArea* scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(cartContainer);

    stackedWidget_->addWidget(scrollArea);
    stackedWidget_->setCurrentWidget(scrollArea);
}

void MainWindow::setupGenreMenu(){
    genreWidget = std::make_unique<QDockWidget>("Genres", this);

    genreList_ = new QListWidget(genreWidget.get());
    genreList_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    genreList_->setMinimumWidth(200);

    genreWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    genreWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    genreWidget->setWidget(genreList_);
    genreWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);

    addDockWidget(Qt::LeftDockWidgetArea, genreWidget.get());

    UpdateGenreList(genreList_, genreMap_);

    // Обработка кликов на жанры
    connect(genreList_, &QListWidget::itemClicked, this, [this](QListWidgetItem* item) {
        FilterBooksByGenre(item->text());
    });
}

void MainWindow::FilterBooksByGenre(const QStringView selectedGenreName) {
    if (profile_->GetUserRole() == UserRoles::User){
        const int genreId = genreMap_.key(selectedGenreName.toString());

        QWidget* booksContainer = new QWidget;
        QVBoxLayout* layout = new QVBoxLayout(booksContainer);

        for (auto& book : library_->GetBooks()) {
            if (book.genre_id == genreId) {
                QFrame* bookFrame = library_->CreateItemFrame(&book, {
                       {"Add to Cart", [this, book]() {
                            cart_->AddToCart(&book);
                            QMessageBox::information(this, "Cart", "The book has been added to the cart.");
                        }},
                       {"Add to Bookmarks", [this, book]() {
                            bookmarks_->AddBookmark(&book);
                            QMessageBox::information(this, "Bookmarks", "The book has been added to bookmarks.");
                        }}
                   });
                layout->addWidget(bookFrame);
            }
        }

        QScrollArea* scrollArea = new QScrollArea;
        scrollArea->setWidgetResizable(true);
        scrollArea->setWidget(booksContainer);

        stackedWidget_->addWidget(scrollArea);
        stackedWidget_->setCurrentWidget(scrollArea);
    }
    else {
        QMessageBox::warning(this, "", "Log in as a USER to filter genres to filter genres.");
    }
}
void MainWindow::UpdateGenreList(QListWidget* genreList, QMap<int, QString>& genreMap){
    // Очищаем текущий список
    genreList->clear();
    genreMap.clear();

    // Проверяем подключение к базе данных
    if (!db_manager_.Open()) {
        qWarning() << "Database is not open. Cannot fetch genres.";
        return;
    }

    // Выполняем запрос на получение списка жанров
    auto query_result = db_manager_.ExecuteSelectQuery(QString("SELECT id, name FROM genres ORDER BY name;"));

    if (!query_result.canConvert<QSqlQuery>()) {
        qWarning() << "Failed to fetch genres:" << query_result.toString();
        return;
    }

    QSqlQuery query = query_result.value<QSqlQuery>();

    // Заполняем список жанров
    while (query.next()) {
        int genreId = query.value("id").toInt();
        QString genreName = query.value("name").toString();
        genreMap[genreId] = genreName;
        genreList->addItem(genreName);
    }

    qDebug() << "Genre list updated successfully.";
}

bool MainWindow::ConfigureDatabase(const bool need_notify, const QStringView host, int port, const QStringView db_name, const QStringView username, const QStringView password){
    db_manager_.UpdateConnection(host, port, db_name, username, password);
    if (db_manager_.Open()){
        if (need_notify){
            QMessageBox::information(this, "Connecting to the database", "The connection was completed successfully.");
        }
        return true;
    }
    if (need_notify){
        QMessageBox::critical(this, "Connection failed.", db_manager_.GetLastError());
    }
    return false;
}
