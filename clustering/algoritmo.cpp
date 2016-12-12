/**
  * @file algoritmo.cpp
  * @brief Implementaci&oacute;n de los m&eactue;todos de la clase Algoritmo.
  * @author Mar&iacute;a Andrea Cruz Bland&oacute;n
  * @date 09/2013, 11/2013
  *
  **/

#include "algoritmo.h"
#include <QString>
#include <math.h>
#include <iostream>


/**
 * @brief Algoritmo::Algoritmo()
 * Contructor por defecto. Inicializa un objeto Algoritmo.
 */

Algoritmo::Algoritmo()
{
}

/**
 * @brief Algoritmo::Algoritmo
 * Constructor. Inicializa un objeto Algoritmo.
 * @param grafo igraph_t& grafo al cual se le realizar&aacute; el agrupamiento.
 * @param nodos int con el n&uacute;mero total de nodos del grafo.
 * @param aristas int con el n&uacute;mero total de aristas del grafo.
 * @param individuos int cantidad de individuos para la poblaci&oacute;n inicial.
 * @param generaciones int n&uacute;mero de generaciones para iterar el algoritmo.
 * @param porcentajeMatingPool double con el porcentaje de la poblaci&oacute;n inicial que se llevar&aacute; al <i>mating pool</i>
 * @param mutarDosBits bool activa mutar sobre dos bits de la matriz de membresia, sino se muta sobre un s&oacute;lo bit.
 */

Algoritmo::Algoritmo(igraph_t &grafo, int nodos, int aristas, int individuos, int generaciones, double porcentajeMatingPool, bool mutarDosBits)
{

    this->nodos = nodos;
    this->aristas = aristas;

    this->individuos = individuos;
    this->generaciones = generaciones;

    this->poblacion = new Cromosoma*[this->individuos];

    this->porcentajeMatingPool = porcentajeMatingPool;
    this->tamMatingPool = floor(this->individuos*this->porcentajeMatingPool);
    this->matingPool = new Cromosoma*[this->tamMatingPool];

    this->mutarDosBits = mutarDosBits;

    this->modularidadFinal = 0.0;
    this->modularidadInicial = 0.0;

    this->grafo = grafo;

    this->mejorIndicePoblacion = -1;
    this->mejorModularidad = 0.0;

    this->cantCambios = 0;
}

Algoritmo::~Algoritmo()
{
    for(int i=0; i<individuos; i++)
    {
        delete this->poblacion[i];
        this->poblacion[i] = nullptr;
    }

    delete this->poblacion;
    this->poblacion = nullptr;

    for(int i=0; i<this->tamMatingPool; i++)
    {
        this->matingPool[i] = nullptr;
    }

    delete this->matingPool;
    this->matingPool = nullptr;

    this->mejorCromosoma = nullptr;

    igraph_vector_destroy(&this->membresiaInicial);
    igraph_vector_destroy(&this->tamanioClustersInicial);

    igraph_destroy(&this->grafo);
}


/**
 * @brief Algoritmo::algoritmo
 * Se realiza primero el <i>clustering</i> haciendo uso del algoritmo de Newman. Paso siguiente se inicializa la poblaci&oacute;n
 * de cromosomas garantizando que tengan solapamiento entre <i>clusters</i>. Luego de tener la poblaci&oacute;n inicial, se realiza
 * la selecci&oacute;n de un porcentaje de cromosomas, usando la modularidad como funci&oacute;n de aptitud.
 * Una vez se carga el <i>mating pool</i> con los cromosomas seleccionados se realiza la reproducci&oacute;n, usando como operador
 * la mutaci&oacute;n.<br/>
 * Finalmente se selecciona como soluci&oacute;n el cromosoma que obtenga la mayor modularidad.
 */

