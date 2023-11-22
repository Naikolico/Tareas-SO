#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define NUM_HIJOS 4

//struct Laberinto, para representar un laberinto 5x5 de manera mas sencilla
typedef struct {
    char laberinto[5][5];
} Laberinto;


//struct Tp_jugador, que representa una conexion de tps para jugador N, indicando los laberintos a donde realizar tp
//(indice_laberinto1 y indice_laberinto2) y las coordenadas en donde se generó cada tp (fila_jugador_tp1
//, columna_jugador_tp1 y fila_jugador_tp2, columna_jugador_tp2 respectivamente)
typedef struct{
    int indice_laberinto1;
    int indice_laberinto2;
    int fila_jugador_tp1;
    int columna_jugador_tp1;
    int fila_jugador_tp2;
    int columna_jugador_tp2;
} Tp_jugador;


//struct Enlace, que representa enlaces entre los disntintos laberintos.
//int indice_laberinto_actual correspondiente al laberinto donde se encuentra actualmente el jugador
//int indice_laberinto_sig correspondiente al laberinto donde el jugador se debe mover
//int fila_jugador_enlace correspondiente a la fila del laberinto actual del enlace
//int columna_jugador_enlace correspondiente a la columna del laberinto actual del enlace

//Tener en cuenta que pueden existir 4 enlaces para un mismo laberinto, por eso en la data
//del juego se guardan 2 enlaces por cada laberinto exitoso encontrado
typedef struct {
    int indice_laberinto_actual;
    int indice_laberinto_sig;
    int fila_jugador_enlace;
    int columna_jugador_enlace;
} Enlace;


//struct Datos, que representa los datos del juego:
//Laberinto arreglo[9], es la representacion del tablero, aqui se almacenan los 9 laberintos (8 tableros y el inicial)
//Enlace enlaces_laberintos[16], Arreglo que contiene las conexiones entre laberintos para ir pasando de uno a otro
//int pos_enlace, Indice o numero que indica en que posicion de llenado nos encontramos en enlaces_laberintos
//int rondas_extra, rondas extras totales que han ganado los jugadores durante la partida
//int rondas_perdidas, rondas perdidas totales que han perdido los jugadores
//int tesoros_generados, arreglo guia para verificar si es que todos los tesoros han sido generados o si existe algun jugador que
//                     le falta generar su tesoro, esto se indica por posicion del arreglo(jugador), 1 si se genero su tesoro 0 si no.
//int tp_generados, arreglo de tps generados por jugador, indica por posicion (jugador) cuantos tp lleva generados, 0 al 2
//int tesoros_encontrados, arreglo guia para identificar que jugador recogio su tesoro, la posicion del arreglo indica indica al jugador y contiene
//                         1 o 0 representado si el jugador tiene o no tiene el tesoro respectivamente
//int jugadores_laberinto_inicial[4], arreglo guia para identificar que jugador está dentro del laberinto inicial, la posicion del arreglo
//                                    indica indica al jugador y contiene 1 o 0 representado si el jugador está o no dentro del Laberinto inicial 
//                                    respectivamente
//int cant_camaras_juego, numero que representa la cantidad de camaras generadas en el juego
//Tp_jugador casillas_tp[4], arreglo de struct Tp_jugador, contiene los enlaces de tp de cada jugador, la posicion del arreglo indica el jugador
typedef struct {
    Laberinto arreglo[9];
    Enlace enlaces_laberintos[16];
    int pos_enlace; //Indice lista enlaces
    int rondas_extra;
    int rondas_perdidas;
    int tesoros_generados[4]; //pos 0, tesoro 1, pos 1, tesoro 2, etc
    int tp_generados[4]; //pos 0, tps player1, pos 1, tps player2, etc
    int tesoros_encontrados[4]; //posicion representa jugador, 1 tiene tesoro, 0 no tiene
    int jugadores_laberinto_inicial[4]; //posicion representa jugador, 1 está en Laberinto inicial, 0 no lo esta
    int cant_camaras_juego;
    Tp_jugador casillas_tp[4];//tps de cada jugador del 0 al 3
} Datos;


//struct Jugador_data, que representala informacion de cada jugador:
//Laberinto labe_act, laberinto actual donde se encuentra actualmente el jugador
//int indice_laberinto, indice del laberinto donde se encuentra actualmente el jugador
//int turno, numero que indica el turno del jugador
//int coordenada[2], arreglo que contiene en [0] fila y en [1] columna la coordenada actual del jugador
//int coordenada_opuesta[2], arreglo que contiene en [0] fila y en [1] columna la coordenada actual opuesta del jugador
//int mazo, numero que indica la carta del jugador, 1 Buscar y 2 Escaleras
//char casilla_anterior, representa la casilla del laberinto que el jugador pisa actualmente.
typedef struct{
    Laberinto labe_act;
    int indice_laberinto;
    int turno;
    int coordenada[2];
    int coordenada_opuesta[2];
    int mazo;  //1 buscar, 2 escaleras
    char casilla_anterior;
}Jugador_data;



//char mapear(char valor):
//Funcion auxiliar para ayudar a cambiar las letras compuestas que representaban jugadores
//en el laberinto inicial, por ej, J1 pasa a ser 'W'.
//Parametros:
//char valor: caracter que es un numero
char mapear(char valor) {
    switch (valor) {
        case '1': return 'W';
        case '2': return 'X';
        case '3': return 'Y';
        case '4': return 'Z';
        default: return valor;
    }
}


//void imprimirLaberinto(Laberinto lab):
//Funcion auxiliar para imprimir un laberinto
//Parametros:
//Laberinto lab: Laberinto de tipo Laberinto
void imprimirLaberinto(Laberinto lab) {
    for (int fila = 0; fila < 5; fila++) {
        for (int columna = 0; columna < 5; columna++) {
            printf("%c ", lab.laberinto[fila][columna]);
        }
        printf("\n");
    }
}

