/**
  * @file algoritmo.h
  * @brief Definici&oacute;n de la clase Algoritmo.
  * @author Mar&iacute;a Andrea Cruz Bland&oacute;n
  * @date 09/2013, 11/2013
  *
  **/

#ifndef ALGORITMO_H
#define ALGORITMO_H

#include <string>
//#include <igraph.h>
//#include <unordered_set>
#include "cromosoma.h"

using namespace std;

class Algoritmo
{
private:    
    int nodos;
    int aristas;

    int individuos;
    int generaciones;

    Cromosoma **poblacion;
    Cromosoma **matingPool;

    int mejorIndicePoblacion;
    double mejorModularidad;
    Cromosoma *mejorCromosoma;

    int tamMatingPool;
    double porcentajeMatingPool;
    bool mutarDosBits;

    igraph_t grafo; //Grafo al cual se le hara clustering

    double modularidadInicial;
    double modularidadFinal;
    int cantClustersInicial;
    int cantCambios;
    igraph_vector_t tamanioClustersInicial;
    igraph_vector_t membresiaInicial;

    vector<vector<bool> > matrizXInicial;
    vector<unordered_set<int> > configuracionInicial;
    vector<unordered_set<int> > listaAdyacencia;
    vector<unordered_set<int> > configuracionFinal;


    void inicializarPoblacion();
    void seleccionar();
    void reproducir();
    void clusteringNewman();
    void inicializarMatrizX();
    void inicializarConfiguracionInicial();
    void construirListaAdyacencia();

public:
    Algoritmo();
    Algoritmo(igraph_t &grafo, int nodos, int aristas,
              int individuos, int generaciones, double porcentajeMatingPool, bool mutarDosBits);
    ~Algoritmo();
    void algoritmo();
    double getModularidad();
    vector<unordered_set<int> > getConfiguracionFinal();
    int getCantCambios();

};

#endif // ALGORITMO_H