void Algoritmo::algoritmo()
{

    construirListaAdyacencia();
    clusteringNewman();
    inicializarConfiguracionInicial();

    if(this->cantClustersInicial != 1)
    {
        inicializarMatrizX();
        inicializarPoblacion();

        for(int i=0; i<this->generaciones; i++)
        {
            seleccionar();
            reproducir();
        }

        /*cout<<"indice poMejor: "<<this->mejorIndicePoblacion<<endl;
        cout<<"modularidad Mejor: "<<this->mejorModularidad<<endl;
        cout<<"cambios: "<<this->cantCambios<<endl;*/
    }else if(this->cantClustersInicial == 1)
    {
        this->individuos = 0;
        this->tamMatingPool = 0;
        this->generaciones = 0;
        this->configuracionFinal = this->configuracionInicial;
    }

    this->modularidadFinal = this->mejorModularidad;
    /*poblacion[mejorIndicePoblacion]->calcularModularidad(listaAdyacencia,aristas);
    cout<<"mejor: "<<poblacion[mejorIndicePoblacion]->getModularidad()<<endl;
    poblacion[mejorIndicePoblacion]->calcularProporcionClusters();
    cout<<"clusters: "<<poblacion[mejorIndicePoblacion]->getProporcionClusters()<<endl;*/

    //PRUEBA MOMENTANEA DE COMO QUEDA AGRUPADO

    /*vector<unordered_set<int> > configuracion = poblacion[mejorIndicePoblacion]->getConfiguracionClustering();
    for(int i=0; i<this->cantClustersInicial; i++)
    {
        cout<<"Cluster: "<<i<<" ";
        unordered_set<int> miembros = configuracion[i];
        for(auto it=miembros.begin(); it != miembros.end(); ++it)
        {
            int miembro = *it;
            //cout<<VAS(&this->grafo,"name",miembro)<<", ";
            cout<<miembro<<", ";
        }
        cout<<endl;
    }*/


    //cout<<"Veces que cambió de cromosoma: "<<this->cantCambios<<endl;

}

/**
 * @brief Algoritmo::clusteringNewman
 * Realiza el primer <i>clustering</i> haciendo uso del algoritmo propuesto por Newman en <b> Finding community structure in very large networks</b>
 * puede ver detalles en <a href="http://igraph.sourceforge.net/doc/html/ch22s06.html">http://igraph.sourceforge.net/doc/html/ch22s06.html</a>
 * y <a href="http://www.arxiv.org/abs/cond-mat/0408187"> http://www.arxiv.org/abs/cond-mat/0408187 </a>.
 */

void Algoritmo::clusteringNewman()
{
    igraph_vector_t modularity;
    igraph_matrix_t merges;

    igraph_vector_init(&modularity,0);
    igraph_vector_init(&this->tamanioClustersInicial,0);
    igraph_vector_init(&this->membresiaInicial,0);
    igraph_matrix_init(&merges,0,0);

    igraph_community_fastgreedy(&this->grafo,0, &merges, &modularity);

    int posMod;

    posMod = igraph_vector_which_max(&modularity);
    this->modularidadInicial = VECTOR(modularity)[posMod];

    igraph_community_to_membership(&merges,this->nodos,posMod,&this->membresiaInicial,&this->tamanioClustersInicial);
    this->cantClustersInicial = igraph_vector_size(&this->tamanioClustersInicial);

    /*cout << "Modularidad inicial: "<<modularidadInicial<< endl;
    cout << "Clusters iniciales: "<<this->cantClustersInicial<< endl;*/

    igraph_vector_destroy(&modularity);
    igraph_matrix_destroy(&merges);
}

/**
 * @brief Algoritmo::getModularidad
 * Retorna la modularidad final obtenida con el mejor cromosoma encontrado.
 * @return double con la modularidad final.
 */

double Algoritmo::getModularidad()
{
    return this->modularidadFinal;
}

/**
 * @brief Algoritmo::getConfiguracionFinal
 * Retorna la configuraci&oacute;n final del mejor cromosoma encontrado.
 * @return vector<unordered_set<int> > con la configuraci&oacute;n final.
 */
vector<unordered_set<int> > Algoritmo::getConfiguracionFinal()
{
    return this->configuracionFinal;
}

/**
 * @brief Algoritmo::getCantCambios
 * Retorna cuantos cambios se hicieron a la referencia del mejor cromosoma al final del algoritmo (Para elegir la soluci&oacute;n).
 * @return int cantCambios
 */
int Algoritmo::getCantCambios()
{
    return  this->cantCambios;
}

