#include "iostream"
#include "vector"
#include "fstream"
#include "string.h"
#include <cmath>
#include <stdexcept>
#include <unordered_map>
#include <list>
#include <chrono>
#include <algorithm>
using namespace std;

class Sector
{
    private:
        int memory_of_sector;
        int *recordPositions;
        string origin;
        int space_available;                // how many space is there respect the memory
        int size_per_register;
        int number_record;
    public:
        Sector(int _memory, int _register_size)
        {
            memory_of_sector = _memory;
            //recordPositions = new int [memory_of_sector/_register_size];
            recordPositions = new int [2];
            space_available = _memory;
            size_per_register = _register_size;
        }
        ~Sector(){delete [] recordPositions;}
        void setSector(int *positions, string _origin)
        {
            origin = _origin;
            
            //Setting positions of record in file
            recordPositions[0] = positions[0];
            recordPositions[1] = positions[1];
            
            // Set number of record
            number_record = (recordPositions[1]-recordPositions[0])+1;
 
            // setting space available 
           
            space_available = space_available - number_record*size_per_register;
           
        }

        void printRecord()
        {
            string aux;
            int size_per_register_file;
            fstream archivo("files/"+ origin +".txt",ios::in | ios::binary);

            // find the size of the first line of the file
            
            archivo.seekg(0,ios::beg);
            getline(archivo,aux);
            
            size_per_register_file = archivo.tellg();       // less final caracter
           
            size_per_register_file -=1;
            
            // Getting positions of the record in the file

            int start =  (recordPositions[0] -1) * size_per_register_file + (recordPositions[0] - 1);
            int end = recordPositions[1] * size_per_register_file + (recordPositions[1]-1);
            
            // Get data in variable
            archivo.seekg(start,ios::beg);
            char *A = new char[end - start +1 ] ;
            archivo.read(A, end - start);
            A[end - start] = '\0';
            cout<<A<<endl;

            delete [] A;
        }
        int getAvailableSpace(){return space_available;}
        int getCapacity(){return memory_of_sector;}
        int getSizeRecord(){return size_per_register;}
        int* getRecordPositions(){return recordPositions;}
        friend class Disk_Manager;
        friend class Disk;
        friend class Platter;
        friend class Surface;
        friend class Track;
        friend class Block;
        friend class nPages;

};

class Track
{
    private:
        int number_sectors;
        
        vector<Sector *> nSectors;
        
    public:
        Track(int num_Sector): number_sectors(num_Sector) 
        {
            nSectors.resize(number_sectors);
        }
        friend class Disk_Manager;
        friend class Disk;
        friend class Platter;
        friend class Surface;

        friend class Block;
        friend class nPages;

};

class Surface
{
    private:
        int number_Tracks;
        vector<Track *> nTrack;
    public:
        Surface(int num_Track):number_Tracks(num_Track)
        {
            nTrack.resize(number_Tracks);
        }
        friend class Disk_Manager;
        friend class Disk;
        friend class Platter;

        friend class Block;
        friend class nPages;
};

class Platter
{
    private:
        int number_Surfaces;
        vector<Surface *> nSurfaces;
        
    public:
        Platter():number_Surfaces(2){nSurfaces.resize(number_Surfaces);}
        friend class Disk_Manager;
        friend class Disk;

        friend class Block;
        friend class nPages;
};

class Disk
{
    private:
        int numero_plato;
        int numero_superficies_por_plato;
        int numero_pistas_por_superficie;
        int numero_sectores_por_pista;
        int memoria_por_sector;
        vector<Platter *> nPlatters;

        
        string name_Table;
        int *freeposition ;
    public:
        Disk(int nPlato, int nTrack, int nSectors, int nBytes)
        {
            numero_plato = nPlato;
            numero_superficies_por_plato=2;
            numero_pistas_por_superficie = nTrack;
            numero_sectores_por_pista = nSectors;
            memoria_por_sector = nBytes;
            nPlatters.resize(numero_plato); 
            freeposition = new int[4];           
        }
        ~Disk(){}

        void cabezal_escritura(int pPlato, int pSuperficie, int pPista, int gap_Sector, int register_size,int *positions, string origin)
        {
        
            if(!nPlatters[pPlato]){ nPlatters[pPlato] = new Platter ();}
        
            if(!nPlatters[pPlato]->nSurfaces[pSuperficie])
            {
                nPlatters[pPlato]->nSurfaces[pSuperficie]  = new Surface(numero_pistas_por_superficie);
                //nPlatters[pPlato]->nSurfaces[pSuperficie]->nTrack.resize(numero_pistas_por_superficie);
            }
            
            if (!nPlatters[pPlato]->nSurfaces[pSuperficie]->nTrack[pPista])
            {
                nPlatters[pPlato]->nSurfaces[pSuperficie]->nTrack[pPista] = new Track(numero_sectores_por_pista);
                //nPlatters[pPlato]->nSurfaces[pSuperficie]->nTrack[pPista]->nSectors.resize(numero_sectores_por_pista);
            }

            nPlatters[pPlato]->nSurfaces[pSuperficie]->nTrack[pPista]->nSectors[gap_Sector] = new Sector(memoria_por_sector, register_size);
            nPlatters[pPlato]->nSurfaces[pSuperficie]->nTrack[pPista]->nSectors[gap_Sector]->setSector( positions, origin);
    
        }
        void setFreePosition(int &pl,int &spr,int &pst,int &sc)
        {
            freeposition[0] = pl;
            freeposition[1] = spr;
            freeposition[2] = pst;    
            freeposition[3] = sc;
        }
        void contadores (int &pl,int &spr,int &pst,int &sc)
        {
           
            if(sc==numero_sectores_por_pista)
            {
                sc=0;
                pst++;
               
            }
            if(pst==numero_pistas_por_superficie)
            {
                spr++;
                pst=0;
            }
            if(spr==numero_superficies_por_plato)
            {
                pl++;
                spr=0;
            }
            
            // if(pl==numero_plato)
            // {
            //     pst++;
            //     sc==0;
            // }
        }
        void loadDataDisk(string nameArchivo, int register_size)
        {
            // variables to navigate into disk
            // p -> plato
            // s -> superficie
            // pst -> pistas
            // sc -> sectores
            int pl,spr,pst,sc;
            int *positions_data = new int[2];
            pl = spr = pst = sc=0;
            
            int *position_ofcontinuation= new int[4];    // freepositions to next safe

            int number_lines=0;         // count the total of record in the file   _____ this can be an entry data
            
            // count number of register in the file
            fstream archivo("files/"+nameArchivo+".txt", ios::in);
            string line;
            while(archivo.peek()!=EOF)
            {
                getline(archivo, line);
                number_lines++;
            }
            archivo.close();


            //check how many register could save in each sector
            int n = floor(memoria_por_sector/register_size);
            // load record to sector in order
            int start = 1;                              // represent first record, this increment 
            for (int i = n; i <= number_lines; i+=n)
            {                
                *(positions_data) = start;
                *(positions_data +1) = i;

                cabezal_escritura(pl, spr, pst, sc, register_size,positions_data, nameArchivo);
                sc++;
                contadores(pl,spr,pst,sc);
                start = i+1;

                if(i+n>number_lines)
                {
                    *(positions_data) = start;
                    *(positions_data +1) = number_lines;
                    cabezal_escritura(pl, spr, pst, sc, register_size,positions_data, nameArchivo);
                    sc++;
                    contadores(pl,spr,pst,sc);
                }
            } 
            setFreePosition(pl,spr,pst,sc);
        
            delete [] position_ofcontinuation;
            delete [] positions_data;
        }
        int* getFreePositions(){return freeposition;}
        string getNameTable(){return name_Table;}
        int getMemorySector(){return memoria_por_sector;}
        int getNumberSector(){return 1* numero_plato*2 *numero_pistas_por_superficie * numero_sectores_por_pista;}
        // represent the giro
        void print_AllData()
        {
            for (size_t i = 0; i < numero_plato; i++)
            {
                if(nPlatters[i])
                {
                    for (size_t j = 0; j < numero_superficies_por_plato; j++)
                    {
                        if(nPlatters[i]->nSurfaces[j])
                        {
                            for (size_t k = 0; k < numero_pistas_por_superficie; k++)
                            {
                                if(nPlatters[i]->nSurfaces[j]->nTrack[k])
                                {
                                    for (size_t l = 0; l < numero_sectores_por_pista; l++)
                                    {
                                        if(nPlatters[i]->nSurfaces[j]->nTrack[k]->nSectors[l])
                                            nPlatters[i]->nSurfaces[j]->nTrack[k]->nSectors[l]->printRecord();
                                    }
                                }
                                
                            }
                        }        
                    }
                } 
            }
        }
        void print_DiskInfo()
        {
            int num_discos = 1;
            cout<<"\n************************************************************************************"<<endl;
            cout<<"\tInformacion... "<<endl;
            cout<<"\t\t\t\t\tPor \tTotal"<<endl;
            cout<<"\t\tDiscos: \t\t"<<num_discos<<"\t"<<num_discos<<endl;
            cout<<"\t\tPlatos: \t\t"<<numero_plato<<"\t"<<num_discos*numero_plato<<endl;
            cout<<"\t\tSuperficies: \t\t"<<2<< "\t"<<num_discos* numero_plato*2 <<endl;
            cout<<"\t\tPistas: \t\t"<<numero_pistas_por_superficie<< "\t"<<num_discos* numero_plato*2 *numero_pistas_por_superficie<<endl;
            cout<<"\t\tSectores: \t\t"<<numero_sectores_por_pista<< "\t" <<num_discos* numero_plato*2 *numero_pistas_por_superficie * numero_sectores_por_pista <<endl;
            cout<<"\t\tMemoria (bytes): \t"<<memoria_por_sector <<"\t"<< memoria_por_sector*numero_sectores_por_pista <<endl;
            cout<<"\t\tCapacidad total: \t"<<num_discos* numero_plato*2 *numero_pistas_por_superficie * numero_sectores_por_pista  * memoria_por_sector<< " bytes"<<endl;
            
            
            // cout<<"\t\tTotal de Discos: \t1"<<endl;
            // cout<<"\t\tTotal de Platos: \t"<<numero_plato<<endl;
            // cout<<"\t\tTotal de Superficies: \t"<<numero_plato*2 <<endl;
            // cout<<"\t\tTotal de Pistas: \t"<< numero_plato*2 *numero_pistas_por_superficie<<endl;
            // cout<<"\t\tTotal de Sectores: \t"<<numero_plato*2 *numero_pistas_por_superficie * numero_sectores_por_pista <<endl;
            // cout<<"\t\tMemoria por sector: \t"<<memoria_por_sector <<" bytes"<<endl;
            // cout<<"\t\tCapacidad total: \t"<<numero_plato*2 *numero_pistas_por_superficie * numero_sectores_por_pista  * memoria_por_sector<< " bytes"<<endl;
            cout<<"\n************************************************************************************"<<endl;

        }
        
