/**
  * @file cromosoma.cpp
  * @brief Implementaci&oacute;n de los m&eactue;todos de la clase Cromosoma.
  * @author Mar&iacute;a Andrea Cruz Bland&oacute;n
  * @date 09/2013, 11/2013
  *
  **/

#include "cromosoma.h"

#include <iostream>

/**
 * @brief Cromosoma::Cromosoma
 * Constructor por defecto.
 */

Cromosoma::Cromosoma()
{
}

/**
 * @brief Cromosoma::Cromosoma
 * Constructor. Se inicializa un objeto de la clase Cromosoma, donde se añade un solapamiento en el <i>clustering</i>.
 * @param matrizXInicial vector<vector<bool> > referencia de la configuraci&oacute;n de <i>clustering</i> que retorn&oacute; el algoritmo de Newman.
 * @param configuracionInicial vector<unordered_set<int> > con la configuraci&oacute;n de <i>clustering</i> que retorn&oacute; el algoritmo de Newman en formato de lista de adyacencia.
 * @param mutarDosBits bool que indica si la mutaci&oacute;n se debe realizar sobre un bit o sobre dos bits de la matriz de membresia.
 * @param nodos int con el n&uacute;mero total de nodos en el grafo.
 * @param cantClustersInicial int con el n&uacute;mero total de <i>clusters</i> que se obtuvieron con el algoritmo de Newman.
 * @param nodo int con el n&uacute;mero del nodo que har&aacute; solapamiento.
 * @param cluster int con el n&uacute;mero del <i>cluster</i> que tendr&aacute; un nodo mas en el grupo.
 */

Cromosoma::Cromosoma(vector<vector<bool> > &matrizXInicial, vector<unordered_set<int> > configuracionInicial, bool mutarDosBits,
                     int nodos, int cantClustersInicial, int nodo, int cluster)
{
    this->nodos = nodos;
    this->cantClustersInicial = cantClustersInicial;
    this->mutarDosBits = mutarDosBits;

    this->modularidad = 0.0;
    this->proporcionClusters = 0.0;
    this->calculoModularidad = false;

    this->cantClustersFinal = this->cantClustersInicial; //Hasta este momento, esto se cumple.

    for(int i=0; i<this->cantClustersInicial; i++)
    {
        vector<bool> vectorNodos(this->nodos);
        matrizX.push_back(vectorNodos);
    }

    for(int i=0; i<this->cantClustersInicial; i++)
    {
        for(int j=0; j<this->nodos; j++)
        {
            matrizX[i][j] = matrizXInicial.at(i).at(j);
        }
    }

    this->configuracion = configuracionInicial;

    //ANADIR CASO UNO DE SI EL NODO YA ESTA NO INGRESAR!!!!!!!
    this->configuracion[cluster].insert(nodo);

    matrizX[cluster][nodo] = true;

    //Se debe actualizar el tamaño del cluster afectado.
    igraph_vector_init(&this->tamanioClusters,this->cantClustersInicial);

    for(int i=0; i<this->cantClustersInicial; i++)
    {
        VECTOR(this->tamanioClusters)[i] = this->configuracion[i].size();
    }
    //VECTOR(this->tamanioClusters)[cluster] += 1;

    //cout<<"Creo un cromosoma, afecto nodo: "<<nodo<<" y cluster: "<<cluster<<endl;
}


/**
 * @brief Cromosoma::~Cromosoma
 * Destructor.
 */

Cromosoma::~Cromosoma()
{
    igraph_vector_destroy(&this->tamanioClusters);
}

/**
 * @brief Cromosoma::calcularModularidad
 * Calcula la modularidad de la configuraci&oacute;n de <i>clustering</i> que se ha realizado en un grafo.
 * @param listaAdyacencia vector<unordered_set<int> > referencia de la lista de adyacencia del grafo.
 * @param aristas int n&uacute;mero total de aristas en el grafo.
 */