/**
 * @brief Algoritmo::inicializarPoblacion
 * Se crean los individuos iniciales de cromosomas, se usa la configuraci&oacute;n de <i>clustering</i> retornada por
 * Newman, y se garantiza que todos los cromosomas iniciales tengan solapamiento en un nodo.
 */

void Algoritmo::inicializarPoblacion()
{
    //El primer cromosoma sera con la configuracion de Newman.
    unordered_set<int> miembros =  this->configuracionInicial[0];
    auto it=miembros.begin();
    int nodo = *it;

    this->poblacion[0] =  new Cromosoma(this->matrizXInicial, this->configuracionInicial, this->mutarDosBits, this->nodos,
                                        this->cantClustersInicial, nodo, 0);

    //Y se selecciona como el mejor hasta ahora en el algoritmo.
    this->mejorCromosoma = this->poblacion[0];
    this->mejorIndicePoblacion = 0;
    this->mejorModularidad = this->modularidadInicial;
    this->configuracionFinal = this->configuracionInicial;

    for(int i=1; i<this->individuos;i++)
    {
        int cluster = rand()% this->cantClustersInicial;
        bool nodoMedio=true;
        int nodo;
        while(nodoMedio)
        {
            int indiceNodoTemp = rand()%this->configuracionInicial[cluster].size();
            unordered_set<int> miembros = this->configuracionInicial[cluster];
            int nodoTemp;

            for(auto it=miembros.begin(); (it!=miembros.end() && indiceNodoTemp>=0); ++it)
            {
                nodoTemp = *it;
                indiceNodoTemp--;
            }

            int posibles=0;
            vector<int> posiblesNodos;
            unordered_set<int> vecinos = this->listaAdyacencia[nodoTemp];

            for(auto it=vecinos.begin(); it!=vecinos.end(); ++it)
            {
                int vecino=*it;
                if(!(this->matrizXInicial[cluster][vecino]))
                {
                    posibles++;
                    posiblesNodos.push_back(vecino);
                }
            }

            if(posibles!=0)
            {
                nodoMedio=false;
                int indiceNodoFinal = rand()%posibles;
                nodo = posiblesNodos[indiceNodoFinal];
            }
        }

        //cout<<"nodo: "<<VAS(&this->grafo, "name", nodo)<<" cluster que va a modificar: "<<cluster<<endl;

        this->poblacion[i] = new Cromosoma(this->matrizXInicial, this->configuracionInicial, this->mutarDosBits, this->nodos,
                                           this->cantClustersInicial, nodo, cluster);

    }

    /*cout<<"Membresia: "<<endl;
    for(int i=0; i < this->nodos; i++)
    {
        cout<<VECTOR(membresiaInicial)[i]<<", ";
    }*/
}

/**
 * @brief Algoritmo::seleccionar
 * La selecci&oacute;n se realiza con la t&eacute;cnica torneo, se seleccionan dos cromosomas al azar, de cada uno
 * se obtiene la modularidad, se elige el cromosoma con mayor modularidad. Si la modularidad llegara a ser igual
 * en ambos cromosomas, se elige el cromosoma que tenga mayor porcentaje de <i>clusters</i> respecto a la cantidad de
 * <i>clusters</i> encontrada al inicio con el algoritmo de Newman. <br/>
 *
 * Adem&aacute;s se guarda el indice del cromosoma en toda la poblaci&oacute;n y que halla sido elegido para el <i>mating pool</i>
 * y que sea el cromosoma con la mayor modularidad obtenida en el proceso.
 */