        friend class Disk_Manager;
        friend class Block;
        friend class Frame;
        friend class DATABASE;
};


class Block
{
    private:
        vector <Sector *> numSectores;
        Disk *ptrDisk;
        int i;
        int size_block;
        int *ptrPosition;
    public:
        Block(int sectors_per_block, Disk *preDis)
        {
            numSectores.resize(sectors_per_block);
            ptrDisk = preDis;
            i=0;
            size_block=0;
            ptrPosition = new int[2];
        }
  
        
        void making_Block (int pPlato, int pSuperficie, int pPista, int pSector)
        {      
            // ptrPosition[0] = pPlato;
            // ptrPosition[1] = pSuperficie;
            // ptrPosition[2] = pPista;
            // ptrPosition[3] = pSector;
            numSectores[i]=(ptrDisk->nPlatters[pPlato]->nSurfaces[pSuperficie]->nTrack[pPista]->nSectors[pSector]); 
            if(i==0)
            {
                ptrPosition[0]=ptrDisk->nPlatters[pPlato]->nSurfaces[pSuperficie]->nTrack[pPista]->nSectors[pSector]->recordPositions[0];
                ptrPosition[1]=ptrDisk->nPlatters[pPlato]->nSurfaces[pSuperficie]->nTrack[pPista]->nSectors[pSector]->recordPositions[1];
            }else
                ptrPosition[1]=ptrDisk->nPlatters[pPlato]->nSurfaces[pSuperficie]->nTrack[pPista]->nSectors[pSector]->recordPositions[1];

            int capacity_sec = (ptrDisk->nPlatters[pPlato]->nSurfaces[pSuperficie]->nTrack[pPista]->nSectors[pSector]->getCapacity());
            int available_esp = (ptrDisk->nPlatters[pPlato]->nSurfaces[pSuperficie]->nTrack[pPista]->nSectors[pSector]->getAvailableSpace());
            size_block+= (capacity_sec - available_esp);
            i++;
        }
    
        //int *¨getPosition(){return ptrPosition;}
        vector <Sector *> * getBlocks(){return &numSectores;}
        int getSizeBlock() {return size_block;}
        bool find_Register(int numberRegister)
        {
            for(auto i:numSectores){
                int *aux = i->getRecordPositions();
                if(aux[0]>=numberRegister && numberRegister <=aux[1])
                    return true;
            }
            return false;
            
        }

        void print_blockContent()
        {
            int j=0;
            int capacity=0;
            //cout<<"\n************************************************************************************"<<endl;
            cout<<"\n\tShow data saved in block\n"<<endl;
            
            for(auto i:numSectores){
              
                i->printRecord();
                cout<<endl;
                cout<<endl;
                int aux = i->getCapacity() - i->getAvailableSpace();
                j++;
                capacity+=aux;
            }

            cout<<"\n\n\t*************************************"<<endl;
            cout<<"\t\tInformation about block"<<endl;
            cout<<"\t1. Total of sector: \t"<<j<<endl;
            cout<<"\t2. Capacity: \t\t"<<capacity<<" bytes"<<endl;
           
            
            cout<<"\t***************************************"<<endl;
            
            cout<<"\n************************************************************************************"<<endl;
        }
        
        friend class Directorio;
        friend class Disk_Manager;
        friend class DATABASE;
};