//void funcionar_casilla(Datos *juego_data, char tipo_casilla, Jugador_data *p_data, char player):
//Funcion para funcionamiento de casillas especiales del juego, modificando los datos del jugador o del juego
//si es necesario
//Parametros:
//Datos *juego_data: Datos del juego
//char tipo_casilla: Tipo de casilla especial a hacer funcionar
//Jugador_data *data: Datos del jugador que esta haciendo funcionar la casilla
//char player: Caracter que representa al jugador
void funcionar_casilla(Datos *juego_data, char tipo_casilla, Jugador_data *p_data, char player){
    int suma_casilla = 0;
    int tp_player = -1;
    int lab1 = -1;
    int lab2 = -1;
    if(tipo_casilla == 'C'){
        //Casilla de camara
        printf("Pasaste por una camara!!\n");
        printf("Ya no molestará nunca más...\n");
        
        //Logica de eliminacion de casilla C
        //Pongo casilla_anterior = 0 simulando que rompi la camara
        juego_data->cant_camaras_juego = juego_data->cant_camaras_juego - 1;
        p_data->casilla_anterior = '0';
        
    }else if (tipo_casilla == 'T'){
        //Casilla de obtener 5 turnos
        printf("Pasaste por una Casilla para ganar rondas!!\n");
        //Verificar si en los laberintos generados no hay mas de 2 camaras
        
       suma_casilla = juego_data->cant_camaras_juego;
        
        if(suma_casilla < 2){
            juego_data->rondas_extra = juego_data->rondas_extra + 5;
            printf(".. Todos los jugadores reciben 5 rondas extras!!!\n");
            p_data->casilla_anterior = '0';
        }else{
            printf("... Pero hay camaras en los laberintos que impiden que las ganes...\n");
        }
    }else if (tipo_casilla == 'N'){
        //Casilla de perder 3 turnos
        juego_data->rondas_perdidas = juego_data->rondas_perdidas + 3;
        printf("NoOooO.... Todos los jugadores pierden 3 rondas !!! :C \n");
        p_data->casilla_anterior = '0';
    }else if (tipo_casilla == 'w' || tipo_casilla == 'x' || tipo_casilla == 'y' || tipo_casilla == 'z'){
        //Casilla de tp de jugador
        //Aqui se traduce el jugador:
        if(player == 'W'){
            tp_player = 0;
        }else if (player == 'X'){
            tp_player = 1;
        }else if (player == 'Y'){
            tp_player = 2;
        }else{
            tp_player = 3;
        }
        
        //Se miran los mapas para ver donde esta la casilla a teletransportarse
        lab1 = juego_data->casillas_tp[tp_player].indice_laberinto1;
        lab2 = juego_data->casillas_tp[tp_player].indice_laberinto2;
        
        if(lab1 != -1 && lab2 != -1){
            //Hay casillas para tepear!
            if(p_data->indice_laberinto == lab1){
                //El jugador esta en el laberinto del enlace tp1
                p_data->coordenada[0] = juego_data->casillas_tp[tp_player].fila_jugador_tp2;
                p_data->coordenada[1] = juego_data->casillas_tp[tp_player].columna_jugador_tp2;
                p_data->indice_laberinto = lab2;
            }else{
                //El jugador esta en el laberinto del enlace tp2
                p_data->coordenada[0] = juego_data->casillas_tp[tp_player].fila_jugador_tp1;
                p_data->coordenada[1] = juego_data->casillas_tp[tp_player].columna_jugador_tp1;
                p_data->indice_laberinto = lab1;
            }
            
            p_data->coordenada_opuesta[0] = 4 - p_data->coordenada[0];
            p_data->coordenada_opuesta[1] = 4 - p_data->coordenada[1];
            p_data->casilla_anterior = tolower(player); //Transformo a letra minuscula el jugador
            printf("El jugador %d se ha teletransportado al laberinto %d! \n", tp_player+1, p_data->indice_laberinto);
            
        }else{
            //No hay tps generados aun
            printf("Aun no está disponible el tp para el jugador %d...\n", tp_player+1);
        }
        
    }else{
        //tomar tesoro y actualizar data
        //Aqui se traduce el jugador:
        if(player == 'W'){
            tp_player = 0;
        }else if (player == 'X'){
            tp_player = 1;
        }else if (player == 'Y'){
            tp_player = 2;
        }else{
            tp_player = 3;
        }
        
        if(tipo_casilla == '1' && player == 'W'){
            //El jugador 1 toma su tesoro!
            p_data->casilla_anterior = '0';
            juego_data->tesoros_encontrados[tp_player] = 1;
            printf("El jugador %d tomo su tesoro!!\n", tp_player+1);
        }else if(tipo_casilla == '2' && player == 'X'){
            //El jugador 2 toma su tesoro!
            p_data->casilla_anterior = '0';
            juego_data->tesoros_encontrados[tp_player] = 1;
            printf("El jugador %d tomo su tesoro!!\n", tp_player+1);
        }else if(tipo_casilla == '3' && player == 'Y'){
            //El jugador 3 toma su tesoro!
            p_data->casilla_anterior = '0';
            juego_data->tesoros_encontrados[tp_player] = 1;
            printf("El jugador %d tomo su tesoro!!\n", tp_player+1);
        }else if(tipo_casilla == '4' && player == 'Z'){
            //El jugador 4 toma su tesoro!
            p_data->casilla_anterior = '0';
            juego_data->tesoros_encontrados[tp_player] = 1;
            printf("El jugador %d tomo su tesoro!!\n", tp_player+1);
        }else{
            //El jugador no puede tomar el tesoro
            printf("El jugador %d no puede tomar el tesoro del jugador %c\n", tp_player+1, tipo_casilla);
        }
        
    }
}

//void generar_laberinto(Laberinto *lab, int *arreglo_tesoros, int *arreglo_tp, int cant_laberintos_generados, int *camaras, Tp_jugador *tp_player, int indice_laberinto_gen):
//Funcion para generar las casillas especiales al descubrir un nuevo laberinto
//Parametros:
//Laberinto *lab: Laberinto seleccionado para generar las casillas
//int *arreglo_tesoros: Arreglo de tesoros para verificar que jugador tiene o no tesoro generado
//int *arreglo_tp: Arreglo de tp para verificar que jugador tiene o no tp generado
//int cant_laberintos_generados: numero de enlaces actuales que tiene el laberinto, para calcular cuantos laberintos hay actualmente
//int *camaras: cantidad de camaras activas actualmente en el juego
//Tp_jugador *tp_player: Arreglo de Tp_jugador de parte de la data del juego por si se genera un tp
//int indice_laberinto_gen: Numero representativo del laberinto a generar
void generar_laberinto(Laberinto *lab, int *arreglo_tesoros, int *arreglo_tp, int cant_laberintos_generados, int *camaras, Tp_jugador *tp_player, int indice_laberinto_gen){
    char casilla_especial = '?';
    
    int aceptable = 1;
    int suma = 0;
    int num_aleatorio2;
    int tp = 0; //Indica si es que se metio generar un tp cualquiera o no
        
    for(int cant = 0; cant < 4; cant++){
        suma = suma + arreglo_tesoros[cant];
    }
        
    if(suma == 4){
        //Se generaron todos los tesoros, entonces se genera una casilla especial aleatoria
        //Generar numero aleatorio del 0 al 3 para ver que tipo de casilla Generar
        num_aleatorio2 = rand() % 4;
            
        if(num_aleatorio2 == 0){
            //Casilla de camara
            casilla_especial = 'C';
            (*camaras)++; //Aumenta la cantidad de camaras en 1
            
        }else if (num_aleatorio2 == 1){
            //Casilla de obtener 5 turnos
            casilla_especial = 'T';
        }else if (num_aleatorio2 == 2){
            //Casilla de perder 3 turnos
            casilla_especial = 'N';
        }else{
            //Casilla de tp de jugador N
            tp = 1;
            //Los tp seran representados por la letra minuscula del jugador
                
            //Hay que verificar si es que no existe mas de 2 tp de un mismo tipo
            while(aceptable){
                num_aleatorio2 = rand() % 4;
                    
                if(arreglo_tp[num_aleatorio2] < 2){
                    //El jugador no tiene mas de 2 tps generados!
                    if(num_aleatorio2 == 0){
                        casilla_especial = 'w';
                    }else if(num_aleatorio2 == 1){
                        casilla_especial = 'x';
                    }else if(num_aleatorio2 == 2){
                        casilla_especial = 'y';
                    }else{
                        casilla_especial = 'z';
                    }
                    
                        
                    arreglo_tp[num_aleatorio2] = arreglo_tp[num_aleatorio2] + 1;
                    aceptable = 0;
                }
            }
        }
        
    }else if( (8 -(cant_laberintos_generados/2)) == (4 - suma)){
        //Hay que generar si o si un tesoro
        while(aceptable){
            //Generar numero aleatorio del 0 al 3 y comprobar si el tesoro no fue generado antes
            num_aleatorio2 = rand() % 4;
            if(arreglo_tesoros[num_aleatorio2] == 0){
                //El tesoro no se ha generado aun!
                if(num_aleatorio2 == 0){
                    casilla_especial = '1';
                }else if(num_aleatorio2 == 1){
                    casilla_especial = '2';
                }else if(num_aleatorio2 == 2){
                    casilla_especial = '3';
                }else{
                    casilla_especial = '4';
                }
                //Se agrega 1 al arreglo para avisar que ese jugador ya genero su tesoro
                arreglo_tesoros[num_aleatorio2] = 1;
                aceptable = 0;
            }
        }
    }else{
        //Se mira que se genera, si tesoro o casilla_especial
        //Tesoro o casilla especial?
        int num_aleatorio = rand() % 2; //Probabilidad del 50%
        aceptable = 1;
        casilla_especial = '*';
        if(num_aleatorio == 0){
            //casilla_especial!
            //Generar numero aleatorio del 0 al 3 para ver que tipo de casilla Generar
            num_aleatorio2 = rand() % 4;
            if(num_aleatorio2 == 0){
                //Casilla de camara
                casilla_especial = 'C';
                (*camaras)++; //Aumenta la cantidad de camaras en 1
            }else if (num_aleatorio2 == 1){
                //Casilla de obtener 5 turnos
                casilla_especial = 'T';
            }else if (num_aleatorio2 == 2){
                //Casilla de perder 3 turnos
                casilla_especial = 'N';
            }else{
                //Casilla de tp de jugador N
                tp = 1;
                //Los tp seran representados por la letra minuscula del jugador
                
                //Hay que verificar si es que no existe mas de 2 tp de un mismo tipo
                while(aceptable){
                    num_aleatorio2 = rand() % 4;
                    casilla_especial = '!';
                    if(arreglo_tp[num_aleatorio2] < 2){
                        //El jugador no tiene mas de 2 tps generados!
                        if(num_aleatorio2 == 0){
                            casilla_especial = 'w';
                        }else if(num_aleatorio2 == 1){
                            casilla_especial = 'x';
                        }else if(num_aleatorio2 == 2){
                            casilla_especial = 'y';
                        }else{
                            casilla_especial = 'z';
                        }
                        
                        arreglo_tp[num_aleatorio2] = arreglo_tp[num_aleatorio2] + 1;
                        aceptable = 0;
                    }
                }
            }
        }else{
            //Tesoro!
            while(aceptable){
                //Generar numero aleatorio del 0 al 3 y comprobar si el tesoro no fue generado antes
                num_aleatorio2 = rand() % 4;
                if(arreglo_tesoros[num_aleatorio2] == 0){
                    //El tesoro no se ha generado aun!
                    if(num_aleatorio2 == 0){
                        casilla_especial = '1';
                    }else if(num_aleatorio2 == 1){
                        casilla_especial = '2';
                    }else if(num_aleatorio2 == 2){
                        casilla_especial = '3';
                    }else{
                        casilla_especial = '4';
                    }
                    //Se agrega 1 al arreglo para avisar que ese jugador ya genero su tesoro
                    arreglo_tesoros[num_aleatorio2] = 1;
                    aceptable = 0;
                }
            }
        }
    }
    
    
    aceptable = 1;
    char casilla_seleccionada;
    int fila, columna;
    //Ciclo para generar coordenada aleatoria dentro del laberinto hasta encontrar
    //una casilla utilizable (solo seria '0'):
    
    while(aceptable){
        
        //Generar coordenada aleatoria
        fila = rand() % 5;
        columna = rand() % 5;
        
        casilla_seleccionada = lab->laberinto[fila][columna];
        //Verificar casilla valida
        if(casilla_seleccionada == '0'){
            //La coordenada generada si sirve y se generara la casilla ahi
            aceptable = 0;
            
            if(tp == 1){
                //Logica de "enlaces" de tp:
                if(tp_player[num_aleatorio2].indice_laberinto1 == -1){
                    //Aun no hay enlace desde el primer laberinto, entonces lo creamos
                    tp_player[num_aleatorio2].indice_laberinto1 = indice_laberinto_gen;
                    tp_player[num_aleatorio2].fila_jugador_tp1 = fila;
                    tp_player[num_aleatorio2].columna_jugador_tp1 = columna;
                }else{
                    //Terminamos la conexion del tp de la casilla
                    tp_player[num_aleatorio2].indice_laberinto2 = indice_laberinto_gen;
                    tp_player[num_aleatorio2].fila_jugador_tp2 = fila;
                    tp_player[num_aleatorio2].columna_jugador_tp2 = columna;
                }
            }
        }
    }
    //printf("Las coordenadas que salieron son: %d , %d\n", fila, columna);
    lab->laberinto[fila][columna] = casilla_especial;
}


