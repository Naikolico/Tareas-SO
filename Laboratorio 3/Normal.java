//Codigo de Busqueda de palabras de forma Normal, utiliza 2 for, uno para buscar de izq a der y otro para buscar de arriba a abajo
//Crea strings por fila en la busqueda de izq a derecha y dentro de ese string se busca la palabra, lo mismo para la busqueda de arriba
//a abajo, crea un string por columna y luego busca la palabra dentro de ese string.

public class Normal {
    public static void main(String[] args) {
        SopasDeLetra info = LeerMatriz.leerMatrizDesdeArchivo("sopa_de_letras.txt");

        char[][] matriz = info.getMatriz();
        String palabra = info.getPalabraABuscar();

        // Ahora puedes trabajar con la matriz y la palabra a buscar
        long inicio = System.currentTimeMillis();
        buscarPalabra(matriz, palabra);
        long fin = System.currentTimeMillis();

        long tiempoTotal = fin - inicio;

        // Imprimir el tiempo
        System.out.println("Encontrar la palabra tardo: " + tiempoTotal + " milisegundos de forma normal");
    }

    public static void buscarPalabra(char[][] matriz, String palabra) {

        int filas = matriz.length;
        int columnas = matriz[0].length;
        
        // Buscar de izquierda a derecha
        for (int i = 0; i < filas; i++) {
            for (int j = 0; j <= columnas - palabra.length(); j++) {
                String fila = new String(matriz[i], j, palabra.length());
                if (fila.equals(palabra)) {
                    System.out.println("Fila " + i + ", columna " + j);
                    return;
                }
            }
        }

        // Buscar de arriba a abajo
        for (int i = 0; i <= filas - palabra.length(); i++) {
            for (int j = 0; j < columnas; j++) {
                StringBuilder columna = new StringBuilder();
                for (int k = 0; k < palabra.length(); k++) {
                    columna.append(matriz[i + k][j]);
                }
                if (columna.toString().equals(palabra)) {
                    System.out.println("Fila " + i + ", columna " + j);
                    return;
                }
            }
        }

        System.out.println("La palabra a buscar no se encontró en la matriz.");

        return;
    }
}