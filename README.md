Simulador de Transporte Vecinal

Equipo Cachacuas

Integrantes:
- Miguel Angel Garcia Mateo
- Carlos Antonio Gordillo Cejudo
- Joshua Orozco Pérez


Descripción del Proyecto

Sistema de simulación que modela el flujo de pasajeros en una ruta de transporte público con 5 paradas. El simulador implementa:

- Cola de prioridad para pasajeros (Discapacidad > Adulto Mayor > Normal)
- Lista circular de paradas conectadas
- Dos unidades de transporte con 10 asientos cada una
- Análisis estadístico de tiempos de espera y ocupación
- Clasificación por horario (Mañana, Tarde, Noche)


Requisitos del Sistema

- Compilador de C (GCC recomendado)
- Sistema operativo: Windows, Linux o macOS
- Memoria: Mínimo 512 MB RAM



Compilación y Ejecución

En Linux/macOS:

```bash
Compilar el programa
gcc -o simulador simulador_transporte.c -Wall

Ejecutar
./simulador
```


En Windows:

```cmd
Compilar con GCC (MinGW)
gcc -o simulador.exe simulador_transporte.c -Wall

Ejecutar
simulador.exe
```

Alternativa con Make:

Crear un archivo `Makefile`:

```makefile
CC = gcc
CFLAGS = -Wall -Wextra
TARGET = simulador

all: $(TARGET)

$(TARGET): simulador_transporte.c
	$(CC) $(CFLAGS) -o $(TARGET) simulador_transporte.c

clean:
	rm -f $(TARGET)
```

Luego ejecutar:
```bash
make
./simulador
```


Casos de Prueba

Caso 1: Prueba Básica con Prioridad

Objetivo: Verificar que el sistema respeta la cola de prioridad

Pasos:
1. Agregar pasajero: Juan, Parada 1→3, Prioridad Normal, Horario Mañana
2. Agregar pasajero: María, Parada 1→4, Prioridad Adulto Mayor, Horario Mañana
3. Agregar pasajero: Pedro, Parada 1→2, Prioridad Discapacidad, Horario Mañana
4. Ejecutar simulación

Resultado esperado:
- Pedro sube primero (Prioridad 3)
- María sube segunda (Prioridad 2)
- Juan sube último (Prioridad 1)


Caso 2: Capacidad Máxima

Objetivo: Verificar comportamiento cuando la unidad está llena

Pasos:
1. Agregar 12 pasajeros en Parada 1 con destino a Parada 5
2. Todos con prioridad Normal
3. Ejecutar simulación

Resultado esperado:
- Unidad 1 lleva 10 pasajeros
- 2 pasajeros quedan esperando
- Unidad 2 recoge a los 2 restantes
- Alerta de pasajeros sin abordar en el reporte


Caso 3: Distribución por Horarios

Objetivo: Verificar análisis estadístico por horario

Pasos:
1. Agregar 3 pasajeros en horario Mañana (Parada 1→4)
2. Agregar 5 pasajeros en horario Tarde (Parada 2→5)
3. Agregar 2 pasajeros en horario Noche (Parada 3→1)
4. Ejecutar simulación
5. Ver reporte

Resultado esperado:
- Reporte muestra: Tarde como horario pico (5 pasajeros)
- Estadísticas correctas por parada


Caso 4: Vuelta Extra Requerida

Objetivo: Verificar manejo de destinos que ya pasaron

Pasos:
1. Agregar pasajero en Parada 4 con destino a Parada 2
2. Ejecutar simulación

Resultado esperado:
- Sistema indica "[Vuelta extra requerida]"
- Pasajero completa el ciclo hasta llegar a Parada 2


Caso 5: Tiempos de Espera

Objetivo: Calcular tiempos promedio de espera

Pasos:
1. Agregar 5 pasajeros en Parada 1
2. Agregar 5 pasajeros en Parada 3
3. Ejecutar simulación
4. Ver reporte

Resultado esperado:
- Parada 1: tiempo de espera menor (primera en la ruta)
- Parada 3: tiempo de espera mayor (tercera en la ruta)
- Promedios calculados correctamente


División de Tareas del Equipo

Miguel Angel Garcia Mateo
Módulos implementados:
- Estructura de datos principal (Pasajero, Parada, Unidad, Asiento)
- Sistema de lista circular de paradas
- Función inicializarParadas() y buscarParada()
- Función agregarPasajero() con validaciones
- Gestión de memoria (liberarMemoria())

