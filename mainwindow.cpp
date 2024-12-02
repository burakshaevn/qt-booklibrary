#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    searchPanel = new QFrame(this);
    // searchPanel = QFrame(this);
    searchPanel->setFrameShape(QFrame::StyledPanel);
    searchPanel->setFixedHeight(50);
    searchPanel->setFixedWidth(300);
    searchPanel->setWindowFlags(Qt::Popup);

    // Создаём поле для ввода текста
    searchLineEdit = new QLineEdit(searchPanel);
    auto *searchLayout = new QHBoxLayout(searchPanel);
    searchLayout->addWidget(searchLineEdit);

    QString host = "localhost";
    int port = 5432;
    QString db_name = "booklibrary";
    QString login = "postgres";
    QString password = "89274800234Nn";
    ConfigureDatabase(false, host, port, db_name, login, password);

    setCentralWidget(new QWidget(this));
    stackedWidget = new QStackedWidget(this);
    setupMainContent();
    setupNavigationPanel();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showLibraryPage() {
    stackedWidget->setCurrentIndex(0); // Переход на страницу библиотеки
    QWidget *mainContentWidget = stackedWidget->widget(0); // Получаем страницу из stackedWidget

    // Очищаем старый layout, если есть
    if (mainLayout) {
        delete mainLayout;
    }

    mainLayout = new QVBoxLayout(mainContentWidget);

    // Создаем область прокрутки
    auto *scrollArea = new QScrollArea(mainContentWidget);
    scrollArea->setWidgetResizable(true);

    // Контейнер для книг
    auto *booksContainer = new QWidget();
    auto *booksLayout = new QVBoxLayout(booksContainer);

    booksContainer->setLayout(booksLayout);
    scrollArea->setWidget(booksContainer); // Устанавливаем контейнер как виджет в ScrollArea

    mainLayout->addWidget(scrollArea); // Добавляем область прокрутки в основной layout
}

void MainWindow::showBookmarksPage() {
    stackedWidget->setCurrentIndex(1); // Переход на страницу закладок
}

void MainWindow::showPaymentPage() {
    stackedWidget->setCurrentIndex(2); // Переход на страницу оплаты    
}

void MainWindow::showProfilePage() {
    stackedWidget->setCurrentIndex(3); // Переход на страницу профиля
}

//Настройка меню жанров слева
void MainWindow::setupGenreMenu() {
    dockWidget = std::make_unique<QDockWidget>("Genres", this);
    genreList_ = new QListWidget(dockWidget.get());
    genreList_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    genreList_->setMinimumWidth(200);

    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dockWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    dockWidget->setWidget(genreList_);
    dockWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);

    addDockWidget(Qt::LeftDockWidgetArea, dockWidget.get());

    // Устанавливаем данные в genreList_
    updateGenreList(genreList_, genreMap_);

    // Обработка кликов на жанры
    connect(genreList_, &QListWidget::itemClicked, this, [this](QListWidgetItem* item) {
        QString selectedGenreName = item->text();
        int selectedGenreId = genreMap_.key(selectedGenreName);

        qDebug() << "Selected genre:" << selectedGenreName << "(ID:" << selectedGenreId << ")";
        filterBooksByGenre(selectedGenreId);
    });
}

// Создаём страницы для навигационной панели
void MainWindow::setupMainContent() {
    setCentralWidget(stackedWidget);
    stackedWidget->addWidget(createLibraryPage());
    stackedWidget->addWidget(createBookmarksPage());
    stackedWidget->addWidget(createPaymentPage());
    stackedWidget->addWidget(createProfilePage());
    stackedWidget->setCurrentIndex(3);
}

// Создаём страницы для QStackedWidget
QWidget *MainWindow::createLibraryPage() {
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);

    auto *scrollArea = new QScrollArea(page);
    scrollArea->setWidgetResizable(true);

    layout->addWidget(scrollArea);

    return page;
}

QWidget *MainWindow::createBookmarksPage() {
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    layout->addWidget(new QLabel("Bookmarks page.", page));
    return page;
}

QWidget *MainWindow::createPaymentPage() {
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    layout->addWidget(new QLabel("Cart page.", page));
    return page;
}

