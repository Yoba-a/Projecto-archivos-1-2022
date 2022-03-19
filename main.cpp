#include <QCoreApplication>
#include "iostream"
#include <QString>
#include <parser.h>  // Nuestro parser
#include <scanner.h>  // Nuestro scanner
#include <QTextStream>
#include <QDebug>
#include <fstream>
#include <regex>
#include <mount.h>

using namespace std;

extern int yyparse(); //
//static QList<discosmontados> discosenmemoria;
int p;
extern int linea; // Linea del token
extern int columna; // Columna de los tokens
extern int yylineno;
QList<disco_montado> discos_usados;

int main(int argc, char *argv[])
{

    string p="------------------------------Ingrese un comando------------------------------\n";

    QCoreApplication a(argc, argv);



    QTextStream qtin(stdin);
    QString line;

    while(line!="salir"){    //esto me sirve para seguir leyendo siempre los comandos sin salirme
        cout << p;
        line = qtin.readLine();
        printf("mounted %d", discos_usados.size());
        if(line!="salir"){
            if(line.isEmpty()==false){
                if(line.contains("exec -path= ",Qt::CaseInsensitive) || line.contains("exec - path =",Qt::CaseInsensitive)
                   || line.contains("exec -path =",Qt::CaseInsensitive)
                    || line.contains("exec - path=",Qt::CaseInsensitive)){
                    try {
                        line.remove("exec",Qt::CaseInsensitive);
                        line.remove("-",Qt::CaseInsensitive);
                        line.remove("path",Qt::CaseInsensitive);
                        line.remove("=",Qt::CaseInsensitive);
                        line.remove("\"");

                        printf("\n");
                        int indice_barra = line.indexOf("/" ,0);

                        line.replace(0,indice_barra,"");

                        printf("\n");
                        std::ifstream f(line.toStdString().c_str());
                        if(f.is_open()){
                            string iline;

                            while(getline(f,iline)){
                               QString comando = QString::fromStdString(iline);
                                    if(!comando.contains("pause",Qt::CaseInsensitive)){

                                    if(iline != " " || iline != "/n" ){

                                        printf("Comando a Ejecutar: " );
                                        printf(comando.toUtf8().constData());
                                        printf("\n");
                                        YY_BUFFER_STATE buffer = yy_scan_string(comando.toUtf8().constData());
                                    }
                                    /*Limpiamos los contadores
                     ya que son variables globales*/
                                    linea = 0;
                                    columna = 0;
                                    yylineno = 0;
                                    if(yyparse()==0) // Si nos da un número negativo, signifca error.
                                    {
                                        printf("\n\nComando ejecutado correctamente\n\n");

                                    }else {

                                        printf("\n\nhay errores\n\n");
                                    }
                                }else{
                                        printf("\n\npresione cualquier tecla para continuar\n\n");
                                        system("read");
                                    }
                            }
                            f.close();

                        }else{
                            printf("(EXEC) RUTA NO ENCONTRADA");
                        }



                    }  catch (exception e) {
                        printf("comando exec no a sido ingresado de la manera correcta");
                    }
                }else{
                    YY_BUFFER_STATE buffer = yy_scan_string(line.toUtf8().constData());

                    /*Limpiamos los contadores exec - path = /home/scast/p.sh
     ya que son variables globales*/
                    linea = 0;
                    columna = 0;
                    yylineno = 0;

                    if(yyparse()==0) // Si nos da un número negativo, signifca error.
                    {
                        printf("\n\nComando ejecutado correctamente\n\n");

                    }else {

                        printf("\n\nhay errores\n\n");
                    }
                }

            }
        }

    }

 return a.exec();
}
