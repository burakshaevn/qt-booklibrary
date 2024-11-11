#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDockWidget>
#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void performSearch();
    void showBookmarksPage();
    void showPaymentPage();
    void showProfilePage();
    void showLibraryPage();
    void showSearchPanel();

private:
    Ui::MainWindow *ui;
    QFrame *searchPanel;
    QLineEdit *searchLineEdit;
    QWidget *mainContentWidget;
    QVBoxLayout *mainLayout;

    void setupGenreMenu();
    void setupMainContent();
    void setupNavigationPanel();
};

#endif // MAINWINDOW_H
