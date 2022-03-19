#include "fdisk.h"
#include <bits/stdc++.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <cstring>
using namespace std;


#include "obmkdisk.h"
#include "structs.h"
#include <QTextStream>
#include <exception>
#include <list>
#include <string>
#include <iostream>
#include <QVariant>
#include <fstream>



fdisk::fdisk()
{
}

bool particiones_vacias(mbr mbr_recuperado){
    bool vacias = false;
    if(mbr_recuperado.mbr_partitions[0].part_status == '0' &&
            mbr_recuperado.mbr_partitions[1].part_status == '0' &&
            mbr_recuperado.mbr_partitions[2].part_status == '0' &&
            mbr_recuperado.mbr_partitions[3].part_status == '0' ){
        vacias = true;
    }
    return vacias;
}

QString determinateUnity(QString unity){
    QString u ="" ;
    if(unity.isNull()){
        u = "k";
    }else if (unity == "b" || unity == "B"){
        u = "b";
    }else if (unity == "m" || unity == "M"){
        u = "m";
    }else if (unity == "k" || unity == "K"){
        u = "k";
    }else{
        printf("(-u)Error debe ingresar un valor de unidad correcto (k, b , m)");
    }
    return u;
}

QString determinate_type(QString type){
    QString ty ="" ;
    if(type.isNull()){
        ty = "p";
    }else if (type == "p" || type == "P"){
        ty = "p";
    }else if (type == "e" || type == "E"){
        ty = "e";
    }else if (type == "l" || type == "L"){
        ty = "l";
    }else{
        printf("(-type)Error debe ingresar un valor de tipo correcto (P, E , L)");
    }
    return ty;
}