//char determinarDireccion(int fila, int columna):
//Funcion auxilar para determinar la direccion de la casilla 'b'
//Parametros:
//int fila: fila de la casilla a determinar direccion
//int columna: columna de la casilla a determinar direccion
char determinarDireccion(int fila, int columna) {
    if (fila == 0 && columna == 2) {
        //arriba
        return 'w';
    } else if (fila == 4 && columna == 2) {
        //abajo
        return 's';
    } else if (fila == 2 && columna == 0) {
        //izquierda
        return 'a';
    } else if (fila == 2 && columna == 4) {
        //derecha
        return 'd';
    } else {
        return '0';
    }
}

//void cargarLaberintos(Laberinto laberintos[], int numLaberintos):
//Funcion para cargar los archivos que contienen los laberintos y guardarlos en un arreglo
//Parametros:
//Laberinto laberintos[]: Arreglo de laberintos a rellenar
//int numLaberintos: Cantidad de laberintos a rellenar
void cargarLaberintos(Laberinto laberintos[], int numLaberintos) {
    //Cargar el laberinto inicial en la primera posición
    FILE *archivoInicio = fopen("Inicio.txt", "r");
    if (archivoInicio == NULL) {
        printf("No se pudo abrir el archivo Inicio.txt\n");
        exit(1);
    }

    for (int fila = 0; fila < 5; fila++) {
        for (int columna = 0; columna < 5; columna++) {
            char valor[4];
            fscanf(archivoInicio, " %s", valor);
            char valor_string[2];
            valor_string[0] = valor[0];
            valor_string[1] = '\0';
            if (strcmp(valor_string, "J") == 0) {
                laberintos[0].laberinto[fila][columna] = mapear(valor[1]);
            } else {
                laberintos[0].laberinto[fila][columna] = mapear(valor[0]);
            }
        }
    }

    fclose(archivoInicio);

    // Cargar los demás laberintos
    for (int i = 1; i < numLaberintos; i++) {
        char nombre_archivo[20];
        snprintf(nombre_archivo, sizeof(nombre_archivo), "tablero%d.txt", i);
        FILE *archivoLaberinto = fopen(nombre_archivo, "r");
        if (archivoLaberinto == NULL) {
            printf("No se pudo abrir el archivo %s\n", nombre_archivo);
            exit(1);
        }
        for (int fila = 0; fila < 5; fila++) {
            for (int columna = 0; columna < 5; columna++) {
                fscanf(archivoLaberinto, " %c", &laberintos[i].laberinto[fila][columna]);
            }
        }
        fclose(archivoLaberinto);
    }
}

//int esCasillaValida(char laberinto[5][5], int fila, int columna, char accion, char player):
//Funcion para comprobar si la casilla donde va a caer el jugador es valida segun su carta
//Parametros:
//char laberinto[5][5]: laberinto a comprobar la casilal valida
//int fila: fila donde se movera el jugador
//int columna: columna donde se movera el jugador
//char accion: caracter que represneta la carta del jugador
//char player: caracter que representa el jugador
int esCasillaValida(char laberinto[5][5], int fila, int columna, char accion, char player) {
    if (accion == 'e'){
        if (player == 'W') {
            return (
            laberinto[fila][columna] != '/' &&
            laberinto[fila][columna] != 'W' &&
            laberinto[fila][columna] != 'X' &&
            laberinto[fila][columna] != 'Y' &&
            laberinto[fila][columna] != 'Z' &&
            laberinto[fila][columna] != 'z' &&
            laberinto[fila][columna] != 'y' &&
            laberinto[fila][columna] != 'x');
        } else if (player == 'Y') {
            return (
            laberinto[fila][columna] != '/' &&
            laberinto[fila][columna] != 'W' &&
            laberinto[fila][columna] != 'X' &&
            laberinto[fila][columna] != 'Y' &&
            laberinto[fila][columna] != 'Z' &&
            laberinto[fila][columna] != 'z' &&
            laberinto[fila][columna] != 'w' &&
            laberinto[fila][columna] != 'x');
        } else  if (player == 'X') {
            return (
            laberinto[fila][columna] != '/' &&
            laberinto[fila][columna] != 'W' &&
            laberinto[fila][columna] != 'X' &&
            laberinto[fila][columna] != 'Y' &&
            laberinto[fila][columna] != 'Z' &&
            laberinto[fila][columna] != 'z' &&
            laberinto[fila][columna] != 'y' &&
            laberinto[fila][columna] != 'w');
        } else if (player == 'Z') {
            return (
            laberinto[fila][columna] != '/' &&
            laberinto[fila][columna] != 'W' &&
            laberinto[fila][columna] != 'X' &&
            laberinto[fila][columna] != 'Y' &&
            laberinto[fila][columna] != 'Z' &&
            laberinto[fila][columna] != 'w' &&
            laberinto[fila][columna] != 'y' &&
            laberinto[fila][columna] != 'x');
        }
    } else{
        if (player == 'W') {
            return (
            laberinto[fila][columna] != '/' &&
            laberinto[fila][columna] != 'W' &&
            laberinto[fila][columna] != 'X' &&
            laberinto[fila][columna] != 'Y' &&
            laberinto[fila][columna] != 'Z' &&
            laberinto[fila][columna] != 'z' &&
            laberinto[fila][columna] != 'y' &&
            laberinto[fila][columna] != 'E' &&
            laberinto[fila][columna] != 'x');
        } else if (player == 'Y') {
            return (
            laberinto[fila][columna] != '/' &&
            laberinto[fila][columna] != 'W' &&
            laberinto[fila][columna] != 'X' &&
            laberinto[fila][columna] != 'Y' &&
            laberinto[fila][columna] != 'Z' &&
            laberinto[fila][columna] != 'z' &&
            laberinto[fila][columna] != 'w' &&
            laberinto[fila][columna] != 'E' &&
            laberinto[fila][columna] != 'x');
        } else  if (player == 'X') {
            return (
            laberinto[fila][columna] != '/' &&
            laberinto[fila][columna] != 'W' &&
            laberinto[fila][columna] != 'X' &&
            laberinto[fila][columna] != 'Y' &&
            laberinto[fila][columna] != 'Z' &&
            laberinto[fila][columna] != 'z' &&
            laberinto[fila][columna] != 'y' &&
            laberinto[fila][columna] != 'E' &&
            laberinto[fila][columna] != 'w');
        } else if (player == 'Z') {
            return (
            laberinto[fila][columna] != '/' &&
            laberinto[fila][columna] != 'W' &&
            laberinto[fila][columna] != 'X' &&
            laberinto[fila][columna] != 'Y' &&
            laberinto[fila][columna] != 'Z' &&
            laberinto[fila][columna] != 'w' &&
            laberinto[fila][columna] != 'y' &&
            laberinto[fila][columna] != 'E' &&
            laberinto[fila][columna] != 'x');
        }
    }
    return -1;
}