class Disk_Manager
{
    private:
        int *freeposition ;
        int *scheme_conditions;
        int size_register;
        int cantidad_bloques;
        int sectores_por_bloques;
        int memory_disk;
        Disk *disk;
        vector<Block *> nPages;
    public:
        Disk_Manager(int num_secs, Disk *_disk)
        {
            disk = _disk;
            cantidad_bloques = ceil(disk->getNumberSector()/num_secs);
            sectores_por_bloques = num_secs;
            nPages.resize(cantidad_bloques);
            memory_disk=0;
            //generatePages();
        }
        void values_KeyWords( int &register_bytes, string type, string amount)
        {
            if(type=="int")
                register_bytes+=4;
            
            if(type=="str")
                register_bytes+= (stoi(amount) * 1);
            
            if(type=="float")
                register_bytes+=4;
            
            if(type=="bool")
                register_bytes+=1;
            
        }
        void generate_txt(int number, int *pesos,string file_original, string file_end)
        {
            
            ifstream archivo("files/"+file_original+".csv");
            
            fstream archivo2("files/"+file_end + ".txt",fstream::app);
            
            string header = "";
            for (int i = 0; i < number; i++)
            {
                for (int q = 0; q < pesos[i]; q++)
                {
                    header+=" ";
                }
                
            }
            //archivo2<<header<<endl;
            


            char c;
            int capacidad = 2;
            int j = 0;
            char *arreglo = new char[capacidad];
            bool header_created=false;
            while (archivo.get(c))
            {
                
                if (c == '\n')
                {
                    const char * word = "";
                    int h=0;
                    int aux = 0;
                    while(arreglo[h] != '\0'){
                        if(arreglo[h] == '#' || arreglo[h] == '\n'){
                            char *palabra = new char[pesos[aux]+1];
                            
                            strcpy(palabra,word);
                            for(int k=strlen(word);k < pesos[aux];k++){
                                strcat(palabra," ");
                            }
                        
                            // archivo2<<std::string(palabra);
                            aux++;
                            word="";
                            delete [] palabra;
                        }
                        else{
                            char *aux_copy = new char [strlen(word)+2];
                            char ss [2]= {arreglo[h],'\0'};
                            const char *ptr = ss;
                            strcpy(aux_copy,word);
                            strcat(aux_copy,ptr);
                            word = aux_copy;
                            
                        }
                        h++;
                    }
                    // archivo2<<endl;
                    capacidad = 2;
                    j = 0;
                    delete[] arreglo;
                    arreglo = new char[capacidad];
                }
                else
                {
                    arreglo[j] = c;
                    arreglo[j+1]='\0';
                    j++;
                    capacidad++;
                    arreglo = (char *)realloc(arreglo, capacidad * sizeof(char)); // redimensionamos el arreglo
                }
            }
            archivo.close();
            archivo2.close();
            cout<<"\n\t!Archivo "<<file_end<< ".txt creado con exito..."<<endl;
            
        }
        void loadDataScheme(string nameScheme, string nameTable)
        {
            ifstream scheme("files/"+nameScheme+".txt" ,fstream::in);
            
            string token;
            string aux;
            int register_size=0;
            bool opc= false;
            int n=1;
            int p = 0;
            int count =0;
            int *arreglo = (int*) malloc(n*sizeof(int));
            while (getline(scheme, token))
            {
                int position = token.find(nameTable);
                if(position != -1)
                {
                    token = token.substr(nameTable.length()+1, token.length()- nameTable.length());
                    int m=1;

                    for (size_t i = 0; i <= token.length(); i++)
                    {
                        if(token[i]=='#' || token[i]=='\0')
                        {
                            string aux2="";
                            string aux3="";
                            if(m%2==0)
                            {
                                for (size_t q = 0; q < aux.length(); q++)
                                {
                                    if(isdigit(aux[q]))
                                        aux2+=aux[q];
                                    else
                                        aux3+=aux[q];    
                                }
                                
                                values_KeyWords(register_size,aux3,aux2);
                                count++;
                                arreglo[p]=stoi(aux2);
                                p++;
			                    n++;
			                    arreglo = (int *)realloc(arreglo, n * sizeof(int)); // redimensionamos el arreglo
                                aux2.clear();
                                aux3.clear();

                            }
                            aux.clear();
                            m++;
                        }
                        else
                            aux+=token[i];
                    }
                }
                aux.clear();     
            }
            
            // save conditions of scheme
            int file_register_size=0;
            scheme_conditions = new int[count];
            for (size_t i = 0; i < count; i++)
            {
                scheme_conditions[i] = arreglo[i];
                file_register_size += arreglo[i];
            }
            
            this->size_register =register_size;
            //this->size_per_register_file = file_register_size;
            disk->name_Table = nameTable+"2";

            //Create a new register
            
            generate_txt(count, arreglo,nameTable,disk->name_Table);
            
            
            delete[]arreglo;
            scheme.close();

            // load data to disk
            
            disk->loadDataDisk(disk->name_Table, register_size);

        }

    // Functions to creata pages in disk
        void generatePages()
        {
            int auxPl=0;
            int auxSpr=0;
            int auxPst=0;
            int pSector=0;
            
            // Put each sector in block
            int sectors = 0;
            for (int i = 0; i < cantidad_bloques; i++)
            {
                cout<<"\tBloque "<<i+1<<endl;
                for (int j = 0; j < sectores_por_bloques; j++)
                {
                    disk->contadores(auxPl, auxSpr, auxPst, pSector);
                    createPage(i, auxPl, auxSpr, auxPst, pSector);
                    pSector++;
                }
            }
     
        }
        void createPage (int pos,int pPlato, int pSuperficie, int pPista, int pSector)
        {
            
            if (!nPages[pos])
            {
                nPages[pos] = new Block(sectores_por_bloques, disk);
            }
            
            if (!disk->nPlatters[pPlato] || !disk->nPlatters[pPlato]->nSurfaces[pSuperficie] || !disk->nPlatters[pPlato]->nSurfaces[pSuperficie]->nTrack[pPista])
             {
                nPages[pos] = nullptr;  
                cout<<"No existe datos en ese sector"<<endl;
                return;
             }
            
            if (disk->nPlatters[pPlato]->nSurfaces[pSuperficie]->nTrack[pPista]->nSectors[pSector])
            {
                nPages[pos]->making_Block(pPlato,pSuperficie,pPista,pSector);
                memory_disk += nPages[pos]->getSizeBlock();
            }
            else
            {
                nPages[pos] = nullptr;    
                cout<<"No existe datos en ese sector"<<endl;
            }
           
        }


    // Function complements
        void print_OneSector(int num_Sector)
        {
            int plato=0;
            int superficie=0;
            int pista=0;
            int sector=0;
            for (int i = 0; i < num_Sector-1; i++)
            {
                sector++;
                disk->contadores(plato,superficie,pista,sector);
            }


            cout<<"\n************************************************************************************"<<endl;
            cout<<"\n\tAll record in the sector "<<endl;

            if (!disk->nPlatters[plato] || !disk->nPlatters[plato]->nSurfaces[superficie] || !disk->nPlatters[plato]->nSurfaces[superficie]->nTrack[pista]) 
            {
                cout<<"No exist data in this sector"<<endl;
                cout<<"\n************************************************************************************\n"<<endl;
                return;
            };
            
            if (disk->nPlatters[plato]->nSurfaces[superficie]->nTrack[pista]->nSectors[sector])
            {
                disk->nPlatters[plato]->nSurfaces[superficie]->nTrack[pista]->nSectors[sector]->printRecord();
                
                int capacidad = disk->nPlatters[plato]->nSurfaces[superficie]->nTrack[pista]->nSectors[sector]->getCapacity();
                int size_per_Registre = disk->nPlatters[plato]->nSurfaces[superficie]->nTrack[pista]->nSectors[sector]->getSizeRecord();
                int espacio = disk->nPlatters[plato]->nSurfaces[superficie]->nTrack[pista]->nSectors[sector]->getAvailableSpace();
                int cant_regis = (capacidad-espacio)/size_per_Registre;
                cout<<"\n\n\t*************************************"<<endl;
                cout<<"\t\tInformation about sector"<<endl;
                cout<<"\t1. Disk: \t\t\t1 "<<endl;
                cout<<"\t2. Plate: \t\t\t"<<plato+1<<endl;
                cout<<"\t3. Surface: \t\t\t"<<superficie + 1<<endl;
                cout<<"\t4. Track: \t\t\t"<<pista+1<<endl;
                cout<<"\t5. Sector: \t\t\t"<<sector+1<<endl;
                cout<<"\t6. Capacity: \t\t\t"<<capacidad<<" bytes"<<endl;
                cout<<"\t5. Bytes per register: \t\t"<<size_per_Registre<<" bytes"<<endl;;
                cout<<"\t7. Number of register: \t\t"<<cant_regis<<endl;
                cout<<"\t8. Available space: \t\t\t"<<espacio<<" bytes"<<endl;
                cout<<"\t***************************************"<<endl;
            }  
            else{
                cout<<"Sector no contiene datos"<<endl;
            }            
            cout<<"\n************************************************************************************\n"<<endl;
        }
        // void getSector(int pPlato, int pSuperficie, int pPista, int gap_Sector)
        // {
        //    
        //     int auxPl=pPlato;
        //     int auxSpr=pSuperficie;
        //     int auxPst=pPista;
        //     int pSector=gap_Sector;
        //     cout<<"\n************************************************************************************"<<endl;
        //     cout<<"\n\tAll data in sector "<<endl;
        //
        //     if (!disk->nPlatters[pPlato] || !disk->nPlatters[pPlato]->nSurfaces[pSuperficie] || !disk->nPlatters[pPlato]->nSuperficies[pSuperficie]->nPistas[pPista]) 
        //     {
        //         cout<<"No exist data in this sector"<<endl;
        //         cout<<"\n************************************************************************************\n"<<endl;
        //         return;
        //     };
        //    
        //     if (disk->nPlatters[auxPl]->nSuperficies[auxSpr]->nPistas[auxPst]->nSectores[pSector])
        //     {
        //         disk->nPlatters[auxPl]->nSuperficies[auxSpr]->nPistas[auxPst]->nSectores[pSector]->getData(); 
        //        
        //         int capacidad = disk->nPlatters[auxPl]->nSuperficies[auxSpr]->nPistas[auxPst]->nSectores[pSector]->getCapacity();
        //         int size_per_Registre = disk->nPlatters[auxPl]->nSuperficies[auxSpr]->nPistas[auxPst]->nSectores[pSector]->getsize_per_register();
        //         int espacio = disk->nPlatters[auxPl]->nSuperficies[auxSpr]->nPistas[auxPst]->nSectores[pSector]->getSpace();
        //         int cant_regis = (capacidad-espacio)/size_per_Registre;
        //         cout<<"\n\n\t*************************************"<<endl;
        //         cout<<"\t\tInformation about sector"<<endl;
        //         cout<<"\t1. Disk: \t\t\t1 "<<endl;
        //         cout<<"\t2. Plate: \t\t\t"<<pPlato+1<<endl;
        //         cout<<"\t3. Surface: \t\t\t"<<pPlato+1<<endl;
        //         cout<<"\t4. Track: \t\t\t"<<pPista+1<<endl;
        //         cout<<"\t5. Sector: \t\t\t"<<pSector+1<<endl;
        //         cout<<"\t6. Capacity: \t\t\t"<<capacidad<<" bytes"<<endl;
        //         cout<<"\t5. Bytes per register: \t\t"<<size_per_Registre<<" bytes"<<endl;;
        //         cout<<"\t7. Number of register: \t\t"<<cant_regis<<endl;
        //         cout<<"\t8. State: \t\t\t"<<espacio<<" bytes available"<<endl;
        //         cout<<"\t***************************************"<<endl;
        //     }  
        //     else{
        //         cout<<"Sector no contiene datos"<<endl;
        //     }            
        //     cout<<"\n************************************************************************************\n"<<endl;
        // }