int * dimensiones_particion (mbr mbr_obtenido,int size){
    static int dimensinones[3];
    int p1_fin = mbr_obtenido.mbr_partitions[0].part_start +mbr_obtenido.mbr_partitions[0].part_size;
    int p2_fin = mbr_obtenido.mbr_partitions[1].part_start +mbr_obtenido.mbr_partitions[1].part_size;
    int p3_fin = mbr_obtenido.mbr_partitions[2].part_start +  mbr_obtenido.mbr_partitions[2].part_size;

    //RETORNA UNA PARTICION SI ALGUNA DE ESTAS ES UNA PARTICION VIRGEN
    if(mbr_obtenido.mbr_partitions[0].part_size == -1 || mbr_obtenido.mbr_partitions[1].part_size == -1
            ||mbr_obtenido.mbr_partitions[2].part_size == -1 ||mbr_obtenido.mbr_partitions[3].part_size == -1){
        printf("entre como si no tuviera particiones hechas");
        if( mbr_obtenido.mbr_partitions[0].part_size == -1  && mbr_obtenido.mbr_partitions[0].part_status =='0'  ){
            dimensinones[0] = 0;
            dimensinones[1] = size + sizeof(mbr_obtenido);
            dimensinones[2] = sizeof (mbr_obtenido) +1;
        }else if( mbr_obtenido.mbr_partitions[1].part_size == -1  && mbr_obtenido.mbr_partitions[1].part_status =='0'  ){
            dimensinones[0] = 1;
            dimensinones[1] = mbr_obtenido.mbr_partitions[0].part_size
                             +size
                             + sizeof (mbr_obtenido);
            dimensinones[2] = p1_fin +1 ;

        }else if( mbr_obtenido.mbr_partitions[2].part_size == -1  && mbr_obtenido.mbr_partitions[2].part_status =='0'  ){
            dimensinones[0] = 2;
            dimensinones[1] =  mbr_obtenido.mbr_partitions[0].part_size
                              +mbr_obtenido.mbr_partitions[1].part_size
                              +  size
                              + sizeof (mbr_obtenido);
            dimensinones[2] = p2_fin +1 ;

        }else if( mbr_obtenido.mbr_partitions[3].part_size == -1  && mbr_obtenido.mbr_partitions[3].part_status =='0'  ){
            dimensinones[0] = 3;
            dimensinones[1] = mbr_obtenido.mbr_partitions[0].part_size
                             +mbr_obtenido.mbr_partitions[1].part_size
                             +mbr_obtenido.mbr_partitions[2].part_size
                             + size + sizeof (mbr_obtenido);
            dimensinones[2] = p3_fin +1 ;

        }else{
            printf("(partition) no hay espacio para ingresar su particion \n");
            dimensinones[0] = -1;
        }
    }else{
        int size_usada = mbr_obtenido.mbr_partitions[0].part_size
         +mbr_obtenido.mbr_partitions[1].part_size
        +  mbr_obtenido.mbr_partitions[2].part_size
         + mbr_obtenido.mbr_partitions[3].part_size + sizeof (mbr_obtenido);
        QString fit;
        fit = mbr_obtenido.disk_fit ;
        printf("fit que tiene el disco %s" , fit.toStdString().c_str());
        if(fit.contains("ff", Qt::CaseInsensitive)){

            bool encontro_particion = false;
            for(int i = 0 ;i<4 ; i++){
                if(size < mbr_obtenido.mbr_partitions[i].part_size && mbr_obtenido.mbr_partitions[i].part_status =='0'){
                    if (i != 0){
                    dimensinones[0] = i;
                    dimensinones[1] = size_usada;
                    dimensinones[2] = mbr_obtenido.mbr_partitions[i-1].part_start +mbr_obtenido.mbr_partitions[i-1].part_size +1;
                    encontro_particion = true;
                    break;
                    }else{
                        dimensinones[0] = i;
                        dimensinones[1] = size_usada;
                        dimensinones[2] = mbr_obtenido.mbr_partitions[i].part_start;
                        encontro_particion = true;
                        break;
                    }
                }
            }
            if(!encontro_particion){
                printf("(partition) no hay espacio para ingresar su particion \n");
                dimensinones[0] = -1;
            }
        }else if(fit.contains("wf", Qt::CaseInsensitive)){
            int part_habiles [4][2] = {{-1,-1},{-1,-1},{-1,-1},{-1,-1}};
            for(int i = 0 ;i<4 ; i++){
                if(size < mbr_obtenido.mbr_partitions[i].part_size && mbr_obtenido.mbr_partitions[i].part_status =='0'){
                    part_habiles[i][0] = mbr_obtenido.mbr_partitions[i].part_size;
                    part_habiles[i][1] = i;
                }
            }
            //ordenamos las opcines descendente mente
            for(int i = 0 ; i<4; i++){
                int temp =part_habiles[i][0];
                int part = part_habiles[i][1];
                int j = i-1;
                while((part_habiles[j][0]<temp ) && j >= 0){
                    part_habiles[j+1][0] = part_habiles[j][0];
                    part_habiles[j+1][1] = part_habiles[j][1];
                    j--;
                }
                part_habiles[j+1][0] = temp;
                part_habiles[j+1][1] = part;
            }
            int particion_elegida = part_habiles[0][1]; // tomamos el indice de la particion de mayor tamaño
            if(part_habiles[0][1] != -1   ){
                if(particion_elegida != 0){
                dimensinones[0] = particion_elegida;
                dimensinones[1] = size_usada;
                dimensinones[2] = mbr_obtenido.mbr_partitions[particion_elegida-1].part_start
                        + mbr_obtenido.mbr_partitions[particion_elegida-1].part_size +1;
                }else{
                    dimensinones[0] = 0;
                    dimensinones[1] = size_usada;
                    dimensinones[2] = mbr_obtenido.mbr_partitions[0].part_start;
                }
            }else{
                printf("(partition) no hay espacio para ingresar su particion \n");
                dimensinones[0] = -1;
            }
         }else if(fit.contains("bf", Qt::CaseInsensitive)){
             printf("entre al bf el disco tienne %s" , fit.toStdString().c_str());
             int part_habiles [4][2] = {{0,-1},{0,-1},{0,-1},{0,-1}};
             for(int i = 0 ;i<4 ; i++){
                 if(size < mbr_obtenido.mbr_partitions[i].part_size && mbr_obtenido.mbr_partitions[i].part_status =='0'){
                     part_habiles[i][0] = mbr_obtenido.mbr_partitions[i].part_size;
                     part_habiles[i][1] = i;
                 }
             }
             //ordenamos las opcines ascendente mente
             for(int i = 0 ; i<4; i++){
                 int temp =part_habiles[i][0];
                 int part = part_habiles[i][1];
                 int j = i-1;
                 while((part_habiles[j][0]>temp ) && j >= 0){
                     part_habiles[j+1][0] = part_habiles[j][0];
                     part_habiles[j+1][1] = part_habiles[j][1];
                     j--;
                 }
                 part_habiles[j+1][0] = temp;
                 part_habiles[j+1][1] = part;
             }
             int particion_elegida = -1; // tomamos el indice de la particion de menor tamaño
             dimensinones[0] = -1;
             for(int i = 0; i<4 ; i++){
                 if(part_habiles[i][0] != 0   ){
                     particion_elegida = part_habiles[i][1];
                     if(particion_elegida != -1   ){
                         if(particion_elegida != 0){
                         dimensinones[0] = particion_elegida;
                         dimensinones[1] = size_usada;   //cantidad de bit usados por todas las particiones
                         //final de la particion final e inicio de la particion a insertar
                         dimensinones[2] = mbr_obtenido.mbr_partitions[particion_elegida-1].part_start
                                 +mbr_obtenido.mbr_partitions[particion_elegida-1].part_size +1;
                         }else{
                             dimensinones[0] = 0;
                             dimensinones[1] = size_usada;
                             dimensinones[2] = mbr_obtenido.mbr_partitions[0].part_start;
                         }
                     }else{
                         printf("(partition) no hay espacio para ingresar su particion \n");
                         dimensinones[0] = -1;
                     }
                     break;
                 }
             }
         }
    }
    return dimensinones;
}

bool particion_vacia(particion particion){
    bool verificacion = false;
    if(particion.part_status == '0' && particion.part_name[0] =='\0' && particion.part_type == '-'){
        verificacion = true;
    }
    return verificacion;
}

QString determinate_fit(QString fit ){
    QString f = "";
    if(fit.isNull() ){
        f = "wf";
    }else if(fit == "WF" || fit == "wf"){
        f = "wf";
    }else if(fit == "FF" || fit == "ff"){
        f = "ff";
    }else if(fit == "BF" || fit == "bf"){
        f = "bf";
    }else{
        printf("(-fit)Error debe ingresar un valor de fit correcto (WF, FF , BF)");
    }
    return f;
}

QString verify_name(QString name){
    QString n = "";
    if(!name.isNull()){
        if(name.length() <=16 && name.length() >0 ){
        n = name;
        }
    }
    return n;
}

bool verificar_p_ext(mbr mbr){
    for(int i = 0 ; i < 4 ;i ++){
        if(mbr.mbr_partitions[i].part_type == 'e'){
            return true;
        }
    }
    return false;
}

