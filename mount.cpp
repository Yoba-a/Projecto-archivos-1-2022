#include "mount.h"
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
extern QList<disco_montado> discos_usados;

mount::mount()
{


}

int * vlrs_particion(mbr mbr){
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

list<ebr> obt_ebrs(mbr mbr_obtenido ,QString path ){
    list<ebr> particiones_logicas;
    int inicio = vlrs_particion(mbr_obtenido)[1];
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

 disco_montado get_idDisk(QString path){
    disco_montado disco_nuevo;
    int ultimo_id = 1;
    if(discos_usados.size() >0 ){

        for(int i = 0 ; i <discos_usados.size() ; i++){
            if(discos_usados[i].path == path.toStdString()){
                return discos_usados[i];
            }
          ultimo_id = discos_usados[i].id_disco + 1;
        }

        disco_nuevo.path = path.toStdString();
        return disco_nuevo;
    }else{
        disco_nuevo.id_disco = ultimo_id;
        disco_nuevo.path = path.toStdString();
        return  disco_nuevo;
    }
}

void mount::resultado_mount(mount *f_mount){

    /*
    particiones_montadas.append(1);
    QString data = QString::number(particiones_montadas.size());
    printf("tamaño lista %s", data.toStdString().c_str());
*/
    try{

        disco_montado disco_mounted = get_idDisk(f_mount->path);
        mbr mbr_obtenido;
        list<ebr> particiones_logicas;
        FILE *file;
        file= fopen(path.toStdString().c_str(),"rb+");
        if(file != NULL)  {
            fseek(file,0,SEEK_SET);
            fread(&mbr_obtenido , sizeof (mbr),1,file);
            fclose(file);

            int particion_externa = -1;
            bool particion_normal =  false;
            particion_montada part_montada;
            for(int x = 0 ; x<4 ;x++){
                QString name_part = mbr_obtenido.mbr_partitions[x].part_name;
                if(mbr_obtenido.mbr_partitions[x].part_type == 'e'){
                   particion_externa = x;
                   if(mbr_obtenido.mbr_partitions[x].part_name == f_mount->name_partition.toStdString()){
                       printf("no se puede montar una particion extendida");
                       return;
                   }
                }
                if(name_part == f_mount->name_partition ){
                    part_montada.tipo_part = 0;
                    part_montada.status = 1;
                    part_montada.name = f_mount->name_partition.toStdString();
                    for(int j = 0 ; j <26 ; j++){
                        if(disco_mounted.montadas[j].status== 0){
                            part_montada.part_id = "23"+to_string( disco_mounted.id_disco) + alfabeto[j];
                            disco_mounted.montadas[j] = part_montada;
                            discos_usados.append(disco_mounted);
                            printf("(mount) particion %s montada correctamente con id %s" , part_montada.name.c_str(), part_montada.part_id.c_str() ) ;
                            return;
                        }
                    }
                    particion_normal = true;
                }
            }
           if(particion_normal){
           particiones_logicas = obt_ebrs(mbr_obtenido,path);
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

    }catch(exception e){
        printf("fallo al montar una particion");
    }

}
