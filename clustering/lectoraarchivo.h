/**
  * @file lectoraarchivo.h
  * @brief Definici&oacute;n de la clase LectoraArchivo.
  * @author Mar&iacute;a Andrea Cruz Bland&oacute;n
  * @date 09/2013, 11/2013
  *
  **/

#ifndef LECTORAARCHIVO_H
#define LECTORAARCHIVO_H

#include <igraph.h>
#include <string>
#include <vector>
#include <unordered_set>

using namespace std;

class LectoraArchivo
{
private:
    string nombreArchivo;

    int nodosOriginal;
    int nodosSubgrafo;
    int aristasOriginal;
    int aristasSubgrafo;
    bool flagArchivo;
    bool flagArchivoSolucion;
    bool flagGrafo;
    bool flagSubgrafo;

    igraph_t subgrafo;
    igraph_t grafo;

    void obtenerGranComponente(const igraph_t* grafo, igraph_t* subgrafo);

public:
    LectoraArchivo();
    ~LectoraArchivo();

    void preprocesar(string nombreArchivo);

    igraph_t getSubgrafo();

    int getNodosOriginal();
    int getNodosSubgrafo();
    int getAristasOriginal();
    int getAristasSubgrafo();
    bool getFlagArchivo();
    bool getFlagArchivoSolucion();
    void crearArchivoSolucion(string nombreArchivo, igraph_t& grafo, vector<unordered_set<int> > configuracionFinal);
};

#endif // LECTORAARCHIVO_H
