#ifndef DOMAIN_H
#define DOMAIN_H

#include <QString>

enum class UserRoles {
    Guest,
    Admin,
    User
};

struct Book {
    int id;
    QString title;
    QString author;
    double price;
    QString published_date;
    int genre_id;
};

#endif // DOMAIN_H