        void print_OneRegister(int n, string nameTable)
        {
            
            fstream archivo("files/"+ nameTable +".txt");
            
            // archivo.seekg(201,ios::beg);
            // char *B = new char[200 +1 ] ;            
            // archivo.read(B, 200);
            // B[200]='\0';
            string aux;
            archivo.seekg(0,ios::beg);
            getline(archivo,aux);
            
            int size_per_register_file = archivo.tellg();       // less final caracter
           
            size_per_register_file -=1;

            int start =  (n -1) * size_per_register_file + (n - 1);
            int end =  n * size_per_register_file + (n-1);
            //cout<<"ST: "<<start<<"  "<<"end: "<<end<<endl;
            archivo.seekg(start,ios::beg);
            
            char *A = new char[end - start +1 ] ;            
            archivo.read(A, end - start);
            A[end - start]='\0';


            archivo.seekg(0,ios::beg);
            //cout<<"len: "<<archivo.tellg()<<endl;
            archivo.seekg(0,ios::end);
            int length = archivo.tellg();
            
            
            cout<<"\n************************************************************************************"<<endl;
            cout<<"\tShow Register "<<n<<":\n"<<endl;
            cout<<A<<endl;
            cout<<"\n\n\t*************************************"<<endl;
            cout<<"\n\tInformation: "<<endl;
            cout<<"\t1. Size register file \t"<<size_per_register_file<<" bytes"<<endl;
            cout<<"\t2. Size register \t"<<size_register<<" bytes"<<endl;
            cout<<"\t2. Size file \t\t"<<length<<" bytes"<<endl;

            cout<<"\n\n\t*************************************"<<endl;
            cout<<"\n************************************************************************************"<<endl;

            delete []A;
            archivo.close();
        }

        void print_Block(int n)
        {
            if(n>cantidad_bloques || nPages[n-1]==nullptr)
            {
                cout<<"No existe ese bloque o no hay datos en ese bloque"<<endl;
                cout<<"************************************************************************************\n"<<endl;
                return;
            }

            if(nPages[n-1])
            {
                cout<<"\n\t\t\tBloque "<<n<<endl;
                cout<<"\t***************************************"<<endl;
                nPages[n-1]->print_blockContent();
            }
            
        }
        void print_AllBlock()
        {
            cout<<"************************************************************************************"<<endl;
            for (int i = 0; i < cantidad_bloques; i++)
            {
                cout<<"\n\t\t\tBloque "<<i+1<<endl;
                cout<<"\t***************************************"<<endl;
                nPages[i]->print_blockContent();
            }
            
        }

        void print_InfoDiskManager()
        {
            int memory = disk->getMemorySector();
            cout<<"\n**************************************************************************************\n"<<endl;
            cout<<"\t\tInformacion del Directorio"<<endl;
            cout<<"\tCantidad de bloques: "<<cantidad_bloques<<endl;
            cout<<"\tCantidad de sectores: "<<cantidad_bloques*sectores_por_bloques<<endl;
            cout<<"\tCapacidad total: "<<memory_disk<<endl;
            cout<<"\n**************************************************************************************\n"<<endl;
        }

        Block * getBlock(int blockId){return nPages[blockId-1];}

        friend class BufferManager;
        friend class Frame;
        friend class DATABASE;
};



class Frame
{
    private:
        int frameId;
        int pageId;
        bool dirtyBit;
        int pinCount;
        long long leastUsed;
        int* position;
        int *sectors;

        int finish_bucle=1;
    public:
        Frame(){}
        Frame(int nframe, int pageid, bool dirty, int pinCo, long long used)
        {
            frameId = nframe;
            pageId = pageid;
            dirtyBit = dirty;
            pinCount = pinCo;
            leastUsed = used;
            position = new int[2];
        }
        void setFrame_with_Page(int* positionRecord, string nameTable)
        {
            position[0]=positionRecord[0];
            position[1]=positionRecord[1];
            fstream frame("buffer/frame"+to_string(frameId)+".txt", ios::out);
            fstream archivo("files/"+nameTable+".txt",ios::binary| ios::in);

            // find the size of the first line of the file
            string aux;
            archivo.seekg(0,ios::beg);
            getline(archivo,aux);
            
            int size_per_register_file = archivo.tellg();       // less final caracter
           
            size_per_register_file -=1;

            
            int start =  (positionRecord[0] -1) * size_per_register_file + (positionRecord[0] - 1);
            int end =  positionRecord[1] * size_per_register_file + (positionRecord[1]-1);

            // Get data in variable
            archivo.seekg(start,ios::beg);
            char *A = new char[end - start +1 ] ;
            archivo.read(A, end - start);
            A[end - start] = '\0';
            archivo.close();


            string header="";
            for (size_t i = 0; i < size_per_register_file; i++)
            {
                header+=" ";
            }
            
            // Insert data in file frame
            frame.seekp(ios::beg);
            frame<<header<<endl;
            //frame.write(A,end - start +1 );
            frame<<A<<endl;
            //cout<<A<<endl;

            frame.close();
            
        }

