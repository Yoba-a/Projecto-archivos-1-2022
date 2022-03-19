%{
#include "scanner.h"//se importa el header del analisis sintactico
#include <QString>
#include <string>
#include "qdebug.h"
#include <iostream>
#include "obmkdisk.h"
#include "fdisk.h"
#include "reportes.h"
#include "mount.h"
using namespace std;
extern int yylineno; //linea actual donde se encuentra el parser (analisis lexico) lo maneja BISON
extern int columna; //columna actual donde se encuentra el parser (analisis lexico) lo maneja BISON
extern char *yytext; //lexema actual donde esta el parser (analisis lexico) lo maneja BISON

int yyerror(const char* mens)
{
std::cout << mens <<" "<<yytext<< std::endl;
return 0;
}
%}
//error-verbose si se especifica la opcion los errores sintacticos son especificados por BISON
%define parse.error verbose
%defines "parser.h"
%output "parser.cpp"
%locations
%union{
//se especifican los tipo de valores para los no terminales y lo terminales
//char TEXT [256];
//QString TEXT;
char TEXT[256];
class obmkdisk *mdisk;
class fdisk *fdisk;
class mount *mount;

}


//TERMINALES DE TIPO TEXT, SON STRINGS
/*
"SIZE" {columna=columna+ anterior;  anterior = strlen(yytext); strcpy(yylval.TEXT, yytext);return psize;}
"MKDISK" {columna=columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return pmkdisk;}
"RMDISK" {columna=columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return prmdisk;}
"FDISK" {columna=columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return pfdisk;}
"TYPE" {columna=columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return ptype;}
"DELETE" {columna=columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return pdelete;}
"NAME" {columna=columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return pname;}
"add" {columna=columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return padd;}
"PATH" {columna=columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return ppath;}
"U" {columna=columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return punity;}
"F" {columna=columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return pfit;}

{entero} {columna=columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext); return entero;}
{cadena} {columna=columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext); return cadena;}
{unidades} {columna=columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext); return unidades;}
{ajustes} {columna=columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext); return ajustes;}
{ruta} {columna=columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext); return ruta;}
{tdelete} {columna=columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext); return tdelete;}
{identificador} {columna=columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext); return identificador;}
{caracter} {columna=columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext); return caracter;}

*/

%token<TEXT> psize;
%token<TEXT> pmkdisk;
%token<TEXT> prmdisk;
%token<TEXT> pfdisk;
%token<TEXT> ptype;
%token<TEXT> pdelete;
%token<TEXT> pname;
%token<TEXT> padd;
%token<TEXT> pmkdir;
%token<TEXT> ppath;
%token<TEXT> punity;
%token<TEXT> pfit;
%token<TEXT> prep;
%token<TEXT> pmount;
%token<TEXT> punto;
%token<TEXT> bracketabre;
%token<TEXT> bracketcierra;
%token<TEXT> corcheteabre;
%token<TEXT> corchetecierra;
%token<TEXT> puntocoma;
%token<TEXT> potencia;
%token<TEXT> coma;
%token<TEXT> parentesisabre;
%token<TEXT> parentesiscierra;

%token<TEXT> llaveabre;
%token<TEXT> llavecierra;
%token<TEXT> mas;
%token<TEXT> menos;
%token<TEXT> multiplicacion;
%token<TEXT> igual;
%token<TEXT> dolar;

//%token<TEXT> barra;

%token<TEXT> entero;
%token<TEXT> cadena;
%token<TEXT> identificador;
%token<TEXT> prueba;
%token<TEXT> idpath;
%token<TEXT> caracter;
%token<TEXT> ruta;
%token<TEXT> rutacom;
%token<TEXT> unidades;
%token<TEXT> ajustes;
%token<TEXT> rutacualquiera;
%token<TEXT> comillas;

//%token<TEXT> pdisk;




%type<mdisk> COMANDOMKDISK;
%type<mdisk> COMANDORMDISK;// lista de instrucciones
%type<fdisk> COMANDOFDISK;
%type<reportes> COMANDOREP;
%type<mount> COMANMOUNT;
%type<TEXT> DIRECCION;

%left suma menos
%left multi division
%left potencia
%start INICIO
%%

INICIO : LEXPA { }

 ;

LEXPA:  pmkdisk COMANDOMKDISK
{
printf("Creacion de disco inicializada \n");
$2->mostrardatos($2);//ejecuto el metodo "mostrardatos" del objeto retornado en COMANDOMKDISK

}
| prmdisk COMANDORMDISK{
printf("Eliminacion de disco inicializada \n");
$2->eliminarDisco($2);
}
| pfdisk COMANDOFDISK{
    $2->resultado_particion($2);
}
| pmount COMANMOUNT {
    $2->resultado_mount($2);
}
| prep COMANDOREP{

}

;