int * valores_particionE(mbr mbr){
    static int dimensinones[3];
    for(int i = 0 ; i < 4 ;i ++){
        if(mbr.mbr_partitions[i].part_type == 'e' ){
                dimensinones[0] = i; //indice de particion
                dimensinones[1] = mbr.mbr_partitions[i].part_start; //inicio de particion
                dimensinones[2] = mbr.mbr_partitions[i].part_size; // tamañop
                break;
        }
    }
    return dimensinones;
}

int sizeOfpartition(int size , QString unity){
    int final_size = 0;
    if(unity == "m" ){
        final_size = size * 1024*1024;
    }else if (unity == "k" ){
        final_size = size*1024;
    }else if(unity == "b" ){
        return size;
    }
    else{
        return -1;
    }
    return final_size;
}

bool nombre_existente(mbr mbr_recuperado, QString name){
    bool ex = false;
    particion p;
    strcpy(p.part_name,name.toStdString().c_str());
    if(QString(mbr_recuperado.mbr_partitions[0].part_name) == name ||
            QString(mbr_recuperado.mbr_partitions[1].part_name) == name ||
            QString(mbr_recuperado.mbr_partitions[2].part_name) == name ||
            QString(mbr_recuperado.mbr_partitions[3].part_name) == name){
        ex = true;
    }
    return ex;
}

list<ebr> obtener_ebrs(mbr mbr_obtenido ,QString path ){
    list<ebr> particiones_logicas;
    int inicio = valores_particionE(mbr_obtenido)[1];
    ebr ebr_obtenido;
    FILE *file;
    file= fopen(path.toStdString().c_str(),"rb+");
    fseek(file,inicio,SEEK_SET);
    fread(&ebr_obtenido , sizeof (ebr),1,file);
    fclose(file);
        int posicion_ebr = inicio;
        for(int i = 0 ; i< 100 ; i++){
            ebr ebr_;
            file= fopen(path.toStdString().c_str(),"rb+");
            fseek(file,posicion_ebr,SEEK_SET);
            fread(&ebr_, sizeof (ebr),1,file);
            fclose(file);
            particiones_logicas.push_back(ebr_);
            if(ebr_.part_next == -1){
                break;
            }else{
                posicion_ebr = 0;
                posicion_ebr += ebr_.part_next ;
            }
        }
        return particiones_logicas;
}

bool nombre_logicos_repetidos(mbr mbr_obtenido,QString path ,QString name){
    bool nombre_repetido = false;
    list<ebr> particiones_logicas;
    particiones_logicas = obtener_ebrs(mbr_obtenido,path);
    list<ebr>::iterator it;
    for(it=particiones_logicas.begin();it!=particiones_logicas.end();it++)
    {
         if(it->part_name== name ){
             nombre_repetido = true;
         }
    }
    return nombre_repetido;
}

void formato_full(QString abPath, int inicio, int size){
   /* FILE *file;
    file = fopen( abPath.toStdString().c_str() , "w+b");

    char *buffer = (char *)calloc(disk_size,sizeof(char));
    fwrite(buffer,disk_size,1,file);
    fclose(file);*/
    FILE *file;
    file = fopen( abPath.toStdString().c_str() , "r b+");
    if (file != NULL){
        char *buffer = (char *)calloc(size,sizeof(char));
        fseek(file,inicio,SEEK_SET);
        fwrite(buffer, size, 1 , file);
        fclose(file);
        free(buffer);
        printf("(DELETE) Todo el contenido de la particion fue eliminado\n");

    }else{
         printf("Error!\n No se puede acceder al disco, EBR no creado\n");
    }
}

