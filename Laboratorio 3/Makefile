# Compilador Java
JAVAC = javac

# Ejecutable Java
JAVA = java

# Lista de archivos fuente Java
SOURCES = Main.java Normal.java Multithreading.java ForkClass.java LeerMatriz.java SopasDeLetra.java

# Lista de archivos .class generados
CLASSES = Main.class Normal.class Multithreading.class ForkClass.class LeerMatriz.class SopasDeLetra.class Multithread.class ForkJoinClass.class

# Clase principal
MAIN_CLASS = Main

# Regla para compilar los archivos fuente Java
%.class: %.java
	$(JAVAC) $<

# Regla para ejecutar el programa
run: $(CLASSES)
	$(JAVA) $(MAIN_CLASS)

# Regla para limpiar los archivos compilados
clean:
	rm -f $(CLASSES)

# Regla por defecto, compila y ejecuta el programa
default: run