Responsabilidades:
- Diseño de la arquitectura de datos
- Validación de entradas del usuario
- Documentación de estructuras


Joshua Orozco Perez
Módulos implementados:
- Motor de simulación (simular())
- Lógica de movimiento de unidades (avanzarUnidad())
- Sistema de abordaje y descenso (subirPasajeros(), bajarPasajeros())
- Funciones auxiliares de verificación (contarAsientosOcupados(), unidadTienePasajeros())
- Coordinación entre las dos unidades

Responsabilidades:
- Lógica central de la simulación
- Gestión del tiempo de recorrido
- Cálculo de tiempos de abordaje


Carlos Antonio Gordillo Cejudo
Módulos implementados:
- Sistema de cola de prioridad (insertarEnCola())
- Generación de reportes (reporte())
- Cálculo de estadísticas (promedios, porcentajes)
- Análisis por horario
- Sistema de alertas

Responsabilidades:
- Algoritmo de prioridad
- Análisis de datos y métricas
- Identificación de cuellos de botella


Características Técnicas

Estructuras de Datos Utilizadas

1. Lista Circular: Representa las 5 paradas conectadas
2. Cola de Prioridad: Organiza pasajeros por nivel de prioridad
3. Arreglo Estático: Almacena los asientos de cada unidad
4. Memoria Dinámica: Gestión de pasajeros con malloc() y free()

Constantes del Sistema

```c
NUM_PARADAS           = 5   // Paradas en la ruta
ASIENTOS_POR_UNIDAD   = 10  // Capacidad por unidad
NUM_UNIDADES          = 2   // Unidades circulando
TIEMPO_ENTRE_PARADAS  = 5   // Minutos de viaje
TIEMPO_ABORDAJE       = 1   // Minuto por pasajero
```


Funcionalidades Principales

1. Agregar Pasajero
- Captura datos del pasajero
- Valida paradas origen/destino
- Asigna prioridad y horario
- Inserta en cola respetando prioridad

2. Ejecutar Simulación
- Unidad 1 inicia en t=0
- Unidad 2 inicia en t=10
- Proceso: Bajar → Subir → Avanzar
- Finaliza cuando ambas unidades regresan vacías a Parada 1

3. Ver Reporte
- Estadísticas por parada (abordajes, tiempos)
- Estadísticas por unidad (ocupación, recorrido)
- Análisis por horario (picos de demanda)
- Alertas de saturación


Ejemplo de Uso Completo

```
========================================
  SIMULADOR DE TRANSPORTE VECINAL
========================================
1. Agregar pasajero
2. Ejecutar simulacion
3. Ver reporte
4. Salir
========================================

Que desea hacer? 1

=========AGREGAR PASAJERO=========

Nombre del pasajero: Ana López
Parada actual (1-5): 1
Parada destino (1-5): 3
Prioridad:
  1 = Normal
  2 = Adulto Mayor
  3 = Discapacidad
Seleccione: 2
Horario de viaje:
  1 = Manana (6am-12pm)
  2 = Tarde (12pm-6pm)
  3 = Noche (6pm-12am)
Seleccione: 1

Pasajero 'Ana López' [ID: 1] agregado a la cola de Parada 1

========================================

Continuar con esta opcion? (s/n): n

[Repetir para agregar más pasajeros...]

Que desea hacer? 2

[Simulación se ejecuta...]

Que desea hacer? 3

[Se muestra el reporte completo...]
```


Notas Importantes

Limitaciones conocidas:
- Máximo 10 pasajeros por unidad
- Una vez ejecutada la simulación, debe reiniciar el programa para una nueva
- Los pasajeros se agregan en t=0 (no hay llegadas dinámicas durante la simulación)

Mejoras futuras:
- Implementar múltiples vueltas
- Agregar llegadas dinámicas de pasajeros
- Exportar reportes a archivo CSV
- Interfaz gráfica


Solución de Problemas

Error: "No hay memoria disponible"
Solución: Cerrar otros programas y reintentar

Error: Compilación fallida
Solución: Verificar que GCC esté instalado correctamente
```bash
gcc --version
```

Comportamiento inesperado en simulación
Solución: Verificar que los destinos sean diferentes al origen


Licencia

Proyecto académico desarrollado para fines educativos.


Contacto

Para reportar problemas o sugerencias, contactar a los integrantes del equipo.
