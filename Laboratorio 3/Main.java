public class Main {
    public static void main(String[] args) {
        //Tiempo Normal
        Normal.main(args);

        //Tiempo Hebras
        Multithread.main(args);

        //Tiempo Fork
        ForkClass.main(args);
    }
}