import java.util.concurrent.atomic.AtomicBoolean;

//Clase Multithreading utilizando Runnable:
//int colum_ini, corresponde a la columna inicial desde donde se va a tomar el cuadrante
//int fila_ini, corresponde a la fila inicial desde donde se va a tomar el cuadrante
//int colum_fin, corresponde a la columna final desde donde se va a tomar el cuadrante
//int fila_fin, corresponde a la fila final desde donde se va a tomar el cuadrante
//char[][] sopita, corresponde a la matriz que representa la sopa de letras
//String palabra, corresponde a la palabra a buscar en la sopa de letras
//static AtomicBoolean palabraEncontrada, variable compartida que indica si la palabra fue encontrada o no por alguna hebra
//Esta ultima si es True, la palabra fue encontrada y por lo tanto las hebras en ejecucion terminaran

class Multithreading implements Runnable {

    private int colum_ini;
    private int fila_ini;
    private int colum_fin;
    private int fila_fin;
    private char[][] sopita;
    private String palabra;
    private static AtomicBoolean palabraEncontrada = new AtomicBoolean(false);

    public Multithreading(int fila_ini, int colum_ini, int fila_fin, int colum_fin, char[][] sopita, String palabra){
        this.fila_ini = fila_ini;
        this.colum_ini = colum_ini;
        this.fila_fin = fila_fin;
        this.colum_fin = colum_fin;
        this.sopita = sopita;
        this.palabra = palabra;
    }

    //Aqui se crean hebras de manera recursiva hasta que el tamano de la sopa de letras calce con el tamano de la palabra
    //y comenzara a buscar la palabra utilizando la "funcion" buscarPalabra
    public void run(){
        try {
            //Ver si la division esta al limite (Tamano Submatriz = tamano palabra)
            int tamano = palabra.length();

            //Verificar si la palabra se ha encontrado en cualquier parte y detener la búsqueda
            if (palabraEncontrada.get()) {
                return;
            }

            if(tamano == (fila_fin-fila_ini)){
                //Busca la palabra
                buscarPalabra(palabra, sopita, fila_ini, fila_fin, colum_ini, colum_fin, tamano);

            }else{
                //Dividir cuadrantes y llamar nuevamante a la funcion
                int n = 4; //numero inicial de hilos (4 cuadrantes)
                Thread[] hilo = new Thread[n];

                //Aux es la distancia que se haria en la division de cuadrantes
                int aux = (fila_fin - fila_ini)/2;
                
                //Segundo cuadrante
                Multithreading data = new Multithreading(fila_ini, colum_ini, fila_ini+aux, colum_ini+aux, sopita, palabra);
                hilo[0] = new Thread(data);
                hilo[0].start();

                //Primer cuadrante
                data = new Multithreading(fila_ini, colum_ini+aux, fila_ini+aux, colum_fin, sopita, palabra);
                hilo[1] = new Thread(data);
                hilo[1].start();

                //Tercer cuadrante
                data = new Multithreading(fila_ini+aux, colum_ini, fila_fin, colum_ini+aux, sopita, palabra);
                hilo[2] = new Thread(data);
                hilo[2].start();

                //Cuarto cuadrante
                data = new Multithreading(fila_ini+aux, colum_ini+aux, fila_fin, colum_fin, sopita, palabra);
                hilo[3] = new Thread(data);
                hilo[3].start();

                for (int i = 0; i < n; i++) {
                    try {
                        hilo[i].join();
                    } catch (InterruptedException e) {
                        System.out.println("El hilo " + i + " fue interrumpido!");
                    }
                }
            }

        }
        catch (Exception e) {
            // Throwing an exception
            System.out.println("Algo salio mal...");
        }
    }

    //void buscarPalabra:
    //Que hace: Busca la palabra dentro de un cuadrante especifico de la sopa de letras, el cuadrante es definido gracias a las filas
    //y columnas iniciales junto a las filas y columnas finales. La busqueda de la palabra utiliza 2 hebras para explotar de mejor
    //manera la arquitectura de multiprocesadores, una hebra busca de izquierda a derecha y otra de arriba a abajo
    //------------------
    //Parametros:
    //String Palabra, Palabra a buscar en la sopa de letras
    //char[][] sopa, Matriz que representa la sopa de letras del archivo
    //int f_inicio, Fila inicial desde donde se empezara a buscar la palabra
    //int f_fin, Fila final desde donde se empezara a buscar la palabra
    //int col_inicio, Columna inicial desde donde se empezara a buscar la palabra
    //int col_fin, Columna final desde donde se empezara buscar la palabra
    //int tamano, Tamano de la palabra a buscar
    private void buscarPalabra(String palabra, char[][] sopa, int f_inicio, int f_fin, int col_inicio, int col_fin, int tamano) {

        //Hebra para buscar de izquierda a derecha
        for (int i = f_inicio; i < f_fin; i++) {
            //Busca la palabra de izq a derecha armando un string de izq a derecha xd
            String fila = new String(sopa[i], col_inicio, tamano);
            int index = fila.indexOf(palabra);
            if (index != -1) {
                //Palabra encontrada!
                System.out.println("Fila: " + i + ", columna: " + col_inicio);
                palabraEncontrada.set(true); //Establece palabraEncontrada en true
                return; //Sale temprano si se encuentra la palabra
            }
            if (palabraEncontrada.get()) {
                return;
            }
        }

        //Hebra para buscar de arriba a abajo
        for (int j = col_inicio; j < col_fin; j++) {
            // Lógica para buscar la palabra de arriba a abajo en la matriz
            StringBuilder columna = new StringBuilder();
            for (int k = 0; k < tamano; k++) {
                columna.append(sopa[fila_ini + k][j]);
            }
            if (columna.toString().equals(palabra)) {
                System.out.println("Fila: " + fila_ini + ", columna: " + j);
                palabraEncontrada.set(true);//Establece palabraEncontrada en true
                return; //Sale temprano si se encuentra la palabra
            }
            if (palabraEncontrada.get()) {
                return;
            }
        }
    }
}

// Main Class, lee el archivo utilizando LeerMatriz y con ello se define una hebra para iniciar la recursion, tomando el tiempo
//Desde que "hilito" empieza a ejecutarse
public class Multithread {
    public static void main(String[] args){
        SopasDeLetra archivo = LeerMatriz.leerMatrizDesdeArchivo("sopa_de_letras.txt");
        char[][] sopita = archivo.getMatriz();
        String palabra = archivo.getPalabraABuscar();
        
        int fila_ini = 0;
        int colum_ini = 0;
        int fin = sopita.length;
        
        Multithreading data = new Multithreading(fila_ini, colum_ini, fin, fin, sopita, palabra);
        Thread hilito = new Thread(data);

        long tiempo_inicio = System.currentTimeMillis();
        hilito.start();

        try {
            hilito.join();
            long tiempo_fin = System.currentTimeMillis();
            long tiempoTotal = tiempo_fin - tiempo_inicio;
            System.out.println("Encontrar la palabra tardo: " + tiempoTotal + " milisegundos usando Hebras");
        } catch (InterruptedException e) {
            System.out.println("El hilito fue interrumpido!");
        }
    }
}