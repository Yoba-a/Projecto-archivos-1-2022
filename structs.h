#ifndef STRUCTS_H
#define STRUCTS_H
#include <QString>
#include <QtCore/qglobal.h>


typedef struct{
    char part_status;
    char part_type;
    char part_fit[2];
    int part_start;
    int part_size;
    char part_name[16];
}particion;


typedef  struct{
    int mbr_tamano;
    char mbr_fecha_creacion[16];
    int mbr_disk_signature;
    char disk_fit[2];
    particion mbr_partitions[4];
}mbr;

typedef  struct{
    char part_status;
    char part_fit[2];
    int part_start;
    int part_size;
    int part_next;
    char part_name[16];
}ebr;


typedef struct  _MP{
    int tipo_part;
    int status = 0;
    std::string part_id;
    std::string name;
}particion_montada;


typedef struct {
    std::string path;
    int id_disco;
    int estatus;
    particion_montada montadas[26];
}disco_montado;

class structs
{
public:
    structs();

};

#endif // STRUCTS_H
