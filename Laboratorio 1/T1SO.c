#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <errno.h>
#include <dirent.h>
#include <ctype.h>

#define MAX_SIZE 1000

void crear_carpeta(){
    char nombre_carpeta[100] = "palabras";

    // Verificar si la carpeta ya existe
    struct stat st;
    if (stat(nombre_carpeta, &st) == 0 && S_ISDIR(st.st_mode)) {
        printf("La carpeta '%s' ya existe.\n", nombre_carpeta);
    } else {
        if (mkdir(nombre_carpeta, 0777) == 0) {
            printf("Carpeta '%s' creada exitosamente.\n", nombre_carpeta);
        } else {
            perror("Error al crear la carpeta");
            return;
        }
    }
    return;
}

void crear_carpeta_nombre(char nombre_carpeta[20]){

    //Verificar si la carpeta ya existe
    struct stat st;
    if (stat(nombre_carpeta, &st) == 0 && S_ISDIR(st.st_mode)) {
        printf("La carpeta '%s' ya existe.\n", nombre_carpeta);
    } else {
        if (mkdir(nombre_carpeta, 0777) == 0) {
            printf("Carpeta '%s' creada exitosamente.\n", nombre_carpeta);
        } else {
            perror("Error al crear la carpeta");
            return;
        }
    }
    return;
}