void eliminar_particion(QString path, QString tipo_del , QString name ){
    obmkdisk disco;
      try {
            mbr mbr_obtenido;
            FILE *file;
            file= fopen(path.toStdString().c_str(),"rb+");
        if(file != NULL)  {
            QTextStream qtin(stdin);
            QString line;
            printf("presione s para eliminar la particion \n");
            line = qtin.readLine();
            if(line == "si" || line == "s" ){
                fseek(file,0,SEEK_SET);
                fread(&mbr_obtenido , sizeof (mbr),1,file);
                fclose(file);
                printf("fecha de creacion de disco: %s \n" , mbr_obtenido.mbr_fecha_creacion );
                printf("tamaño de disco : %d  \n" , mbr_obtenido.mbr_tamano );
                printf("signature: %d \n" , mbr_obtenido.mbr_disk_signature );
                printf("fit: %s \n" , mbr_obtenido.disk_fit);
            if(tipo_del == "fast"){
                if(nombre_existente(mbr_obtenido, name) || nombre_logicos_repetidos(mbr_obtenido, path, name)){
                    if(nombre_logicos_repetidos(mbr_obtenido, path, name)){
                          list<ebr> _ebr = obtener_ebrs(mbr_obtenido, path);
                          list<ebr>::iterator it;
                          ebr ebr_vacio;
                          //printf("---------particiones logicas--------- ");
                          for(it=_ebr.begin();it!=_ebr.end();it++)
                          {
                              if(it->part_name == name){
                                  strcpy(ebr_vacio.part_fit,"-");
                                  ebr_vacio.part_name[0] = '\0';
                                  ebr_vacio.part_status = '0';
                                  ebr_vacio.part_next = it->part_next;
                                  ebr_vacio.part_size = it->part_size;
                                  ebr_vacio.part_start = it->part_start;
                                  disco.insertar_ebr(&ebr_vacio, path , it->part_start);
                                  break;
                              }
                          }
                    }else{
                        for(int i = 0 ; i<4 ; i ++){
                            if(mbr_obtenido.mbr_partitions[i].part_name == name){
                                strcpy(mbr_obtenido.mbr_partitions[i].part_fit , "-");
                                mbr_obtenido.mbr_partitions[i].part_status='0';//status inactivo
                                mbr_obtenido.mbr_partitions[i].part_type='-';
                                mbr_obtenido.mbr_partitions[i].part_name[0] = '\0';
                                disco.insertar_mbr(&mbr_obtenido,path);
                                break;
                            }
                        }
                    }
                }else{
                    printf("(NAME) nombre de particion no encontrado \n" );
                }
            }else if (tipo_del == "full"){
                if(nombre_logicos_repetidos(mbr_obtenido, path, name)){
                      list<ebr> _ebr = obtener_ebrs(mbr_obtenido, path);
                      list<ebr>::iterator it;
                      ebr ebr_vacio;
                      for(it=_ebr.begin();it!=_ebr.end();it++)
                      {
                          if(it->part_name == name){
                              strcpy(ebr_vacio.part_fit,"-");
                              ebr_vacio.part_name[0] = '\0';
                              ebr_vacio.part_status = '0';
                              ebr_vacio.part_next = it->part_next;
                              ebr_vacio.part_size = it->part_size;
                              ebr_vacio.part_start = it->part_start;
                              disco.insertar_ebr(&ebr_vacio, path , it->part_start);
                              formato_full(path,it->part_start +sizeof(ebr_vacio),it->part_size-sizeof(ebr_vacio) );
                              break;
                          }
                      }
                }else{
                    for(int i = 0 ; i<4 ; i ++){
                        if(mbr_obtenido.mbr_partitions[i].part_name == name){
                            strcpy(mbr_obtenido.mbr_partitions[i].part_fit , "-");
                            mbr_obtenido.mbr_partitions[i].part_status='0';//status inactivo
                            mbr_obtenido.mbr_partitions[i].part_type='-';
                            mbr_obtenido.mbr_partitions[i].part_name[0] = '\0';
                            disco.insertar_mbr(&mbr_obtenido,path);
                            formato_full(path,mbr_obtenido.mbr_partitions[i].part_start,mbr_obtenido.mbr_partitions[i].part_size);
                            break;
                        }
                    }
                }
            }else{
                printf("(NAME) nombre de particion no encontrado \n" );
            }
          }else{
                printf("(ADD) SE CANCELO LA ELIMINACION DE LA PARTICION \n" );
            }
        }else{
            printf("(PATH) el disco no fue encontrado, path erronea \n" );
        }
    }catch (exception& e) {
        printf("%s" , e.what());
    }
}