QWidget *MainWindow::createProfilePage() {
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);

    auto *loginLabel = new QLabel("Username:", page);
    auto *loginLineEdit = new QLineEdit(page);

    auto *passwordLabel = new QLabel("Password:", page);
    auto *passwordLineEdit = new QLineEdit(page);
    passwordLineEdit->setEchoMode(QLineEdit::Password);

    auto *loginButton = new QPushButton("Login", page);

    stackedWidget_profile = new QStackedWidget(this);

    // Создаем страницу логина
    auto *loginPage = new QWidget(this);
    auto *loginLayout = new QFormLayout(loginPage);
    loginLayout->addRow(loginLabel, loginLineEdit);
    loginLayout->addRow(passwordLabel, passwordLineEdit);
    loginLayout->addWidget(loginButton);

    stackedWidget_profile->addWidget(loginPage); // Добавляем страницу логина

    // Обработчик для кнопки "Login"
    connect(loginButton, &QPushButton::clicked, this, [this, loginLineEdit, passwordLineEdit]() {
        QString username = loginLineEdit->text();
        QString password = passwordLineEdit->text();

        QSqlQuery query;
        query.prepare("SELECT password_hash, role FROM users WHERE username = :username");
        query.bindValue(":username", username);

        if (query.exec() && query.next()) {
            QString storedHash = query.value(0).toString();
            current_role = query.value(1).toString();

            if (verifyPassword(password, storedHash)) {
                clearStackedWidget();
                current_username = username;
                if (current_role == "admin") {
                    stackedWidget_profile->addWidget(createAdminPage());
                } else if (current_role == "user") {
                    LoadBooks();
                    setupGenreMenu();
                    QWidget* userPage = createUserPage();
                    stackedWidget_profile->addWidget(userPage);
                }
                stackedWidget_profile->setCurrentIndex(1);
            } else {
                QMessageBox::warning(this, "Error", "Incorrect password.");
            }
        } else {
            QMessageBox::warning(this, "Error", "Incorrect login or password.");
            qDebug() << "SQL Error:" << query.lastError().text();
        }
    });

    layout->addWidget(stackedWidget_profile);

    return page;
}

// Вспомогательная функция для проверки хэша пароля
bool MainWindow::verifyPassword(const QString &password, const QString &hash) {
    // Здесь можно использовать библиотеку для хэширования паролей, например bcrypt
    // Пример: return bcrypt::checkpw(password.toStdString(), hash.toStdString());
    return password == hash; // прямое сравнение, можно заменить на реальную проверку
}

QWidget *MainWindow::createAdminPage() {
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);

    auto *label = new QLabel("Welcome, Admin!", page);
    layout->addWidget(label);

    // Выбор таблицы
    auto *tableSelector = new QComboBox(page);
    tableSelector->addItems({"books", "genres", "users", "payments", "book_genres"});
    layout->addWidget(tableSelector);

    // Таблица для отображения данных
    auto *dataTable = new QTableView(page);
    dataTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    dataTable->setSelectionMode(QAbstractItemView::SingleSelection);
    dataTable->horizontalHeader()->setStretchLastSection(true);
    dataTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    layout->addWidget(dataTable);

    // Кнопки управления
    auto *buttonLayout = new QHBoxLayout();
    auto *addButton = new QPushButton("Add Record", page);
    auto *deleteButton = new QPushButton("Delete Record", page);
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(deleteButton);
    layout->addLayout(buttonLayout);

    auto *logoutButton = new QPushButton("Logout", page);
    layout->addWidget(logoutButton);

    // Модель для работы с таблицами
    auto *model = new QSqlTableModel(this);
    dataTable->setModel(model);

    // Функция для загрузки таблицы
    auto loadTableData = [model, dataTable](const QString &tableName) {
        model->setTable(tableName);
        model->setEditStrategy(QSqlTableModel::OnRowChange); // Сохранение изменений при изменении строки
        if (!model->select()) {
            QMessageBox::critical(dataTable, "Error", "Failed to load table: " + model->lastError().text());
            return;
        }

        // Настройка отображения
        dataTable->resizeColumnsToContents();
        dataTable->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);
    };

    // Загрузка данных при выборе таблицы
    connect(tableSelector, &QComboBox::currentTextChanged, this, [loadTableData](const QString &tableName) {
        loadTableData(tableName);
    });

    // Добавление записи
    connect(addButton, &QPushButton::clicked, this, [model, dataTable]() {
        // Добавляем новую строку
        int newRow = model->rowCount();

        if (!model->insertRow(newRow)) {
            QMessageBox::critical(dataTable, "Error", "Failed to add record: " + model->lastError().text());
            return;
        }

        // Получаем последний добавленный id
        QSqlQuery query;
        if (!query.exec("SELECT MAX(id) FROM users")) {
            QMessageBox::critical(dataTable, "Error", "Failed to fetch last ID: " + query.lastError().text());
            return;
        }

        QString lastInsertedId;
        if (query.next()) {
            lastInsertedId = query.value(0).toString();
        }

        // Устанавливаем значения по умолчанию
        model->setData(model->index(newRow, model->fieldIndex("username")), "new_user" + lastInsertedId);
        model->setData(model->index(newRow, model->fieldIndex("password_hash")), "default_hash");
        model->setData(model->index(newRow, model->fieldIndex("role")), "user");

        // Сохраняем изменения
        if (!model->submitAll()) {
            QMessageBox::critical(dataTable, "Error", "Failed to save record: " + model->lastError().text());
            return;
        }

        // Перезагружаем таблицу
        model->select();
    });

    // Удаление записи
    connect(deleteButton, &QPushButton::clicked, this, [model, dataTable]() {
        auto selectedRows = dataTable->selectionModel()->selectedRows();
        for (const QModelIndex &index : selectedRows) {
            model->removeRow(index.row());
        }
        if (model->submitAll()) {
            QMessageBox::information(dataTable, "Succsess", "The action was completed successfully.");
        }
        else{
            QMessageBox::critical(dataTable, "Error", "Failed to delete record: " + model->lastError().text());
        }
        model->select();
    });

    connect(model, &QSqlTableModel::dataChanged, this, [model, dataTable](const QModelIndex &topLeft, const QModelIndex &bottomRight) {
        if (model->submitAll()) {
            QMessageBox::information(dataTable, "Success", "Changes saved successfully.");
        } else {
            QMessageBox::critical(dataTable, "Error", "Failed to save changes: " + model->lastError().text());
            model->revertAll();
        }
    });

    // Кнопка Logout
    connect(logoutButton, &QPushButton::clicked, this, [this]() {
        logout();
    });

    // Инициализация первой таблицы
    loadTableData(tableSelector->currentText());

    return page;
}

