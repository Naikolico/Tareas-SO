//Clase SopasDeLetra:
//Clase para obtener de forma mas sencilla y rapida la sopa de letras y la palabra a buscar del archivo leido.
//char[][] matriz, corresponde a la sopa de letras
//String palabraABuscar, corresponde a la palabra a buscar xd
//Metodos:
//getMatriz(), Para obtener la matriz leida del archivo
//getPalabraABuscar(), Para obtener la palabra a buscar correspondiente del archivo
public class SopasDeLetra {
    private char[][] matriz;
    private String palabraABuscar;

    public SopasDeLetra(char[][] matriz, String palabraABuscar) {
        this.matriz = matriz;
        this.palabraABuscar = palabraABuscar;
    }

    public char[][] getMatriz() {
        return matriz;
    }

    public String getPalabraABuscar() {
        return palabraABuscar;
    }
}