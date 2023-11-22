import java.util.concurrent.atomic.AtomicBoolean;

class Hilos extends Thread {

    private int colum_ini;
    private int fila_ini;
    private int colum_fin;
    private int fila_fin;
    private char[][] sopita;
    private String palabra;
    private static AtomicBoolean palabraEncontrada = new AtomicBoolean(false);

    public Hilos(int fila_ini, int colum_ini, int fila_fin, int colum_fin, char[][] sopita, String palabra){
        this.fila_ini = fila_ini;
        this.colum_ini = colum_ini;
        this.fila_fin = fila_fin;
        this.colum_fin = colum_fin;
        this.sopita = sopita;
        this.palabra = palabra;
    }

    public void run(){
        try {
            //Verificar si la palabra se ha encontrado en cualquier parte y detener la búsqueda
            if (palabraEncontrada.get()) {
                return;
            }

            int tamano = palabra.length();
            //Ver si la division esta al limite (Tamano Submatriz = tamano palabra)
            if(tamano == (fila_fin-fila_ini)){

                //Buscar la palabra dentro de la matriz
                buscarPalabra(palabra, sopita, fila_ini, fila_fin, colum_ini, colum_fin, tamano);

            }else{
                //Dividir cuadrantes y llamar nuevamante a la funcion

                //Aux es la distancia que se haria en la division de cuadrantes
                int aux = (fila_fin - fila_ini)/2;
                
                //Segundo cuadrante
                Hilos data1 = new Hilos(fila_ini, colum_ini, fila_ini+aux, colum_ini+aux, sopita, palabra);
                data1.start();

                //Primer cuadrante
                Hilos data2 = new Hilos(fila_ini, colum_ini+aux, fila_ini+aux, colum_fin, sopita, palabra);
                data2.start();

                //Tercer cuadrante
                Hilos data3 = new Hilos(fila_ini+aux, colum_ini, fila_fin, colum_ini+aux, sopita, palabra);
                data3.start();

                //Cuarto cuadrante
                Hilos data4 = new Hilos(fila_ini+aux, colum_ini+aux, fila_fin, colum_fin, sopita, palabra);
                data4.start();
                 try {
                    data1.join();
                    data2.join();
                    data3.join();
                    data4.join();
                } catch (InterruptedException e) {
                    System.out.println("Ha sido interrumpido!");
                }
            }

        }
        catch (Exception e) {
            //error
            System.out.println("Algo salio mal...");
        }
    }

    private void buscarPalabra(String palabra, char[][] sopa, int f_inicio, int f_fin, int col_inicio, int col_fin, int tamano) {

        //Hebra para buscar de izquierda a derecha
        for (int i = f_inicio; i < f_fin; i++) {
            //Busca la palabra de izq a derecha armando un string de izq a derecha xd
            String fila = new String(sopa[i], col_inicio, tamano);
            int index = fila.indexOf(palabra);
            if (index != -1) {
                //Palabra encontrada!
                System.out.println("Fila: " + i + ", columna: " + col_inicio);
            }
            //Verificar si la palabra se ha encontrado en cualquier parte y detener la búsqueda
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
                //Palabra encontrada!
                System.out.println("Fila: " + fila_ini + ", columna: " + j);
            }
            //Verificar si la palabra se ha encontrado en cualquier parte y detener la búsqueda
            if (palabraEncontrada.get()) {
                return;
            }
        }
        
    }
}

// Main Class
public class Hilitos {
    public static void main(String[] args){
        SopasDeLetra archivo = LeerMatriz.leerMatrizDesdeArchivo("sopa_de_letras.txt");
        char[][] sopita = archivo.getMatriz();
        String palabra = archivo.getPalabraABuscar();
        
        int fila_ini = 0;
        int colum_ini = 0;
        int fin = sopita.length;

        //Crear una hebra con los valores iniciales
        Hilos data = new Hilos(fila_ini, colum_ini, fin, fin, sopita, palabra);

        long tiempo_inicio = System.currentTimeMillis();
        data.start();

        try {
            data.join();
            long tiempo_fin = System.currentTimeMillis();
            long tiempoTotal = tiempo_fin - tiempo_inicio;
            System.out.println("Encontrar la palabra tardo: " + tiempoTotal + " milisegundos usando Hebras");
        } catch (InterruptedException e) {
            System.out.println("El hilito fue interrumpido!");
        }
    }
}