//void moverJugador(char direccion, int casillas, char player, Jugador_data *jugador, Datos *juego_data):
//Funcion para mover al jugador a una casilla comprobando para no chocarse con un muro, jugador o cualqueir cosa que donde no se le permita pasar
//Parametros:
//char direccion: caracter que representa la direccion a moverse del jugador -> w arriba, s abajo, a izquierda, d derecha
//int casillas: Cantidad de casillas a moverse el jugador
//char player: Caracter que representa el jugador que se movera
//Jugador_data *jugador: Datos del jugador que se movera
//Datos *juego_data:  Datos del juego
void moverJugador(char direccion, int casillas, char player, Jugador_data *jugador, Datos *juego_data) {
    char accion_mazo;
    if (jugador->mazo == 1){
        accion_mazo = 'b';
    } else {
        accion_mazo = 'e';
    }
    //Realizar verificaciones y moverse
    if (direccion == 'w' && jugador->coordenada[0] > 0) {
        // Mover hacia arriba
        juego_data->arreglo[jugador->indice_laberinto].laberinto[jugador->coordenada[0]][jugador->coordenada[1]] = jugador->casilla_anterior;
        //Aqui hay movimiento casilla por casilla
        for (int i = 0; i < casillas; i++) {
            if (accion_mazo == 'b'){
                if (esCasillaValida(juego_data->arreglo[jugador->indice_laberinto].laberinto, jugador->coordenada[0] - 1, jugador->coordenada[1], accion_mazo, player)){
                    (jugador->coordenada[0])--;
                    (jugador->coordenada_opuesta[0])++;
                    jugador->casilla_anterior = juego_data->arreglo[jugador->indice_laberinto].laberinto[jugador->coordenada[0]][jugador->coordenada[1]];
                    if (jugador->casilla_anterior == 'T' || jugador->casilla_anterior == 'C' || jugador->casilla_anterior == 'N'){
                        funcionar_casilla(juego_data, jugador->casilla_anterior, jugador, player);
                        juego_data->arreglo[jugador->indice_laberinto].laberinto[jugador->coordenada[0]][jugador->coordenada[1]] = jugador->casilla_anterior;
                    }
                }  else {
                    i = casillas;
                    printf("Hay un muro de por medio!!!\n");
                }
            } else {
                if (esCasillaValida(juego_data->arreglo[jugador->indice_laberinto].laberinto, jugador->coordenada[0] - 1, jugador->coordenada[1], accion_mazo, player)){
                    (jugador->coordenada[0])--;
                    (jugador->coordenada_opuesta[0])++;
                    jugador->casilla_anterior = juego_data->arreglo[jugador->indice_laberinto].laberinto[jugador->coordenada[0]][jugador->coordenada[1]];
                    if (jugador->casilla_anterior == 'T' || jugador->casilla_anterior == 'C' || jugador->casilla_anterior == 'N'){
                        funcionar_casilla(juego_data, jugador->casilla_anterior, jugador, player);
                        juego_data->arreglo[jugador->indice_laberinto].laberinto[jugador->coordenada[0]][jugador->coordenada[1]] = jugador->casilla_anterior;
                    }
                } else {
                    i = casillas;
                    printf("Hay un muro de por medio!!!\n");
                }
            }  
        }
        //tesoro y tp es casilla final
        jugador->casilla_anterior = juego_data->arreglo[jugador->indice_laberinto].laberinto[jugador->coordenada[0]][jugador->coordenada[1]];
        if (jugador->casilla_anterior == 'w' ||
        jugador->casilla_anterior == 'y' ||
        jugador->casilla_anterior == 'x' ||
        jugador->casilla_anterior == 'z'){
            funcionar_casilla(juego_data, jugador->casilla_anterior, jugador, player);
        }
        juego_data->arreglo[jugador->indice_laberinto].laberinto[jugador->coordenada[0]][jugador->coordenada[1]] = player;
    } else if (direccion == 's' && jugador->coordenada[0] < 4) {
        //Mover hacia abajo
        juego_data->arreglo[jugador->indice_laberinto].laberinto[jugador->coordenada[0]][jugador->coordenada[1]] = jugador->casilla_anterior;
        //Aqui hay movimiento casilla por casilla
        for (int i = 0; i < casillas; i++) {
            if (accion_mazo == 'b'){
                if (esCasillaValida(juego_data->arreglo[jugador->indice_laberinto].laberinto, jugador->coordenada[0] + 1, jugador->coordenada[1], accion_mazo, player)){
                    (jugador->coordenada[0])++;
                    (jugador->coordenada_opuesta[0])--;
                    jugador->casilla_anterior = juego_data->arreglo[jugador->indice_laberinto].laberinto[jugador->coordenada[0]][jugador->coordenada[1]];
                    if (jugador->casilla_anterior == 'T' || jugador->casilla_anterior == 'C' || jugador->casilla_anterior == 'N'){
                        funcionar_casilla(juego_data, jugador->casilla_anterior, jugador, player);
                        juego_data->arreglo[jugador->indice_laberinto].laberinto[jugador->coordenada[0]][jugador->coordenada[1]] = jugador->casilla_anterior;
                    }
                }  else {
                    i = casillas;
                    printf("Hay un muro de por medio!!!\n");
                }
            } else {
                if (esCasillaValida(juego_data->arreglo[jugador->indice_laberinto].laberinto, jugador->coordenada[0] + 1, jugador->coordenada[1], accion_mazo, player)){
                    (jugador->coordenada[0])++;
                    (jugador->coordenada_opuesta[0])--;
                    jugador->casilla_anterior = juego_data->arreglo[jugador->indice_laberinto].laberinto[jugador->coordenada[0]][jugador->coordenada[1]];
                    if (jugador->casilla_anterior == 'T' || jugador->casilla_anterior == 'C' || jugador->casilla_anterior == 'N'){
                        funcionar_casilla(juego_data, jugador->casilla_anterior, jugador, player);
                        juego_data->arreglo[jugador->indice_laberinto].laberinto[jugador->coordenada[0]][jugador->coordenada[1]] = jugador->casilla_anterior;
                    }
                } else {
                    i = casillas;
                    printf("Hay un muro de por medio!!!\n");
                }
            }                
        }
        jugador->casilla_anterior = juego_data->arreglo[jugador->indice_laberinto].laberinto[jugador->coordenada[0]][jugador->coordenada[1]];
        if (
        jugador->casilla_anterior == 'w' ||
        jugador->casilla_anterior == 'y' ||
        jugador->casilla_anterior == 'x' ||
        jugador->casilla_anterior == 'z'){
            funcionar_casilla(juego_data, jugador->casilla_anterior, jugador, player);
        }
        juego_data->arreglo[jugador->indice_laberinto].laberinto[jugador->coordenada[0]][jugador->coordenada[1]] = player;
    } else if (direccion == 'a' && jugador->coordenada[1] > 0) {
        //Mover hacia la izquierda
        juego_data->arreglo[jugador->indice_laberinto].laberinto[jugador->coordenada[0]][jugador->coordenada[1]] = jugador->casilla_anterior;
        //Aqui hay movimiento casilla por casilla
        for (int i = 0; i < casillas; i++) {
            if (accion_mazo == 'b'){
                if (esCasillaValida(juego_data->arreglo[jugador->indice_laberinto].laberinto, jugador->coordenada[0], jugador->coordenada[1] - 1, accion_mazo, player)){
                    (jugador->coordenada[1])--;
                    (jugador->coordenada_opuesta[1])++;
                    jugador->casilla_anterior = juego_data->arreglo[jugador->indice_laberinto].laberinto[jugador->coordenada[0]][jugador->coordenada[1]];
                    if (jugador->casilla_anterior == 'T' || jugador->casilla_anterior == 'C' || jugador->casilla_anterior == 'N'){
                        funcionar_casilla(juego_data, jugador->casilla_anterior, jugador, player);
                        juego_data->arreglo[jugador->indice_laberinto].laberinto[jugador->coordenada[0]][jugador->coordenada[1]] = jugador->casilla_anterior;
                    }
                }  else {
                    i = casillas;
                    printf("Hay un muro de por medio!!!\n");
                }
            } else {
                if (esCasillaValida(juego_data->arreglo[jugador->indice_laberinto].laberinto, jugador->coordenada[0], jugador->coordenada[1] - 1, accion_mazo, player)){
                    (jugador->coordenada[1])--;
                    (jugador->coordenada_opuesta[1])++;
                    jugador->casilla_anterior = juego_data->arreglo[jugador->indice_laberinto].laberinto[jugador->coordenada[0]][jugador->coordenada[1]];
                    if (jugador->casilla_anterior == 'T' || jugador->casilla_anterior == 'C' || jugador->casilla_anterior == 'N'){
                        funcionar_casilla(juego_data, jugador->casilla_anterior, jugador, player);
                        juego_data->arreglo[jugador->indice_laberinto].laberinto[jugador->coordenada[0]][jugador->coordenada[1]] = jugador->casilla_anterior;
                    }
                } else {
                    i = casillas;
                    printf("Hay un muro de por medio!!!\n");
                }
            }  
        }
        jugador->casilla_anterior = juego_data->arreglo[jugador->indice_laberinto].laberinto[jugador->coordenada[0]][jugador->coordenada[1]];
        if (
        jugador->casilla_anterior == 'w' ||
        jugador->casilla_anterior == 'y' ||
        jugador->casilla_anterior == 'x' ||
        jugador->casilla_anterior == 'z'){
            funcionar_casilla(juego_data, jugador->casilla_anterior, jugador, player);
        }
        juego_data->arreglo[jugador->indice_laberinto].laberinto[jugador->coordenada[0]][jugador->coordenada[1]] = player;
    } else if (direccion == 'd' && jugador->coordenada[1] < 4) {
        //Mover hacia la derecha
        juego_data->arreglo[jugador->indice_laberinto].laberinto[jugador->coordenada[0]][jugador->coordenada[1]] = jugador->casilla_anterior;
        //Aqui hay movimiento casilla por casilla
        for (int i = 0; i < casillas; i++) {
            if (accion_mazo == 'b'){
                if (esCasillaValida(juego_data->arreglo[jugador->indice_laberinto].laberinto, jugador->coordenada[0], jugador->coordenada[1] + 1, accion_mazo, player)){
                    (jugador->coordenada[1])++;
                    (jugador->coordenada_opuesta[1])--;
                    jugador->casilla_anterior = juego_data->arreglo[jugador->indice_laberinto].laberinto[jugador->coordenada[0]][jugador->coordenada[1]];
                    if (jugador->casilla_anterior == 'T' || jugador->casilla_anterior == 'C' || jugador->casilla_anterior == 'N'){
                        funcionar_casilla(juego_data, jugador->casilla_anterior, jugador, player);
                        juego_data->arreglo[jugador->indice_laberinto].laberinto[jugador->coordenada[0]][jugador->coordenada[1]] = jugador->casilla_anterior;
                    }
                }  else {
                    i = casillas;
                    printf("Hay un muro de por medio!!!\n");
                }
            } else {
                if (esCasillaValida(juego_data->arreglo[jugador->indice_laberinto].laberinto, jugador->coordenada[0], jugador->coordenada[1] + 1, accion_mazo, player)){
                    (jugador->coordenada[1])++;
                    (jugador->coordenada_opuesta[1])--;
                    jugador->casilla_anterior = juego_data->arreglo[jugador->indice_laberinto].laberinto[jugador->coordenada[0]][jugador->coordenada[1]];
                    if (jugador->casilla_anterior == 'T' || jugador->casilla_anterior == 'C' || jugador->casilla_anterior == 'N'){
                        funcionar_casilla(juego_data, jugador->casilla_anterior, jugador, player);
                        juego_data->arreglo[jugador->indice_laberinto].laberinto[jugador->coordenada[0]][jugador->coordenada[1]] = jugador->casilla_anterior;
                    }
                } else {
                    i = casillas;
                    printf("Hay un muro de por medio!!!\n");
                }
            }      
        }
        jugador->casilla_anterior = juego_data->arreglo[jugador->indice_laberinto].laberinto[jugador->coordenada[0]][jugador->coordenada[1]];
        if (
        jugador->casilla_anterior == 'w' ||
        jugador->casilla_anterior == 'y' ||
        jugador->casilla_anterior == 'x' ||
        jugador->casilla_anterior == 'z'){
            funcionar_casilla(juego_data, jugador->casilla_anterior, jugador, player);
        }
        juego_data->arreglo[jugador->indice_laberinto].laberinto[jugador->coordenada[0]][jugador->coordenada[1]] = player;
    }
}


