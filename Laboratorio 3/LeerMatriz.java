import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

//LeerMatriz:
//Lee el archivo de nombre "archivo" que esta presente en la misma carpeta del programa, extrae la informacion del txt como la matriz y
//la palabra a buscar en la sopa de letras guardando esa info en la clase SopasDeLetra para su futura extraccion.
public class LeerMatriz {
    public static SopasDeLetra leerMatrizDesdeArchivo(String archivo) {
        try {
            BufferedReader br = new BufferedReader(new FileReader(archivo));

            //Lectura de la dimensión de la matriz
            int dimension = Integer.parseInt(br.readLine().trim());

            //Lectura de la palabra a buscar
            String palabraABuscar = br.readLine().trim();

            //Inicializa la variable sopaDeLetras
            char[][] sopaDeLetras = new char[dimension][dimension];

            //Lectura de la sopa de letras
            for (int i = 0; i < dimension; i++) {
                String fila = br.readLine().trim();
                for (int j = 0; j < dimension; j++) {
                    sopaDeLetras[i][j] = fila.charAt(j * 2); // Asumiendo que las letras están separadas por un espacio
                }
            }

            //Cierre del BufferedReader
            br.close();

            //Retorna la info como SopasDeLetras
            return new SopasDeLetra(sopaDeLetras, palabraABuscar);

        } catch (IOException e) {
            e.printStackTrace();
        }
        return null;
    }
}
