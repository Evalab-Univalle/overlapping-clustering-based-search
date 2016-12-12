/**
  * @file lectoraarchivo.cpp
  * @brief Implementaci&oacute;n de los m&eactue;todos de la clase LectoraArchivo.
  * @author Mar&iacute;a Andrea Cruz Bland&oacute;n
  * @date 09/2013, 11/2013
  *
  **/

#include "lectoraarchivo.h"

#include <iostream>
#include <QString>
#include <assert.h>

/**
 * @brief LectoraArchivo::LectoraArchivo
 * Constructor.
 */

LectoraArchivo::LectoraArchivo()
{
    this->flagArchivo = true;
    this->flagArchivoSolucion = true;
    this->flagGrafo = false;
    this->flagSubgrafo = false;
}

/**
 * @brief LectoraArchivo::~LectoraArchivo
 * Destructor.
 */

LectoraArchivo::~LectoraArchivo()
{
    if(this->flagGrafo)
    {
        igraph_destroy(&this->grafo);
    }


    if(this->flagSubgrafo)
    {
        igraph_destroy(&this->subgrafo);
    }

}

/**
 * @brief LectoraArchivo::preprocesar
 * @param nombreArchivo string con la ruta del archivo que contiene el grafo.
 * Lee el archivo, extrae el grafo, si el grafo no es conexo obtiene el gran componente de &eacute;ste.
 * @note Si el archivo no existe, o es sint&aacute;cticamente incorrecto el procesamiento no continua.
 */

void LectoraArchivo::preprocesar(string nombreArchivo)
{
    this->nombreArchivo = nombreArchivo;
    FILE* f;
    f = fopen(this->nombreArchivo.c_str(),"rb");

    if(!f)
    {
        this->flagArchivo = false;
        return;
    }

    QString extension= QString::fromStdString(nombreArchivo);

    bool txt = extension.endsWith(".txt");
    bool graphml = extension.endsWith(".graphml");

    igraph_error_handler_t* oldhandler; //PARA MANEJAR EXCEPCIONES

    igraph_i_set_attribute_table(&igraph_cattribute_table); //PARA QUE SE PUEDAN OBTENER ATRIBUTOS
    oldhandler=igraph_set_error_handler(igraph_error_handler_ignore); //No terminar el programa cuando se presenta una excepcion.

    if(txt)//If para leer formato edgelist
    {
        int result;
        result = igraph_read_graph_edgelist(&this->grafo,f,0,IGRAPH_UNDIRECTED);

        if(result == IGRAPH_PARSEERROR)
        {
            this->flagArchivo = false;
            return;
        }

    }
    if(graphml)//If para leer formato graphml
    {
        int result;
        result = igraph_read_graph_graphml(&this->grafo,f,0);

        if(result == IGRAPH_PARSEERROR || result == IGRAPH_UNIMPLEMENTED)
        {
            this->flagArchivo = false;
            return;
        }

    }

    this->flagGrafo = true;

    //Cerrar archivo e iguala puntero a nullptr.
    fclose(f);
    f = nullptr;

    igraph_set_error_handler(oldhandler);

    this->nodosOriginal = igraph_vcount(&this->grafo);
    this->aristasOriginal = igraph_ecount(&this->grafo);

    /**
     * @note Si se esta trabajando con un grafo cargado con un formato de edgelist,
     * se debe ajustar los atributos de los vertices para que estos no pierdan
     * su id de identificaci&oacute;n.
     */

    if(txt)
    {
        for(int i=0; i<this->nodosOriginal;i++)
        {
            const char *value= QString::number(i).toStdString().c_str();
            SETVAS(&this->grafo,"name",i,value);
            //cout <<"valores names vertices: "<<value<<" indice: "<<i<<endl;
        }
    }

    //Se obtiene el subgrafo con el que se va a trabajar el algoritmo.
    this->obtenerGranComponente(&this->grafo,&this->subgrafo);

    this->flagSubgrafo = true;

}