void Algoritmo::seleccionar()
{
    for (int i=0; i<this->tamMatingPool; i++)
    {
        int cromosoma1 = rand()%this->individuos;
        int cromosoma2 = rand()%this->individuos;

        bool calculoModularidad1 = this->poblacion[cromosoma1]->getCalculoModularidad();
        bool calculoModularidad2 = this->poblacion[cromosoma2]->getCalculoModularidad();

        if(!calculoModularidad1)
        {
            this->poblacion[cromosoma1]->calcularModularidad(this->listaAdyacencia, this->aristas);
        }
        if(!calculoModularidad2)
        {
            this->poblacion[cromosoma2]->calcularModularidad(this->listaAdyacencia, this->aristas);
        }

        double modularidad1 = this->poblacion[cromosoma1]->getModularidad();
        double modularidad2 = this->poblacion[cromosoma2]->getModularidad();

        //cout<<"iteracion numero: "<<i<<" Modularidad 1: "<<modularidad1<<" Modularidad 2: "<<modularidad2<<endl;

        if(modularidad1 == modularidad2)
        {
            this->poblacion[cromosoma1]->calcularProporcionClusters();
            this->poblacion[cromosoma2]->calcularProporcionClusters();

            double proporcion1 = this->poblacion[cromosoma1]->getProporcionClusters();
            double proporcion2 = this->poblacion[cromosoma2]->getProporcionClusters();

            if(proporcion1 >= proporcion2)
            {
                this->matingPool[i] = this->poblacion[cromosoma1];

                if(this->mejorModularidad<=modularidad1)
                {
                    this->mejorModularidad=modularidad1;
                    this->mejorIndicePoblacion=cromosoma1;
                    this->mejorCromosoma = this->poblacion[this->mejorIndicePoblacion];
                    this->configuracionFinal = this->mejorCromosoma->getConfiguracionClustering();
                }

            }else{
                this->matingPool[i] = this->poblacion[cromosoma2];

                if(this->mejorModularidad<=modularidad2)
                {
                    this->mejorModularidad=modularidad2;
                    this->mejorIndicePoblacion=cromosoma2;
                    this->mejorCromosoma = this->poblacion[this->mejorIndicePoblacion];
                    this->configuracionFinal = this->mejorCromosoma->getConfiguracionClustering();
                }
            }


        }
        if(modularidad1 < modularidad2)
        {
            this->matingPool[i] = this->poblacion[cromosoma2];

            if(this->mejorModularidad<=modularidad2)
            {
                this->mejorModularidad=modularidad2;
                this->mejorIndicePoblacion=cromosoma2;
                this->mejorCromosoma = this->poblacion[this->mejorIndicePoblacion];
                this->configuracionFinal = this->mejorCromosoma->getConfiguracionClustering();
            }

        }else
        {
            this->matingPool[i] = this->poblacion[cromosoma1];

            if(this->mejorModularidad<=modularidad1)
            {
                this->mejorModularidad=modularidad1;
                this->mejorIndicePoblacion=cromosoma1;
                this->mejorCromosoma = this->poblacion[this->mejorIndicePoblacion];
                this->configuracionFinal = this->mejorCromosoma->getConfiguracionClustering();
            }
        }
    }
}

/**
 * @brief Algoritmo::reproducir
 * Para la reproducci&oacute;n, se usa la mutaci&oacute;n sobre un cromosoma, esta puede darse de dos formas, seg&uacute;n
 * el se halla definido. Una es mutar un solo <i>bit</i> de la matriz X, y la otra mutar dos <i>bits</i> de la matriz X.
 * Si el cromosoma es el indicado por el indice del mejor cromosoma en el mating pool, este no se muta y se pasa directo
 * a la siguiente generaci&oacute;n.
 */

void Algoritmo::reproducir()
{

    int cantidadMutaciones = 0;
    if(mutarDosBits)
    {
        cantidadMutaciones = 2;
    }
    else
    {
        cantidadMutaciones = 1;
    }

    for(int i=0; i<this->tamMatingPool; i++)
    {
        if(this->matingPool[i] != this->mejorCromosoma)
        {
            int nodosMutar[cantidadMutaciones];
            int clustersMutar[cantidadMutaciones];
            vector<unordered_set<int> > configuracionActual = this->matingPool[i]->getConfiguracionClustering();

            for(int j=0; j<cantidadMutaciones; j++)
            {
                int cluster = rand()%this->cantClustersInicial;
                unordered_set<int> miembros = configuracionActual[cluster];

                int nodo;

                if(miembros.size()==0) //Un cluster que fue eliminado, es decir, se quedó sin nodos en el proceso.
                {
                    nodo = rand()%this->nodos;
                } else
                {
                    int indiceNodoTemp = rand()%miembros.size();
                    int nodoTemp;

                    for(auto it=miembros.begin(); (it != miembros.end() && indiceNodoTemp>=0); ++it)
                    {
                        nodoTemp = *it;
                        indiceNodoTemp--;
                    }

                    unordered_set<int> vecinos = this->listaAdyacencia[nodoTemp];
                    int indiceNodo = rand()%vecinos.size();

                    for(auto it=vecinos.begin(); (it != vecinos.end() && indiceNodo>=0); ++it)
                    {
                        nodo = *it;
                        indiceNodo--;
                    }
                }

                nodosMutar[j] = nodo;

                clustersMutar[j] = cluster;
            }

            this->matingPool[i]->mutar(nodosMutar, clustersMutar);

        }
    }
}