QWidget* MainWindow::createUserPage() {
    auto* page = new QWidget(this);
    auto* layout = new QVBoxLayout(page);

    auto* label = new QLabel("Welcome, User!", page);
    layout->addWidget(label);

    auto* browsePaymentsButton = new QPushButton("View Payments", page);
    layout->addWidget(browsePaymentsButton);

    auto* logoutButton = new QPushButton("Logout", page);
    layout->addWidget(logoutButton);

    // Создаём таблицу для отображения платежей
    auto* tableWidget = new QTableWidget(page);
    tableWidget->setColumnCount(4);
    tableWidget->setHorizontalHeaderLabels({"Book Title", "Amount", "Payment Date", "Author"});
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(tableWidget);
    tableWidget->setVisible(false);

    connect(browsePaymentsButton, &QPushButton::clicked, this, [this, tableWidget]() {
        tableWidget->setVisible(true);
        tableWidget->setRowCount(0); // Очищаем таблицу перед заполнением
        QSqlQuery query;

        // 1. Получаем ID пользователя
        query.prepare("SELECT id FROM users WHERE username = :username");
        query.bindValue(":username", current_username);

        if (!query.exec() || !query.next()) {
            QMessageBox::warning(this, "Error", "Failed to retrieve user ID: " + query.lastError().text());
            return;
        }

        int userId = query.value(0).toInt();

        // 2. Получаем платежи пользователя
        query.prepare(R"(
            SELECT b.title, p.amount, p.payment_date, b.author
            FROM payments p
            JOIN books b ON p.book_id = b.id
            WHERE p.user_id = :user_id
        )");
        query.bindValue(":user_id", userId);

        if (!query.exec()) {
            QMessageBox::warning(this, "Error", "Failed to retrieve payments: " + query.lastError().text());
            return;
        }

        // 3. Заполняем таблицу данными
        int row = 0;
        while (query.next()) {
            tableWidget->insertRow(row);
            tableWidget->setItem(row, 0, new QTableWidgetItem(query.value("title").toString()));
            tableWidget->setItem(row, 1, new QTableWidgetItem(query.value("amount").toString()));
            tableWidget->setItem(row, 2, new QTableWidgetItem(query.value("payment_date").toString()));
            tableWidget->setItem(row, 3, new QTableWidgetItem(query.value("author").toString()));
            ++row;
        }

        if (row == 0) {
            QMessageBox::information(this, "No Payments", "No payments found for user: " + current_username);
        }
    });

    connect(logoutButton, &QPushButton::clicked, this, [this]() {
        logout();
    });

    return page;
}