/**
 * @brief LectoraArchivo::obtenerGranComponente
 * @param grafo Puntero al grafo original.
 * @param subgrafo Puntero al grafo que se crear&aacute; una vez se obtenga el gran componente del grafo original.
 * @note Si el grafo fue cargado con edgelist los ids de los vertices corresponden a los n&uacute;meros que hab&iacute;
 * en el archivo. Por el contrario si fue cargado con graphml los ids corresponder&aacute;n a la configuraci&oacute;n
 * del archivo graphml.
 */

void LectoraArchivo::obtenerGranComponente(const igraph_t *grafo, igraph_t *subgrafo)
{
    //Datos necesarios para identificar el gran componente y el numero total de nodos que contiene
    int granComponente = 0,totalNodos = 0;

    igraph_vector_t membership; //Especifica cual es la pertenecia de un nodo membership[Nodo]=Componente_Nodo
    igraph_vector_t csize; //Especifica el tamaño de los componentes(cantidad de nodos).
    igraph_integer_t clusters; //El total de componentes en el grafo.

    igraph_vector_init(&membership,0);
    igraph_vector_init(&csize,0);


    igraph_clusters(grafo,&membership,&csize,&clusters,IGRAPH_STRONG);//Obtener los componentes


    //Identificar el componente que contiene la mayoria de los nodos.
    for(int i=0; i<clusters; i++)
    {
        int temp = VECTOR(csize)[i];
        if(temp>totalNodos)
        {
            granComponente = i;
            totalNodos = temp;
        }

    }

    //Obtener los nodos que pertenecen al gran componente.
    igraph_vector_t nodos;
    igraph_vector_init(&nodos,0);

    for(int i=0; i<igraph_vector_size(&membership); i++)
    {
        if(VECTOR(membership)[i] == granComponente)
        {
            igraph_vector_insert(&nodos,0, i);

        }
    }

    igraph_vs_t nodos_vs;
    igraph_vs_vector_copy(&nodos_vs,&nodos); //Necesario para usar la funcion que crea el subgrafo.

    igraph_subgraph(grafo, subgrafo, nodos_vs); //Se obtiene el sugrafo.

    //Se configura la informacion del subgrafo obtenido para mostrar al usuario.
    this->nodosSubgrafo = totalNodos;
    this->aristasSubgrafo = igraph_ecount(subgrafo);

    igraph_vector_destroy(&membership);
    igraph_vector_destroy(&csize);

    /*
    //TRATANDO DE OBTENER ATRIBUTOS
    igraph_vector_t gtypes, vtypes, etypes;
    igraph_strvector_t gnames, vnames, enames;

    igraph_vector_init(&gtypes, 0);
      igraph_vector_init(&vtypes, 0);
      igraph_vector_init(&etypes, 0);
      igraph_strvector_init(&gnames, 0);
      igraph_strvector_init(&vnames, 0);
      igraph_strvector_init(&enames, 0);

      igraph_cattribute_list(grafo, &gnames, &gtypes, &vnames, &vtypes, &enames, &etypes);


      printf("Graph attributes: ");
      for (int i=0; i<igraph_strvector_size(&gnames); i++) {
        printf("%s (%i) ", STR(gnames, i), (int)VECTOR(gtypes)[i]);
      }
      printf("\n");
      printf("Vertex attributes: ");
      for (int i=0; i<igraph_strvector_size(&vnames); i++) {
        printf("%s (%i) ", STR(vnames, i), (int)VECTOR(vtypes)[i]);
      }
      printf("\n");
      printf("Edge attributes: ");
      for (int i=0; i<igraph_strvector_size(&enames); i++) {
        printf("%s (%i) ", STR(enames, i), (int)VECTOR(etypes)[i]);
      }
      printf("\n");*/


}

/**
 * @brief LectoraArchivo::getSubgrafo
 * Retorna igraph_t una copia exacta del subgrafo, que corresponde al componente conexo m&aacute;s grande del grafo original ingresado.
 * @return subgrafo obtenido en el proceso.
 */

igraph_t LectoraArchivo::getSubgrafo()
{
    igraph_t subgrafoObtenido;
    igraph_copy(&subgrafoObtenido, &this->subgrafo);

    return subgrafoObtenido;
}
/**
 * @brief LectoraArchivo::getNodosOriginal
 * Retorna la cantidad de nodos del grafo original.
 * @return int nodos del grafo original
 */

