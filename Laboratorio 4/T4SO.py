import threading
import time
import random
from datetime import datetime

#Cantidad de hebras/personas a generar
N = 500

# Departamentos del enunciado
departamentos = ["Departamento de Matematicas", "Departamento de Informatica", "Departamento de Fisica",
                  "Departamento de Quimica", "DEFIDER", "Departamento de Mecanica", "Departamento de Minas"]

# Diccionario de tiempos de cada hebra generada, contiene todos sus tiempos
tiempos_personas = {}

for j in range(1, N + 1):
    clave = f"Persona{j}"
    tiempos_personas[clave] = {"tiempo_fila0": "0",
                                    "tiempo_ingreso0": "0",
                                    "tiempo_fila1": "0",
                                    "tiempo_ingreso1": "0",
                                    "tiempo_lamp": "0",
                                    "tiempo_salida": "0"}

# Diccionario que simula cantidad de personas que entra al departamento

contador_departamento = {
    "Departamento de Matematicas": 0,
    "Departamento de Informatica": 0,
    "Departamento de Fisica": 0,
    "Departamento de Quimica": 0,
    "DEFIDER": 0,
    "Departamento de Mecanica": 0,
    "Departamento de Minas": 0,
}

# Diccionario de candados de cada archivo a escribir

escritura_locks = {
    "Departamento de Matematicas": threading.Lock(),
    "Departamento de Informatica": threading.Lock(),
    "Departamento de Fisica": threading.Lock(),
    "Departamento de Quimica": threading.Lock(),
    "DEFIDER": threading.Lock(),
    "Departamento de Mecanica": threading.Lock(),
    "Departamento de Minas": threading.Lock(),
    "Lamparas": threading.Lock(),
    "Salida": threading.Lock(),
}

# Diccionario de candados de cada departamento

departamento_locks = {
    "Departamento de Matematicas": threading.Lock(),
    "Departamento de Informatica": threading.Lock(),
    "Departamento de Fisica": threading.Lock(),
    "Departamento de Quimica": threading.Lock(),
    "DEFIDER": threading.Lock(),
    "Departamento de Mecanica": threading.Lock(),
    "Departamento de Minas": threading.Lock(),
}

# Diccionario de Semaforos que representa la fila de cada departamento

departamento_capacidad_semaforos = {
    "Departamento de Matematicas": threading.Semaphore(20),
    "Departamento de Informatica": threading.Semaphore(8),
    "Departamento de Fisica": threading.Semaphore(15),
    "Departamento de Quimica": threading.Semaphore(6),
    "DEFIDER": threading.Semaphore(6),
    "Departamento de Mecanica": threading.Semaphore(9),
    "Departamento de Minas": threading.Semaphore(7),
}

# Diccionario de Semaforos que representa la capacidad para atender de cada departamento

departamento_atencion_semaforos = {
    "Departamento de Matematicas": threading.Semaphore(10),
    "Departamento de Informatica": threading.Semaphore(2),
    "Departamento de Fisica": threading.Semaphore(5),
    "Departamento de Quimica": threading.Semaphore(3),
    "DEFIDER": threading.Semaphore(5),
    "Departamento de Mecanica": threading.Semaphore(4),
    "Departamento de Minas": threading.Semaphore(2),
}

# Funcion que retorna la capacidad de atencion (capacidad de departamento) de cada departamento dependiendo del string departamento

def capacidad_atender(departamento):
    if departamento == "Departamento de Matematicas":
        return 10
    elif departamento == "Departamento de Informatica":
        return 2
    elif departamento == "Departamento de Fisica":
        return 5
    elif departamento == "Departamento de Quimica":
        return 3
    elif departamento == "DEFIDER":
        return 5
    elif departamento == "Departamento de Mecanica":
        return 4
    elif departamento == "Departamento de Minas":
        return 2
    else:
        return 0
    
# Funcion que retorna la capacidad de la fila de cada departamento dependiendo del string departamento

