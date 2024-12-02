// #include "library.h"

// Library::Library(DatabaseManager& dbManager)
//     : dbManager_(dbManager) {}

// void Library::LoadBooks() {
//     books_.clear();
//     bookIndexById_.clear();

//     // Запрос на выборку книг
//     const QString query = "SELECT id, title, author, description, genre, year FROM books";
//     QVariant result = dbManager_.ExecuteSelectQuery(query);

//     if(result.canConvert<QSqlQuery>()){
//         QSqlQuery sql_query = result.value<QSqlQuery>();
//         int index = 0;
//         while (sql_query.next()) {
//             Book book;
//             book.id = sql_query.value("id").toInt();
//             book.title = sql_query.value("title").toString();
//             book.author = sql_query.value("author").toString();
//             book.description = sql_query.value("description").toString();
//             book.genre = sql_query.value("genre").toString();
//             book.year = sql_query.value("year").toInt();

//             books_.append(book);
//             bookIndexById_[book.id] = index++;
//         }
//     }
//     else if (result.canConvert<QString>()) {
//         // QMessageBox::
//     }
//     else{

//     }
// }

// const QVector<Book>& Library::GetBooks() const {
//     return books_;
// }

// const Book* Library::FindBookById(int id) const {
//     if (bookIndexById_.contains(id)) {
//         return &books_[bookIndexById_.value(id)];
//     }
//     return nullptr; // Если книга не найдена
// }
