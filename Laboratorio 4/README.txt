>>> La tarea fue realizada con python3 en WSL <<<

>>> Nombres:

> Nicolás Olivos Muñoz Rol: 202073507-7 Paralelo: 201

> Diego Morales Arellano Rol: 202073511-5 Paralelo: 201

>>> Nombres de archivos:

> T4SO.py : Contiene el código principal del laboratorio 4

>>> Instrucciones Generales:

- Debe arrastrar el archivo T4SO.py a una carpeta cualquiera dentro de su sistema.

- Ya hecho lo anterior, debe abrir la consola y dirigirse con el comando "cd" a la carpeta que contiene el archivo mencionado anteriormente.

- Una vez dentro de la carpeta, para que el código pueda ser utilizado correctamente se debe tener en cuenta lo siguiente:

- Para ejecutar el código simplemente se debe de ejecutar el comando python3 T4SO.py en la terminal

- Se pedirá un input por consola, aquel input corresponde al tiempo en segundos, que deben de esperar las hebras 
que aún no han sido atendidas y se quedaron en la fila sin cumplir el requisito para que las atiendan. Cabe recalcar
que el tiempo que se debe de ingresar por la consola tiene que ser mayor a la cantidad de hebras a crear (500) , porque,
se puede dar el caso que las hebras que vayan a la segunda ida a apelacion, tengan que esperar a las hebras que estan en la primera ida
ya que, no se sabe con certeza si ciertas hebras iran al mismo departamento para esperarlas y cumplir el requisito para que las atiendan normalmente.

La formula ocupada para sacar el tiempo de espera se basa en:

hora_fin = time.time() + tiempo_espera - i + 1

donde:
hora_fin = tiempo total de espera de la hebra, es decir, este es el tiempo limite que una hebra puede estar en la fila de un departamento 
time.time() = el tiempo actual
tiempo_espera = tiempo ingresado por consola
i = numero de la hebra
1 = número obligado para que la primera hebra se coordine con las demás hebras

Aquella formula se realizó de esa manera para que en los diferentes casos que se cumpla el tiempo de espera de las hebras
puedan ser atendidas al mismo tiempo, ya que, de ser de otra forma entrarían una por una:

hora_fin = time.time() + tiempo_espera

Si la formula fuera de esta forma, provocaría que se liberaran las hebras que estaban esperando en la fila, ingresando
una tras otra y no al mismo tiempo

- Una vez ingresado el input y darle al enter, comenzará la ejecución del programa, generando los .txt correspondientes
para su revisión

- Por recomendacion se sugiere el input del tiempo de espera de unos 550 segundos, para que todo se ejecute en orden

- Con el tiempo de 550 segundos se demoro aproximadamente 20 minutos en una ejecucion, cabe recalcar que dependera de muchos casos y 
el azar de como queden las hebras esperando