def capacidad_fila(departamento):
    if departamento == "Departamento de Matematicas":
        return 20
    elif departamento == "Departamento de Informatica":
        return 8
    elif departamento == "Departamento de Fisica":
        return 15
    elif departamento == "Departamento de Quimica":
        return 6
    elif departamento == "DEFIDER":
        return 6
    elif departamento == "Departamento de Mecanica":
        return 9
    elif departamento == "Departamento de Minas":
        return 7

# Funcion que retorna la duracion de consulta de cada departamento dependiendo del string departamento   
def duracion_consulta(departamento):
    if departamento == "Departamento de Matematicas":
        return 9
    elif departamento == "Departamento de Informatica":
        return 5
    elif departamento == "Departamento de Fisica":
        return 7
    elif departamento == "Departamento de Quimica":
        return 4
    elif departamento == "DEFIDER":
        return 1
    elif departamento == "Departamento de Mecanica":
        return 4
    elif departamento == "Departamento de Minas":
        return 5

# Aqui hay funciones para escribir en el formato especificado en la tarea

def escribir_archivo_depa(tiempo_fila, tiempo_ingreso, persona, apelaciones, lock, archivo):
    lock.acquire()
    nom_archivo = archivo.replace(" ", "_")
    texto = f"{persona}, {tiempo_fila}, {tiempo_ingreso}, {apelaciones+1}\n"
    file = open(f"{nom_archivo}.txt", "a")
    file.write(texto)
    file.close()
    lock.release()

def escribir_archivo_lampara(persona, ingreso_lampara, departamento1, tiempo_fin_depa1, departamento2, tiempo_fin_depa2, lock):
    lock.acquire()
    archivo = "PdLamparas"
    texto = f"{persona}, {ingreso_lampara}, {departamento1}, {tiempo_fin_depa1}, {departamento2}, {tiempo_fin_depa2}\n"
    file = open(f"{archivo}.txt", "a")
    file.write(texto)
    file.close()
    lock.release()

def escribir_archivo_salida(tiempo, persona, lock):
    lock.acquire()
    archivo = "Salida"
    texto = f"{persona}, {tiempo}\n"
    file = open(f"{archivo}.txt", "a")
    file.write(texto)
    file.close()
    lock.release()

# Entrar_fila:
# Inputs:
# persona: String que contiene el nombre de la persona (Por ejemplo Persona1)
# depto: String del departamento al que entra la persona
# apelaciones: numero que representa la cantidad de apelaciones que ha hecho la persona
# Funcionalidad:
# Entrar_fila simula la entrada de una fila hacia cualquier departamento, es una funcion general
# donde los semaforos y locks utilizados se extraen de diccionarios previamente definidos
# dependiendo del departamento.
def entrar_fila(persona, depto, apelaciones, hora_fin, i, tiempo_espera):

    departamento = depto[apelaciones]
    flag = False
    duracion_consult = duracion_consulta(departamento)
    candado = departamento_locks[departamento]
    
    #Simula la fila del departamento
    departamento_capacidad_semaforos[departamento].acquire()

    #aqui va el tiempo cuando ingresa a la fila
    tiempos_personas[persona][f"tiempo_fila{apelaciones}"] = datetime.now().strftime("%H:%M:%S.%f")
    #print(f'Persona {persona} entró a la fila {depto[apelaciones]}.')
    #print(tiempos_personas[persona][f"tiempo_fila{apelaciones}"])

    #Simula la cantidad minima necesaria para que ingrese una persona a apelar
    departamento_atencion_semaforos[departamento].acquire()

    departamento_capacidad_semaforos[departamento].release()
    
    candado.acquire()
    contador_departamento[departamento] += 1
    candado.release()
    
    while(contador_departamento[departamento] < capacidad_atender(departamento)):
        if time.time() > hora_fin:
            flag = True
        if(flag):
            break
        #print("estoy esperando")
    
    #aqui va el tiempo cuando lo van a atender
    tiempos_personas[persona][f"tiempo_ingreso{apelaciones}"] = datetime.now().strftime("%H:%M:%S.%f")
    #tiempos_personas[persona] = datetime.now().strftime("%H:%M:%S.%f")

    #print(f'Persona {persona} entró al departamento de {departamento}.')
    time.sleep(duracion_consult)
    #print(f'Persona {persona} salió del departamento de {departamento}.')
    #print(tiempos_personas[persona][f"tiempo_ingreso{apelaciones}"])
    
    candado.acquire()
    contador_departamento[departamento] -= 1
    candado.release()

    #Escribimos en el departamento correspondiente
    escribir_archivo_depa(tiempos_personas[persona][f"tiempo_fila{apelaciones}"], tiempos_personas[persona][f"tiempo_ingreso{apelaciones}"],
                           persona, apelaciones, escritura_locks[departamento], departamento)

    departamento_atencion_semaforos[departamento].release()
    
    apelaciones += 1
    hora_fin = time.time() + tiempo_espera - duracion_consult
    Patio_lamparas(persona, depto, apelaciones, hora_fin, i , tiempo_espera)
    
