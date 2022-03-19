#include "reportes.h"
#include "obmkdisk.h"
#include "structs.h"
#include <QTextStream>
#include <exception>
#include <list>
#include <string>
#include <iostream>
#include <fstream>
#include <QString>
#include <QDir>
#include <iostream>
#include <stdlib.h>
#include "string"
#include <locale>

using namespace std;
reportes::reportes()
{

}


void reportes::reporte_mbr(QString abPath){
    QString path = QDir::currentPath() + "/"+"reporte.dot" ;
    QString path2 = QDir::currentPath() + "/"+"reporte.jpg" ;
    try{
        mbr mbr_obtenido;
        FILE *file;
        file = fopen(abPath.toStdString().c_str(), "rb+");
        if(file != NULL){
            fseek(file,0,SEEK_SET);
            fread(&mbr_obtenido , sizeof (mbr),1,file);
            fclose(file);
         FILE *doc = fopen(path.toStdString().c_str(),"r");
         if(doc == NULL){
             string cmm = "mkdir -p \"" + path.toStdString() + "\"";
                        string cmm2 = "rmdir \"" + path.toStdString() + "\"";
                        system(cmm.c_str());
                        system(cmm2.c_str());
         }else{
             fclose(doc);
         }
         FILE *img = fopen(path2.toStdString().c_str() , "r");
         if(img == NULL){
             string cmm = "mkdir -p \"" + path2.toStdString() + "\"";
                        string cmm2 = "rmdir \"" + path2.toStdString() + "\"";
                        system(cmm.c_str());
                        system(cmm2.c_str());
         }else{
             fclose(img);
         }

        string contenido = "digraph G { \n"
                "ordering = out \n"
                "forcelabels=true \n"
                "graph[ranksep=1,margin=0.3  ]; \n"
                "node [shape = plaintext];\n "
                "1 [ label = <<TABLE color = \"black\"> \n"
                 "<TR>\n"
                    "<td > mbr tamaño_disco= "+ std::to_string(mbr_obtenido.mbr_tamano) +"</td>\n"
                    "<td >"+ mbr_obtenido.mbr_partitions[0].part_name +"</td>\n"
                    "<td >"+ mbr_obtenido.mbr_partitions[1].part_name +"</td>\n"
                    "<td >"+ mbr_obtenido.mbr_partitions[2].part_name +"</td>\n"
                    "<td >"+ mbr_obtenido.mbr_partitions[3].part_name +"</td>\n"
                 "</TR>\n"
                "</TABLE>> dir =none color=white style =none]\n"
                "}";
        ofstream outfile(path.toStdString());
        outfile << contenido.c_str() << endl;
        outfile.close();
        string funcion = "dot -Tjpg " + path.toStdString()+ " -o" + " reporte.jpg" ;
        system(funcion.c_str());
         }

    }catch(exception & e)
    {
        printf("error al generar reporte");
    }
};