void editar_mbr(int size , QString unity , QString path , QString type, QString fit , QString name){
        obmkdisk disco ;
        list<ebr> particiones_logicas;
        try {
          mbr mbr_obtenido;
          FILE *file;
          file= fopen(path.toStdString().c_str(),"rb+");
          if(file != NULL)  {
              fseek(file,0,SEEK_SET);
              fread(&mbr_obtenido , sizeof (mbr),1,file);
              fclose(file);

              printf("fecha de creacion de disco: %s \n" , mbr_obtenido.mbr_fecha_creacion );
              printf("tamaño de disco : %d  \n" , mbr_obtenido.mbr_tamano );
              printf("signature: %d \n" , mbr_obtenido.mbr_disk_signature );
              printf("fit: %s \n" , mbr_obtenido.disk_fit);
              /*
              casos de fit:
                primer caso:
                    todas las particiones esta vacias
                        solo aceptar particiones primarias y extendidas solo una vez
                    segundo caso:
                        insertar particiones seguidas de la anterior
                        validar si en las particiones ya creadas hay una particion extendida
                    tercer caso:
                        se elimina una particion
                        se inserta una nueva
              */
              if(particiones_vacias(mbr_obtenido)){
                  if(type != "l"){
                        mbr_obtenido.mbr_partitions[0].part_status = '1';
                        std::strcpy(mbr_obtenido.mbr_partitions[0].part_fit , fit.toStdString().c_str());
                        if(type == "p"){
                            mbr_obtenido.mbr_partitions[0].part_type = 'p';
                        }else{
                            ebr ebr_vacio;
                            mbr_obtenido.mbr_partitions[0].part_type = 'e';
                            strcpy(ebr_vacio.part_fit,"-");
                            ebr_vacio.part_name[0] = '\0';
                            ebr_vacio.part_next = -1;
                            ebr_vacio.part_size = -1;
                            ebr_vacio.part_start = -1;
                            ebr_vacio.part_status = '0';
                            disco.insertar_ebr(&ebr_vacio, path , sizeof(mbr) +1);
                        }
                        mbr_obtenido.mbr_partitions[0].part_start = sizeof(mbr) +1;
                        mbr_obtenido.mbr_partitions[0].part_size = sizeOfpartition(size,unity);

                        printf("tamaño de inicio : %d  \n" , mbr_obtenido.mbr_partitions[0].part_start );
                        printf("tamaño de final : %d  \n" , sizeOfpartition(size,unity));
                        strcpy(mbr_obtenido.mbr_partitions[0].part_name , name.toStdString().c_str());
                        disco.insertar_mbr(&mbr_obtenido,path);
                        printf("(partition) particion inicial creada exitosamente, en indice %d \n" , 0);

                  }else{
                      printf("(partition) Nose puede crear una particion logica debido a que no hay una particion extendida \n");
                  }
              }else{
                  if(type != "l"){
                      int size_ofnewP =sizeOfpartition(size,unity);
                      if(size_ofnewP<mbr_obtenido.mbr_tamano){
                          int * part_info = dimensiones_particion(mbr_obtenido, size_ofnewP);
                          int part_amodificar = part_info[0];
                          int tamano_usado = part_info[1];
                          int start_partition = part_info[2];
                          if(!nombre_existente(mbr_obtenido, name)){
                              if(tamano_usado<mbr_obtenido.mbr_tamano){
                                  if(part_amodificar != -1){
                                      if( type == 'p' ){
                                          mbr_obtenido.mbr_partitions[part_amodificar].part_status = '1';
                                          std::strcpy(mbr_obtenido.mbr_partitions[part_amodificar].part_fit , fit.toStdString().c_str());
                                          mbr_obtenido.mbr_partitions[part_amodificar].part_type = 'p';
                                          mbr_obtenido.mbr_partitions[part_amodificar].part_start = start_partition;
                                          mbr_obtenido.mbr_partitions[part_amodificar].part_size = sizeOfpartition(size,unity);
                                          printf("tamaño particion : %d  \n" , mbr_obtenido.mbr_partitions[part_amodificar].part_size );
                                          printf("bit de inicio : %d  \n" , mbr_obtenido.mbr_partitions[part_amodificar].part_start );
                                          printf("bit final de particion: %d  \n" , mbr_obtenido.mbr_partitions[part_amodificar].part_size
                                                 +mbr_obtenido.mbr_partitions[part_amodificar].part_start );
                                          strcpy(mbr_obtenido.mbr_partitions[part_amodificar].part_name , name.toStdString().c_str());
                                          disco.insertar_mbr(&mbr_obtenido,path);
                                          printf("(partition) particion primaria creada exitosamente, en indice %d \n" , part_amodificar);
                                      }else if(!verificar_p_ext(mbr_obtenido) && type == 'e' ){
                                          ebr ebr_vacio;
                                          mbr_obtenido.mbr_partitions[part_amodificar].part_status = '1';
                                          std::strcpy(mbr_obtenido.mbr_partitions[part_amodificar].part_fit , fit.toStdString().c_str());
                                          mbr_obtenido.mbr_partitions[part_amodificar].part_type = 'e';
                                          mbr_obtenido.mbr_partitions[part_amodificar].part_start = start_partition;
                                          mbr_obtenido.mbr_partitions[part_amodificar].part_size = sizeOfpartition(size,unity);
                                          printf("tamaño particion : %d  \n" , mbr_obtenido.mbr_partitions[part_amodificar].part_size );
                                          printf("bit de inicio : %d  \n" , mbr_obtenido.mbr_partitions[part_amodificar].part_start );
                                          printf("bit final de particion: %d  \n" , mbr_obtenido.mbr_partitions[part_amodificar].part_size
                                                 +mbr_obtenido.mbr_partitions[part_amodificar].part_start );
                                          strcpy(mbr_obtenido.mbr_partitions[part_amodificar].part_name , name.toStdString().c_str());
                                          disco.insertar_mbr(&mbr_obtenido,path);
                                          strcpy(ebr_vacio.part_fit,"-");
                                          ebr_vacio.part_name[0] = '\0';
                                          ebr_vacio.part_next = -1;
                                          ebr_vacio.part_size = -1;
                                          ebr_vacio.part_start = -1;
                                          ebr_vacio.part_status = '0';
                                          disco.insertar_ebr(&ebr_vacio, path , start_partition);
                                          printf("(partition) particion extendida creada exitosamente, en indice %d \n" , part_amodificar);
                                      }else{
                                          printf("(partition) Solo se puede tener una particion extendida \n");
                                      }
                                  }else{
                                      printf("(partition) error no se pudo crear la particion\n");
                                  }

                               }else{
                                  printf("(partition) no hay espacio en el disco\n");
                              }
                            }else{
                              printf("(partition) El nombre de la particion ya esta ocupado escriba otro \n");
                          }
                      }else{
                          printf("(partition) la particion tiene un tamanio mayor al disco\n");
                      }
                  }else{
                      printf("(partition) creando particion logica \n");
                      printf("-----> entre aca \n" );
                      if(verificar_p_ext(mbr_obtenido)){
                          int * valores = valores_particionE(mbr_obtenido);
                          int indice = valores[0];
                          int inicio = valores[1];
                          int tamano_e = valores[2];
                          int size_ofnewP =sizeOfpartition(size,unity);
                          QString partition_fit = mbr_obtenido.mbr_partitions[indice].part_fit;
                          if(mbr_obtenido.mbr_partitions[indice].part_status != '0' ){
                          ebr ebr_obtenido;
                          file= fopen(path.toStdString().c_str(),"rb+");
                          fseek(file,inicio,SEEK_SET);
                          fread(&ebr_obtenido , sizeof (ebr),1,file);
                          fclose(file);

                          if(ebr_obtenido.part_status == '1' || ebr_obtenido.part_size != -1 ){
                              if(!nombre_logicos_repetidos(mbr_obtenido,path,name)){
                                  int posicion_ebr = inicio;
                                  int size_logic = 0;
                                  for(int i = 0 ; i< 100 ; i++){
                                      ebr ebr_;
                                      file= fopen(path.toStdString().c_str(),"rb+");
                                      fseek(file,posicion_ebr,SEEK_SET);
                                      fread(&ebr_, sizeof (ebr),1,file);
                                      fclose(file);

                                     if(ebr_.part_next == -1){
                                          printf("-----> entre aca \n" );
                                          size_logic += ebr_.part_size+ size_ofnewP ;
                                          if(ebr_.part_size >= size_ofnewP && ebr_.part_name[0] == '\0' && ebr_.part_status =='0'){
                                              particiones_logicas.push_back(ebr_);
                                          }
                                        if(particiones_logicas.size() > 0){
                                              ebr ebr_validos [particiones_logicas.size()];
                                              int iterador = 0;
                                              for (auto const& ebr_valido : particiones_logicas) {
                                                   ebr_validos[iterador] = ebr_valido;
                                                   iterador++;
                                              }
                                              if(partition_fit.contains("bf")){
                                                  for(int i = 0 ; i<(int)particiones_logicas.size(); i++){
                                                        ebr temp = ebr_validos[i];
                                                        int j = i-1;
                                                        while((ebr_validos[j].part_size >= temp.part_size) && j>=0 ){
                                                            ebr_validos[j+1] = ebr_validos[j];
                                                            j--;
                                                        }
                                                        ebr_validos[j+1] = temp;
                                                  }
                                                  ebr ebr_validado = ebr_validos[0];
                                                  strcpy(ebr_validado.part_fit,fit.toStdString().c_str());
                                                  strcpy(ebr_validado.part_name,name.toStdString().c_str())  ;
                                                  ebr_validado.part_size = size_ofnewP;
                                                  ebr_validado.part_status = '1';
                                                  disco.insertar_ebr(&ebr_validado, path , ebr_validado.part_start);
                                                  printf("(partition) particion insertada de manera correcta \n");
                                                  break;
                                              }else if(partition_fit.contains("wf")){
                                                  for(int i = 0 ; i<(int)particiones_logicas.size(); i++){
                                                        ebr temp = ebr_validos[i];
                                                        int j = i-1;
                                                        while((ebr_validos[j].part_size <= temp.part_size) && j>=0 ){
                                                            ebr_validos[j+1] = ebr_validos[j];
                                                            j--;
                                                        }
                                                        ebr_validos[j+1] = temp;
                                                  }
                                                  ebr ebr_validado = ebr_validos[0];
                                                  strcpy(ebr_validado.part_fit,fit.toStdString().c_str());
                                                  strcpy(ebr_validado.part_name,name.toStdString().c_str())  ;
                                                  ebr_validado.part_size = size_ofnewP;
                                                  ebr_validado.part_status = '1';
                                                  disco.insertar_ebr(&ebr_validado, path , ebr_validado.part_start);
                                                  printf("(partition) particion insertada de manera correcta \n");
                                                  break;
                                              }
                                          }
                                         if(size_logic < tamano_e){
                                              ebr_.part_next = ebr_.part_size + ebr_.part_start +1;
                                              disco.insertar_ebr(&ebr_, path , posicion_ebr);
                                              ebr nuevo_ebr;
                                              strcpy(nuevo_ebr.part_fit,fit.toStdString().c_str());
                                              strcpy(nuevo_ebr.part_name,name.toStdString().c_str())  ;
                                              nuevo_ebr.part_start = ebr_.part_next ;
                                              nuevo_ebr.part_size = size_ofnewP;
                                              nuevo_ebr.part_next = -1;
                                              nuevo_ebr.part_status = '1';
                                              disco.insertar_ebr(&nuevo_ebr, path , ebr_.part_next);
                                              break;
                                          }else{
                                              printf("(partition) el tamaño de las particiones logicas sobrepasa a la extendida \n");
                                              break;
                                          }

                                      }else if(ebr_.part_name[0] == '\0' && ebr_.part_status =='0'){
                                         if(partition_fit.contains("ff") && ebr_.part_size > size_ofnewP ){
                                             strcpy(ebr_.part_fit,fit.toStdString().c_str());
                                             strcpy(ebr_.part_name,name.toStdString().c_str())  ;
                                             ebr_.part_size = size_ofnewP;
                                             ebr_.part_status = '1';
                                             disco.insertar_ebr(&ebr_, path , ebr_.part_start);
                                             printf("(partition) particion insertada de manera correcta \n");
                                             break;
                                         }else{
                                             posicion_ebr = 0;
                                             posicion_ebr += ebr_.part_next ;
                                             size_logic += ebr_.part_size;
                                             if(ebr_.part_size >= size_ofnewP){
                                                 particiones_logicas.push_back(ebr_);
                                             }

                                         }

                                      }else{
                                          posicion_ebr = 0;
                                          posicion_ebr += ebr_.part_next ;
                                          size_logic += ebr_.part_size;
                                      }
                                  }
                              }else{
                                              printf("(partition) el nombre ya existe en las particiones \n");
                              }
                          }else{
                            if(!nombre_logicos_repetidos(mbr_obtenido,path,name)){
                                  strcpy(ebr_obtenido.part_fit,fit.toStdString().c_str());
                                  strcpy(ebr_obtenido.part_name,name.toStdString().c_str())  ;
                                  ebr_obtenido.part_start =inicio;
                                  ebr_obtenido.part_size = size_ofnewP;
                                  ebr_obtenido.part_next = -1;
                                  ebr_obtenido.part_status = '1';
                                  disco.insertar_ebr(&ebr_obtenido, path , inicio);
                             }
                          }
                      }else{
                          printf("(partition) Para crear una particion logica debe estar activa la extendida \n");
                      }


                  }else{
                          printf("(partition) Para crear una particion logica debe crear primero una extendida \n");
                  }
                  }
              }
              for(int x = 0 ; x<4 ;x++){
                  QString name_part = mbr_obtenido.mbr_partitions[x].part_name;
                  QString active = QString(mbr_obtenido.mbr_partitions[x].part_status);
                  QString ti_part  = QString(mbr_obtenido.mbr_partitions[x].part_type);
                  printf("------------------------------  Particion numero: %d \n",x);
                  printf("Nombre de particion: %s \n" ,name_part.toStdString().c_str() );
                  printf("tipo de particion: %s \n" ,ti_part.toStdString().c_str() );
                  printf("Estado:  %s \n",  active.toStdString().c_str()  );
                  printf("tamaño de la particion : %d  \n" , mbr_obtenido.mbr_partitions[x].part_size );
                  printf("inicio de la particion : %d  \n" , mbr_obtenido.mbr_partitions[x].part_start );
                  printf("fin de la particion : %d  \n" , mbr_obtenido.mbr_partitions[x].part_size +
                         mbr_obtenido.mbr_partitions[x].part_start);
                  if(mbr_obtenido.mbr_partitions[x].part_type == 'e'){
                      particiones_logicas = obtener_ebrs(mbr_obtenido,path);
                     for(const auto& it : particiones_logicas){
                          printf("particion logica: %s \n", it.part_name);
                          printf("estado de particion: %d \n", it.part_status);
                          printf("inicio de particion: %d \n", it.part_start);
                          printf("tamaño de particion: %d \n", it.part_size);
                          printf("fit de particion: %s \n", it.part_fit);
                          printf("prox ebr de particion: %d \n", it.part_next);
                          }
                  }
              }
          }
        }  catch (exception& e) {
            printf("holaaaaaaaaa");
        }
    }

