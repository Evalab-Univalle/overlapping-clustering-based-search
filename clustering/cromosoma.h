/**
  * @file cromosoma.h
  * @brief Definici&oacute;n de la clase Cromosoma.
  * @author Mar&iacute;a Andrea Cruz Bland&oacute;n
  * @date 09/2013, 11/2013
  *
  **/

#ifndef CROMOSOMA_H
#define CROMOSOMA_H

#include <igraph.h>
#include <vector>
#include <unordered_set>

using namespace std;

class Cromosoma
{
private:
    vector< vector<bool> > matrizX;
    vector< unordered_set<int> > configuracion;

    double modularidad;
    float proporcionClusters;
    bool calculoModularidad;

    int nodos;
    int cantClustersInicial;
    int cantClustersFinal;
    igraph_vector_t tamanioClusters;

    bool mutarDosBits;

    double calcularFraccionAristasInternas(vector<unordered_set<int> >& listaAdyacencia, int clusterA, int clusterB, int aristas);
    double calcularFraccionAristasExternas(vector<unordered_set<int> >& listaAdyacencia, int clusterA, int aristas);


public:
    Cromosoma();
    Cromosoma(vector<vector<bool> >& matrizXInicial, vector<unordered_set <int> >  configuracionInicial, bool mutarDosBits,
              int nodos, int cantClustersInicial, int nodo, int cluster);
    ~Cromosoma();

    void calcularModularidad(vector<unordered_set<int> >& listaAdyacencia, int aristas);
    void calcularProporcionClusters();

    double getModularidad();
    double getProporcionClusters();

    void mutar(int nodosMutar[], int clustersMutar[]);

    vector<unordered_set<int> > getConfiguracionClustering();
    bool getCalculoModularidad();

    int getCantidadClustersFinal();
};

#endif // CROMOSOMA_H