        string get_RecordBuffer(int n)
        {
            fstream archivo("buffer/frame"+ to_string(frameId) +".txt",ios::in);
            
            string aux;
            archivo.seekg(0,ios::beg);
            getline(archivo,aux);
            
            int size_per_register_file = archivo.tellg();       // less final caracter
           
            size_per_register_file -=1;

            int start =  (n -1) * size_per_register_file + (n - 1);
            int end =  n * size_per_register_file + (n-1);
            //cout<<"ST: "<<start<<"  "<<"end: "<<end<<endl;
            archivo.seekg(start,ios::beg);
            
            char *A = new char[end - start +1 ] ;            
            archivo.read(A, end - start);
            A[end - start]='\0';

            archivo.seekg(0,ios::beg);
            //cout<<"len: "<<archivo.tellg()<<endl;
            archivo.seekg(0,ios::end);
            int length = archivo.tellg();
            
            delete []A;
            archivo.close();
            return A;

        }
        

        // codigo para modificaciones
        void reemplazar(int frameId, int num_reg, string nueva_linea)
        {
            fstream archivo("buffer/frame"+ to_string(frameId) +".txt", ios::in | ios::out | ios::binary); // Abre el archivo en modo lectura y escritura

            size_t lineaModificar = num_reg; // Índice de la línea a modificar (empezando desde 0)
            size_t posicionInicio = 0;
            size_t longitudLinea = 0;
            string linea;

            // Busca la posición de inicio y la longitud de la línea a modificar
            for (size_t i = 0; i <= lineaModificar; ++i)
            {
                posicionInicio = archivo.tellg(); // Obtiene la posición actual en el archivo
                if (!getline(archivo, linea))
                {
                    cout << "La línea especificada no existe." << endl;
                    finish_bucle++;

                }
                if(finish_bucle==10)
                    return;
                longitudLinea = linea.length() + 1; // +1 para contar el carácter de nueva línea ('\n')
            }
            // Modifica la línea deseada
            string nuevaLinea = nueva_linea;
            for (int j = nuevaLinea.length(); j < longitudLinea - 2; j++)
            {
                nuevaLinea += ' ';
            }
            // nuevaLinea += '\n';
            archivo.seekp(posicionInicio);
            archivo.write(nuevaLinea.c_str(), nuevaLinea.length()); // Escribe la nueva línea en el archivo

            archivo.close(); // Cierra el archivo
        }



        bool findRecord(int frameId, int recordID)
        {
            
            cout<<"HOMUOO: "<<recordID<<":->"<<position[0]<<":"<<position[1]<<endl;
            
            string value_to_find = to_string(recordID);
            int size_of_valueFind = 5;
            int start_of_valueFind = 0;
            fstream archivo("buffer/frame"+ to_string(frameId) +".txt");
            archivo.seekg(0, ios::beg);
            string temp1;
            getline(archivo, temp1);
            int size_ofLinesBytes = archivo.tellg();
            
            //cout<<"tamnio linea: "<<size_ofLinesBytes<<endl;
            archivo.seekg(0, ios::beg);
            int lines_counter=0;
            char *value = new char[size_of_valueFind+1];
            while (true)
            {
                if(archivo.eof()) break;
                lines_counter++;
                archivo.seekg(start_of_valueFind,ios::beg);
                archivo.read(value,size_of_valueFind);
                value[size_of_valueFind]='\0';
               // cout<<"->"<<value<<":"<<lines_counter<<endl;
                start_of_valueFind+=size_ofLinesBytes;

                //find register to delete
                string temp2(value);
                temp2.erase(std::remove_if(temp2.begin(), temp2.end(), [](char c) { return std::isspace(c); }), temp2.end());

                if(temp2==value_to_find)
                {
                    cout<<temp2<<" eCONR: "<<endl;
                    delete_RecordBuffer(frameId, recordID, lines_counter);
                    return true;
                }
            }
            return false;
        }
        void delete_RecordBuffer(int frameId, int recordID, int lines_counter)
        {
            fstream archivo("buffer/frame"+ to_string(frameId) +".txt");
            int temp = 0;
            
            // Getting the header of file and delete spaces
            archivo.seekg(0,ios::beg);
            string linea;
            getline(archivo, linea);
            linea.erase(remove(linea.begin(), linea.end(), ' '), linea.end());
            int inicio = 0;     // start of pointer of delete file

            // Validate if the header is empty(exist a delete register)
            if (linea.size() > 0)
            {
                temp = 1;
                inicio = stoi(linea);
            }
           // cout<<"iNICIO FIRST: "<<inicio<<endl;


            size_t pos_inicio;
            while (temp)                                // existe a delete record
            {
                //cout<<"ENTRA A MI BUCLE:::"<<endl;
                string aux;
                archivo.seekg(0, ios::beg);
                for (int i = 0; i < inicio - position[0]+2; i++)
                {
                    pos_inicio = archivo.tellg();
                    getline(archivo, aux);
                }
                //getline(archivo, aux);
                aux.erase(remove(aux.begin(), aux.end(), ' '), aux.end());
               // cout<<"IMPRIMIR AUX: "<<aux<<endl;
                if (aux.size() > 0)
                {
                    inicio = stoi(aux)- position[0] +2; //- inicio + 1;
                   // cout<<"\tInico dentro: "<<inicio<<endl;
                }
                else
                {
                   // cout<<"LINEA VAIICA :"<<inicio<<endl;
                    inicio= inicio - position[0]+1;
                    temp = 0;
                }
            }
            
            //cout<<"Inicio:file: "<<inicio<<"->"<<lines_counter<<endl;
            reemplazar(frameId, inicio, to_string(position[0] + lines_counter -2));      // replace to the position of the empty espace
            reemplazar(frameId, lines_counter-1, "");                     // replace with spaces
            
        }


        void insert_RecordBuffer(int frameId, string data)
        {
            ifstream archivo("buffer/frame"+ to_string(frameId) +".txt");
            int temp = 0;
            string linea;
            getline(archivo, linea);
            linea.erase(remove(linea.begin(), linea.end(), ' '), linea.end());
            int inicio = 0;
            if (linea.size() > 0)
            {
                temp = 1;
                inicio = stoi(linea);
            }
            else{
                ofstream archivo2("buffer/frame"+ to_string(frameId) +".txt",ios::app);
                archivo2<<data<<endl;   
            }

            string aux;
            archivo.seekg(0, ios::beg);
            for (int i = 0; i < inicio; i++)
            {
                getline(archivo, aux);
            }
            getline(archivo, aux);
            aux.erase(remove(aux.begin(), aux.end(), ' '), aux.end());

            // reemplazar(frameId, inicio, data);
            // reemplazar(frameId, 0, aux);

        }

        void swap_RecordBuffer(int frameId, int reg1, int reg2){
            ifstream archivo("buffer/frame"+ to_string(frameId) +".txt");
            string temp1,temp2;

            archivo.seekg(0, ios::beg);
            for (int i = 0; i < reg1; i++)
            {
                getline(archivo, temp1);
            }
            getline(archivo, temp1);

            archivo.seekg(0, ios::beg);
            for (int i = 0; i < reg2; i++)
            {
                getline(archivo, temp2);
            }
            getline(archivo, temp2);

            // reemplazar(frameId,reg1,temp2);
            // reemplazar(frameId,reg2,temp1);
        }
        