void add_size(QString path, QString unity, int sizeAdd, QString name){
    obmkdisk disco;

      try {
            mbr mbr_obtenido;
            FILE *file;
            file= fopen(path.toStdString().c_str(),"rb+");
            int size_agregar = sizeOfpartition(sizeAdd,unity);
        if(file != NULL) {
                fseek(file,0,SEEK_SET);
                fread(&mbr_obtenido , sizeof (mbr),1,file);
                fclose(file);

                if(nombre_existente(mbr_obtenido, name) || nombre_logicos_repetidos(mbr_obtenido, path, name)){
                    if(nombre_logicos_repetidos(mbr_obtenido, path, name)){
                          list<ebr> _ebr = obtener_ebrs(mbr_obtenido, path);
                          vector<ebr> ebrs(_ebr.begin(),_ebr.end());
                          int cantidad_ebrs = sizeof (ebrs)/sizeof (ebrs[0]);
                          for(int i=0; i < cantidad_ebrs; i++){
                              if(ebrs[i].part_name == name){
                                  if(i != cantidad_ebrs -1 && size_agregar <0 ){
                                      if(size_agregar*-1 < ebrs[i].part_size){
                                            ebrs[i].part_size += size_agregar;
                                            disco.insertar_ebr(&ebrs[i], path , ebrs[i].part_start);
                                      }else{
                                            printf("(Error add) nose puede restar una cantidad mayor a su tamaño actual \n" );
                                      }

                                  }else{
                                      int tamano_particion = 0;
                                      for(int j = 0; j<4 ; j++){
                                          if(mbr_obtenido.mbr_partitions[j].part_type == 'e'){
                                              tamano_particion = mbr_obtenido.mbr_partitions[j].part_size;
                                          }
                                      }

                                      if(size_agregar < tamano_particion){
                                          ebrs[i].part_size += size_agregar;
                                          disco.insertar_ebr(&ebrs[i], path , ebrs[i].part_start);

                                      }else{
                                          printf("(Error add)tamaño exede a la particion extendida \n" );
                                      }

                                  }
                              }
                          }


                    }else{
                        for(int i = 0 ; i<4 ; i ++){
                            if(mbr_obtenido.mbr_partitions[i].part_name == name ){
                                if(i !=3){
                                    int tamano_particion = mbr_obtenido.mbr_partitions[i].part_size +mbr_obtenido.mbr_partitions[i].part_start ;
                                    if((tamano_particion  + size_agregar) <  mbr_obtenido.mbr_partitions[i+1].part_start && mbr_obtenido.mbr_partitions[i+1].part_status != '0' ){
                                        mbr_obtenido.mbr_partitions[i].part_size += size_agregar;
                                        disco.insertar_mbr(&mbr_obtenido,path);
                                    }else if (mbr_obtenido.mbr_partitions[i+1].part_status != '0'){

                                    }
                                }else{

                                    int final_part4 = mbr_obtenido.mbr_partitions[i].part_start +mbr_obtenido.mbr_partitions[i].part_size ;
                                    if((final_part4 + size_agregar) < mbr_obtenido.mbr_tamano  ){
                                        mbr_obtenido.mbr_partitions[i].part_size += size_agregar;
                                        disco.insertar_mbr(&mbr_obtenido,path);
                                    }
                                }
                            }
                        }
                    }
                }else{
                    printf("(PARTITION) particion no encontrada \n" );
                }
                }else{
                    printf("(PATH) el disco no fue encontrado, path erronea \n" );
                }
            }catch (exception& e) {
                printf("%s" , e.what());
            }
}

