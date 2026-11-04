#pragma once
#include <QMainWindow>
#include <memory>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
public:

 explicit MainWindow(QWidget* parent = nullptr);

 ~MainWindow();

  

private:

 std::unique_ptr<Ui::MainWindow> ui;

}; 