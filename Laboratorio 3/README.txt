>>> Nombres:

> Nicolás Olivos Muñoz  Rol: 202073507-7  Paralelo: 201

> Diego Morales Arellano Rol: 202073511-5  Paralelo: 201

>>> Nombres de archivos:

> ForkClass.java : Parte de busqueda de palabra con fork

> LeerMatriz.java : Lee el archivo sopa_de_letras.txt y guarda en una clase SopasDeLetra

> Main.java : Codigo main que ejecuta todos los metodos de busqueda

> Multithread.java : Parte de busqueda de palabra con hebras

> Normal.java : Parte de busqueda de palabra sin ningun algoritmo

> SopasDeLetra : Clase para almacenar matriz y palabra a buscar

> Makefile: Archivo tipo make para la compilacion de los .java

> Hilitos.java : Archivo EXTRA, este es una busqueda de hebras con extend Threads que fue hecho despues de Multithread.java (Mas info abajo)

> Analisis.xlsx : Archivo Excel que contiene el analisis de los resultados obtenidos con la solucion de la tarea


>>> Instrucciones Generales:

Para que el codigo pueda ser utilizado correctamente se debe tener en cuenta lo siguiente:

- Debe arrastrar los archivos .java (No incluir Hilitos.java) junto con el Makefile a una carpeta cualquiera dentro de su sistema.

- Debe colocar el archivo .txt (sopa_de_letras) de prueba en la carpeta donde coloco los archivos mencionados en el punto anterior (Asegurarse de que los archivos .txt no esten corruptos o mal nombrados)

Ya hecho lo anterior, debe abrir la consola y dirigirse con el comando "cd" a la carpeta que contiene todos los archivos mencionados anteriormente. Una vez dentro de la carpeta, dentro de la consola debe escribir el comando "make" y para ejecutar debe escribir en la consola "make run".

- Ejecutar make clean para limpiar los archivos .class

>>> Puntos importantes a tomar en cuenta:

- La busqueda de palabras con Hebras se realizo utilizando la clase Multithreading implements Runnable, ya que habian dos maneras de implementarlo (extends Thread y implements Runnable). El ayudante Nicolás nos mencionó
  que colocaramos esto en el readme. Sin embargo, a pesar de realizar la busqueda utilizando implements Runnable, creamos un archivo .java aparte llamado Hilitos.java, que es la misma busqueda de hebras pero utilizando
  extends Thread, se puede probar sin problema, pero decidimos quedarnos con el primer archivo que creamos (Multithread.java) para la entrega.

- El archivo Hilitos.java es la busqueda de palabras con hebras pero utilizando "extends Thread", puede probarlo sin problema, pero no esta incluido en el make para ser testeado

- Utilizamos una variable compartida para detener los procesos y hebras cuando en uno de los cuadrantes se encuentra la palabra

- En la Busqueda de hebras, sabemos que podemos realizar una busqueda mas eficiente utilizando hebras por cada busqueda (de izq a derecha y de arriba a abajo), sin embargo, al realizar hebras por cada busqueda
  por alguna razon nos daba peor tiempo, asi que decidimos que cuando los cuadrantes no se puedan reducir más, la hebra realizará una busqueda "Normal" en su cuadrante correspondiente.
