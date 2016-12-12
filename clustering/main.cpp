/**
  * @file main.cpp
  * @brief Archivo principal del proyecto.
  * @author Mar&iacute;a Andrea Cruz Bland&oacute;n
  * @date 09/2013, 11/2013
  *
  **/

/*
 *Agradecimientos especiales por sus sugerencias a los ingenieros:
 *  Julián Andrés Camargo
 *  Yerminson Doney Gónzalez Muñoz
 *  Cristian Leonardo Ríos López
 *  Luis Felipe Vargas Rojas
 **/

#include "mainwindow.h"
#include <QApplication>
#include <time.h>



/**
 * @brief main Se inicializa la aplicaci&oacute;n.
 */

int main(int argc, char *argv[])
{
    /**
     * @note En algoritmo geneticos es recomendables s&oacute;lo ejecutar la inicializacion del Rand una vez en toda la ejecuci&oacute;n del algoritmo.
     **/

    srand(time(NULL));

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    
    return a.exec();

}
