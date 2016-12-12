/**
  * @file mainwindow.cpp
  * @brief Implementaci&oacute;n de los m&eactue;todos de la clase MainWindow.
  * @author Mar&iacute;a Andrea Cruz Bland&oacute;n
  * @date 09/2013, 11/2013
  *
  **/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>

#include <iostream>

using namespace std;


/**
 * @brief MainWindow::MainWindow(QWidget *parent)
 * Constructor. Inicializa objeto MainWindow
 * @param parent
 */

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionAbrir,SIGNAL(triggered()),this, SLOT(abrirArchivo()));
    connect(ui->aplicarAlgoritmo, SIGNAL(clicked()),this,SLOT(ejecutar()));
    connect(ui->actionGuardar,SIGNAL(triggered()),this,SLOT(guardarSolucion()));

    deshabilitarAlgoritmo(false);
    ui->actionGuardar->setEnabled(false);

    this->flagAlgoritmo = false;
    this->flagLectora = false;

    lectoraDeArchivo = nullptr;
    algoritmo = nullptr;
}

/**
 * @brief MainWindow::~MainWindow()
 * Destructor.
 */

MainWindow::~MainWindow()
{
    delete ui;
    ui = nullptr;

    if(this->flagLectora)
    {
        delete lectoraDeArchivo;
        lectoraDeArchivo = nullptr;
    }

    if(this->flagAlgoritmo)
    {
        delete algoritmo;
        algoritmo = nullptr;
    }
}

/**
 * @brief MainWindow::deshabilitarAlgoritmo
 * Habilita o deshabilita la opci&oacute;n de usar el algoritmo.
 * @param habilitar bool
 */

void MainWindow::deshabilitarAlgoritmo(bool habilitar)
{
    ui->aplicarAlgoritmo->setEnabled(habilitar);
    ui->poblacionInicial->setEnabled(habilitar);
    ui->numeroDeGeneraciones->setEnabled(habilitar);
    ui->porcentajeMutacion->setEnabled(habilitar);
    ui->mutarDosBit->setEnabled(habilitar);
    ui->mutarUnBit->setEnabled(habilitar);
}

/**
 * @brief MainWindow::limpiarDatos
 * Limpia los campos de los paneles de datos de entrada y datos de salida.
 */

void MainWindow::limpiarDatos()
{
    //Datos de preprocesamiento.
    ui->nodosEntrada->setText("N");
    ui->nodosSubgrafo->setText("N");
    ui->aristasEntrada->setText("E");
    ui->aristasSubgrafo->setText("E");
    ui->nombreArchivo->setText("Nombre archivo.");

    //Datos Salida
    ui->modularidad->setText("Q");
    ui->nodosSalida->setText("N");
    ui->aristasSalida->setText("E");
}

/**
 * @brief MainWindow::ejecutar()
 * Ejecuta el algoritmo. Y actualiza los campos con los datos obtenidos con el algoritmo.
 */

void MainWindow::ejecutar()
{
    ui->actionGuardar->setEnabled(true);
    int individuos = ui->poblacionInicial->value();
    int generaciones = ui->numeroDeGeneraciones->value();
    double porcentajeMutaciones = ui->porcentajeMutacion->value();
    bool mutarDosBits = ui->mutarDosBit->isCheckable();

    igraph_t grafo = lectoraDeArchivo->getSubgrafo();

    int nodos = lectoraDeArchivo->getNodosSubgrafo();
    int aristas = lectoraDeArchivo->getAristasSubgrafo();

    //Verificar que no haya otra instancia.
    if (algoritmo != nullptr) {
        delete algoritmo;
        algoritmo = nullptr;
    }

    algoritmo = new Algoritmo(grafo, nodos, aristas, individuos,generaciones,porcentajeMutaciones,mutarDosBits);

    this->flagAlgoritmo = true;

    algoritmo->algoritmo();

    ui->nodosSalida->setText(QString::number(nodos));
    ui->aristasSalida->setText(QString::number(aristas));

    double modularidadFinal = algoritmo->getModularidad();
    ui->modularidad->setText(QString::number(modularidadFinal));



}