# Patio_lamparas:
# Inputs:
# persona: String que contiene el nombre de la persona (Por ejemplo Persona1)
# depto: String del departamento al que entra la persona
# apelaciones: numero que representa la cantidad de apelaciones que ha hecho la persona
# Funcionalidad:
# Simula la entrada al patio de lamparas de la universidad, cualquier hebra siempre empezará desde aqui.
# Segun el numero de apelaciones que la persona ha realizado, se retorna la funcion, es decir, 
# se finaliza la hebra.
# Si la persona no ha hecho 2 apelaciones, ingresa al departamento correspondiente

def Patio_lamparas(persona, depto, apelaciones, hora_fin, i, tiempo_espera):
    global departamentos
    if apelaciones == 0:
        #Entra por primera vez al patio
        tiempos_personas[persona]["tiempo_lamp"] = datetime.now().strftime("%H:%M:%S.%f")

    if apelaciones == 2:
        tiempos_personas[persona]["tiempo_salida"] = datetime.now().strftime("%H:%M:%S.%f")
        #print(f'{persona} salio de la u :c.')
        #Salio de la U, entonces se escribe el archivo patio de lamparas y el de salida
        escribir_archivo_lampara(persona, tiempos_personas[persona]["tiempo_lamp"],
                                depto[0], tiempos_personas[persona]["tiempo_fila0"],
                                depto[1], tiempos_personas[persona]["tiempo_fila1"],
                                escritura_locks["Lamparas"])
        
        escribir_archivo_salida(tiempos_personas[persona]["tiempo_salida"],
                                persona, escritura_locks["Salida"])

        return
    else:
        #print(f'Persona {persona} entra al Patio de lamparas.')
        entrar_fila(persona, depto, apelaciones, hora_fin, i , tiempo_espera)
    return

# crear_archivos:
# Inputs: VOID
# Funcionalidad:
# Crea los archivos necesarios para la posterior escritura

def crear_archivos():
    for nombre in departamentos:
        nom_archivo = nombre.replace(" ", "_")
        archivo = open(f"{nom_archivo}.txt", "w")
        archivo.close()
    nom_archivo = "PdLamparas"
    archivo = open(f"{nom_archivo}.txt", "w")
    archivo.close()
    nom_archivo = "Salida"
    archivo = open(f"{nom_archivo}.txt", "w")
    archivo.close()

# simular_apelaciones:
# Inputs: VOID
# Funcionalidad:
# Se invoca crear_archivos para crear los archivos y se inicializan las hebras y las hace ingresar al patio de lamparas
def simular_apelaciones():
    global N
    crear_archivos()
    threads = []
    apelaciones_realizadas = 0
    tiempo_espera = int(input("Tiempo de espera para las hebras: "))
    for i in range(1, N+1):
        persona = f'Persona{i}'
        hora_fin = time.time() + tiempo_espera - i + 1
        departamento = random.sample(departamentos, 2)
        thread = threading.Thread(target=Patio_lamparas, args=(persona, departamento, apelaciones_realizadas, hora_fin, i, tiempo_espera))
        threads.append(thread)
   
        time.sleep(1)
        thread.start()

    for thread in threads:
        thread.join()

# Inicio
if __name__ == "__main__":
    simular_apelaciones()