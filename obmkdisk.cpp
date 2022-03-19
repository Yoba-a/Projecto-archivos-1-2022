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
#include <fstream>
#include <QString>

obmkdisk::obmkdisk()
{

}


bool obmkdisk::file_exists (QString path)
{
      std::ifstream f(path.toStdString().c_str());
      return f.good();
}


QString obmkdisk::obtainPath(QString path){
    int lenPath = path.length();
    int posicion = 0 ;
    if(path.at(0) != '\"'){
        for (int i = 0 ; i< lenPath ; i++){
            if(path.at(i) == '/' ){
                posicion = i;
                }
            }
        path = path.mid(0,posicion);
        printf("Ruta sin comillas: %s \n" , path.toStdString().c_str());
        return path;
    }else{
        for (int i = 0 ; i< lenPath ; i++){
            if(path.at(i) == '/' ){
                posicion = i;
                }
            }
        path = QString("%1").arg(path.mid(1,posicion));
        printf("Ruta con comillas: %s \n" , path.toStdString().c_str());
        return path;
    }
}

QString obmkdisk::obtainAbsPath(QString path){
    int lenPath = path.length();
    int posicion = 0 ;
    if(path.at(0) !='\"'){

        return path;
    }else{
        for (int i = 0 ; i< lenPath ; i++){
            if(path.at(i) == '\"' ){
                posicion = i;
                }
            }
        path = QString("%1").arg(path.mid(1,posicion-1));
        return path;
    }
}


bool obmkdisk::findConfigFile(QString path)
{

        if (file_exists(path)==1)
        {
           printf("Ruta existente:  %s  \n",path.toStdString().c_str());
          return true;
        }else{
            if(mkdir(path.toStdString().c_str(),0777) == -1){
                printf("no se pudo crear la ruta :  %s por falta de permisos \n",path.toStdString().c_str());
                return false;
            }
            else{
                printf("se creo la ruta :  %s  \n",path.toStdString().c_str());
                return true;
            }

        }

}

int obmkdisk::sizeOfDisk(int size , QString unity){
    int final_size = 0;

    if(unity == "m" || unity == "M"){
        final_size = size * 1024*1024;
    }else if (unity == "k" || unity == "K"){
        final_size = size*1024;
    }else{
        return -1;
    }
    return final_size;
}

string obmkdisk::currentDateTime () {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[18];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %R", &tstruct);
    return buf;
}

void obmkdisk::eliminarDisco(obmkdisk *disco){
    QString path = disco->line;
    if(findConfigFile(obtainPath(path))){
        bool confirmacion = true;
        QString abPath = obtainAbsPath(disco->line);
        while (confirmacion) {
            printf("\n\n---------CONFIRMACION REQUERIDA----------\n\n");
            printf("¿Desea continuar con la eliminacion del disco? [s,n] \n");
            printf("ingrese S para continuar \n");
            printf("ingrese N para cancelar \n");
            QTextStream s(stdin);
            QString value = s.readLine();
            if (value == "S" || value == "s"){
                if(remove(abPath.toStdString().c_str()) != 0 ){
                    printf("Error al borrar archivo!.");
                    confirmacion = false;
                }else{
                    printf("El archivo se borro con exito!");
                    confirmacion = false;
                }
            }else if (value == "N" || value == "n"){
                confirmacion = false;
                printf("Cancelando..... \n");
                printf("Se cancelo la eliminacion del disco en : %s \n",disco->line.toStdString().c_str());
            }else{
                printf("ingrese una opcion correcta \n");
            }
        }

    }
    free(disco);
}


void obmkdisk::insertar_ebr(ebr *ebr_disk,QString abPath, int inicio ){
    FILE *file;
    file = fopen( abPath.toStdString().c_str() , "rb+");
    if (file != NULL){
        fseek(file,inicio,SEEK_SET);
        fwrite(ebr_disk, sizeof(ebr), 1, file);
        fclose(file);


    }else{
         printf("Error!\n No se puede acceder al disco, EBR no creado\n");
    }
}


void obmkdisk::insertar_mbr(mbr *mbr_disk,QString abPath ){
    FILE *file;
    file = fopen( abPath.toStdString().c_str() , "rb+");
    if (file != NULL){
        fseek(file,0,SEEK_SET);
        fwrite(mbr_disk, sizeof(mbr), 1, file);
        fclose(file);

        printf("DISCO CREADO CORRECTAMENTE \nSE AGREGO EL MBR DE MANERA CORRECTA\n");

    }else{
         printf("Error!\n No se puede acceder al disco, MBR no creado\n");
    }
}

void obmkdisk::mostrardatos(obmkdisk *disco){
    if (disco->unity.isNull()){

        disco->unity= "m";
    }
    if (disco->fit.isNull()){

        disco->fit= "ff";
    }

      mbr *mbr_disk = (mbr*)malloc(sizeof(mbr));
      QTextStream out(stdout);
      printf("\n\n---------DATOS----------\n\n");

      printf("EL tamano es: %d \n",disco->size);
      printf("La ruta es: %s \n",disco->line.toStdString().c_str());
      printf("Las unidades: son %s \n",disco->unity.toStdString().c_str());
      printf("El ajuste es: %s \n",disco->fit.toStdString().c_str());
      if(disco->sizeNegativo == false){
      try {
          if(findConfigFile(obtainPath(disco->line))){
              //Direccion del disco
              QString abPath = obtainAbsPath(disco->line);
              //tamaño del disco
              int disk_size = sizeOfDisk(disco->size, disco->unity);
              //asignacion de tamaño a mbr
              if(disk_size > 0){
                  //creacion de disco
                  FILE *file;
                  file = fopen( abPath.toStdString().c_str() , "w+b");
                  char *buffer = (char *)calloc(disk_size,sizeof(char));
                  fwrite(buffer,disk_size,1,file);
                  fclose(file);
                  mbr_disk->mbr_tamano=disk_size;

                  //fecha de creacion de disco
                  string fecha_creacion = currentDateTime();
                  //asignacion de fecha de creacion a mbr y disco
                  strcpy(mbr_disk->mbr_fecha_creacion, fecha_creacion.c_str());
                  disco->fechacreacion = QString::fromStdString(fecha_creacion);
                  mbr_disk->mbr_disk_signature = (rand() % 100);
                  //ingresando fit a mbr
                  strcpy(mbr_disk->disk_fit,  disco->fit.toStdString().c_str());
                  printf("\nFECHA DE CREACION DE DISCO : %s\n",mbr_disk->mbr_fecha_creacion);
                  particion vacia;
                  strcpy(vacia.part_fit , "-");
                  vacia.part_status='0';//status inactivo
                  vacia.part_type='-';
                  vacia.part_start=-1;
                  vacia.part_size=-1;
                  vacia.part_name[0] = '\0';


                  //creacion de las particiones en el mbr
                  for(int i = 0 ; i <4 ; i++)
                      mbr_disk->mbr_partitions[i] = vacia;


                  //insercion de la mbr al disco
                  insertar_mbr(mbr_disk, abPath);


                  //liberar apuntadores
                  free(buffer);

              }else{
                  printf("Error!\n No se puede crear disco, unidad de entrada no aceptada\n");
              }
          }
      }  catch (exception& e) {
        printf("%s" , e.what());
      }
     }else{
          printf("Error!\n No se puede crear un disco con tamaño negativo\n");

      }
      free(disco);
      free(mbr_disk);


}