//encontrarCoordenada(Laberinto matriz, char elemento, int *fila, int *columna):
//Funcion para encontrar la coordenada inicial de un jugador
//Parametros:
//Laberinto matriz: Laberinto inicial donde los jugadores se encuentran
//char elemento: Jugador a buscar (W, X, Y, Z)
//int *fila: fila a modificar de donde se encuentra el jugador
//int *columna: columna a modificar de donde se encuentra el jugador
void encontrarCoordenada(Laberinto matriz, char elemento, int *fila, int *columna) {
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            if (matriz.laberinto[i][j] == elemento) {
                *fila = i;
                *columna = j;
                return; //Terminamos la búsqueda una vez que encontramos el elemento
            }
        }
    }
    //Si no encontramos el elemento, fila y columna a -1
    *fila = -1;
    *columna = -1;
}


//void tengo_carta(int num):
//Funcion auxiliar para printear que carta tiene el jugador
//Parametros:
//int num: Numero representativo de la carta que tiene el jugador
void tengo_carta(int num){
    if(num == 1){
        printf("Tengo la carta: Busqueda (B)\n");
    }else{
        printf("Tengo la carta: Escaleras (E)\n");
    }
    return;
}

//void cambio(int arr[], int n):
//Funcion auxiliar para "revolver" los mazos de cartas o laberintos
//Parametros:
//int arr[]: Arreglo a modificar para revolver cartas o laberintos (es tipo int porque representa los indices de los mazos originales)
//int n:  Tamaño del arreglo a modificar
void cambio(int arr[], int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        // Intercambia arr[i] y arr[j]
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}