void Cromosoma::calcularModularidad(vector<unordered_set<int> > &listaAdyacencia, int aristas)
{
    this->modularidad = 0.0;
    for(int i=0; i<this->cantClustersInicial;i++)
    {
        double fraccionAristasInternas, fraccionAristasExternas;

        fraccionAristasInternas = calcularFraccionAristasInternas(listaAdyacencia,i,i,aristas);
        fraccionAristasExternas = calcularFraccionAristasExternas(listaAdyacencia,i,aristas);

        //cout<<"clusters: "<<i<<" e: "<<fraccionAristasInternas<<", a: "<<fraccionAristasExternas<<endl;
        this->modularidad += fraccionAristasInternas - (fraccionAristasExternas*fraccionAristasExternas);
    }
    this->calculoModularidad = true;
    //cout<<"modularidad obtenida: "<<this->modularidad<<endl;
}

/**
 * @brief Cromosoma::calcularProporcionClusters
 * Calcula la proporci&oacute;n de <i>clusters</i>. Divide la cantidad de <i>clusters</i> actuales por la cantidad de <i>clusters</i> inicial.
 */

void Cromosoma::calcularProporcionClusters()
{
    this->proporcionClusters = 0.0;
    this->proporcionClusters = this->cantClustersFinal/this->cantClustersInicial;
}


/**
 * @brief Cromosoma::calcularFraccionAristasInternas
 * Calcula la cantidad la fracci&oacute;n de las aristas que conectan nodos que pertenencen al <i>clusterA</i> con nodos que pertenencen al <i>clusterB</i>.
 * @param listaAdyacencia vector<unordered_set<int> > referencia de la lista de adyacencia del grafo.
 * @param clusterA int como identificador de un cluster.
 * @param clusterB int como identificador de un cluster.
 * @param aristas int n&uacute;mero total de aristas en el grafo.
 * @return doble con la fracci&oacute;n de aristas internas de un <i>cluster</i>.
 */

double Cromosoma::calcularFraccionAristasInternas(vector<unordered_set<int> > &listaAdyacencia, int clusterA, int clusterB, int aristas)
{
    double fraccionAristasInternas = 0.0;
    for(int i=0; i<this->nodos; i++)
    {
        unordered_set<int> vecinos = listaAdyacencia[i];
        for(auto it=vecinos.begin(); it!=vecinos.end(); ++it)
        {
            int vecino=*it;
            if((matrizX[clusterA][i]) && (matrizX[clusterB][vecino]))
            {
                fraccionAristasInternas += 1.0;
            }

        }
    }

    fraccionAristasInternas = fraccionAristasInternas/(2*aristas);

    return fraccionAristasInternas;
}

/**
 * @brief Cromosoma::calcularFraccionAristasExternas
 * Calcula la fracci&oactute;n de aristas que terminan en nodos del <i>clusterA</i>.
 * @param listaAdyacencia vector<unordered_set<int> > referencia de la lista de adyacencia del grafo.
 * @param clusterA int como identificador de un cluster.
 * @param aristas int n&uacute;mero total de aristas en el grafo.
 * @return doble con la fracci&oacute;n de aristas externas de un <i>cluster</i>.
 */

double Cromosoma::calcularFraccionAristasExternas(vector<unordered_set<int> > &listaAdyacencia, int clusterA, int aristas)
{
    double fraccionAristasExternas = 0.0;
    for(int i=0; i<this->nodos; i++)
    {
        unordered_set<int> vecinos = listaAdyacencia[i];
        if(matrizX[clusterA][i])
        {
            fraccionAristasExternas += vecinos.size();
        }
    }

    fraccionAristasExternas = fraccionAristasExternas/(2*aristas);

    return fraccionAristasExternas;
}

/**
 * @brief Cromosoma::getModularidad
 * Retorna modularidad del cromosoma.
 * @return double con el valor de la modularidad, para la configuraci&oacute;n del cromosoma.
 */

double Cromosoma::getModularidad()
{
    return this->modularidad;
}