void MainWindow::clearStackedWidget() {
    while (stackedWidget_profile->count() > 0) {
        QWidget *widget = stackedWidget_profile->widget(0);
        stackedWidget_profile->removeWidget(widget);
        delete widget;
    }
}

void MainWindow::logout() {
    current_username.clear();
    current_role.clear();
    clearStackedWidget();
    stackedWidget_profile->addWidget(createProfilePage());
    stackedWidget_profile->setCurrentIndex(0);
    dockWidget.reset();
}

void MainWindow::updateGenreList(QListWidget* genreList, QMap<int, QString>& genreMap) {
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
    mainLayout->addWidget(stackedWidget);
    mainLayout->addWidget(navigationPanel);

    setCentralWidget(centralWidget);

    connect(libraryButton, &QPushButton::clicked, this, [this]() {
        if (db_manager_.Open()) {
            if (current_role == "user"){
                setupLibraryPage();
            }
            else{
                QMessageBox::warning(this, "Error", "Log in as a user.");
            }
        }
        else{
            QMessageBox::warning(this, "Error", "Need connect to database.");
        }
    });
    connect(bookmarksButton, &QPushButton::clicked, this, [this]() {
        if (db_manager_.Open()){
            if (current_role == "user"){
                setupBookmarksPage();
            }
            else{
                QMessageBox::warning(this, "Error", "Log in as a user.");
            }
        }
        else{
            QMessageBox::warning(this, "Error", "Need connect to database.");
        }
    });
    connect(paymentButton, &QPushButton::clicked, this, [this]() {
        if (db_manager_.Open()) {
            if (current_role == "user") {
                setupCartPage();
            }
            else{
                QMessageBox::warning(this, "Error", "Log in as a user.");
            }
        }
        else{
            QMessageBox::warning(this, "Error", "Need connect to database.");
        }
    });
    connect(profileButton, &QPushButton::clicked, this, [this]() {
        stackedWidget->setCurrentIndex(3);
    });
}

void MainWindow::setupPages(){

}

void MainWindow::setupLibraryPage() {
    if (books_.empty()) {
        qWarning() << "No books to display.";
        return;
    }

    QWidget* booksContainer = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(booksContainer);

    for (auto& book : books_) {
        QFrame* bookFrame = createItemFrame(&book, {
                                                       {"Add to Cart", [this, &book]() {
                                                            cart_[book.title] = &book;
                                                            QMessageBox::information(this, "Cart", "The book has been added to the cart.");
                                                        }},
                                                       {"Add to Bookmarks", [this, &book]() {
                                                            bookmarks_[book.title] = &book;
                                                            QMessageBox::information(this, "Bookmarks", "The book has been added to bookmarks.");
                                                        }}
                                                   });
        layout->addWidget(bookFrame);
    }

    QScrollArea* scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(booksContainer);

    stackedWidget->addWidget(scrollArea);
    stackedWidget->setCurrentWidget(scrollArea);
}

QFrame* MainWindow::createItemFrame(
    const Book* book,
    const QVector<QPair<QString, std::function<void()>>>& buttons)
{
    QFrame* itemFrame = new QFrame;
    itemFrame->setFrameShape(QFrame::StyledPanel);
    itemFrame->setFrameShadow(QFrame::Raised);
    itemFrame->setStyleSheet("background-color: #f5f5f5; padding: 10px; margin: 5px;");
    itemFrame->setFixedHeight(300);

    QVBoxLayout* layout = new QVBoxLayout(itemFrame);

    // Название книги
    QLabel* titleLabel = new QLabel(book->title);
    titleLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #333;");
    layout->addWidget(titleLabel);

    // Автор
    QLabel* authorLabel = new QLabel("Author: " + book->author);
    authorLabel->setStyleSheet("font-size: 14px; color: #333;");
    layout->addWidget(authorLabel);

    // Цена
    QLabel* priceLabel = new QLabel("Price: " + QString::number(book->price));
    priceLabel->setStyleSheet("font-size: 14px; color: #228B22; font-weight: bold;");

    // Горизонтальный лейаут для размещения даты и цены
    QHBoxLayout* priceLayout = new QHBoxLayout();
    priceLayout->addWidget(priceLabel);

    // Дата публикации
    QLabel* publish_dateLabel = new QLabel("Date of publication: " + book->published_date);
    publish_dateLabel->setStyleSheet("font-size: 14px; color: #333;");

    // Добавление spacer для отступа, чтобы элементы не перекрывались
    priceLayout->addStretch(1);  // Пространство для отступа
    priceLayout->addWidget(publish_dateLabel, 0, Qt::AlignRight);  // Правое выравнивание для даты

    layout->addLayout(priceLayout);  // Добавляем горизонтальный лейаут в основной вертикальный лейаут

    // Добавьте это к QLabel для авторов, чтобы текст не сжимался
    authorLabel->setWordWrap(true); // Включает перенос текста
    authorLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    priceLabel->setWordWrap(true);
    priceLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    // Добавляем кнопки
    for (const auto& buttonInfo : buttons) {
        const QString& buttonText = buttonInfo.first;
        const std::function<void()>& onClick = buttonInfo.second;

        QPushButton* button = new QPushButton(buttonText);
        button->setStyleSheet("background-color: #fafafa; border-radius: 0px;");
        connect(button, &QPushButton::clicked, this, onClick);
        layout->addWidget(button);
    }

    return itemFrame;
}

