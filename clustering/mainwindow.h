/**
  * @file mainwindow.h
  * @brief Definici&oacute;n de la clase MainWindow.
  * @author Mar&iacute;a Andrea Cruz Bland&oacute;n
  * @date 09/2013, 11/2013
  *
  **/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "algoritmo.h"
#include "lectoraarchivo.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;
    Algoritmo* algoritmo;
    LectoraArchivo* lectoraDeArchivo;
    void deshabilitarAlgoritmo(bool habilitar);
    void limpiarDatos();
    bool flagLectora;
    bool flagAlgoritmo;
    void destruirDatos();

private slots:
    void ejecutar();
    void abrirArchivo();
    void guardarSolucion();

};

#endif // MAINWINDOW_H