void fdisk::resultado_particion(fdisk *f_disco){
    obmkdisk disco ;
    printf("-------------------INFORMACION DE COMANDO----------------\n\n");
    if(f_disco->add && !f_disco->path.isNull()){
        if(f_disco->sizeNegativo && f_disco->sizeadd != 0 ){
            QString unity = determinateUnity(f_disco->unity);
            bool existe = disco.file_exists(disco.obtainAbsPath(f_disco->path));
            QString _type = determinate_type(f_disco->type);
            QString fit = determinate_fit(f_disco->fit);
            QString _name = verify_name(f_disco->name);
            QString un  = QVariant(existe).toString() ;
            QString abPath = disco.obtainAbsPath(f_disco->path);
            qDebug("unidad:  "+ unity.toLatin1() + " existe: " + un.toLatin1() + " Tipo: "
                   + _type.toLatin1() + " nombre: "+ _name.toLatin1() );
            if(existe && _name != "" ){
                printf("(ADD) direccion de disco :  %s  \n",abPath.toStdString().c_str() );
                printf("(ADD) unidades :  %s  \n",unity.toStdString().c_str());
                printf("(ADD) nombre de particion :  %s  \n",_name.toStdString().c_str());
                printf("(ADD) tamaño :  %d  \n",f_disco->sizeadd );
                add_size(abPath,unity,f_disco->sizeadd*-1 , _name );
            }
        }else{
            QString unity = determinateUnity(f_disco->unity);
            bool existe = disco.file_exists(disco.obtainAbsPath(f_disco->path));
            QString _type = determinate_type(f_disco->type);
            QString fit = determinate_fit(f_disco->fit);
            QString _name = verify_name(f_disco->name);
            QString un  = QVariant(existe).toString() ;

            qDebug("unidad:  "+ unity.toLatin1() + " existe: " + un.toLatin1() + " Tipo: "
                   + _type.toLatin1() + " nombre: "+ _name.toLatin1() );
            if(existe && _name != "" ){
                QString abPath = disco.obtainAbsPath(f_disco->path);
                printf("(ADD) direccion de disco :  %s  \n",abPath.toStdString().c_str() );
                printf("(ADD) unidades :  %s  \n",unity.toStdString().c_str());
                printf("(ADD) nombre de particion :  %s  \n",_name.toStdString().c_str());
                printf("(ADD) tamaño :  %d  \n",f_disco->sizeadd );
                add_size(abPath,unity,f_disco->sizeadd , _name );
            }
        }
        free(f_disco);
    }else if(f_disco->delete_partition){
        if( f_disco->unity.isNull() && f_disco->type.isNull() && f_disco->fit.isNull()  ){
            if(!f_disco->path.isNull()  && !f_disco->name.isNull()  && !f_disco->type_of_delete.isNull() ){
                printf("(ELIMINAR PARTICION)direccion de disco :  %s  \n",f_disco->path.toStdString().c_str() );
                printf("(ELIMINAR PARTICION)nombre particion:  %s  \n",f_disco->name.toStdString().c_str() );
                printf("(ELIMINAR PARTICION)tipo de eliminacion:  %s  \n",f_disco->type_of_delete.toStdString().c_str() );
                QString tipo_del = f_disco->type_of_delete.toLower();
                eliminar_particion(f_disco->path, tipo_del, f_disco->name);
            }else{
                bool parametros [3] = {f_disco->path.isNull(),f_disco->name.isNull(),f_disco->type_of_delete.isNull()};
                string errores [3] = {"direccion del disco incorrecta", "nombre incorrecto ", "tipo de delete vacio"};
                for (int i = 0 ; i<3 ; i++){
                    if(parametros[i]){
                        string error = errores[i];
                        printf("Error %s  \n" , error.c_str() );
                    }
                }
            }

        }else{
            printf("Comando fdisk delete necesita como parametros -path , -name \n");

        }
        free(f_disco);
    }else if(f_disco->add == false && f_disco->delete_partition == false ){
        if(!f_disco->sizeNegativo ){
            printf("(crear particion)size:  %d  \n",f_disco->size );
            printf("(crear particion)unity:  %s  \n",f_disco->unity.toStdString().c_str() );
            printf("(crear particion)path:  %s  \n",f_disco->path.toStdString().c_str() );
            printf("(crear particion)type:  %s  \n",f_disco->type.toStdString().c_str() );
            printf("(crear particion)fit:  %s  \n",f_disco->fit.toStdString().c_str() );
            printf("(crear particion)name:  %s  \n",f_disco->name.toStdString().c_str() );
            QString u = determinateUnity(f_disco->unity);
            bool existe = disco.file_exists(disco.obtainAbsPath(f_disco->path));
            QString _type = determinate_type(f_disco->type);
            QString fit = determinate_fit(f_disco->fit);
            QString _name = verify_name(f_disco->name);
            QString un  = QVariant(existe).toString() ;
            qDebug("unidad:  "+ u.toLatin1() + " existe disco: " + un.toLatin1() +
                   " Tipo: "+ _type.toLatin1() + " nombre: "+ _name.toLatin1() );

            if(u != "" && existe  &&  _type != "" && fit != "" && _name != "" ){
                QString abPath = disco.obtainAbsPath(f_disco->path);

                editar_mbr(f_disco->size, u, abPath , _type , fit , _name );
            }else{
                if(!existe){
                    printf("(path) Disco inexistente! ");
                }
            }

         }else{
                    printf("(size) debe ingresar valores mayores a 0  ");
        }

        free(f_disco);

    }else{
        printf("(path) Falta el comando path en la instruccion");
    }
}
