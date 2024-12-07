#pragma once

#ifndef PROFILE_H
#define PROFILE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QStackedWidget>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QMessageBox>
#include <QTableWidget>
#include <QFormLayout>
#include <QTableView>
#include <QComboBox>
#include <QHeaderView>

#include "domain.h"
#include "library.h"
#include "database_manager.h"

class Profile : public QWidget
{
    Q_OBJECT
public:
    explicit Profile(Library* library, QWidget *parent = nullptr);

    void SetUserRole(const UserRoles& role);
    UserRoles GetUserRole() const;

    void SetUsername(const QString& username);
    QString GetUsername() const;

    QWidget* CreateProfilePage(); // Создание страницы профиля
    QWidget* CreateAdminPage();   // Создание страницы администратора
    QWidget* CreateUserPage();    // Создание страницы пользователя

    inline QStackedWidget* GetDisplay(){
        return stackedWidget_profile_;
    }

signals:
    void ProfileUpdated();
    void UserLoggedIn();
    void UserLogout();

private slots:
    void OnLogout();

private:
    bool VerifyPassword(const QString &password, const QString &hash); // Проверка пароля

    QStackedWidget* stackedWidget_profile_;
    UserRoles current_role_ = UserRoles::Guest;
    QString username_;

    Library* library_;
    QLabel* username_label_;
    QLabel* role_label_;
    QPushButton* edit_button_;
    QPushButton* logout_button_;
    QLineEdit* username_edit_;
};

#endif // PROFILE_H