/**
 * @brief Cromosoma::getProporcionClusters
 * Retorna la proporci&oacute;n de <i>clusters</i> respecto a la cantidad inicial.
 * @return double con el valor de la proporci&oacute;n de cantidad de <i>clusters</i> finales respecto a la cantidad de <i>clusters</i> inicial.
 */

double Cromosoma::getProporcionClusters()
{
    return this->proporcionClusters;
}

/**
 * @brief Cromosoma::mutar
 * Cambia a <i>true</i> la pertencia del nodo, si el nodo a mutar no pertenec&iacute;a al <i>cluster</i>, o a <i>false</i> en caso contrario.
 * Cuando cambia a <i>false</i> la pertenencia del nodo; se verifica que el nodo pertenezca al menos a un <i>cluster</i>, sino se deja el cromosoma sin mutar.
 * @param nodosMutar arreglo de int que contiene los nodos que ser&aacute;n mutados.
 * @param clustersMutar arreglo de int que contiene los <i>clusters</i> que ser&aacute;n mutados.
 */

void Cromosoma::mutar(int nodosMutar[], int clustersMutar[])
{
    int cantidadMutaciones = 0;
    if(this->mutarDosBits)
    {
        cantidadMutaciones = 2;
    }else{
        cantidadMutaciones = 1;
    }

    for(int i=0; i<cantidadMutaciones; i++)
    {
        bool pertenencia = this->matrizX[clustersMutar[i]][nodosMutar[i]];
        if(pertenencia)
        {
            this->matrizX[clustersMutar[i]][nodosMutar[i]] = false;

            bool almenosUno=false;

            for(int j=0; j<this->cantClustersInicial; j++) //Verificar que el nodo pertenezca al menos a un cluster.
            {
                almenosUno = almenosUno || matrizX[j][nodosMutar[i]];
            }

            if(!almenosUno)
            {
                this->matrizX[clustersMutar[i]][nodosMutar[i]] = true;

            }else
            {
                this->configuracion[clustersMutar[i]].erase(nodosMutar[i]);

                VECTOR(this->tamanioClusters)[clustersMutar[i]] = VECTOR(this->tamanioClusters)[clustersMutar[i]] - 1;

                if(VECTOR(this->tamanioClusters)[clustersMutar[i]] == 0)
                {
                    this->cantClustersFinal -= 1;
                }
            }
        }else{

            this->configuracion[clustersMutar[i]].insert(nodosMutar[i]);

            this->matrizX[clustersMutar[i]][nodosMutar[i]] = true;

            VECTOR(this->tamanioClusters)[clustersMutar[i]] = VECTOR(this->tamanioClusters)[clustersMutar[i]] + 1;

            if(VECTOR(this->tamanioClusters)[clustersMutar[i]] == 1)
            {
                this->cantClustersFinal += 1;
            }
        }
    }

    this->calculoModularidad = false; //Cuando ocurre un cambio es necesario que se vuelva a calcular la modularidad.

}

/**
 * @brief Cromosoma::getConfiguracionClustering
 * Retorna la configuraci&oactue;n de <i>clustering</i> obtenida en el cromosoma.
 * @return vector<unordered_set<int> > que contiene la configuraci&oacute;n de los <i>clusters</i> en el formato de
 * lista de adyacencia.
 */

vector<unordered_set<int> > Cromosoma::getConfiguracionClustering()
{
    return this->configuracion;
}

/**
 * @brief Cromosoma::getCalculoModularidad
 * Retorna el booleano que representa si ya se ha realizado el c&aacute;lculo de la modularidad.
 * @return bool calculoModularidad.
 */

bool Cromosoma::getCalculoModularidad()
{
    return this->calculoModularidad;
}

/**
 * @brief Cromosoma::getCantidadClustersFinal
 * Retorna la cantidad de <i>clusters</i> de la configuraci&oacute;n final de <i>clustering</i>.
 * @return int cantClustersFinal.
 */

int Cromosoma::getCantidadClustersFinal()
{
    return this->cantClustersFinal;
}




