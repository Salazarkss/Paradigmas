#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct Palabra{//estructura que agrupa cada palara(char*) y linkea con su arreglo de enteros que representan los documentos. 1=esa palabra está en el documento, 0 no está.
	int largo;//largo de la palabra
	int numeroDocs;//numero de documentos en el archivo
	int palabras;//numero de palabras en el indice;
	char *nombre;
	int *documentos;
	char *documento;
} Palabra;

typedef struct Index{
	int cantidadPalabras;
	int cantidadDocs;
	Palabra *palabras;
}Index;

typedef struct StopWords{
	int cantidad;
	char** stopwords;
}StopWords;

typedef struct Ranking{
	int numeroDocumento;
	int palabrasContenidas;
	char *documento;
}Ranking;	

typedef struct Result{
	char *Indice;
	char *Autor;
	char *Titulo;
	char *Cuerpo;
	Ranking ranking;
}Result;

char* leerArchivo(char* nombre){
	FILE *archivo;
	int cantidad=0;
	archivo=fopen(nombre, "r");
	if(archivo==NULL){
		exit(1);
	}
	while(feof(archivo)==0){
		fgetc(archivo);
		cantidad++;
	}
	rewind(archivo);
	char *aux=(char*)malloc(sizeof(char)*cantidad);
	int i=0;
	while(feof(archivo)==0){
		aux[i]=fgetc(archivo);
		i++;
	}
	return aux;
}
int determinarTamanoTexto(char* texto){
	int i;
	for(i=0; texto[i]!=EOF;i++){}
	return i;
}
int length(char* palabra){
	int i=0;
	if(palabra==NULL){
		return 0;
	}
	while(palabra[i]!='\0'){
			i++;
	}
	return i;
}
int calza(char* stop, char* palabra){
	int a=length(stop);
	int b=length(palabra);
	int comprueba=0;
	for(int i=0;i<a;i++){
		if(stop[i]==palabra[i]){
			comprueba++;
		}
	}
	if(comprueba==a && comprueba==b){
		return 1;
	}
	else{
		return 0;
	}
}
int esStop(StopWords sw, char* palabra){
	char **stops=sw.stopwords;
	char *aux;
	if(length(palabra)<3){
		return 2;
	}
	for(int i=1; i<=sw.cantidad; i++){
		int comprueba=calza(stops[i], palabra);
		if(comprueba==1){
			return 1;
		}
	}
	return 0;
}
char* sacarNoLetras(char* palabra){
	char abecedadario[26]={'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
	int largo=length(palabra);
	int letras=0;
	for(int i=0;i<largo;i++){
		for(int j=0; j<26;j++){
			if(palabra[i]==abecedadario[j]){
				letras++;
			}
		}
	}
	int aux=letras;
	char *filtrada=(char*)malloc(sizeof(char)*letras);
	for(int i=0;i<largo;i++){
		for(int j=0;j<26;j++){
			if(palabra[i]==abecedadario[j]){
				filtrada[aux-letras]=palabra[i];
				letras--;
			}
		}
	}
	return filtrada;
}
char** encontrarPalabras(char* texto, int largo, StopWords sw, int **cantidad){// funcion que filtra las palabras, quitando los \r \n, las stopswords, las ','
//los '.', etc. aún falta poder quitar los paréntesis y estaríamos ok. luego habría que empezar a hacer el proximo paso de generar los index
// y terminar de afinar el filtro de palabras para evitar errores mas adelante. SOBRE TODO CON LAS COMAS, PARENTESIS, ETC.
	char aux[largo];
	strcpy(aux, texto);
	int c=0;
	for(int i=0; i<largo;i++){
		if(aux[i]==' '){
			c++;
		}
	}
	char **listaPrevia1=(char**)malloc(sizeof(char*)*c);//se genera la primera lista solo con el filtro de los espacios
	char **listaPrevia2=(char**)malloc(sizeof(char*)*c);//se genera una segunda lista con las Stops filtradas
	char *ptr;
	ptr=strtok(aux," ");
	listaPrevia1[0]=ptr;
	int i=1;
	while( (ptr = strtok(NULL," ")) != NULL ){
   			listaPrevia1[i]=ptr;
   			i++;
    }
    int contador=0;
    char *ptr2;
    char *ptr3;
    char *ptr4;
    char *ptr5;
    int es;
    for(int j=0; j<i;j++){
    	char *a=listaPrevia1[j];
   		if(strchr(a,'\r')!=NULL){
   			ptr2=strtok(a,"\r");
   			ptr3=strtok(NULL,"\r");
	   		ptr4=strtok(ptr2,"\n");
	   		ptr5=strtok(ptr3,"\n");
	   		ptr4=sacarNoLetras(ptr4);
	   		ptr5=sacarNoLetras(ptr5);
	   		es=esStop(sw, ptr4);
	   		if(es==0){
	   			if(ptr4!=NULL){
					listaPrevia2[contador]=ptr4;
	   				contador++;
	   			}
	   		}
	   		es=esStop(sw, ptr5);
	   		if(es==0){
	   			if(ptr5!=NULL){
					listaPrevia2[contador]=ptr5;
	   				contador++;
	   			}
	   		}
	   	}
	   	else if(strchr(a,'\n')!=NULL){
	   		ptr2=strtok(a,"\n");
	   		ptr2=sacarNoLetras(ptr2);
	   		es=esStop(sw, ptr2);
	   		if(es==0){
	   			if(ptr2!=NULL){
					listaPrevia2[contador]=ptr2;
	   				contador++;
	   			}
	   		}
	   	}
	   	else{
	   		a=sacarNoLetras(a);
	   		es=esStop(sw, a);
	   		if(es==0){
	   			if(a!=NULL){
					listaPrevia2[contador]=a;
	   				contador++;
	   			}
	   		}
	   	}
	}
    char **listaFinal=(char**)malloc(sizeof(char*)*contador+1);//se genera una tercera lista sin palabras repetidas
    listaFinal[0]=listaPrevia2[0];
    int final=1;
    int count=0;
    char *a;
    char *b;
    for(int i=0; i<contador;i++){
    	count=0;
    	a=listaPrevia2[i];
    	for(int j=0;j<final;j++){
    		b=listaFinal[j];
    		int comprueba=calza(a,b);
    		if(comprueba==1){
    			count++;
    		}
    	}
    	if(count==0){
    		listaFinal[final]=a;
    		final++;
    	}
    }
    **cantidad=final;
    return listaFinal;
}

StopWords* leerSW(char* texto, int largo){
	StopWords sw;
	int i=1;
	char aux[largo];
	strcpy(aux,texto);
	char **lista=(char**)malloc(sizeof(char*)*320);
	char *ptr1;
	ptr1=strtok(aux,"\r\n");
	lista[0]=ptr1;
	while((ptr1=strtok(NULL,"\r\n")) != NULL){
		ptr1=sacarNoLetras(ptr1);
		lista[i]=ptr1;
		i++;
	}
	sw.stopwords=lista;
	sw.cantidad=320;
	StopWords *stop=(StopWords*)malloc(sizeof(StopWords));
	stop[0]=sw;
	return stop;
}
char* generarDocumentos(int pos, int largo, char aux[largo]){
	int contador=0;
	pos++;
	int i;
	int count;
	char *ptr;
	int k;
	for(i=0; i<largo-2;i++){
		if(aux[i]=='.' && aux[i+1]=='I' && aux[i+2]==' '){
			pos--;
		}
		if(pos==0){
			count=i;
			i++;
			for(k=i; k<largo-2;k++){
				if(aux[k]=='.' && aux[k+1]=='I' && aux[k+2]==' '){
					contador=k-count;
					k=largo-2;
				}
			}
			if(aux[k+1]==EOF){
				contador=largo-count;
			}
			ptr=(char*)malloc(sizeof(char)*contador);
			i=largo-2;
		}
	}
	k=0;
	for(int j=count;j<contador+count;j++){
		ptr[k]=aux[j];
		k++;
	}
	return ptr;
}
int encontrarPalabra(char *palabra, char *documento) 
{

    int i, j; 
    int l_documento=length(documento); 
    int l_palabra= length(palabra); 
    if(l_documento< l_palabra){ 
      return 0; 
    }
    for(i=0; i < l_documento; i++){ 
       if(documento[i]==palabra[0]){ 
           if(l_documento-i < l_palabra){ 
              return 0; 
            }
            for(j=0; j < l_palabra; j++){ 
                if(documento[i+j]!=palabra[j]){ 
                    break;
                }
            }
            if(j==l_palabra){
            	
               return 1;
            } 
        }
    }
    return 0; 
}
Index* createIndex(char* nombreArchivo, StopWords* sw){
	char *texto=leerArchivo(nombreArchivo);
	int l=strlen(texto);
	int *cantidad1=(int*)malloc(sizeof(int));
	char **palabras=encontrarPalabras(texto, l, *sw, &cantidad1);
	int cantidad=*cantidad1;
	Palabra *palabrotas=(Palabra*)malloc(sizeof(Palabra)*cantidad);
	FILE *archivo;
	archivo=fopen(nombreArchivo, "r");
	int numeroDocs=0;
	int caracteres=0;
	if(archivo==NULL){
		exit(1);
	}
	while(feof(archivo)==0){
		fgetc(archivo);
		caracteres++;
	}
	char aux[caracteres];
	rewind(archivo);
	int p=0;
	while(feof(archivo)==0){
		aux[p]=fgetc(archivo);
		p++;
	}
	for(int i=0; i<caracteres-2;i++){
		if(aux[i]=='.' && aux[i+1]=='I' && aux[i+2]==' '){
			numeroDocs++;
		}
	}
	char **documentos=(char**)malloc(sizeof(char*)*numeroDocs);
	int q=0;
	for(int i=0; i<numeroDocs;i++){
		documentos[q]=generarDocumentos(q, caracteres, aux);
		q++;
	}
	int k=0;
	int esta;
	for(int i=0; i<cantidad;i++){
		int *incidencia=(int*)malloc(sizeof(int)*numeroDocs);
		for(int j=0;j<numeroDocs;j++){
			//printf("%s\n", palabras[i]);
			esta=encontrarPalabra(palabras[i], documentos[j]);
			if(esta==1){
				incidencia[j]=1;
			}
			else if(esta==0){
				incidencia[j]=0;
			}
		}
		Palabra singular;
		singular.largo=length(palabras[i]);
		singular.numeroDocs=numeroDocs;
		singular.nombre=(char*)malloc(sizeof(char)*singular.largo);
		singular.nombre=palabras[i];
		singular.nombre=NULL;
		singular.nombre=palabras[i];
		singular.documentos=incidencia;
		singular.palabras=cantidad;
		palabrotas[k]=singular;
		k++;
	}
	for(int i=0; i<cantidad;i++){
		palabras[i]=NULL;
		free(palabras[i]);
	}
	free(palabras);
	for(int i=0; i<numeroDocs; i++){
		palabrotas[i].documento=documentos[i];
	}
	for(int i=0; i<numeroDocs; i++){
		free(documentos[i]);
	}
	free(documentos);
	Index *index=(Index*)malloc(sizeof(Index));
	index[0].cantidadPalabras=cantidad;
	index[0].cantidadDocs=numeroDocs;
	index[0].palabras=(Palabra*)malloc(sizeof(Palabra)*k);
	for(int i=0; i<k;i++){
		index[0].palabras[i]=palabrotas[i];
	}
	return index;
}

Ranking* query(Index *in, StopWords *sw, char* entrada){
	Palabra *index=in[0].palabras;
	char **palabras1;
	char **palabras2;
	char *ptr;
	int i;
	int count=1;
	for(i=0; i<length(entrada);i++){
		if(entrada[i]==' '){
			count++;
		}
	}
	palabras1=(char**)malloc(sizeof(char*)*count);
	i=1;
	ptr=strtok(entrada," ");
	palabras1[0]=ptr;
	while((ptr = strtok(NULL," "))!=NULL){//se filtra la entrada del usuario por los espacios y se guarda en palabras1
		palabras1[i]=ptr;
		i++;
	}
	printf("%s\n", palabras1[0]);
	for(i=0; i<count; i++){
		palabras1[i]=sacarNoLetras(palabras1[i]);
		printf("%s\n", palabras1[i]);
	}
	int k=0;
	for(i=0; i<count;i++){
		int es=esStop(*sw, palabras1[i]);
		if(es==0){
			k++;
		}
	}
	palabras2=(char**)malloc(sizeof(char*)*k);
	k=0;
	for(i=0; i<count;i++){
		int es=esStop(*sw, palabras1[i]);
		if(es==0){
			palabras2[k]=palabras1[i];
			palabras2[k]=sacarNoLetras(palabras2[k]);
			printf("%s\n", palabras2[k]);
			k++;	
		}
	}
	free(palabras1);
	Palabra auxiliar=index[0];
	int cantidad=auxiliar.palabras;
	Palabra *index1=(Palabra*)malloc(sizeof(Palabra)*k+1);
	int w=0;
	int j;
	int p;
	printf("aqui va\n");
	for(j=0; j<k; j++){
		for(p=0; p<cantidad; p++){
			auxiliar=index[p];
			if(encontrarPalabra(palabras2[j], auxiliar.nombre)==1){
				printf("%s - %i\n", auxiliar.nombre, p);
				index1[w]=auxiliar;
				w++;
			}
		}
	}
	Ranking *r=(Ranking*)malloc(sizeof(Ranking)*(auxiliar.numeroDocs));
	int numeroDocs=auxiliar.numeroDocs;
	int puntaje=0;
	int q=0;
	for(k=0; k<numeroDocs;k++){
		puntaje=0;
		for(int i=0; i<w;i++){
			auxiliar=index1[i];
			puntaje=puntaje+(auxiliar.documentos[k]);
			printf("documento%i sumo%i\n",k, auxiliar.documentos[k]);
		}
		Ranking ran;
		ran.numeroDocumento=k+1;
		ran.palabrasContenidas=puntaje;
		ran.documento=index[k].documento;
		r[q]=ran;
		q++;
	}
	int c, d;
	Ranking swap;
	for(c=0;c<(numeroDocs-1);c++)
  	{
    	for (d=0;d<numeroDocs-c-1;d++)
    	{
      		if (r[d].palabrasContenidas>r[d+1].palabrasContenidas)
      		{
        		swap=r[d];
        		r[d]=r[d+1];
        		r[d+1]=swap;
        	}
      	}
    }
	//recordar preguntarle al cristian especificaciones sobre las funciones saveIndex y LoadIndex, sobre todo
	//en que momento se usaran y como se deben guardar los archivos. y preguntar si eso tambien aplica para save
	// y load ranking.
	return r;
}

void displayResults(Ranking *r, int TopK){
	for(int i=0; i<TopK;i++){
		printf("%s\n", r[i].documento);
	}
}
// se debe agregar el statusCode ENUM, para eso agregar errores como no lectura de archivo, archivo inexistente, problema con la generacion de id, id incorrecto
int main(int argc, char *argv[])
{
	char *nombreArchivo=(char*)malloc(sizeof(char)*100);
	printf("Ingrese el nombre de su archivo (recuerde agregar el '.txt' al final del nombre): \n");
	scanf("%s", nombreArchivo);
	char *documentos=leerArchivo(nombreArchivo);
	int largoDocs=determinarTamanoTexto(documentos);
	char *nombreStops=(char*)malloc(sizeof(char)*100);
	printf("Ingrese el nombre de sus StopWords (recuerde agregar el '.txt' al final del nombre): \n");
	scanf("%s", nombreStops);
	char *stopwords=leerArchivo(nombreStops);
	int largoStops=determinarTamanoTexto(stopwords);
	StopWords *sw=leerSW(stopwords, largoStops);
	Index *i=createIndex(nombreArchivo, sw);
	char texto;
	printf("Ingrese su consulta\n");
	scanf("%s", &texto);
	Ranking *ran=query(i, sw, &texto);
	int Top;
	printf("Ingrese el numero de documentos a mostrar\n");
	scanf("%i", &Top);
	displayResults(ran, Top);
	return 0;
}