        void reemplazar_DiksFile(string file_name, int num_reg, string nueva_linea)
        {

            fstream archivo("files/"+file_name+".txt", ios::in | ios::out | ios::binary); // Abre el archivo en modo lectura y escritura

            size_t lineaModificar = num_reg; // Índice de la línea a modificar (empezando desde 0)
            size_t posicionInicio = 0;
            size_t longitudLinea = 0;
            string linea;

            // Busca la posición de inicio y la longitud de la línea a modificar
            for (size_t i = 0; i <= lineaModificar; ++i)
            {
                posicionInicio = archivo.tellg(); // Obtiene la posición actual en el archivo
                if (!getline(archivo, linea))
                {
                    cout << "La línea especificada no existe." << endl;
                }
                longitudLinea = linea.length() + 1; // +1 para contar el carácter de nueva línea ('\n')
            }
            // Modifica la línea deseada
            string nuevaLinea = nueva_linea;
            for (int j = nuevaLinea.length(); j < longitudLinea - 2; j++)
            {
                nuevaLinea += ' ';
            }
            // nuevaLinea += '\n';
            archivo.seekp(posicionInicio);
            archivo.write(nuevaLinea.c_str(), nuevaLinea.length()); // Escribe la nueva línea en el archivo

            archivo.close(); // Cierra el archivo
        }
        void save_Disk(string nameFile, int frameId)
        {
            cout<<"Frame"<<to_string(frameId)<<endl;
            fstream archivo("buffer/frame"+ to_string(frameId) +".txt",  ios::in|ios::out | ios::binary); // Abre el archivo en modo lectura y escritura
            string aux;
            archivo.seekg(0, ios::beg);
            getline(archivo, aux);
            int size_file_register = archivo.tellg();
            string header = aux;
            string texto_in="";

            while (getline(archivo, aux)) {
                // Procesar la línea
            
                // Verificar si se ha llegado al final del archivo
                //cout<<aux<<endl;
                if (archivo.eof()) {
                    std::cout << "Se ha llegado al final del archivo." << std::endl;
                    break;
                }
                texto_in+=(aux+'\n');
            }

            int cantidalineas = position[1]- position[0] + 1;
            char *text =new char[cantidalineas*size_file_register];
            archivo.read(text, cantidalineas*size_file_register);
            archivo.close();


            fstream archivo2("files/"+ nameFile +".txt", ios::in | ios::out | ios::binary); // Abre el archivo en modo lectura y escritura

            
            size_t lineaModificar = position[0]-1; // Índice de la línea a modificar (empezando desde 0)
            size_t posicionInicio = 0;
            size_t longitudLinea = 0;
            string linea;
            cout<<"______________________________________________________________________________________________________________________________________________________________________"<<endl;
            cout<<texto_in<<endl;
            cout<<"______________________________________________________________________________________________________________________________________________________________________"<<endl;
            cout<<"\t\tName: "<<nameFile<<endl;
            cout<<"\t\tCantidad lineas: "<<cantidalineas<<endl;
            cout<<"\t\tTmanio:: "<<size_file_register<<endl;
            cout<<"\t\tLinea a modificar: "<<lineaModificar<<endl;
            cout<<"\t\tHeader: "<<header<<endl;
            
            // Busca la ultima linea vacia para poner el puntero (header del frame)
            int temp = 0;
            string lineaTemp;
            getline(archivo2, lineaTemp);
            lineaTemp.erase(remove(lineaTemp.begin(), lineaTemp.end(), ' '), lineaTemp.end());
            int inicio = 0;
            if (lineaTemp.size() > 0)
            {
                temp = 1;
                inicio = stoi(lineaTemp);
            }
            size_t pos_inicio;
            while (temp)
            {
                string aux;
                archivo2.seekg(0, ios::beg);
                for (int i = 0; i < inicio; i++)
                {
                    pos_inicio = archivo2.tellg();
                    getline(archivo2, aux);
                }
                getline(archivo2, aux);
                aux.erase(remove(aux.begin(), aux.end(), ' '), aux.end());
                if (aux.size() > 0)
                {
                    inicio = stoi(aux); //- inicio + 1;
                }
                else
                {
                    temp = 0;
                }
            }

            for (int k = 0; k < size_file_register-1; k++)
            {
                if(header.size()!=size_file_register-1)
                {
                    header+=" ";
                }
            }
            header+='\n';
            cout<<"\tInico: "<<inicio<<endl;
            //archivo2.write(header.c_str(),header.length());
            reemplazar_DiksFile(nameFile,inicio,header);
            
            // Busca la posición de inicio y la longitud de la línea a modificar
            archivo2.clear();
            archivo2.seekg(0,ios::beg);
            for (size_t i = 0; i <= lineaModificar; ++i)
            {
                posicionInicio = archivo2.tellg(); // Obtiene la posición actual en el archivo
                if (!getline(archivo2, linea))
                {
                    cout << "La línea especificada no existe." << endl;
                    finish_bucle++;

                }
                if(finish_bucle==10)
                    return;
                longitudLinea = linea.length() + 1; // +1 para contar el carácter de nueva línea ('\n')
            }
           
            archivo2.seekp(posicionInicio);
            archivo2.write(texto_in.c_str(), texto_in.length()); // Escribe la nueva línea en el archivo

            archivo2.close(); // Cierra el archivo
        }
        friend class BufferManager;
        friend class DATABASE;
};

class BufferManager {
private:
    std::unordered_map<int, std::list<Frame>::iterator> frameMap;
    std::list<Frame> bufferPool;
    int bufferSize;
    //Disk_Manager *DM;
    bool empty;
    int nextFrameId;
public:
    BufferManager(int size) : bufferSize(size), empty(true), nextFrameId(0){}
    
    bool readRecord(int pageId, int record, int *data, string nameTable, string &strRecord, Disk_Manager* DM) {
          
        //cout<<"entra qui"<<endl;
        cout<<"**********"<<endl;
        cout<<"**********"<<endl;
        cout<<"**********"<<endl;
        if (frameMap.find(pageId) != frameMap.end()) {
            // La página ya está en el búfer, se actualiza su posición en el minheap y se incrementa el pin count
            auto frameIter = frameMap[pageId];
            frameIter->leastUsed = getCurrentTimestamp();
            frameIter->pinCount++;
            
            // reading

            cout<<"_____________________________________________________________________________________"<<endl;
            cout<<"RECORD: "<<record<<"->"<<frameIter->position[0]<<":"<<frameIter->position[1]<<endl;
            cout<<endl;
            std::cout << "Reading Page " << pageId << " from buffer." << std::endl;
            int start = frameIter->position[0];
            int end = frameIter->position[1];
            if(start >=record && record <= end)
            {
                int position_Frame = (record-start + 1);
                strRecord = frameIter->get_RecordBuffer(position_Frame);
                return true;
            }

        } else {
            // La página no está en el búfer, se carga desde el almacenamiento secundario
            cout<<frameMap.size()<<"::"<<bufferSize<<endl;
            if (frameMap.size() == bufferSize) {
                // El búfer está lleno, se debe hacer espacio
                evictPageLRU(DM);
            }
            // Se crea un nuevo frame para la página y se agrega al búfer
            Frame newFrame(getNextFrameId(), pageId, false, 1, getCurrentTimestamp());
            newFrame.setFrame_with_Page(data,nameTable);
            bufferPool.push_front(newFrame);
            frameMap[pageId] = bufferPool.begin();
            auto frameIter = frameMap[pageId];

            cout<<"_____________________________________________________________________________________"<<endl;
            cout<<"RECORD: "<<record<<"->"<<frameIter->position[0]<<":"<<frameIter->position[1]<<endl;
            cout<<endl;
            std::cout << "Reading Page " << pageId << " from storage." << std::endl;
            int start = frameIter->position[0];
            int ending = frameIter->position[1];
            
           // cout<<"\n"<<start<<":"<<ending<<"->"<<record<< endl<<endl;
            if(record >=start && record <= ending)
            {
                cout<<"REGISTRO ENCONTRADO: "<<(record-start + 1)<<endl;
                int position_Frame = (record-start + 1);
                strRecord = frameIter->get_RecordBuffer(position_Frame);
                
                cout<<"\n\nREGISTRO ENCONTRADO"<<endl;
                return true;

            }
        }
        return false;
    }

   

    
    bool writePage_Delete(int pageId, int record, int *data, string nameTable, Disk_Manager * DM) {
        cout<<"**********"<<endl;
        cout<<"**********"<<endl;
        cout<<"**********"<<endl;
        if (frameMap.find(pageId) != frameMap.end()) {
            auto frameIter = frameMap[pageId];
            //frameIter->dirtyBit = true;
            frameIter->leastUsed = getCurrentTimestamp();
            frameIter->pinCount++;
            
            cout<<"_____________________________________________________________________________________"<<endl;
            cout<<"RECORD: "<<record<<"->"<<frameIter->position[0]<<":"<<frameIter->position[1]<<endl;
            cout<<endl;
            std::cout << "Writing Page " << pageId << " to buffer." << std::endl;
            int start = frameIter->position[0];
            int end = frameIter->position[1];
             if(frameIter->findRecord(frameIter->frameId, record))
            {
                cout<<"REGISTRO ENCONTRADO: "<<(record-start + 1)<<endl;
                int position_Frame = (record-start + 1);
                //frameIter->delete_RecordBuffer(frameIter->frameId, position_Frame, record);
                frameIter->dirtyBit=true;
                return true;

            }
           
            
        } else {
            if (frameMap.size() == bufferSize) {
                evictPageLRU(DM);
            }
            Frame newFrame(getNextFrameId(), pageId, false, 1, getCurrentTimestamp());
            newFrame.setFrame_with_Page(data,nameTable);
            bufferPool.push_front(newFrame);
            frameMap[pageId] = bufferPool.begin();
            auto frameIter = frameMap[pageId];

            cout<<"_____________________________________________________________________________________"<<endl;
            cout<<"RECORD: "<<record<<"->"<<frameIter->position[0]<<":"<<frameIter->position[1]<<endl;
            cout<<endl;
            std::cout << "Writing Page " << pageId << " to STORE." << std::endl;
            int start = frameIter->position[0];
            int ending = frameIter->position[1];
            
           // cout<<start<<" : "<<ending<<endl;
            cout<<"\n"<<start<<":"<<ending<<"->"<<record<< endl<<endl;
            if(frameIter->findRecord(frameIter->frameId, record))
            {
                cout<<"REGISTRO ENCONTRADO: "<<(record-start + 1)<<endl;
                int position_Frame = (record-start + 1);
                //frameIter->delete_RecordBuffer(frameIter->frameId, position_Frame, record);
                frameIter->dirtyBit=true;
                return true;

            }
            

        }
        return false;
    }