int main() {
    //Definir las rondas
    int num_rondas = 15;
    int rondas_extra_acumuladas = 0;
    int rondas_perdidas_acumuladas = 0;

    //Número total de laberintos, incluido el inicial
    const int numLaberintos = 9;

    //Arreglo de laberintos para almacenar todos los laberintos
    Laberinto laberintos[numLaberintos];
    
    //Aqui se cargan todos los tableros.txt
    cargarLaberintos(laberintos, numLaberintos);
    
    
    //asigno cantidad de cartas de ambos tipos
    int mazo_cartas[4] = {1,1,2,2};
    srand(time(NULL));
    cambio(mazo_cartas, 4);
    
    //Variables auxiliares para cambiar coordenadas de jugadores y laberintos
    int filaJugador_char, columnaJugador_char, lab_char;
    
    //Aqui se simula barajar el mazo de los laberintos
    int numeros[] = {1, 2, 3, 4, 5, 6, 7, 8};
    int longitud = sizeof(numeros) / sizeof(numeros[0]);
    cambio(numeros, longitud);
    int filaAux, ColumnaAux = 0;

    // Arreglo para almacenar las tuberías (una para cada hijo)
    int pipes[NUM_HIJOS][2];
    //Aqui se almacena los datos del jugador
    Jugador_data datos[NUM_HIJOS];
    //Aqui se almacenan los datos del juego
    Datos datos_juego;

    // Crear tuberías para la comunicación
    for (int i = 0; i < NUM_HIJOS; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("Error al crear la tubería");
            exit(EXIT_FAILURE);
        }
    }

    // Crear procesos hijos
    for (int i = 0; i < NUM_HIJOS; i++) {
        //Creo datos iniciales para el jugador i:
        datos[i].turno = 1;  //anteriormente: datos[i].turno = i + 1
        datos[i].labe_act = laberintos[0];
        datos[i].indice_laberinto = 0;
        datos_juego.pos_enlace = 0;
        //Crear proceso hijo
        pid_t child_pid = fork();

        if (child_pid == -1) {
            perror("Error al crear el proceso hijo");
            exit(EXIT_FAILURE);
        } else if (child_pid == 0) { // Proceso hijo
            //Generar semilla aleatoria para cada hijo:
            srand(time(NULL) + getpid());
            
            close(pipes[i][1]); // Cerrar el extremo de escritura de la tubería en el hijo

            int numeroHijo = i + 1;
            //int turno = 0;
            int cant_jugadores_laberinto_inicial = 0;
            int tesoros_recolectados_jugadores = 0;
            datos[i].casilla_anterior = '0';
            int ronda = 0;
            
            int terminar_juego = 1;
            
            while (terminar_juego) {
                // Leer el turno desde la tubería
                read(pipes[i][0], &datos_juego, sizeof(Datos));
                
                tesoros_recolectados_jugadores = 0;
                cant_jugadores_laberinto_inicial = 0;
                
                //Verifico si todos los jugadores estan en el laberinto inicial, si la suma total es 4, estan todos:
                //De paso verifico si todos tienen su tesoro recolectado
                //Se puede hacer mas eficiente si es que este fragmento de codigo se ejecuta solo cuando el jugador actual esta en el inicio
                for(int estan_inicio = 0; estan_inicio < 4; estan_inicio++){
                    if(datos_juego.jugadores_laberinto_inicial[estan_inicio] == 1){
                        cant_jugadores_laberinto_inicial++;
                    }
                    if(datos_juego.tesoros_encontrados[estan_inicio] == 1){
                        tesoros_recolectados_jugadores++;
                    }
                }
                
                
                //Test rondas extras:
                if (rondas_extra_acumuladas != datos_juego.rondas_extra){
                    
                    num_rondas = num_rondas + (datos_juego.rondas_extra - rondas_extra_acumuladas);
                    rondas_extra_acumuladas = datos_juego.rondas_extra;
                }
                    
                //Test rondas perdidas:
                if (rondas_perdidas_acumuladas != datos_juego.rondas_perdidas){
                    
                    num_rondas = num_rondas - (datos_juego.rondas_perdidas - rondas_perdidas_acumuladas);
                    rondas_perdidas_acumuladas = datos_juego.rondas_perdidas;
                }
                
                
                if(cant_jugadores_laberinto_inicial == 4 && tesoros_recolectados_jugadores == 4){
                    ronda = num_rondas; //Para finalizar el juego
                    if(numeroHijo == 1){
                        printf("Los jugadores han Ganado!!! \n");
                    }
                }else if (ronda < num_rondas){
                    //Aqui se define el laberinto actual del jugador segun el indice guardado
                    datos[i].labe_act = datos_juego.arreglo[datos[i].indice_laberinto];
                    
                    printf("Jugador %d, Ronda %d: Es mi turno numero %d ! \n\n", numeroHijo, ronda, datos[i].turno);
                    
                    //Se comprueba coordenadas iniciales de cada Jugador y se le pasa la carta respectiva
                    if (datos[i].turno == 1){
                        //Se le pasa la carta para que juegue
                        datos[i].mazo = mazo_cartas[i];
                        //Se mira todas las casillas del tablero inicial buscando el jugador correspondiente
                        char elemento = '0';
                        if (numeroHijo == 1){
                            elemento = 'W';
                        }else if (numeroHijo == 2){
                            elemento = 'X';
                        }else if (numeroHijo == 3){
                            elemento = 'Y';
                        }else{
                            elemento = 'Z';
                        }
                        //Se guardan las coordenadas en la estructura Datos que tiene cada jugador
                        encontrarCoordenada(datos[i].labe_act, elemento, &datos[i].coordenada[0], &datos[i].coordenada[1]);
                        
                        printf("Mis coordenadas iniciales son: %d, %d \n\n", datos[i].coordenada[0], datos[i].coordenada[1]);
                        //Asigno coordenada opuesta del jugador
                        datos[i].coordenada_opuesta[0] = 4 - datos[i].coordenada[0];
                        datos[i].coordenada_opuesta[1] = 4 - datos[i].coordenada[1];
                    }
                    
                    
                    tengo_carta(datos[i].mazo);
                    
                    printf("Mis coordenadas actuales son: %d, %d \n\n", datos[i].coordenada[0], datos[i].coordenada[1]);
    
                    printf("Laberinto actual del jugador %d: \n\n", numeroHijo);
                    imprimirLaberinto(datos[i].labe_act);
                    printf("\n");
                    
                    //Aqui va la logica de movimiento o accion
                    
                    char eleccion[2]; // Un array para almacenar la elección del usuario
                    char eleccion2[2];
                    char eleccion3[2];
                    
                    char anterior = (char)(numeroHijo + '0');;
                    
                    printf("Por favor, selecciona una opción:\n");
                    printf("1) Movimiento\n2) Accion\n3) Salir\n");
                    
                    if(anterior == datos[i].casilla_anterior){
                        printf("4) Tomar tesoro\n");
                    }
    
                    scanf("%s",eleccion);
                    
                    char player;
                    //Aqui se traduce el jugador:
                    if(numeroHijo == 1){
                        player = 'W';
                    }else if (numeroHijo == 2){
                        player = 'X';
                    }else if (numeroHijo == 3){
                        player = 'Y';
                    }else{
                        player = 'Z';
                    }
                    //Opcion de moverse
                    if (strcmp(eleccion, "1") == 0) {
                        int es_valido = 1;
                        int es_valido2 = 1;
                        int casillas;
                        char direc_casilla_b;
                        int limites_jugador = -1;
                        int limites_jugador2 = -1;
                        
                        while(es_valido){
                            printf("Por favor, selecciona la dirección de Movimiento:\n");
                            printf("w) Arriba\ns) Abajo\na) Izquierda\nd) Derecha\n");
                            scanf("%s",eleccion2);
                        
                            printf("¿Cuántas casillas deseas moverte? ");
                            scanf("%d", &casillas);
                            
                            limites_jugador = datos[i].coordenada[0];
                            limites_jugador2 = datos[i].coordenada[1];
                            
                            
                            if(eleccion2[0] == 'w'){
                                //Mov arriba
                                if(limites_jugador - casillas >= 0){
                                    //Es valido!
                                    es_valido2 = 1;
                                }else{
                                    es_valido2 = 0;
                                }
                            }else if(eleccion2[0] == 's'){
                                //Mov abajo
                                if(limites_jugador + casillas <= 4){
                                    //Es valido!
                                    es_valido2 = 1;
                                }else{
                                    es_valido2 = 0;
                                }
                            }else if(eleccion2[0] == 'a'){
                                //Mov izq
                                if(limites_jugador2 - casillas >= 0){
                                    //Es valido!
                                    es_valido2 = 1;
                                }else{
                                    es_valido2 = 0;
                                }
                            }else if(eleccion2[0] == 'd'){
                                //Mov der
                                if(limites_jugador2 + casillas <= 4){
                                    //Es valido!
                                    es_valido2 = 1;
                                }else{
                                    es_valido2 = 0;
                                }
                            }else{
                                es_valido2 = 0;
                            }
                            
                            //Casilla final es valida?
                            
                            if(es_valido2 == 1){
                                moverJugador(eleccion2[0], casillas, player, &datos[i], &datos_juego);
                                es_valido = 0;
                            }else{
                                //Movimiento invalido!
                                printf("Movimiento invalido!!, por favor, volver a colocar su movimiento\n");
                                printf("\n");
                                printf("\n");
                                printf("Laberinto actual del jugador %d: \n\n", numeroHijo);
                                imprimirLaberinto(datos[i].labe_act);
                                printf("\n");
                            }
                            
                        }
                        
                        //Aqui deberia ir el tp automatico de laberinto si es que la casilla actual del jugador es "b" .
                        
                        if(datos[i].casilla_anterior == 'b'){
                            int flaging = 1;
                            direc_casilla_b = determinarDireccion(datos[i].coordenada[0], datos[i].coordenada[1]);
                            //printf("la direccion de la casilla b es: %c\n", direc_casilla_b);
                            if (eleccion2[0] == direc_casilla_b){
                                while (flaging){
                                    printf("Deseas ir al otro laberinto? (y/n):\n");
                                    scanf("%s",eleccion3);
                                    if(strcmp(eleccion3, "y") == 0){
                                        for(int testeo = 0; testeo < datos_juego.pos_enlace; testeo++){
                                            if (datos[i].coordenada[0] == datos_juego.enlaces_laberintos[testeo].fila_jugador_enlace && datos[i].coordenada[1] == datos_juego.enlaces_laberintos[testeo].columna_jugador_enlace && datos[i].indice_laberinto == datos_juego.enlaces_laberintos[testeo].indice_laberinto_actual){
                                                if (datos_juego.arreglo[datos_juego.enlaces_laberintos[testeo].indice_laberinto_sig].laberinto[datos[i].coordenada_opuesta[0]][datos[i].coordenada_opuesta[1]] == 'b'){
                                                    //tp
                                                    datos[i].casilla_anterior = 'b';
                                                    datos_juego.arreglo[datos_juego.enlaces_laberintos[testeo].indice_laberinto_actual].laberinto[datos[i].coordenada[0]][datos[i].coordenada[1]] = datos[i].casilla_anterior;
                                                    datos[i].coordenada[0] = datos[i].coordenada_opuesta[0];
                                                    datos[i].coordenada[1] = datos[i].coordenada_opuesta[1];
                                                    datos_juego.arreglo[datos_juego.enlaces_laberintos[testeo].indice_laberinto_sig].laberinto[datos[i].coordenada_opuesta[0]][datos[i].coordenada_opuesta[1]] = player;
                                                    datos[i].coordenada_opuesta[0] = datos_juego.enlaces_laberintos[testeo].fila_jugador_enlace;
                                                    datos[i].coordenada_opuesta[1] = datos_juego.enlaces_laberintos[testeo].columna_jugador_enlace;
                                                    datos[i].indice_laberinto = datos_juego.enlaces_laberintos[testeo].indice_laberinto_sig;
                                                    
                                                    //Cambio arreglo jugadores_laberinto_inicial
                                                    if(datos[i].indice_laberinto == 0){
                                                        //Estoy en laberinto inicial, modifico arreglo jugadores_laberinto_inicial
                                                        datos_juego.jugadores_laberinto_inicial[i] = 1;
                                                    }else{
                                                        datos_juego.jugadores_laberinto_inicial[i] = 0;
                                                    }
                                                    
                                                    printf("\n");
                                                    printf("Laberinto actual del jugador %d: \n\n", numeroHijo);
                                                    imprimirLaberinto(datos_juego.arreglo[datos[i].indice_laberinto]);
                                                    printf("\n");
                                                    
                                                    int casillas;
                                                    printf("¿Cuántas casillas deseas moverte? ");
                                                    scanf("%d", &casillas);
                                                    
                                                    if(casillas != 0){
                                                        moverJugador(eleccion2[0], casillas, player, &datos[i], &datos_juego);
                                                        if (datos[i].casilla_anterior != 'b'){
                                                            flaging = 0;
                                                        }
                                                    }else {
                                                        flaging = 0;
                                                    }
                                                    
                                                }else{
                                                    //no tp, me quedo en la casilla de b
                                                    printf("No podras ir al otro laberinto, hay un jugador al otro lado :(\n");
                                                    printf("Laberinto actual del jugador %d: \n\n", numeroHijo);
                                                    imprimirLaberinto(datos_juego.arreglo[datos[i].indice_laberinto]);
                                                    printf("\n");
                                                    flaging = 0;
                                                }
                                                testeo = datos_juego.pos_enlace;
                                            }
                                        }  
                                    } else {
                                        flaging = 0;
                                    }
                                }   
                            }
                        }

                        printf("\n\n Informacion del ultimo movimiento del jugador %d\n\n", numeroHijo);
                        printf("Las coordenadas finales del jugador %d son: Fila: %d, Columna: %d\n\n", numeroHijo, datos[i].coordenada[0], datos[i].coordenada[1]);
                        printf("Encontrandose en el siguiente laberinto: \n\n");
                        datos[i].labe_act = datos_juego.arreglo[datos[i].indice_laberinto];
                        imprimirLaberinto(datos[i].labe_act);
                        printf("\n");
                        printf("\n");




                    } else if (strcmp(eleccion, "2") == 0) {
                        //Casilla actual en donde se encuentra el jugador
                        char casilla_actual = datos[i].casilla_anterior;
                        printf("la casilla actual es: %c\n", casilla_actual);
                        if (casilla_actual == 'B' || casilla_actual == 'E'){
                            //Se mira la carta del jugador para dar la opcion
                            char accion_mazo;
                            if (datos[i].mazo == 1){
                                accion_mazo = 'b';
                            }else{
                                accion_mazo = 'e';
                            }
                            
                            printf("Por favor, selecciona una Accion:\n");
                            
                            if(accion_mazo == 'b'){
                                printf("Buscar: %c\n", accion_mazo);
                            }else{
                                printf("Escaleras: %c\n", accion_mazo);
                            }
                    
                            scanf("%s",eleccion2);
                            
                            if (strcmp(eleccion2, "b") == 0 && datos[i].mazo == 1) {
                                if (casilla_actual == 'B') {
                                    //aqui se genera el laberinto adecuado a la entrada del laberinto actual segun el orden del mazo de laberintos
                                    int boleano = 0;
                                    int k;
                                    for (k = 0 ; k < longitud ; k++){
                                        if (datos_juego.arreglo[numeros[k]].laberinto[datos[i].coordenada_opuesta[0]][datos[i].coordenada_opuesta[1]] == 'B'){
                                            boleano = 1;
                                            for (int iterador = 0 ; iterador < datos_juego.pos_enlace ; iterador++ ){
                                                if (numeros[k] == datos_juego.enlaces_laberintos[iterador].indice_laberinto_actual){
                                                   boleano = 0;
                                                   iterador = datos_juego.pos_enlace;
                                                }
                                            }
                                        }
                                        if (boleano){
                                            generar_laberinto(&datos_juego.arreglo[numeros[k]], datos_juego.tesoros_generados, datos_juego.tp_generados, datos_juego.pos_enlace, &datos_juego.cant_camaras_juego, datos_juego.casillas_tp, numeros[k]);
                                            
                                            
                                            datos[i].labe_act.laberinto[datos[i].coordenada[0]][datos[i].coordenada[1]] = 'b';
                                            datos_juego.arreglo[datos[i].indice_laberinto] = datos[i].labe_act;
                                            //datos_juego.pos_enlace++;
                                            datos_juego.arreglo[numeros[k]].laberinto[datos[i].coordenada_opuesta[0]][datos[i].coordenada_opuesta[1]] = player;
                                            datos[i].casilla_anterior = 'b';
                                            
                                            //convertir los numeros a char para guardar los enlaces
                                            filaJugador_char = datos[i].coordenada[0];
                                            columnaJugador_char = datos[i].coordenada[1];
                                            
                                            lab_char = datos[i].indice_laberinto;
                                            
                                            datos_juego.enlaces_laberintos[datos_juego.pos_enlace].fila_jugador_enlace = filaJugador_char;
                                            datos_juego.enlaces_laberintos[datos_juego.pos_enlace].columna_jugador_enlace = columnaJugador_char;
                                            datos_juego.enlaces_laberintos[datos_juego.pos_enlace].indice_laberinto_actual = lab_char;
                                            datos_juego.enlaces_laberintos[datos_juego.pos_enlace].indice_laberinto_sig = numeros[k];
                                            
                                            //printf("Se ha creado el primer enlace: %d%d%d%d\n", datos_juego.enlaces_laberintos[datos_juego.pos_enlace].fila_jugador_enlace, datos_juego.enlaces_laberintos[datos_juego.pos_enlace].columna_jugador_enlace, datos_juego.enlaces_laberintos[datos_juego.pos_enlace].indice_laberinto_actual, datos_juego.enlaces_laberintos[datos_juego.pos_enlace].indice_laberinto_sig);
                                            
                                            datos_juego.pos_enlace = datos_juego.pos_enlace + 1;
                                            
                                            
                                            datos_juego.enlaces_laberintos[datos_juego.pos_enlace].fila_jugador_enlace = datos[i].coordenada_opuesta[0];
                                            datos_juego.enlaces_laberintos[datos_juego.pos_enlace].columna_jugador_enlace = datos[i].coordenada_opuesta[1];
                                            datos_juego.enlaces_laberintos[datos_juego.pos_enlace].indice_laberinto_actual = numeros[k];
                                            datos_juego.enlaces_laberintos[datos_juego.pos_enlace].indice_laberinto_sig = lab_char;
                                            
                                            //printf("Se ha creado el segundo enlace: %d%d%d%d\n", datos_juego.enlaces_laberintos[datos_juego.pos_enlace].fila_jugador_enlace, datos_juego.enlaces_laberintos[datos_juego.pos_enlace].columna_jugador_enlace, datos_juego.enlaces_laberintos[datos_juego.pos_enlace].indice_laberinto_actual, datos_juego.enlaces_laberintos[datos_juego.pos_enlace].indice_laberinto_sig);
                                            
                                            datos_juego.pos_enlace = datos_juego.pos_enlace + 1;
                                            
                                            
                                            datos[i].indice_laberinto = numeros[k];
                                            filaAux = datos[i].coordenada_opuesta[0];
                                            ColumnaAux = datos[i].coordenada_opuesta[1];
                                            
                                            datos[i].coordenada_opuesta[0] = datos[i].coordenada[0];
                                            datos[i].coordenada_opuesta[1] = datos[i].coordenada[1];
                                            
                                            datos[i].coordenada[0] = filaAux;
                                            datos[i].coordenada[1] = ColumnaAux;
                                            
                                            filaJugador_char = datos[i].coordenada[0];
                                            columnaJugador_char = datos[i].coordenada[1];
                                            
    
                                            printf("\n");
                                            printf("\n");
                                            printf("Jugador %d: Me voy al laberinto %d\n", numeroHijo, numeros[k]);
                                            printf("\n");
                                            printf("\n");
                                            //Cambio arreglo jugadores_laberinto_inicial
                                            datos_juego.jugadores_laberinto_inicial[i] = 0;
                                            
                                            //Para salir del for
                                            k = longitud;

                                            printf("\n\n Informacion del ultimo movimiento del jugador %d\n\n", numeroHijo);
                                            printf("Las coordenadas finales del jugador %d son: Fila: %d, Columna: %d\n\n", numeroHijo, datos[i].coordenada[0], datos[i].coordenada[1]);
                                            printf("Encontrandose en el siguiente laberinto: \n\n");
                                            datos[i].labe_act = datos_juego.arreglo[datos[i].indice_laberinto];
                                            imprimirLaberinto(datos[i].labe_act);
                                            printf("\n");
                                            printf("\n");
                                        }
                                    }
                                    if (k == longitud){
                                        printf("Parece que no hay mas laberintos por aqui... he perdido mi turno\n");


                                        printf("\n\n Informacion del ultimo movimiento del jugador %d\n\n", numeroHijo);
                                        printf("Las coordenadas finales del jugador %d son: Fila: %d, Columna: %d\n\n", numeroHijo, datos[i].coordenada[0], datos[i].coordenada[1]);
                                        printf("Encontrandose en el siguiente laberinto: \n\n");
                                        datos[i].labe_act = datos_juego.arreglo[datos[i].indice_laberinto];
                                        imprimirLaberinto(datos[i].labe_act);
                                        printf("\n");
                                        printf("\n");
                                    }
                                }else{
                                    printf("Accion invalida!!, no puedes usar eso aqui\n");
                                    printf("Pierdes el turno!!\n");

                                    printf("\n\n Informacion del ultimo movimiento del jugador %d\n\n", numeroHijo);
                                    printf("Las coordenadas finales del jugador %d son: Fila: %d, Columna: %d\n\n", numeroHijo, datos[i].coordenada[0], datos[i].coordenada[1]);
                                    printf("Encontrandose en el siguiente laberinto: \n\n");
                                    imprimirLaberinto(datos[i].labe_act);
                                    printf("\n");
                                    printf("\n");

                                }
                            } else if (strcmp(eleccion2, "e") == 0 && datos[i].mazo == 2) {
                                if (casilla_actual == 'E'){
                                    printf("Ya puse la escalera, soy el jugador %c numero: %d\n", player, numeroHijo);
                                    datos[i].labe_act.laberinto[datos[i].coordenada[0]][datos[i].coordenada[1]] = player;
                                    datos[i].casilla_anterior = '0';
                                    datos_juego.arreglo[datos[i].indice_laberinto] = datos[i].labe_act;


                                    printf("\n\n Informacion del ultimo movimiento del jugador %d\n\n", numeroHijo);
                                    printf("Las coordenadas finales del jugador %d son: Fila: %d, Columna: %d\n\n", numeroHijo, datos[i].coordenada[0], datos[i].coordenada[1]);
                                    printf("Encontrandose en el siguiente laberinto: \n\n");
                                    datos[i].labe_act = datos_juego.arreglo[datos[i].indice_laberinto];
                                    imprimirLaberinto(datos[i].labe_act);
                                    printf("\n");
                                    printf("\n");
                                }else{
                                    printf("Accion invalida!!, no puedes usar eso aqui\n");
                                    printf("Pierdes el turno!!\n");

                                    printf("\n\n Informacion del ultimo movimiento del jugador %d\n\n", numeroHijo);
                                    printf("Las coordenadas finales del jugador %d son: Fila: %d, Columna: %d\n\n", numeroHijo, datos[i].coordenada[0], datos[i].coordenada[1]);
                                    printf("Encontrandose en el siguiente laberinto: \n\n");
                                    imprimirLaberinto(datos[i].labe_act);
                                    printf("\n");
                                    printf("\n");
                                }
                            }else{
                                printf("Accion invalida!!, no puedes usar eso aqui\n");
                                printf("Pierdes el turno!!\n");

                                printf("\n\n Informacion del ultimo movimiento del jugador %d\n\n", numeroHijo);
                                    printf("Las coordenadas finales del jugador %d son: Fila: %d, Columna: %d\n\n", numeroHijo, datos[i].coordenada[0], datos[i].coordenada[1]);
                                    printf("Encontrandose en el siguiente laberinto: \n\n");
                                    imprimirLaberinto(datos[i].labe_act);
                                    printf("\n");
                                    printf("\n");
                            }
                        }else{
                            printf("No estas en una casilla para realizar acciones!!\n");
                            printf("Pierdes el turno\n");

                            printf("\n\n Informacion del ultimo movimiento del jugador %d\n\n", numeroHijo);
                            printf("Las coordenadas finales del jugador %d son: Fila: %d, Columna: %d\n\n", numeroHijo, datos[i].coordenada[0], datos[i].coordenada[1]);
                            printf("Encontrandose en el siguiente laberinto: \n\n");
                            imprimirLaberinto(datos[i].labe_act);
                            printf("\n");
                            printf("\n");
                        }
                    } else if (strcmp(eleccion, "3") == 0){
                        datos_juego.rondas_perdidas = 100;
                        //break;
                    }else if (strcmp(eleccion, "4") == 0 && anterior == datos[i].casilla_anterior){
                        //Tomar el tesoro
                        funcionar_casilla(&datos_juego, anterior, &datos[i], player);
                    }
                    
                }else{
                    if(numeroHijo == 1){
                        printf("Los jugadores han perdido :C\n");
                    }
                    write(pipes[(i + 1) % NUM_HIJOS][1], &datos_juego, sizeof(Datos));
                    terminar_juego = 0;
                }
                    
                datos[i].turno = datos[i].turno + 1;
                
                ronda++;
                
                write(pipes[(i + 1) % NUM_HIJOS][1], &datos_juego, sizeof(Datos));
            }

            close(pipes[i][0]);
            exit(EXIT_SUCCESS);
        } else { // Proceso padre
            close(pipes[i][0]); // Cerrar el extremo de lectura de la tubería en el padre
        }
    }
    

    // Iniciar el turno con el primer proceso hijo
    datos_juego.rondas_extra = 0;
    datos_juego.rondas_perdidas = 0;
    datos_juego.pos_enlace = 0;
    datos_juego.cant_camaras_juego = 0;
    
    //relleno la matriz para enlaces:
    for(int cant = 0; cant < 16; cant++){
        datos_juego.enlaces_laberintos[cant].fila_jugador_enlace = -1;
        datos_juego.enlaces_laberintos[cant].columna_jugador_enlace = -1;
        datos_juego.enlaces_laberintos[cant].indice_laberinto_actual = -1;
        datos_juego.enlaces_laberintos[cant].indice_laberinto_sig = -1;
    }
    
    //inicializar tesoros tesoros_generados ,tp_generados, enlaces de tp por jugador
    for(int cant = 0; cant < 4; cant++){
        datos_juego.tesoros_generados[cant] = 0;
        datos_juego.tp_generados[cant] = 0;
        datos_juego.tesoros_encontrados[cant] = 0;
        datos_juego.jugadores_laberinto_inicial[cant] = 1;
        
        datos_juego.casillas_tp[cant].indice_laberinto1 = -1;
        datos_juego.casillas_tp[cant].indice_laberinto2 = -1;
        datos_juego.casillas_tp[cant].fila_jugador_tp1 = -1;
        datos_juego.casillas_tp[cant].columna_jugador_tp1 = -1;
        datos_juego.casillas_tp[cant].fila_jugador_tp2 = -1;
        datos_juego.casillas_tp[cant].columna_jugador_tp2 = -1;
    }

    
    // Copia el contenido del arreglo en la estructura
    for (int z = 0; z < 9; z++) {
        datos_juego.arreglo[z] = laberintos[z];
    }
    
    write(pipes[0][1], &datos_juego, sizeof(Datos));

    // Esperar a que los procesos hijos terminen (esto es opcional)
    for (int i = 0; i < NUM_HIJOS; i++) {
        wait(NULL);
    }

    return 0;
}