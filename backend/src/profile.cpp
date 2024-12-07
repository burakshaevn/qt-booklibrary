#include "profile.h"

Profile::Profile(Library* library, QWidget *parent)
    : library_(library)
    , QWidget(parent),
    stackedWidget_profile_(new QStackedWidget(this)),
    current_role_(UserRoles::Guest) // Начальное значение роли
{
    auto layout = new QVBoxLayout(this);
    layout->addWidget(stackedWidget_profile_);
    stackedWidget_profile_->addWidget(CreateProfilePage());
}

void Profile::SetUserRole(const UserRoles& role) {
    current_role_ = role;
    emit ProfileUpdated();
}

UserRoles Profile::GetUserRole() const {
    return current_role_;
}

void Profile::SetUsername(const QString& username) {
    username_ = username;
}

QString Profile::GetUsername() const {
    return username_;
}

QWidget* Profile::CreateProfilePage() {
    auto* page = new QWidget(this);
    auto* layout = new QVBoxLayout(page);

    auto* loginLabel = new QLabel("Username:", page);
    auto* loginLineEdit = new QLineEdit(page);

    auto* passwordLabel = new QLabel("Password:", page);
    auto* passwordLineEdit = new QLineEdit(page);
    passwordLineEdit->setEchoMode(QLineEdit::Password);

    auto* loginButton = new QPushButton("Login", page);

    auto* loginLayout = new QFormLayout();
    loginLayout->addRow(loginLabel, loginLineEdit);
    loginLayout->addRow(passwordLabel, passwordLineEdit);
    loginLayout->addWidget(loginButton);

    layout->addLayout(loginLayout);

    connect(loginButton, &QPushButton::clicked, this, [this, loginLineEdit, passwordLineEdit]() {
        QString username = loginLineEdit->text();
        QString password = passwordLineEdit->text();

        QSqlQuery query;
        query.prepare("SELECT password_hash, role FROM users WHERE username = :username");
        query.bindValue(":username", username);

        if (query.exec() && query.next()) {
            QString storedHash = query.value(0).toString();
            QString role = query.value(1).toString();

            if (VerifyPassword(password, storedHash)) {
                SetUsername(username);
                if (role == "admin") {
                    SetUserRole(UserRoles::Admin);
                    stackedWidget_profile_->addWidget(CreateAdminPage());
                } else {
                    library_->LoadBooks();
                    emit UserLoggedIn();
                    SetUserRole(UserRoles::User);
                    stackedWidget_profile_->addWidget(CreateUserPage());
                }
                stackedWidget_profile_->setCurrentIndex(1);
            } else {
                QMessageBox::warning(this, "Error", "Incorrect password.");
            }
        } else {
            QMessageBox::warning(this, "Error", "Incorrect login or password.");
        }
    });
    return page;
}

QWidget* Profile::CreateAdminPage() {
    auto* page = new QWidget(this);
    auto* layout = new QVBoxLayout(page);

    auto* label = new QLabel("Welcome, Admin!", page);
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
        model->setEditStrategy(QSqlTableModel::OnManualSubmit);
        if (!model->select()) {
            QMessageBox::critical(dataTable, "Error", "Failed to load table: " + model->lastError().text());
            return;
        }

        dataTable->resizeColumnsToContents();
        dataTable->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);
        model->select();
    };

    auto *saveButton = new QPushButton("Save Changes", page);
    layout->addWidget(saveButton);

    connect(saveButton, &QPushButton::clicked, this, [model, dataTable]() {
        if (!model->submitAll()) {
            QMessageBox::critical(dataTable, "Error", "Failed to save changes: " + model->lastError().text());
        } else {
            QMessageBox::information(dataTable, "Success", "Changes saved successfully.");
        }
        model->select();
    });

    connect(tableSelector, &QComboBox::currentTextChanged, this, [loadTableData](const QString &tableName) {
        loadTableData(tableName);
    });

    // Добавление записи
    connect(addButton, &QPushButton::clicked, this, [model, dataTable]() {
        int newRow = model->rowCount();

        if (!model->insertRow(newRow)) {
            QMessageBox::critical(dataTable, "Error", "Failed to add record: " + model->lastError().text());
            return;
        }

        QSqlQuery query;
        if (!query.exec("SELECT MAX(id) FROM users")) {
            QMessageBox::critical(dataTable, "Error", "Failed to fetch last ID: " + query.lastError().text());
            return;
        }
        QString lastInsertedId;
        if (query.next()) {
            lastInsertedId = query.value(0).toString();
        }

        // Указываем для обязательных полей значения по умолчанию.
        // То есть, указываем для полей, которые NOT NULL, значения по умолчанию
        model->setData(model->index(newRow, model->fieldIndex("username")), "new_user" + lastInsertedId);
        model->setData(model->index(newRow, model->fieldIndex("password_hash")), "default_hash");
        model->setData(model->index(newRow, model->fieldIndex("role")), "user");
    });

    // Удаление записи
    connect(deleteButton, &QPushButton::clicked, this, [model, dataTable]() {
        auto selectedRows = dataTable->selectionModel()->selectedRows();
        for (const QModelIndex &index : selectedRows) {
            model->removeRow(index.row());
        }
        if (!model->submitAll()) {
            QMessageBox::critical(dataTable, "Error", "Failed to delete record: " + model->lastError().text());
            model->revertAll();
        } else {
            QMessageBox::information(dataTable, "Success", "Record deleted successfully.");
        }
        model->select();
    });

    connect(logoutButton, &QPushButton::clicked, this, [this]() {
        OnLogout();
    });

    // Инициализация первой таблицы
    loadTableData(tableSelector->currentText());

    return page;
}

QWidget* Profile::CreateUserPage() {
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
        query.bindValue(":username", username_);

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
            QMessageBox::information(this, "No Payments", "No payments found for user: " + username_);
        }
    });

    connect(logoutButton, &QPushButton::clicked, this, [this]() {
        OnLogout();
    });

    return page;
}

bool Profile::VerifyPassword(const QString &password, const QString &hash) {
    // Здесь можно использовать библиотеку для хэширования паролей, например bcrypt
    // Пример: return bcrypt::checkpw(password.toStdString(), hash.toStdString());
    return password == hash; // прямое сравнение, можно заменить на реальную проверку
}

void Profile::OnLogout() {
    stackedWidget_profile_->setCurrentIndex(0); // Возврат на страницу входа
    username_.clear();
    current_role_ = UserRoles::Guest;
    while (stackedWidget_profile_->count() > 0) {
        QWidget *widget = stackedWidget_profile_->widget(0);
        stackedWidget_profile_->removeWidget(widget);
        delete widget;
    }
    stackedWidget_profile_->addWidget(CreateProfilePage());
    stackedWidget_profile_->setCurrentIndex(0);
    emit UserLogout();
}