    // bool writePage_Insert(int pageId, int record, int *data, string nameTable) {
    //     if (frameMap.find(pageId) != frameMap.end()) {
    //         auto frameIter = frameMap[pageId];
    //         frameIter->dirtyBit = true;
    //         frameIter->leastUsed = getCurrentTimestamp();
    //         frameIter->pinCount++;
    //        
    //         std::cout << "Writing Page " << pageId << " to buffer." << std::endl;
    //         int start = frameIter->position[0];
    //         int end = frameIter->position[1];
    //         if(start >=record && record <= end)
    //         {
    //
    //             frameIter->delete_RecordBuffer(frameIter->frameId, record);
    //             return true;
    //         }
    //        
    //     } else {
    //         if (frameMap.size() == bufferSize) {
    //             evictPageLRU();
    //         }
    //         Frame newFrame(getNextFrameId(), pageId, false, 1, getCurrentTimestamp());
    //         newFrame.setFrame_with_Page(data,nameTable);
    //         bufferPool.push_front(newFrame);
    //         frameMap[pageId] = bufferPool.begin();
    //         auto frameIter = frameMap[pageId];
//
    //         std::cout << "Writing Page " << pageId << " to buffer." << std::endl;
    //         int start = frameIter->position[0];
    //         int ending = frameIter->position[1];
    //      
    //         cout<<"\n"<<start<<":"<<ending<<"->"<<record<< endl<<endl;
    //         if(record >=start && record <= ending)
    //         {
    //             cout<<"REGISTRO ENCONTRADO: "<<(record-start + 1)<<endl;
    //             frameIter->delete_RecordBuffer(frameIter->frameId, record);
    //             return true;
    //
    //         }
    //     }
    //     return false;
    // }


    void pinPage(int pageId) {
        if (frameMap.find(pageId) != frameMap.end()) {
            auto frameIter = frameMap[pageId];
            frameIter->pinCount++;
            std::cout << "Pinning Page " << pageId << "." << std::endl;
        } else {
            std::cout << "Page " << pageId << " not found in buffer." << std::endl;
        }
    }

    void unpinPage(int pageId) {
        if (frameMap.find(pageId) != frameMap.end()) {
            auto frameIter = frameMap[pageId];
            if (frameIter->pinCount > 0) {
                frameIter->pinCount--;
            }
            std::cout << "Unpinning Page " << pageId << "." << std::endl;
        } else {
            std::cout << "Page " << pageId << " not found in buffer." << std::endl;
        }
    }
    bool IsEmptyBuffer()
    {
        return (bufferPool.size() < bufferSize)? true: false;
    }
    friend class DATABASE;
private:
    long long getCurrentTimestamp() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
    }

    int getNextFrameId() {
        nextFrameId++;
        if(nextFrameId>bufferSize)
            nextFrameId = 1;
        return nextFrameId;
    }

    void evictPageLRU(Disk_Manager* DM) {
        
        auto lruFrameIter = std::min_element(
            bufferPool.begin(), bufferPool.end(),
            [](const Frame& a, const Frame& b) {
                if (a.pinCount == 0 && b.pinCount == 0) {
                    return a.leastUsed < b.leastUsed;
                } else if (a.pinCount == 0) {
                    return true;
                } else if (b.pinCount == 0) {
                    return false;
                }
                return false;
            }
        );

        Frame lruFrame = *lruFrameIter;
        cout<<"_____________________________________________________________________________________"<<endl;
        cout<<"RECORD: "<<lruFrame.frameId +1 <<"->"<<lruFrameIter->position[0]<<":"<<lruFrameIter->position[1]<<"()"<<lruFrameIter->dirtyBit<<endl;
        cout<<endl;
        std::cout << "\n\nEvicting Page " << lruFrame.pageId << " from buffer." << std::endl;
        if(lruFrameIter->dirtyBit)
        {
            cout<<"SAVE IN DISK"<<endl;

            int *positions_of_frameFiles = lruFrameIter->position;
            lruFrameIter->save_Disk(DM->disk->getNameTable(), lruFrameIter->frameId);
            cout<<"REEMPLAZODO"<<endl;
        }
        
        bufferPool.erase(lruFrameIter);
        frameMap.erase(lruFrame.pageId);
        
        
       // cout<<bufferPool.size()<<endl;
        

    }
};


class DATABASE
{
    private:
        Disk_Manager *DM;
        BufferManager *BM;

    public:
        DATABASE(Disk_Manager *dm, BufferManager* bm)
        {
            DM = dm;
            BM = bm;
        }
        void sql_Request(int recordNum)
        {
            string record;
            list<Frame>::iterator it;
            if(BM->IsEmptyBuffer())
            {
                for (int i = 0; i < DM->cantidad_bloques; i++)
                {
                    if(BM->readRecord(i, recordNum, DM->nPages[i]->ptrPosition, DM->disk->getNameTable(), record, DM))
                    {                        
                        break;
                    }
                    BM->unpinPage(i);
                    
                }
            }else{
                cout<<"pipasidfapsidfpaifpadsifadpsifoadsifásifásifdásidfásfas´dfiasdfia´sidfasífasdfasfasdf"<<endl;
                int size = BM->bufferSize;
                bool terminarBucles = false;
                for (int i = 0; i < DM->cantidad_bloques && !terminarBucles; i++)
                {
                    for (it = BM->bufferPool.begin(); it != BM->bufferPool.end(); ++it)
                    {
                        // if (*it=)
                        // {
                        //     /* code */
                        // }
                        cout<<"eNTRAIF: "<<endl;
                        cout<<"eNTRAIF2: "<<(it->frameId)<<endl;
                        if((*it).pageId !=i )
                        {
                                cout<<"ada32::"<<i<<endl;
                            if(BM->readRecord(i, recordNum, DM->nPages[i]->ptrPosition, DM->disk->getNameTable(), record, DM))
                            {
                                cout<<"adaTRUE"<<endl;

                                terminarBucles = true;
                                break;
                            }
                                cout<<"ada"<<endl;
                                BM->unpinPage(i);
                            
                        }
                    }   
                }
            }

            cout<<"DATABASE: "<<record<<endl;
        }

