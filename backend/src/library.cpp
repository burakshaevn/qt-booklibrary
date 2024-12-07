#include "library.h"

Library::Library(QStackedWidget* stackedWidget, DatabaseManager& dbManager, QWidget* parent)
    : QMainWindow(parent)
    , stackedWidget_(stackedWidget)
    , db_manager_(dbManager)
    , libraryLayout_(nullptr)
{
    QWidget* libraryPage = CreateLibraryPage();
    stackedWidget_->addWidget(libraryPage);
}

QWidget *Library::CreateLibraryPage() {
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);

    auto *scrollArea = new QScrollArea(page);
    scrollArea->setWidgetResizable(true);

    layout->addWidget(scrollArea);

    return page;
}

void Library::UpdateLibraryPage() {
    QLayoutItem *child;
    while ((child = libraryLayout_->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    auto *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);

    auto *booksContainer = new QWidget();
    auto *booksLayout = new QVBoxLayout(booksContainer);

    for (const auto &book : books_) {
        booksLayout->addWidget(new QLabel(book.title));
    }

    scrollArea->setWidget(booksContainer);
    libraryLayout_->addWidget(scrollArea);
}

void Library::SetBookmarks(Bookmarks *bookmarks) {
    bookmarks_ = bookmarks;
}

void Library::AddBookmark(const Book* book){
    bookmarks_->AddBookmark(book);
}

void Library::RemoveBookmark(const Book* book){
    bookmarks_->RemoveBookmark(book);
}
QMap<const QString, const Book*> Library::GetBookmarks() {
    return bookmarks_->GetBookmarks();
}

void Library::SetCart(Cart* cart){
    cart_ = cart;
}
void Library::AddToCart(const Book* book){
    cart_->AddToCart(book);
}
void Library::RemoveFromCart(const Book* book){
    cart_->RemoveFromCart(book);
}
QMap<const QString, const Book*> Library::GetCart() {
    return cart_->GetCart();
}

void Library::LoadBooks() {
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

std::deque<Book> Library::GetBooks() const{
    return books_;
}

QFrame* Library::CreateItemFrame(const Book* book, const QVector<QPair<QString, std::function<void()>>>& buttons) {
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

    priceLayout->addStretch(1);
    priceLayout->addWidget(publish_dateLabel, 0, Qt::AlignRight);

    layout->addLayout(priceLayout);

    authorLabel->setWordWrap(true);
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