int main() {
    clock_t start, end;
    double cpu_time_used;
    char carpeta1[20] = "CWD";

    crear_carpeta_nombre(carpeta1);

    char dir_padre[256];

    //Obtener el directorio padre
    if (getcwd(dir_padre, sizeof(dir_padre)) == NULL) {
        perror("Error al obtener el directorio actual");
        return 1;
    }
    //Tengo direccion padre de la carpeta Tarea1, que contiene el codigo y los txt

    //Ir al directorio "CWD"
    if (chdir(carpeta1) != 0) {
        perror("Error al cambiar de directorio");
        return 1;
    }else{
        printf("Si llegue '%s' ya existe.\n", carpeta1);
    }

    char dir_cwd[256];

    if (getcwd(dir_cwd, sizeof(dir_cwd)) == NULL) {
        perror("Error al obtener el directorio actual");
        return 1;
    }

    char carpeta1_1[20] = "Horizontal";
    char carpeta1_2[20] = "Vertical";

    crear_carpeta_nombre(carpeta1_1);
    crear_carpeta_nombre(carpeta1_2);


    if (chdir(carpeta1_1) != 0) {
        perror("Error al cambiar de directorio");
        return 1;
    }else{
        printf("Si llegue '%s' ya existe.\n", carpeta1);
    }

    char dir_ho[256];

    if (getcwd(dir_ho, sizeof(dir_ho)) == NULL) {
        perror("Error al obtener el directorio actual");
        return 1;
    }

    if (chdir(dir_cwd) != 0) {
        perror("Error al cambiar de directorio");
        return 1;
    }else{
        printf("Si llegue '%s' ya existe.\n", carpeta1);
    }

    if (chdir(carpeta1_2) != 0) {
        perror("Error al cambiar de directorio");
        return 1;
    }else{
        printf("Si llegue '%s' ya existe.\n", carpeta1);
    }

    char dir_ve[256];

    if (getcwd(dir_ve, sizeof(dir_ve)) == NULL) {
        perror("Error al obtener el directorio actual");
        return 1;
    }


    //Lectura de archivos

    int found = 0;
    char direccion[11];
    char nombre_archivo[256];


    //Volver al directorio padre
    if (chdir(dir_padre) != 0) {
        perror("Error al cambiar de directorio");
        return 1;
    }

    //Abre el directorio actual
    DIR *dir = opendir(".");
    if (dir == NULL) {
        perror("Error al abrir el directorio");
        return 1;
    }

    //Lee los nombres de los archivos en el directorio
    struct dirent *entrada;
    while ((entrada = readdir(dir)) != NULL) {
        // Verifica si el nombre de archivo termina en ".txt"
        if (strstr(entrada->d_name, ".txt") != NULL) {
            // Abre el archivo en modo de lectura
            FILE *archivo = fopen(entrada->d_name, "r");
            if (archivo == NULL) {
                perror("Error al abrir el archivo");
                return 1;
            }

            //Obtener nombre del archivo = palabra
            strncpy(nombre_archivo, entrada->d_name, strlen(entrada->d_name) - 4);
            nombre_archivo[strlen(entrada->d_name) - 4] = '\0';

            printf("Leyendo archivo: %s\n", nombre_archivo);

            char nombre_original[256];
            strcpy(nombre_original, nombre_archivo);

            //Transformamos la palabra en mayusculas
            for (int i = 0; nombre_archivo[i]; i++) {
                nombre_archivo[i] = toupper(nombre_archivo[i]);
            }

            //Leer orientacion
            fgets(direccion, sizeof(direccion), archivo);

            //Transformamos la direccion en mayuscula
            for (int i = 0; direccion[i]; i++) {
                direccion[i] = toupper(direccion[i]);
            }

            //Nos aseguramos de que la direccion contenga un salto de linea \n
            int len = strlen(direccion);
            if (len > 0 && direccion[len - 1] != '\n') {
                // Agrega un salto de línea manualmente
                direccion[len] = '\n';
                direccion[len + 1] = '\0';
            }
    
            printf("Orientacion de la palabra: %s", direccion);


            //Se crea la matriz ignorando los espacios en blanco
            int rows = 0, cols = 0;
            char matrix[MAX_SIZE][MAX_SIZE];
            char line[MAX_SIZE];
            while (fgets(line, sizeof(line), archivo)) {
                int col = 0;
                for (int i = 0; line[i]; i++) {
                    if (!isspace(line[i])) {  //Aqui se ignoran los espacios en blanco
                        matrix[rows][col] = toupper(line[i]);
                        col++;
                    }
                }

                if (col > 0) {
                    if (col > cols) {
                        cols = col;
                    }
                    rows++;
                }
            }

            if (strcmp(direccion, "HORIZONTAL\n") == 0) {
                start = clock();
                for (int i = 0; i < rows; i++) {
                    if (strstr(matrix[i], nombre_archivo) != NULL) {
                        printf("Palabra encontrada en la fila %d\n", i + 1);
                        found = 1;
                        break;
                    }
                }
            } else if (strcmp(direccion, "VERTICAL\n") == 0) {
                start = clock();
                for (int j = 0; j < cols; j++) {
                    char col[MAX_SIZE];
                    for (int i = 0; i < rows; i++) {
                        col[i] = matrix[i][j];
                    }
                    col[rows] = '\0';

                    if (strstr(col, nombre_archivo) != NULL) {
                        printf("Palabra encontrada en la columna %d\n", j + 1);
                        found = 1;
                        break;
                    }
                }
            }
            end = clock();


            // Calcula el tiempo de CPU utilizado en segundos
            cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

            //Ver si no se encontro la palabra
            if (!found) {
                printf("Palabra no encontrada\n");
            }

            printf("Tiempo de CPU utilizado: %f segundos\n", cpu_time_used);

            printf("tamano de la matriz %d x %d \n", rows,cols);


            // Cierra el archivo
            fclose(archivo);
            found = 0;


            char carpeta_n[256];
            char dir_n[256];
            char dir_archivo[1024];
            char dir_archivo_n[1024];

            if (strcmp(direccion, "HORIZONTAL\n") == 0) {

                if (chdir(dir_cwd) != 0) {
                    perror("Error al cambiar de directorio");
                    return 1;
                }

                if (chdir(dir_ho) != 0) {
                    perror("Error al cambiar de directorio");
                    return 1;
                }

                sprintf(carpeta_n, "%dx%d", rows, cols);
                crear_carpeta_nombre(carpeta_n);

                if (chdir(carpeta_n) != 0) {
                    perror("Error al cambiar de directorio");
                    return 1;
                }

                 if (getcwd(dir_n, sizeof(dir_n)) == NULL) {
                    perror("Error al obtener el directorio actual");
                    return 1;
                }

                if (chdir(dir_padre) != 0) {
                    perror("Error al cambiar de directorio");
                    return 1;
                }

                //Aqui se mueven los archivos

                snprintf(dir_archivo,sizeof(dir_archivo), "%s/%s.txt", dir_padre, nombre_original);
                snprintf(dir_archivo_n,sizeof(dir_archivo_n), "%s/%s.txt", dir_n, nombre_original);

                if (rename(dir_archivo, dir_archivo_n) == 0) {
                    printf("Archivo movido exitosamente.\n");
                } else {
                    perror("Error al mover el archivo");
                    return 1;
                }

            }else{

                if (chdir(dir_cwd) != 0) {
                    perror("Error al cambiar de directorio");
                    return 1;
                }

                if (chdir(dir_ve) != 0) {
                    perror("Error al cambiar de directorio");
                    return 1;
                }

                sprintf(carpeta_n, "%dx%d", rows, cols);
                crear_carpeta_nombre(carpeta_n);

                if (chdir(carpeta_n) != 0) {
                    perror("Error al cambiar de directorio");
                    return 1;
                }

                if (getcwd(dir_n, sizeof(dir_n)) == NULL) {
                    perror("Error al obtener el directorio actual");
                    return 1;
                }

                if (chdir(dir_padre) != 0) {
                    perror("Error al cambiar de directorio");
                    return 1;
                }

                //Aqui se mueven los archivos

                snprintf(dir_archivo,sizeof(dir_archivo), "%s/%s.txt", dir_padre, nombre_original);
                snprintf(dir_archivo_n,sizeof(dir_archivo_n), "%s/%s.txt", dir_n, nombre_original);

                if (rename(dir_archivo, dir_archivo_n) == 0) {
                    printf("Archivo movido exitosamente.\n");
                } else {
                    perror("Error al mover el archivo");
                    return 1;
                }
            }

            printf("\n");

        }
    }

    // Cierra el directorio
    closedir(dir);


    return 0;
}