void MainWindow::clearLayout(QLayout *layout) {
    while (QLayoutItem *item = layout->takeAt(0)) {
        if (QWidget *widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }
}

void MainWindow::setupBookmarksPage() {
    stackedWidget->setCurrentIndex(1);

    // Контейнер для закладок
    QWidget* bookmarksContainer = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(bookmarksContainer);

    if (bookmarks_.empty()) {
        QLabel* emptyLabel = new QLabel("Bookmarks is empty.");
        emptyLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(emptyLabel);
    } else {
        for (const auto& book : bookmarks_) {
            // Создаём карточку закладки с двумя кнопками
            QFrame* bookmarkFrame = createItemFrame(book.second,{
                                                                  {"Add to Cart", [this, book]() {
                                                                   if (book.second->price < 0) {
                                                                       QMessageBox::warning(this, "Error", "Uncnown price.");
                                                                       return;
                                                                   }
                                                                    cart_.emplace(book);
                                                                    QMessageBox::information(this, "Cart", "Added to cart.");
                                                                   }},
                                                                  {"Delete from bookmarks", [this, book]() {
                                                                       bookmarks_.erase(book.first);
                                                                       setupBookmarksPage(); // Перерисовываем страницу
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
    stackedWidget->addWidget(scrollArea);
    stackedWidget->setCurrentWidget(scrollArea);
}

void MainWindow::setupCartPage() {
    stackedWidget->setCurrentIndex(2);

    // Контейнер для корзины
    QWidget* cartContainer = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(cartContainer);

    if (cart_.empty()) {
        QLabel* emptyLabel = new QLabel("Cart is empty.");
        emptyLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(emptyLabel);
    } else {
        double totalCost = 0.0;

        for (const auto& book : cart_) {
            int id = book.second->id;
            const QString& title = book.second->title;
            double price = book.second->price;

            QString info = QString("ID: %1\nTitle: %2\nPrice: $%3")
                               .arg(id)
                               .arg(title)
                               .arg(price, 0, 'f', 2);

            // Создаём карточку товара
            QFrame* cartItemFrame = createItemFrame(book.second, {
                                                                     {"Delete from cart", [this, id, title]() {
                                                                          cart_.erase(title);
                                                                          setupCartPage(); // Перерисовываем страницу
                                                                      }}
                                                                 });

            layout->addWidget(cartItemFrame);
            totalCost += price;
        }

        QLabel* totalLabel = new QLabel(QString("Total cost: $%1").arg(totalCost, 0, 'f', 2));
        totalLabel->setAlignment(Qt::AlignRight);
        layout->addWidget(totalLabel);

        QPushButton* payButton = new QPushButton("To pay");
        connect(payButton, &QPushButton::clicked, this, [this]() {
            QSqlQuery query;

            // Получение ID текущего пользователя
            query.prepare("SELECT id FROM users WHERE username = :username");
            query.bindValue(":username", current_username);

            if (!query.exec() || !query.next()) {
                QMessageBox::warning(this, "Error", "The user ID could not be retrieved.");
                return;
            }

            int userId = query.value(0).toInt();

            // Обновление данных в БД
            for (const auto& book : cart_) {
                int bookId = book.second->id;
                double price = book.second->price;

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
            cart_.clear(); // Очищаем корзину
            setupCartPage(); // Перерисовываем страницу
        });

        layout->addWidget(payButton);
    }

    QScrollArea* scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(cartContainer);

    stackedWidget->addWidget(scrollArea);
    stackedWidget->setCurrentWidget(scrollArea);
}

void MainWindow::setupProfilePage() {
    // Очистка текущего содержимого
    clearStackedWidget();
    stackedWidget->setCurrentIndex(3);
    if (current_role == "admin") {
        // Если админ, добавляем админскую панель
        stackedWidget_profile->addWidget(createAdminPage());
    } else if (current_role == "user") {
        // Если пользователь, выводим список купленных книг
        QWidget* userPage = new QWidget(this);
        QVBoxLayout* layout = new QVBoxLayout(userPage);

        // Приветствие
        auto* welcomeLabel = new QLabel("Welcome, " + current_username + "!", userPage);
        layout->addWidget(welcomeLabel);

        // Запрос для получения списка книг
        QSqlQuery query;
        query.prepare("SELECT b.title FROM books b "
                      "JOIN payments p ON p.book_id = b.id "
                      "WHERE p.user_id = (SELECT id FROM users WHERE username = :username)");
        query.bindValue(":username", current_username);

        if (query.exec()) {
            // Таблица для отображения списка книг
            QTableView* booksTable = new QTableView(userPage);
            QStandardItemModel* model = new QStandardItemModel(this);

            model->setHorizontalHeaderLabels({"Title"});
            int row = 0;
            while (query.next()) {
                QString bookTitle = query.value(0).toString();
                model->setItem(row, 0, new QStandardItem(bookTitle));
                row++;
            }
            booksTable->setModel(model);
            layout->addWidget(booksTable);
        } else {
            QMessageBox::warning(this, "Error", "Failed to load purchased books: " + query.lastError().text());
        }

        auto* logoutButton = new QPushButton("Logout", userPage);
        layout->addWidget(logoutButton);

        connect(logoutButton, &QPushButton::clicked, this, [this]() {
            logout();
        });

        stackedWidget_profile->addWidget(userPage);
    }

    stackedWidget_profile->setCurrentIndex(0);
}


void MainWindow::LoadBooks() {
    if (!db_manager_.Open()) {
        qWarning() << "Database is not open. Cannot load books.";
        return;
    }

    auto query_result = db_manager_.ExecuteSelectQuery(QString("SELECT id, title, author, price, published_date, genre_id FROM books;"));
    if (!query_result.canConvert<QSqlQuery>()) {
        qWarning() << "Failed to fetch books:" << query_result.toString();
        return;
    }

    QSqlQuery query = query_result.value<QSqlQuery>();
    books_.clear();

    while (query.next()) {
        Book book;
        book.id = query.value("id").toInt();
        book.title = query.value("title").toString();
        book.author = query.value("author").toString();
        book.price = query.value("price").toDouble();
        book.published_date = query.value("published_date").toString();
        book.genre_id = query.value("genre_id").toInt();
        books_.emplace_back(std::move(book));
    }

    qDebug() << "Books loaded. Total books:" << books_.size();
}

void MainWindow::filterBooksByGenre(int genreId) {
    QWidget* booksContainer = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(booksContainer);

    for (auto& book : books_) {
        if (book.genre_id == genreId) {
            QFrame* bookFrame = createItemFrame(&book, {
                                                           {"Add to Cart", [this, &book]() {
                                                                cart_[book.title] = &book;
                                                                QMessageBox::information(this, "Cart", "The book has been added to the cart.");
                                                            }},
                                                           {"Add to Bookmarks", [this, &book]() {
                                                                bookmarks_[book.title] = &book;
                                                                QMessageBox::information(this, "Bookmarks", "The book has been added to bookmarks.");
                                                            }}
                                                       });
            layout->addWidget(bookFrame);
        }
    }

    QScrollArea* scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(booksContainer);

    stackedWidget->addWidget(scrollArea);
    stackedWidget->setCurrentWidget(scrollArea);
}

void MainWindow::displayBooks(QSqlQuery& query) {
    QWidget* libraryPage = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(libraryPage);

    while (query.next()) {
        QString title = query.value("title").toString();
        QString author = query.value("author").toString();
        double price = query.value("price").toDouble();
        QString publishedDate = query.value("published_date").toString();

        QString bookInfo = QString("Title: %1\nAuthor: %2\nPrice: $%3\nPublished Date: %4")
                               .arg(title)
                               .arg(author)
                               .arg(price, 0, 'f', 2)
                               .arg(publishedDate);

        QLabel* bookLabel = new QLabel(bookInfo);
        bookLabel->setWordWrap(true);
        layout->addWidget(bookLabel);
    }

    stackedWidget->addWidget(libraryPage);
    stackedWidget->setCurrentWidget(libraryPage);
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
