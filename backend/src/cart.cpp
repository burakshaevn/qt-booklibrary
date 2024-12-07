#include "cart.h"

Cart::Cart(QStackedWidget* stackedWidget, QWidget* parent){

}

QWidget* Cart::CreateCartPage(){
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    layout->addWidget(new QLabel("Cart page.", page));
    return page;
}
void Cart::AddToCart(const Book* book){
    cart_[book->title] = book;
}
void Cart::RemoveFromCart(const Book* book){
    auto it = cart_.find(book->title);
    if (it != cart_.end()){
        cart_.erase(it);
    }
}
void Cart::DisplayBookmarks(){

}
QMap<const QString, const Book*> Cart::GetCart(){
    return cart_;
}
void Cart::Clear(){
    cart_.clear();
}
