import java.util.concurrent.RecursiveTask;
import java.util.concurrent.ForkJoinPool;
import java.util.concurrent.atomic.AtomicBoolean;

//Clase RecursiveTask utilizando RecursiveTask<Void>:
//char[][] matriz, corresponde a la matriz que representa la sopa de letras
//String palabra, corresponde a la palabra a buscar en la sopa de letras
//int fila_ini, corresponde a la fila inicial desde donde se va a tomar el cuadrante
//int fila_fin, corresponde a la fila final desde donde se va a tomar el cuadrante
//int colum_ini, corresponde a la columna inicial desde donde se va a tomar el cuadrante
//int colum_fin, corresponde a la columna final desde donde se va a tomar el cuadrante
//static AtomicBoolean palabraEncontrada, variable compartida que indica si la palabra fue encontrada o no por alguna hebra
//Esta ultima si es True, la palabra fue encontrada y por lo tanto las hebras en ejecucion terminaran

class ForkJoinClass extends RecursiveTask<Void> {
    private static final AtomicBoolean palabraEncontrada = new AtomicBoolean(false);
    private char[][] matriz;
    private String palabra;
    private int filaInicio, filaFin, colInicio, colFin;

    public ForkJoinClass(char[][] matriz, String palabra, int filaInicio, int filaFin, int colInicio, int colFin) {
        this.matriz = matriz;
        this.palabra = palabra;
        this.filaInicio = filaInicio;
        this.filaFin = filaFin;
        this.colInicio = colInicio;
        this.colFin = colFin;
    }

    //Aqui se crean procesos de manera recursiva hasta que el tamano de la sopa de letras calce con el tamano de la palabra
    //y comenzara a buscar la palabra utilizando la "funcion" buscarPalabraEnCuadrante
    @Override
    protected Void compute() {
        if (palabraEncontrada.get()) {
            return null;
        }
        else if (palabra.length() == (filaFin-filaInicio)) {
            //Si el cuadrante es lo suficientemente pequeño, buscar la palabra directamente
            buscarPalabraEnCuadrante();
        } else {
            //Dividir en 4 cuadrantes y realizar la búsqueda de manera recursiva
            int aux = (filaFin - filaInicio) / 2;

            invokeAll(
                new ForkJoinClass(matriz, palabra, filaInicio, filaInicio+aux, colInicio, colInicio+aux),
                new ForkJoinClass(matriz, palabra, filaInicio, filaInicio+aux, colInicio+aux, colFin),
                new ForkJoinClass(matriz, palabra, filaInicio+aux, filaFin, colInicio, colInicio+aux),
                new ForkJoinClass(matriz, palabra, filaInicio+aux, filaFin, colInicio+aux, colFin)
            );
        }
        return null;
    }

    //void buscarPalabraEnCuadrante:
    //Que hace: Busca la palabra dentro de un cuadrante especifico de la sopa de letras, el cuadrante es definido gracias a las filas
    //y columnas iniciales junto a las filas y columnas finales que se guardan en los procesos. La busqueda de la palabra se realiza
    //utilizando for, uno para busqueda de izq a derecha y otro para busqueda de arriba a abajo.
    private void buscarPalabraEnCuadrante() {
        int tamano = palabra.length();
        for (int i = filaInicio; i < filaFin; i++) {
            //Busca la palabra de izq a derecha armando un string de izq a derecha xd
            String fila = new String(matriz[i], colInicio, tamano);
            int index = fila.indexOf(palabra);
            if (index != -1) {
                //Palabra encontrada!
                palabraEncontrada.set(true);
                System.out.println("Fila: " + i + ", columna: " + colInicio);
                return;
            }
            if (palabraEncontrada.get()) {
                return;
            }
        }
        for (int j = colInicio; j < colFin; j++) {
            //Lógica para buscar la palabra de arriba a abajo en la matriz
            StringBuilder columna = new StringBuilder();
            for (int k = 0; k < tamano; k++) {
                columna.append(matriz[filaInicio + k][j]);
            }
            if (columna.toString().equals(palabra)) {
                //Palabra encontrada
                palabraEncontrada.set(true);
                System.out.println("Fila: " + filaInicio + ", columna: " + j);
                return;
            }
            if (palabraEncontrada.get()) {
                return;
            }
        }
    }
}

// Main Class, lee el archivo utilizando LeerMatriz y con ello se define un proceso para iniciar la recursion, tomando el tiempo
//Desde que "forkJoinPool.invoke(task);" empieza a ejecutar la tarea "task".
public class ForkClass {
    public static void main(String[] args) {
        //Acceder a la matriz desde la clase original
        SopasDeLetra info = LeerMatriz.leerMatrizDesdeArchivo("sopa_de_letras.txt");
        char[][] matriz = info.getMatriz();
        String palabra = info.getPalabraABuscar();
        int tamano = matriz.length;

        ForkJoinPool forkJoinPool = new ForkJoinPool();

        //Crea la tarea como RecursiveTask y luego la convierte a ForkJoinTask
        ForkJoinClass task = new ForkJoinClass(matriz, palabra, 0, tamano, 0, tamano);

        long tiempo_inicio = System.currentTimeMillis();
        forkJoinPool.invoke(task);
        long tiempo_fin = System.currentTimeMillis();
        long tiempoTotal = tiempo_fin - tiempo_inicio;
        System.out.println("Encontrar la palabra tardo: " + tiempoTotal + " milisegundos usando Fork");
    }
}