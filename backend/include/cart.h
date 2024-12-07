#ifndef CART_H
#define CART_H

#include <QWidget>
#include <QObject>
#include <QMainWindow>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QLabel>

#include "domain.h"

class Cart : public QMainWindow
{
public:
    explicit Cart(QStackedWidget* stackedWidget, QWidget* parent = nullptr);

    QWidget* CreateCartPage();

    void AddToCart(const Book* book);
    void RemoveFromCart(const Book* book);
    void DisplayBookmarks();
    QMap<const QString, const Book*> GetCart();
    void Clear();

private:
    QStackedWidget* stackedWidget_;
    QMap<const QString, const Book*> cart_;
};

#endif // CART_H