COMANDOMKDISK:
COMANDOMKDISK menos psize igual entero {int tam=atoi($5); $1->size=tam; $$=$1;}
|menos psize igual entero {int tam = atoi($4); obmkdisk *disco=new obmkdisk();disco->size=tam; $$=disco; }
|COMANDOMKDISK menos psize igual menos entero {int tam=atoi($6)*-1; $1->size=tam; $1->sizeNegativo = true; $$=$1;}
|menos psize igual menos entero{int tam = atoi($5)*-1; obmkdisk *disco=new obmkdisk(); disco->size=tam; disco->sizeNegativo = true ; $$=disco; }
|COMANDOMKDISK menos punity igual unidades {$1->unity=$5; $$=$1; }
|menos punity igual unidades {obmkdisk *disco=new obmkdisk(); disco->unity=$4; $$= disco; }
|COMANDOMKDISK menos pfit igual ajustes {$1->fit =$5 ; $$=$1;}
|menos pfit igual ajustes {obmkdisk *disco=new obmkdisk(); disco->fit= $4 ; $$=disco;}
|COMANDOMKDISK menos ppath igual DIRECCION {$1->line=$5; $$=$1;}
|menos ppath igual DIRECCION  {obmkdisk *disco=new obmkdisk(); disco->line = $4 ; $$=disco;};


COMANDORMDISK:
menos ppath igual DIRECCION {obmkdisk *disco=new obmkdisk(); disco->line =$4 ; $$ = disco;};

DIRECCION : ruta {strcpy($$, $1);}
|cadena {strcpy($$ , $1);};

COMANDOFDISK:
COMANDOFDISK menos psize igual entero {int tam=atoi($5); $1->size=tam; $$=$1;}
|menos psize igual entero {int tam = atoi($4); fdisk *f_disk=new fdisk();f_disk->size=tam; $$=f_disk; }
|COMANDOFDISK menos psize igual menos entero {int tam=atoi($6)*-1; $1->size=tam; $1->sizeNegativo = true; $$=$1;}
|menos psize igual menos entero{int tam = atoi($5)*-1; fdisk *f_disk=new fdisk(); f_disk->size=tam; f_disk->sizeNegativo = true ; $$=f_disk; }
|COMANDOFDISK menos punity igual unidades {$1->unity=$5; $$=$1; }
|menos punity igual unidades {fdisk *f_disk=new fdisk(); f_disk->unity=$4; $$= f_disk; }
|COMANDOFDISK menos pfit igual ajustes {$1->fit =$5 ; $$=$1;}
|menos pfit igual ajustes {fdisk *f_disk=new fdisk(); f_disk->fit= $4 ; $$=f_disk;}
|COMANDOFDISK menos ppath igual DIRECCION {$1->path=$5; $$=$1;}
|menos ppath igual DIRECCION  {fdisk *f_disk=new fdisk(); f_disk->path = $4 ; $$=f_disk;}
|COMANDOFDISK menos pname igual identificador  {QString name_ = $5; $1->name = name_ ; $$=$1;  }
|menos pname igual identificador {fdisk *f_disk=new fdisk(); f_disk->name = $4 ; $$=f_disk;}
|COMANDOFDISK menos pname igual prueba {QString name_ = $5; $1->name = name_ ; $$=$1;  }
|menos pname igual prueba {fdisk *f_disk=new fdisk(); f_disk->name = $4 ; $$=f_disk;}
|COMANDOFDISK menos ptype igual identificador {QString type_ = $5; $1->type = type_ ; $$=$1;  }
|menos ptype igual identificador {fdisk *f_disk=new fdisk(); f_disk->type = $4 ; $$=f_disk;}
|COMANDOFDISK menos pdelete igual identificador {QString type_of_del = $5; $1->type_of_delete = type_of_del;$1->delete_partition = true ; $$=$1;  }
|menos pdelete igual identificador {fdisk *f_disk=new fdisk(); f_disk->type_of_delete = $4;f_disk->delete_partition = true ; $$=f_disk;}
|COMANDOFDISK menos padd igual entero {int tam = atoi($5); $1->sizeadd = tam;$1->add = true ; $$=$1;  }
|menos padd igual entero {int tam = atoi($4); fdisk *f_disk=new fdisk(); f_disk->sizeadd = tam;f_disk->add = true ; $$=f_disk;}
|COMANDOFDISK menos padd igual menos entero {int tam = atoi($5); $1->sizeadd = tam;$1->add = true;$1->sizeNegativo = true ; $$=$1;  }
|menos padd igual menos entero {int tam = atoi($5)*-1; fdisk *f_disk=new fdisk(); f_disk->sizeadd = tam;f_disk->add = true;f_disk->sizeNegativo = true ; $$=f_disk;};

COMANMOUNT:
COMANMOUNT menos ppath igual DIRECCION {$1->path = $5 ; $$=$1; }
|menos ppath igual DIRECCION {mount *m_mount= new mount(); m_mount->path = $4; $$=m_mount; }
|COMANMOUNT menos pname igual identificador{$1->name_partition = $5 ; $$=$1;}
|menos pname igual identificador{mount *m_mount =new mount(); m_mount->name_partition= $4; $$=m_mount; }

COMANDOREP:
igual DIRECCION {reportes *rep = new reportes(); rep->reporte_mbr($2);}