/**
 * @brief MainWindow::abrirArchivo()
 * Este m&eacute;todo permite cargar un archivo de grafo, como primer paso para aplicar el algoritmo.
 * @note Los formatos admitidos son .graphml y .txt El formato .txt corresponde al formato de edgelist de un
 * grafo.<br/>
 * A continuaci&oacute;n un ejemplo del formato .txt (edgelist):<br/>
 * 1 2 <br/>
 * 3 6 <br/>
 * 7 8 <br/><br/>
 *
 * Para conocer m&aacute;s acerca del formato .graphml visite
 * <a href="http://graphml.graphdrawing.org/primer/graphml-primer.html"> http://graphml.graphdrawing.org/primer/graphml-primer.html</a>
 */

void MainWindow::abrirArchivo()
{

    if (lectoraDeArchivo != nullptr) {
        cout<<"entro x.x"<<endl;
        delete lectoraDeArchivo;
        lectoraDeArchivo = nullptr;
    }

    this->lectoraDeArchivo = new LectoraArchivo();

    this->flagLectora = true;


    this->limpiarDatos();

    QString rutaArchivo,nombreArchivo;
    rutaArchivo = QFileDialog::getOpenFileName(this, tr("Abrir grafo"), ".",tr("Grafos (*.txt *.graphml)"));
    QStringList ruta = rutaArchivo.split("/");
    nombreArchivo = ruta.at(ruta.size()-1);

    lectoraDeArchivo->preprocesar(rutaArchivo.toStdString());

    if(!lectoraDeArchivo->getFlagArchivo())
    {
        QMessageBox::critical(this,tr("Leer archivo grafo"),
                              tr("Hay un problema al leer el archivo, "
                                 "o el archivo es sint&aacute;cticamente incorrecto: %1."
                                 "<br/>Por favor vuelva a intentarlo").arg(nombreArchivo));

        return;

    }

    ui->nombreArchivo->setText(nombreArchivo); //Solo el nombre del archivo no la ruta.

    int nodos,aristas,nodosSubgrafo,aristasSubgrafo;

    nodos = lectoraDeArchivo->getNodosOriginal();
    aristas = lectoraDeArchivo->getAristasOriginal();
    nodosSubgrafo = lectoraDeArchivo->getNodosSubgrafo();
    aristasSubgrafo = lectoraDeArchivo->getAristasSubgrafo();

    ui->nodosEntrada->setText(QString::number(nodos));
    ui->aristasEntrada->setText(QString::number(aristas));
    ui->nodosSubgrafo->setText(QString::number(nodosSubgrafo));
    ui->aristasSubgrafo->setText(QString::number(aristasSubgrafo));

    ui->nodosSalida->setText(QString::number(nodosSubgrafo));
    ui->aristasSalida->setText(QString::number(aristasSubgrafo));

    deshabilitarAlgoritmo(true);
    ui->mutarUnBit->click();
    ui->actionGuardar->setEnabled(false);

    /*FILE* ofile;

    ofile=fopen("prueba.graphml","w");
    int res;
    if (ofile)
    {

        if(res=igraph_write_graph_graphml(&grafo,ofile))
        {
            std::cout<<"ok"<<std::endl;
        }
        fclose(ofile);
    }*/
}

/**
 * @brief MainWindow::guardarSolucion()
 * Guarda la solucion encontrada en formato <i>graphml</i>.
 */

void MainWindow::guardarSolucion()
{
    QString rutaArchivo = QFileDialog::getSaveFileName(this,tr("Guardar soluci&oacute;n"), "./solucion.graphml",tr("Grafos ( *.graphml)"));
    igraph_t grafoFinal = this->lectoraDeArchivo->getSubgrafo();
    vector<unordered_set<int> > configuracionFinal = this->algoritmo->getConfiguracionFinal();

    this->lectoraDeArchivo->crearArchivoSolucion(rutaArchivo.toStdString(), grafoFinal, configuracionFinal);
}


