#ifndef LIBRARY_H
#define LIBRARY_H

#include "database_manager.h"
#include <QString>
#include <QVector>
#include <QHash>

// Структура для хранения информации о книге
struct Book {
    int id;
    QString title;
    QString author;
    double price;
    QString published_date;
    int genre_id;
};

class Library {
public:
    explicit Library(DatabaseManager& dbManager);

    // Загрузить данные из базы данных
    void LoadBooks();

    // Получить все книги
    const QVector<Book>& GetBooks() const;

    // Найти книгу по ID
    const Book* FindBookById(int id) const;

private:
    // DatabaseManager& dbManager_;   // Ссылка на менеджер базы данных
    // QVector<Book> books_;          // Кэш загруженных книг
    // QHash<int, int> bookIndexById_; // Индекс для быстрого поиска книг по ID
};
#endif // LIBRARY_H