int LectoraArchivo::getNodosOriginal()
{
    return this->nodosOriginal;
}

/**
 * @brief LectoraArchivo::getNodosSubgrafo
 * Retorna la cantidad de nodos del subgrafo obtenido(gran componente).
 * @return int nodos del subgrafo.
 */

int LectoraArchivo::getNodosSubgrafo()
{
    return this->nodosSubgrafo;
}

/**
 * @brief LectoraArchivo::getAristasOriginal
 * Retorna la cantidad de aristas del grafo original.
 * @return int aristas del grafo original.
 */

int LectoraArchivo::getAristasOriginal()
{
    return this->aristasOriginal;
}

/**
 * @brief LectoraArchivo::getAristasSubgrafo
 * Retorna la cantidad de aristas del subgrafo obtenido(gran componente).
 * @return int aristas del subgrafo.
 */

int LectoraArchivo::getAristasSubgrafo()
{
    return this->aristasSubgrafo;
}

/**
 * @brief LectoraArchivo::getFlagArchivo
 * Retorna el valor de flagArchivo. si es FALSE existieron errores de lectura de archivo.
 * Si es TRUE no hubo errores de lectura de archivo de grafo.
 * @return bool flagArchivo.
 */

bool LectoraArchivo::getFlagArchivo()
{
    return this->flagArchivo;
}

/**
 * @brief LectoraArchivo::getFlagArchivoSolucion
 * Retorna el valor de flagArchivoSolucion. si es FALSE existieron errores en la creaci&oacute;n del archivo.
 * Si es TRUE no hubo errores de creaci&oacute;n del archivo de soluci&oacute;n.
 * @return bool flagArchivoSolucion.
 */

bool LectoraArchivo::getFlagArchivoSolucion()
{
    return this->flagArchivoSolucion;
}

/**
 * @brief LectoraArchivo::crearArchivoSolucion
 * Crea el archivo de soluci&oacute;n en formato <i>graphml</i> que contiene toda la configuraci&oacute;n de <i>clustering</i>
 * encontrada en el algoritmo.
 * @param nombreArchivo string que contiene el nombre del archivo con el que se guarda.
 * @param grafo igraph_t& que contiene el grafo que ser&aacute; guardado en el archivo.
 * @param configuracionFinal vector<unordered_set<int> > que contiene la configuraci&oacute;n final de <i>clustering</i>
 */

void LectoraArchivo::crearArchivoSolucion(string nombreArchivo, igraph_t &grafo, vector<unordered_set<int> > configuracionFinal)
{
    int clusters = configuracionFinal.size();
    int nodos = igraph_vcount(&grafo);
    vector<int> membresiaNodos(nodos);

    for(int i=0; i<nodos; i++)
    {
        membresiaNodos[i]=0;
    }


    for(int cluster=0; cluster<clusters;cluster++)
    {
        unordered_set<int> miembros = configuracionFinal[cluster];
        for(auto it=miembros.begin(); it != miembros.end(); ++it)
        {
            int nodo = *it;

            membresiaNodos[nodo]++;
            QString modularityClassTemp= "modularity-class-" + QString::number(membresiaNodos[nodo]);
            SETVAN(&grafo,modularityClassTemp.toStdString().c_str(),nodo,cluster);
            //cout<<" nodo: "<<nodo<<" cluster: "<<cluster<<endl;
        }
    }


    FILE* ofile;

    ofile=fopen(nombreArchivo.c_str(),"w");

    if(!ofile)
    {
        this->flagArchivoSolucion = false;
        return;
    }

    igraph_error_handler_t* oldhandler; //PARA MANEJAR EXCEPCIONES
     oldhandler=igraph_set_error_handler(igraph_error_handler_ignore); //No terminar el programa cuando se presenta una excepcion.
    int resultado;
    if (ofile)
    {

        resultado = igraph_write_graph_graphml(&grafo,ofile);
        if(resultado == IGRAPH_EFILE)
        {
            this->flagArchivoSolucion = false;
            return;
        }

    }
    //Cerrar archivo e iguala puntero a nullptr.
    fclose(ofile);
    ofile = nullptr;

    igraph_set_error_handler(oldhandler);

}
