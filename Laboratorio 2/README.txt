>>> Nombres:

> Nicolás Olivos Muñoz  Rol: 202073507-7  Paralelo: 201

> Diego Morales Arellano Rol: 202073511-5  Paralelo: 201

>>> Nombres de archivos:

> T2SO.c : Tarea con el codigo de busqueda de palabras de sopa de letras sin optimizar

> Makefile: Archivo tipo make para la compilacion

>>> Instrucciones Generales:

Para que el codigo pueda ser utilizado correctamente se debe tener en cuenta lo siguiente:

- Debe arrastrar el archivo .c junto con el Makefile a una carpeta cualquiera dentro de su sistema.

- Debe colocar los archivos .txt (laberintos) de prueba en la carpeta donde coloco los archivos mencionados en el punto anterior (Asegurarse de que los archivos .txt no esten corruptos o mal nombrados)

Ya hecho lo anterior, debe abrir la consola y dirigirse con el comando "cd" a la carpeta que contiene todos los archivos mencionados anteriormente. Una vez dentro de la carpeta, dentro de la consola debe escribir el comando "make" que compilara el archivo T2SO.c y para ejecutar debe escribir en la consola "make run".


>>> Puntos importantes a tomar en cuenta:

- La tarea es una combinacion de la primera forma de la tarea y la segunda, esto se debe a que estuvimos haciendo la tarea
  con el pdf inicial y les hicimos modificaciones segun lo que se hablaba en discord. Hablamos con el ayudante Nicolás
  sobre el tema y nos especifico colocar esto en el readme.

- La comunicacion de pipes se realiza pasandole el tablero al jugador que le toque jugar (Se le pasa el tablero e informacion
  adicional que sirve para optimizar y gestionar ciertas partes de la partida), entonces existe una comunicacion circular
  entre los hijos con el tablero hasta que acaban las rondas o se cumpla la condicion de ganar (Todos los hijos en el tablero inicial con sus tesoros).
  Cuando una condicion de finalizacion se da, los pipes se cierran y un solo hijo imprime si ganaron o perdieron el juego. Finalizando el programa por completo

- Los inputs se piden por jugador.

- Los jugadores por comodidad se "cambiaron" por caracteres más sencillos de utilizar que los que aparecen en la tarea, es   decir J1 pasa a ser 'W', J2 pasa a   ser 'X', J3 pasa a ser 'Y' y por ultimo J4 pasa a ser 'Z'.

- Tal como los jugadores cambiaron por caracteres más sencillos de los que aparecen en la tarea, tambien lo hacen las casillas   especiales, estos cambios son:
      1) Casillas bc de camaras = 'C'
      2) Casillas bt de ganar turnos = 'T'
      3) Casillas bn de perder turnos = 'N'
      4) Casillas btpN de tp para jugador N = 'w' || 'x' || 'y' || 'z' ; Tal que estos van del jugador 1 al 4

- Los tps especiales de los jugadores ('w', 'x', 'y', 'z') son instantaneos, es decir, solo cuando el jugador caiga sobre
  ellas se accionarán automaticamente y se teletransportara donde corresponda siempre y cuando las condiciones se lo permitan.
  Condiciones:
      1) Hay 2 casillas del tp para el jugador
      2) No hay un jugador sobre la casilla de tp

- Los tps especiales de los jugadores los limitamos a maximo 2 por jugador.

- Los tps especiales solo los puede pisar el jugador al que le corresponde el tp, por lo que actua como muro para otros

- En la tarea, suponemos que al realizar la accion buscar 'B' debe si o si descubrir un nuevo laberinto.

- En algunos momentos, luego de realizar la accion buscar, en el mapa aparecerá una 'b', esta letra representa que
  la puerta 'B' fue abierta y cualquier jugador puede acceder a ella.

- Si se quiere cambiar de laberinto, es decir pasar por una puerta 'b' (ejemplo: del laberinto 1 a 0)
  se debe caer sobre la casilla 'b' y haber tenido un movimiento a la misma direccion de la puerta (Si el mov
  del jugador fue 'w' es decir, hacia arriba y cae sobre una casilla 'b' que esta arriba del laberinto, cambia
  al otro laberinto siempre y cuando el jugador quiera cambiarse de laberinto, caso contrario no se cambiara de laberinto
  ni se le preguntara al jugador si quiere cambiarse de laberinto)

- Si el jugador quiere realizar accion sin estar sobre una casilla donde pueda realizarlo, pierde su turno

- Asumimos que los jugadores colocarán inputs correctos por consola, igualmente programamos posibles equivocaciones o errores   para evitar que el jugador pierda su turno al colocar inputs incorrectos.

- Se agregó una opcion de 4) Tomar tesoro que solo aparece cuando el jugador puede tomar su tesoro

- Los jugadores que no les corresponde el tesoro, lo veran como un muro, es decir, no lo pueden traspasar ni ponerse encima

- El juego finaliza solo si todos los jugadores tienen sus tesoros y estan en el laberinto de inicio o cuando se acaban las   rondas

- Se agrego un boton para finalizar el programa antes de tiempo (opcion 3) salir)