        void sql_Request_Delete(int recordNum)
        {
            string record;
            list<Frame>::iterator it;
            if(BM->IsEmptyBuffer())
            {
                for (int i = 0; i < DM->cantidad_bloques; i++)
                {
                    
                    if( BM->writePage_Delete(i, recordNum, DM->nPages[i]->ptrPosition, DM->disk->getNameTable(), DM))
                    {  
                        BM->unpinPage(i);
                        break;
                    }
                    
                }
            }else{
                cout<<"\nEntraadad"<<endl;
                int size = BM->bufferSize;
                bool terminarBucles = false;
                for (int i = 0; i < DM->cantidad_bloques && !terminarBucles; i++)
                {
                    for (it = BM->bufferPool.begin(); it != BM->bufferPool.end() && !terminarBucles; ++it)
                    {
                        if((*it).pageId !=i )
                        {
                            if( BM->writePage_Delete(i, recordNum, DM->nPages[i]->ptrPosition, DM->disk->getNameTable(),  DM))
                            {

                                BM->unpinPage(i);
                                terminarBucles = true;
                            }
                            
                        }
                    }   
                }
            }

            cout<<"Registro eliminado!"<<endl;

        }

        void sql_Request_Insert(int recordNum)
        {
            string record;
            list<Frame>::iterator it;
            if(BM->empty)
            {
                for (int i = 0; i < DM->cantidad_bloques; i++)
                {
                    
                    if( BM->writePage_Delete(i, recordNum, DM->nPages[i]->ptrPosition, DM->disk->getNameTable(), DM))
                    {  
                        break;
                    }
                    BM->unpinPage(i);
                    
                }
            }else{
                int size = BM->bufferSize;
                bool terminarBucles = false;
                for (int i = 0; i < DM->cantidad_bloques && !terminarBucles; i++)
                {
                    for (it = BM->bufferPool.begin(); it != BM->bufferPool.end() && !terminarBucles; ++it)
                    {
                        if((*it).pageId !=i )
                        {
                            if( BM->writePage_Delete(i, recordNum, DM->nPages[i]->ptrPosition, DM->disk->getNameTable(), DM))
                                terminarBucles = true;
                            
                            BM->unpinPage(i);
                        }
                    }   
                }
            }

            cout<<"Registro Insertado!"<<endl;
        }
};

/*
_________________________________________________________________________________________________

*/


void menu()
{
    cout<<"\n**************************************************************************************\n"<<endl;
}


//Get info disk
void opcion_1(Disk *ptrDisk)
{
    ptrDisk->print_DiskInfo();
}

// Register
void opcion_2(Disk_Manager *ptrDiskManager, Disk *disk)
{
    menu();
    string nameTable = disk->getNameTable();
    int n;
    cout<<"\tNumero de registro: ";cin>>n; 
    cout<<"dd: "<<n<<" "<<nameTable<<endl;
    ptrDiskManager->print_OneRegister(n,nameTable);
}

// get Sector
void opcion_3(Disk_Manager *ptrDiskManager)
{
    menu();
    
    int n;
    cout<<"Sector a imprimir: ";cin>>n;
    ptrDiskManager->print_OneSector(n);
}

//making Directotio

void opcion_4( Disk_Manager *directorios)
{
    menu();
    
    cout<<"\n*************************************"<<endl;   
    cout<<"\tCreation of directory complete!"<<endl;
    int ops;
    while (true)
    {
        cout<<"\t\t1. Mostrar DATA de Bloque"<<endl;
        cout<<"\t\t2. Mostrar DATA del Directorio"<<endl;
        cout<<"\t\t3. Informarcio"<<endl;
        cout<<"\t\t4. Salir"<<endl;
        cout<<"\t\t Opc: ";cin>>ops;
        
        if(ops==1) 
        {
            int aux;
            cout<<"\t\t\tNumero de Bloque: ";cin>>aux;
            directorios->print_Block(aux);
        }
        else if(ops==2)
        {
            directorios->print_AllBlock();
        }
        else if(ops==3)
        {
            directorios->print_InfoDiskManager();
        }
        else break;
    }
    

}

void menu_opciones(Disk *ptrDisco, Disk_Manager *directorios)
{
    int opc;
    
    while (true)
    {
        cout<<"WELCOME TO MEGAST\n"<<endl;
        cout<<"Opciones que desea realizar"<<endl;
        cout<<"\t1. Obtener informacion del disco"<<endl;
        cout<<"\t2. Imprimir un registro"<<endl;
        cout<<"\t3. Imprimir datos de un SECTOR"<<endl;
        cout<<"\t4. Opciones de Directorio"<<endl;
        cout<<"\t5. Salir"<<endl;
        cout<<"\topc (1-5)?: ";cin>>opc;
        
        if(opc==1) opcion_1(ptrDisco);
        else if(opc==2) opcion_2(directorios, ptrDisco);
        else if(opc==3) opcion_3(directorios);
        else if(opc==4) opcion_4(directorios);
        else if(opc==5) return;
        menu();
    }
    
}



int main()
{
    
//     //692 -> 4r  173 registro
//     // se almacena 20 registros por sector
//     // 45 sectores necesarios pa todo
//     // pistas : 2 5 5 -> 3460
//     // 5 sectores por bloque (9)

    
    string nameTable = "titanic";

    Disk disco(1, 5, 5, 3460);  //5 register per sector
    Disk_Manager directorios(5, &disco);
    BufferManager buffer(3);
    //cout<<nameTable<<endl;
    directorios.loadDataScheme("scheme",nameTable.c_str());
    directorios.generatePages();
    cout<<"\tDatos subidos con exito al disco"<<endl;


    // make a request: queiro registro 20
    DATABASE db(&directorios,&buffer);
    //db.sql_Request(424);
    db.sql_Request_Delete(203);
    db.sql_Request_Delete(213);
    db.sql_Request(313);
    db.sql_Request(413);
    db.sql_Request(713);
    db.sql_Request(113);
   

   

}

// int main()
// {
//     int plato;
//     int pista;
//     int sector;
//     int memoria;
//     string nameTable;
//     //692 -> 4r  173 registro
//     // se almacena 20 registros por sector
//     // 45 sectores necesarios pa todo
//     // pistas : 2 5 5 -> 3460
//     // 5 sectores por bloque (9)


//     menu();
    
//     //string nameTable = "titanic";
//     // Disco disco(4, 10, 200, 692);
//     // disco.loadDataScheme("scheme",nameTable.c_str());
//     // disco.getOneRegister(891,"titanic2");
//     // disco.getDisk_info();
//     // disco.getSector(0,0,0,1);
//     // opcion_4(&disco);
//     int num_blocks;
//     int num_sec_per_blocks;
//     cout<<"\tDisk setting"<<endl;
//     cout<<"\tPlato: ";cin>>plato;
//     cout<<"\tPista: ";cin>>pista;
//     cout<<"\tSector: ";cin>>sector;
//     cout<<"\tMemoria: ";cin>>memoria;
//     cout<<"Making a directory..."<<endl;
//     //cout<<"\tNumero de bloques: ";cin>>num_blocks;
//     cout<<"\tNumero de sectores por bloque: ";cin>>num_sec_per_blocks;
//     cout<<"\nInserta nombre del archivo: ";
//     cin.ignore();
//     getline(cin,nameTable);
//     cout<<"Making new file with format"<<endl;
//     cout<<"Wait... "<<endl;

//     Disk disco(plato, pista, sector, memoria);  //5 register per sector
//     Disk_Manager directorios(num_sec_per_blocks, &disco);
//     //cout<<nameTable<<endl;
//     directorios.loadDataScheme("scheme",nameTable.c_str());
//     directorios.generatePages();
//     cout<<"\tDatos subidos con exito al disco"<<endl;
//     menu();

//     menu_opciones(&disco,&directorios );

    
//     return 0;
// }