/**
 * @brief Algoritmo::inicializarMatrizX
 * Inicializa la matriz de membresia (binaria), tal y como el algoritmo de Newma configur&oacute; los <i>clusters</i>.
 * La matriz tiene la forma descrita en el trabajo <a href="http://dl.acm.org/citation.cfm?id=1339693"> A Extraction Method of Overlapping Cluster Based on Network Structure Analysis</a>
 * Las filas corresponden a los <i>clusters</i> y las columnas a los nodos.
 */

void Algoritmo::inicializarMatrizX()
{
    for(int i=0; i<this->cantClustersInicial; i++)
    {
        vector<bool> vectorNodos(this->nodos);
        matrizXInicial.push_back(vectorNodos);
    }

    for(int i=0; i<this->nodos; i++)
    {
        matrizXInicial[VECTOR(this->membresiaInicial)[i]][i] = true;
    }

    /*for(int i=0; i<this->cantClustersInicial; i++)
    {
        for (int j=0 ; j<this->nodos; j++)
        {
            cout<<matrizXInicial[i][j]<<",";
        }
        cout<<endl;
    }*/
}

/**
 * @brief Algoritmo::inicializarConfiguracionInicial
 * Inicializa la configuraci&oacute;n incial de <i>clustering</i> en el formato de lista de adyacencia.
 */

void Algoritmo::inicializarConfiguracionInicial()
{
    //Lista de adyacencia de la configuracion de clustering.
    vector<unordered_set <int> > configuracionInicial(this->cantClustersInicial);
    for(int i=0; i<this->nodos; i++)
    {
        int cluster = VECTOR(this->membresiaInicial)[i];
        configuracionInicial[cluster].insert(i);
    }

    this->configuracionInicial = configuracionInicial;

    /*for(int i=0; i<this->cantClustersInicial; i++)
    {
        cout<<"cluster: "<<i<<endl;
        unordered_set<int> miembros = configuracionInicial[i];

        for(auto it=miembros.begin(); it != miembros.end(); ++it)
        {
            int nodo= *it;
            //cout<<VAS(&this->grafo,"name", nodo)<<",";
            cout<<nodo<<", ";
        }
        cout<<endl;
    }*/
}


/**
 * @brief Algoritmo::construirListaAdyacencia
 * Crea la lista de adyacencia del grafo, la cual corresponde a una estructura de dato mucho m&aacute;s
 * eficiente que la matriz de adyacencia. Se usa como estructura de dato vector<unordered_set<int>>.
 */

void Algoritmo::construirListaAdyacencia()
{
    for(int i=0; i<this->nodos; i++)
    {
        igraph_vector_t vecinosV;
        igraph_vector_init(&vecinosV,0);

        unordered_set<int> vecinos;

        igraph_neighbors(&this->grafo,&vecinosV, i, IGRAPH_ALL);


        for(int j=0; j<igraph_vector_size(&vecinosV); j++)
        {
            int vecino = VECTOR(vecinosV)[j];
            vecinos.insert(vecino);
        }

        this->listaAdyacencia.push_back(vecinos);

        igraph_vector_destroy(&vecinosV);
    }

    /*
    for(int i=0; i<this->nodos; i++)
    {
        unordered_set<int> vecinos = this->listaAdyacencia[i];
        cout<<"vecinos de "<<VAS(&this->grafo,"name",i)<<endl;
        for(auto it=vecinos.begin(); it != vecinos.end(); ++it)
        {
            int v=*it;

            cout<<VAS(&this->grafo,"name",v)<<",";
        }
        cout<<endl;
    }*/
}


