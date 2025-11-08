/*
================================================================================
SIMULADOR DE TRANSPORTE VECINAL
EQUIPO CACHACUAS
Integrantes:
-Miguel Angel Garcia Mateo
-Carlos Antonio Gordillo Cejudo
-Joshua Orozco Perez
================================================================================
Descripción:
    Simula el flujo de pasajeros en una ruta de transporte público con 5 paradas.
    Permite agregar pasajeros con prioridad, simular recorridos y
    generar reportes de ocupación y tiempos de espera.
================================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Se modificara la contante de asientos para comprobar el funcionamiento 10 -> 3
//Hubo un error y no modifique los asientos pero igual se demostro el funcionamiento


// ============================================================================
// CONSTANTES DEL SISTEMA
// ============================================================================
#define NUM_PARADAS 5                   // Número de paradas en la ruta
#define ASIENTOS_POR_UNIDAD 10          // Capacidad de cada unidad
#define NUM_UNIDADES 2                  // Cantidad de unidades circulando
#define TIEMPO_ENTRE_PARADAS 5          // Minutos entre cada parada
#define TIEMPO_ABORDAJE 1               // Minutos por pasajero al abordar

// ============================================================================
// ESTRUCTURA: PASAJERO
// ============================================================================
/*
    Representa a un pasajero esperando en una parada.
    Se organiza en una cola de prioridad (lista enlazada).
*/
typedef struct Pasajero {
    int id_P;                           // ID único del pasajero
    char nombre[50];                    // Nombre del pasajero
    int paradaActual;                   // Parada donde espera (1-5)
    int paradaDestino;                  // Parada a donde va (1-5)
    int prioridad;                      // 1=Normal, 2=Adulto Mayor, 3=Discapacidad
    int tiempoLlegada;                  // Minuto en que llegó a la parada
    int tiempoEspera;                   // Minutos que esperó
    int horarioTipo;                    // 1=Mañana, 2=Tarde, 3=Noche
    struct Pasajero *siguiente;         // Puntero al siguiente en la cola
} Pasajero;

// ============================================================================
// ESTRUCTURA: PARADA
// ============================================================================
/*
    Representa una parada de la ruta.
    Las paradas forman una lista circular (5 → 1).
*/
typedef struct Parada {
    int numero;                         // Número de la parada (1-5)
    Pasajero *colaPasajeros;            // Cola de pasajeros esperando
    int totalPasajerosSubieron;         // Contador de pasajeros que abordaron
    int totalPasajerosEsperando;        // Pasajeros actualmente en espera
    int tiempoTotalAbordaje;            // Suma de minutos de abordaje
    int sumaTiemposEspera;              // Suma de tiempos de espera
    float tiempoPromedioEspera;         // Promedio calculado
    int pasajerosManana;                // Contador por horario
    int pasajerosTarde;                 // Contador por horario
    int pasajerosNoche;                 // Contador por horario
    struct Parada *siguiente;           // Puntero a la siguiente parada
} Parada;

// ============================================================================
// ESTRUCTURA: ASIENTO
// ============================================================================
/*
    Representa un asiento dentro de la unidad de transporte.
*/
typedef struct {
    int ocupado;                        // 0=Libre, 1=Ocupado
    Pasajero *pasajero;                 // Puntero al pasajero sentado (NULL si libre)
} Asiento;

// ============================================================================
// ESTRUCTURA: UNIDAD
// ============================================================================
/*
    Representa una unidad de transporte.
    Contiene un arreglo de asientos.
*/
typedef struct {
    int id;                             // Identificador de la unidad (1 o 2)
    Asiento asientos[ASIENTOS_POR_UNIDAD]; // Arreglo de asientos
    int pasajerosTransportados;         // Total de pasajeros que han subido
    float ocupacionPromedio;            // Porcentaje promedio de ocupación
    int sumaOcupaciones;                // Suma de asientos ocupados por parada
    int paradasVisitadas;               // Contador de paradas visitadas
    int tiempoTotal;                    // Tiempo total del recorrido
    int tiempoActual;                   // Tiempo actual en la simulación
} Unidad;

// ============================================================================
// VARIABLES GLOBALES
// ============================================================================
int contadorID = 1;                     // Generador de IDs únicos para pasajeros
int simulacionRealizada = 0;            // Bandera: 0=No simulado, 1=Ya simulado

// ============================================================================
// PROTOTIPOS DE FUNCIONES
// ============================================================================

// --- Menús ---
int menu(void);
int continuar(int opc);

// --- Inicialización ---
void inicializarParadas(Parada **primera);
void inicializarUnidades(Unidad unidades[]);

// --- Operaciones principales ---
void agregarPasajero(Parada **primera);
void simular(Parada *primera, Unidad unidades[]);
void reporte(Parada *primera, Unidad unidades[]);

// --- Funciones auxiliares ---
void insertarEnCola(Parada *parada, Pasajero *nuevo);
Parada* buscarParada(Parada *primera, int numero);
void avanzarUnidad(Unidad *unidad, Parada **paradaActual);
void subirPasajeros(Unidad *unidad, Parada *parada);
void bajarPasajeros(Unidad *unidad, int numeroParada);
int contarAsientosOcupados(Unidad *unidad);
int unidadTienePasajeros(Unidad *unidad);
void liberarMemoria(Parada **primera, Unidad unidades[]);

// ============================================================================
// FUNCIÓN PRINCIPAL
// ============================================================================
int main(void) {
    int opc;                            // Variable para guardar opción del menú
    Parada *primeraParada = NULL;       // Puntero a la primera parada
    Unidad unidades[NUM_UNIDADES];      // Arreglo de unidades de transporte
    
    // Inicializar estructuras
    inicializarParadas(&primeraParada);
    inicializarUnidades(unidades);
    
    // Mostrar menú inicial
    opc = menu();
    
    // Bucle principal del programa
    do {
        switch(opc) {
            case 1: // Agregar pasajero
                agregarPasajero(&primeraParada);
                opc = continuar(1);
                break;
                
            case 2: // Ejecutar simulación
                simular(primeraParada, unidades);
                opc = continuar(2);
                break;
                
            case 3: // Ver reporte
                reporte(primeraParada, unidades);
                opc = continuar(3);
                break;
                
            case 4: // Salir
                printf("Saliendo del programa...\n");
                liberarMemoria(&primeraParada, unidades);
                return 0;
                
            default: // Opción inválida
                printf("====OPCION NO DISPONIBLE====\n\n");
                opc = menu();
                break;
        }
    } while(opc);
    
    return 0;
}

// ============================================================================
// FUNCIÓN: menu
// ============================================================================
/*
    Muestra el menú principal y captura la opción del usuario.
    
    Retorna:
        int Opc = Opción seleccionada (1-4)
*/
int menu(void) {
    int opc;
    
    printf("========================================\n");
    printf("  SIMULADOR DE TRANSPORTE VECINAL\n");
    printf("========================================\n");
    printf("1. Agregar pasajero\n");
    printf("2. Ejecutar simulacion\n");
    printf("3. Ver reporte\n");
    printf("4. Salir\n");
    printf("========================================\n\n");
    
    printf("Que desea hacer? ");
    scanf("%d", &opc);
    getchar(); // Limpiar buffer
    
    printf("\n========================================\n\n");
    
    return opc;
}

// ============================================================================
// FUNCIÓN: continuar
// ============================================================================
/*
    Pregunta al usuario si desea repetir la operación actual o volver al menú.
    
    Parámetros:
        opc - Opción actual que se está ejecutando
        
    Retorna:
        int - La misma opción si responde 's', o muestra el menú si responde 'n'
*/
int continuar(int opc) {
    char resp;
    
    printf("\n========================================\n\n");
    printf("Continuar con esta opcion? (s/n): ");
    scanf(" %c", &resp);
    getchar(); // Limpiar buffer
    
    if(resp == 's' || resp == 'S') {
        printf("\n========================================\n\n");
        return opc;
    } else {
        printf("\n========================================\n\n");
        return menu();
    }
}

// ============================================================================
// FUNCIÓN: inicializarParadas
// ============================================================================
/*
    Crea la lista circular de 5 paradas conectadas.
    Estructura: Parada1 -> Parada2 -> Parada3 -> Parada4 -> Parada5 -> Parada1
    
    Parámetros:
        primera = Puntero doble a la primera parada (modificable)
*/
void inicializarParadas(Parada **primera) {
    Parada *anterior = NULL;
    int i;
    
    // Crear 5 paradas
    for(i = 1; i <= NUM_PARADAS; i++) {
        Parada *nueva = (Parada*)malloc(sizeof(Parada));
        
        // Validar asignación de memoria
        if(nueva == NULL) {
            printf("Error: No hay memoria disponible\n");
            return;
        }
        
        // Inicializar campos de la parada
        nueva->numero = i;
        nueva->colaPasajeros = NULL;
        nueva->totalPasajerosSubieron = 0;
        nueva->totalPasajerosEsperando = 0;
        nueva->tiempoTotalAbordaje = 0;
        nueva->sumaTiemposEspera = 0;
        nueva->tiempoPromedioEspera = 0.0;
        nueva->pasajerosManana = 0;
        nueva->pasajerosTarde = 0;
        nueva->pasajerosNoche = 0;
        nueva->siguiente = NULL;
        
        // Conectar con la lista
        if(*primera == NULL) {
            *primera = nueva; // Primera parada
        } else {
            anterior->siguiente = nueva; // Enlazar con la anterior
        }
        
        anterior = nueva; // Enlazar con la catual
    }
    
    // Cerrar el círculo: última parada apunta a la primera
    if(anterior != NULL) {
        anterior->siguiente = *primera;
    }
}

// ============================================================================
// FUNCIÓN: inicializarUnidades
// ============================================================================
/*
    Inicializa el arreglo de unidades de transporte.
    Pone todos los asientos en estado libre (ocupado = 0).
    
    Parámetros:
        unidades[] = Arreglo de unidades
*/
void inicializarUnidades(Unidad unidades[]) {
    int i, j;
    
    for(i = 0; i < NUM_UNIDADES; i++) {
        unidades[i].id = i + 1; // Identificador de la unidad (1 o 2)
        // Todos los datos en cero
        unidades[i].pasajerosTransportados = 0;
        unidades[i].ocupacionPromedio = 0.0;
        unidades[i].sumaOcupaciones = 0;
        unidades[i].paradasVisitadas = 0;
        unidades[i].tiempoTotal = 0;
        unidades[i].tiempoActual = 0;
        
        // Inicializar todos los asientos como libres
        for(j = 0; j < ASIENTOS_POR_UNIDAD; j++) {
            unidades[i].asientos[j].ocupado = 0;
            unidades[i].asientos[j].pasajero = NULL;
        }
    }
}

// ============================================================================
// FUNCIÓN: agregarPasajero
// ============================================================================
/*
    Solicita datos de un nuevo pasajero y lo agrega a la cola de su parada.
    Asigna automáticamente:
        - ID único
        - Tiempo de llegada = 0 (Desde elinicio de la simulación)
    
    Parámetros:
        primera = Puntero doble a la primera parada
*/
void agregarPasajero(Parada **primera) {
    Pasajero *nuevo;
    Parada *parada;
    
    // Validar que existan paradas
    if(*primera == NULL) {
        printf("Error: No hay paradas inicializadas\n");
        return;
    }
    
    // Asignar memoria para el nuevo pasajero
    nuevo = (Pasajero*)malloc(sizeof(Pasajero));

    // Validar asignación de memoria
    if(nuevo == NULL) {
        printf("Error: No hay memoria disponible\n");
        return;
    }
    
    // Asignar ID único
    nuevo->id_P = contadorID++;
    
    printf("=========AGREGAR PASAJERO=========\n\n");
    
    // Solicitar nombre
    printf("Nombre del pasajero: ");
    fgets(nuevo->nombre, 50, stdin); //Guarda el nombre
    nuevo->nombre[strcspn(nuevo->nombre, "\n")] = 0; // Quitar salto de línea
    
    // Solicitar parada actual (con validación)
    do {
        printf("Parada actual (1-%d): ", NUM_PARADAS);
        scanf("%d", &nuevo->paradaActual); //Guarda la parada actual
        getchar();
        
        //Valida si existe la parada
        if(nuevo->paradaActual < 1 || nuevo->paradaActual > NUM_PARADAS) {
            printf("Parada invalida. Intente de nuevo.\n");
        }
    } while(nuevo->paradaActual < 1 || nuevo->paradaActual > NUM_PARADAS);
    
    // Solicitar parada destino (con validación)
    do {
        printf("Parada destino (1-%d): ", NUM_PARADAS);
        scanf("%d", &nuevo->paradaDestino); //Guarda la parada destino
        getchar();
        
        //Valida si existe la parada y que sea distinta que la actual
        if(nuevo->paradaDestino < 1 || nuevo->paradaDestino > NUM_PARADAS) {
            printf("Parada invalida. Intente de nuevo.\n");
        } else if(nuevo->paradaDestino == nuevo->paradaActual) {
            printf("La parada destino no puede ser igual a la actual.\n");
        }
    } while(nuevo->paradaDestino < 1 || nuevo->paradaDestino > NUM_PARADAS ||
            nuevo->paradaDestino == nuevo->paradaActual);
    
    // Solicitar prioridad (con validación)
    do {
        printf("Prioridad:\n");
        printf("  1 = Normal\n");
        printf("  2 = Adulto Mayor\n");
        printf("  3 = Discapacidad\n");
        printf("Seleccione: ");
        scanf("%d", &nuevo->prioridad); //Guarda la prioridad
        getchar();
        
        //Valida si existe
        if(nuevo->prioridad < 1 || nuevo->prioridad > 3) {
            printf("Prioridad invalida. Intente de nuevo.\n");
        }
    } while(nuevo->prioridad < 1 || nuevo->prioridad > 3);
    
    // Solicitar horario de viaje (con validación)
    do {
        printf("Horario de viaje:\n");
        printf("  1 = Manana (6am-12pm)\n");
        printf("  2 = Tarde (12pm-6pm)\n");
        printf("  3 = Noche (6pm-12am)\n");
        printf("Seleccione: ");
        scanf("%d", &nuevo->horarioTipo); //Guarda el horario
        getchar();
        
        //Valida si existe
        if(nuevo->horarioTipo < 1 || nuevo->horarioTipo > 3) {
            printf("Horario invalido. Intente de nuevo.\n");
        }
    } while(nuevo->horarioTipo < 1 || nuevo->horarioTipo > 3);
    
    // Inicializar campos automáticos
    nuevo->tiempoLlegada = 0;     // Todos llegan al inicio (t=0)
    nuevo->tiempoEspera = 0;
    nuevo->siguiente = NULL;
    
    // Buscar la parada correspondiente
    parada = buscarParada(*primera, nuevo->paradaActual);
    
    if(parada != NULL) {
        // Insertar en la cola (respetando prioridad)
        insertarEnCola(parada, nuevo);
        parada->totalPasajerosEsperando++; // Suma 1 al total de pasajeros esperando
        
        printf("\nPasajero '%s' [ID: %d] agregado a la cola de Parada %d\n",
               nuevo->nombre, nuevo->id_P, nuevo->paradaActual);
    } else {
        printf("Error: Parada no encontrada\n");
        free(nuevo); // Libera memoria si no lo encontro
    }
    
    printf("\n========================================\n");
}

// ============================================================================
// FUNCIÓN: insertarEnCola
// ============================================================================
/*
    Inserta un pasajero en la cola de una parada respetando el orden de prioridad.
    Orden: Prioridad 3 (Discapacidad) -> Prioridad 2 (Adulto Mayor) -> Prioridad 1 (Normal)
    
    Parámetros:
        parada = Parada donde se insertará el pasajero
        nuevo = Pasajero a insertar
*/
void insertarEnCola(Parada *parada, Pasajero *nuevo) {
    // Caso 1: Cola vacía o nuevo pasajero tiene mayor prioridad que el primero
    if(parada->colaPasajeros == NULL ||
       parada->colaPasajeros->prioridad < nuevo->prioridad) {
        nuevo->siguiente = parada->colaPasajeros;
        parada->colaPasajeros = nuevo;
    }
    // Caso 2: Buscar posición correcta en la cola
    else {
        Pasajero *actual = parada->colaPasajeros;
        
        // Avanzar hasta encontrar la posición correcta
        while(actual->siguiente != NULL &&
              actual->siguiente->prioridad >= nuevo->prioridad) {
            actual = actual->siguiente;
        }
        
        // Insertar después de 'actual'
        nuevo->siguiente = actual->siguiente;
        actual->siguiente = nuevo;
    }
}

// ============================================================================
// FUNCIÓN: buscarParada
// ============================================================================
/*
    Busca una parada en la lista circular por su número.
    
    Parámetros:
        primera = Primera parada de la lista circular
        numero = Número de la parada a buscar (1-5)
        
    Retorna:
        Parada* = Puntero a la parada encontrada, o NULL si no existe
*/
Parada* buscarParada(Parada *primera, int numero) {
    Parada *actual;
    
    if(primera == NULL) {
        return NULL; // No existe
    }
    
    actual = primera; // Primera sera actual
    
    // Recorrer la lista circular
    do {
        if(actual->numero == numero) {
            return actual; // Retorna la actual
        }
        actual = actual->siguiente; // Avanza a la siguiente parada
    } while(actual != primera);
    
    return NULL; // No encontrada
}

// ============================================================================
// FUNCIÓN: simular
// ============================================================================
/*
    Ejecuta la simulación del sistema de transporte.
    - Unidad 1 sale en t=0
    - Unidad 2 sale en t=10 (cuando Unidad 1 llega a parada 3)
    - Ambas unidades recorren las paradas hasta llegar a parada 1
    
    Parámetros:
        primera = Primera parada de la ruta
        unidades[] = Arreglo de unidades de transporte
*/
void simular(Parada *primera, Unidad unidades[]) {
    // Punteros a cada unidad
    Parada *paradaUnidad1;
    Parada *paradaUnidad2;

    //Indican si cada unidad ya terminó su recorrido
    int unidad1Terminada = 0;
    int unidad2Terminada = 0;

    // Unidad 2 ya inició su recorrido (empiza t=10)
    int unidad2Inicio = 0;

    // Puntero para recorer las paradas
    Parada *p;
    
    printf("========================================\n");
    printf("       INICIANDO SIMULACION\n");
    printf("========================================\n\n");
    
    // Validar que existan paradas
    if(primera == NULL) {
        printf("Error: No hay paradas inicializadas\n");
        return;
    }
    
    // Reiniciar estadísticas de paradas
    p = primera;
    do {
        p->totalPasajerosSubieron = 0;
        p->tiempoTotalAbordaje = 0;
        p->sumaTiemposEspera = 0;
        p->tiempoPromedioEspera = 0.0;
        p->pasajerosManana = 0;
        p->pasajerosTarde = 0;
        p->pasajerosNoche = 0;
        p = p->siguiente; //Avanza a la  siguiente parada
    } while(p != primera);
    
    // Reiniciar unidades
    inicializarUnidades(unidades);
    
    // Configurar unidades
    paradaUnidad1 = primera;
    unidades[0].tiempoActual = 0;
    
    paradaUnidad2 = primera;
    unidades[1].tiempoActual = -1; // Indicamos que no ha iniciado
    
    printf("Unidad 1 sale de Parada 1 en t=0 min\n");
    printf("Unidad 2 saldra de Parada 1 en t=10 min\n\n");
    
    // Bucle principal de simulación
    while(!unidad1Terminada || !unidad2Terminada) {
        
        // Unidad 1
        if(!unidad1Terminada) {
            printf("--- [t=%d min] Procesando Unidad 1 ---\n", unidades[0].tiempoActual);
            
            // Avanza la unidad 1 (baja, sube, avanza)
            avanzarUnidad(&unidades[0], &paradaUnidad1);
            
            // Verificar si terminó (en parada 1 y vacía)
            if(paradaUnidad1->numero == 1 && !unidadTienePasajeros(&unidades[0])) {
                unidad1Terminada = 1;
                printf("\nUnidad 1 finalizo su recorrido en t=%d min\n\n",
                       unidades[0].tiempoActual);
            }
            
            // Activar Unidad 2 cuando Unidad 1 llega a t=10
            if(!unidad2Inicio && unidades[0].tiempoActual >= 10) {
                unidad2Inicio = 1;
                unidades[1].tiempoActual = 10;
                printf("Unidad 2 inicia su recorrido en t=10 min\n\n");
            }
        }
        
        // Unidad 2
        if(unidad2Inicio && !unidad2Terminada) {
            printf("--- [t=%d min] Procesando Unidad 2 ---\n", unidades[1].tiempoActual);

            // Avanza la unidad  (baja, sube, avanza)
            avanzarUnidad(&unidades[1], &paradaUnidad2);
            
            // Verificar si terminó (en parada 1 y vacía)
            if(paradaUnidad2->numero == 1 && !unidadTienePasajeros(&unidades[1])) {
                unidad2Terminada = 1;
                printf("\nUnidad 2 finalizo su recorrido en t=%d min\n\n",
                       unidades[1].tiempoActual);
            }
        }
        
        // Evitamos bucle infinito
        if(unidades[0].tiempoActual > 500 || unidades[1].tiempoActual > 500) {
            printf("Simulacion detenida por tiempo excesivo\n");
            break;
        }
    }
    
    // Marcar que la simulación se realizó
    simulacionRealizada = 1;
    
    printf("========================================\n");
    printf("       SIMULACION COMPLETADA\n");
    printf("========================================\n");
}

// ============================================================================
// FUNCIÓN: avanzarUnidad
// ============================================================================
/*
    Avanza una unidad a la siguiente parada.
    1. Baja pasajeros que llegaron a su destino
    2. Sube pasajeros de la cola (respetando prioridad y capacidad)
    3. Avanza a la siguiente parada
    4. Incrementa el tiempo
    
    Parámetros:
        unidad = Unidad a avanzar
        paradaActual = Puntero doble a la parada actual (se modifica)
*/
void avanzarUnidad(Unidad *unidad, Parada **paradaActual) {
    int ocupados;
    
    printf("Unidad %d llega a Parada %d\n", unidad->id, (*paradaActual)->numero);
    
    // 1. Bajar pasajeros que llegaron a su destino
    bajarPasajeros(unidad, (*paradaActual)->numero);
    
    // 2. Subir pasajeros de la parada
    subirPasajeros(unidad, *paradaActual);
    
    // 3. Registrar ocupación para estadísticas
    ocupados = contarAsientosOcupados(unidad);
    unidad->sumaOcupaciones += ocupados;
    unidad->paradasVisitadas++;
    
    // 4. Avanzar a la siguiente parada
    *paradaActual = (*paradaActual)->siguiente;
    
    // 5. Incrementar tiempo
    unidad->tiempoActual += TIEMPO_ENTRE_PARADAS;
    unidad->tiempoTotal = unidad->tiempoActual;
    
    printf("\n");
}

// ============================================================================
// FUNCIÓN: bajarPasajeros
// ============================================================================
/*
    Baja a los pasajeros que llegaron a su destino.
    Libera sus asientos y la memoria del pasajero.
    
    Parámetros:
        unidad = Unidad de transporte
        numeroParada = Número de la parada actual
*/
void bajarPasajeros(Unidad *unidad, int numeroParada) {
    int i;
    int pasajerosBajaron = 0;
    
    // Recorrer todos los asientos
    for(i = 0; i < ASIENTOS_POR_UNIDAD; i++) {
        // Si está ocupado y el pasajero llegó a su destino
        if(unidad->asientos[i].ocupado &&
           unidad->asientos[i].pasajero->paradaDestino == numeroParada) {
            
            printf("   Baja: %s [ID: %d] (llego a su destino)\n",
                   unidad->asientos[i].pasajero->nombre,
                   unidad->asientos[i].pasajero->id_P);
            
            // Liberar memoria y asiento
            free(unidad->asientos[i].pasajero);
            unidad->asientos[i].ocupado = 0;
            unidad->asientos[i].pasajero = NULL;
            pasajerosBajaron++;
        }
    }
    
    if(pasajerosBajaron > 0) {
        printf("   Bajaron %d pasajero(s)\n", pasajerosBajaron);
    }
}

// ============================================================================
// FUNCIÓN: subirPasajeros
// ============================================================================
/*
    Sube pasajeros de la cola de la parada a la unidad.
    Respeta el orden de prioridad y la capacidad disponible.
    Calcula tiempos de espera y registra estadísticas por horario.
    
    Parámetros:
        unidad = Unidad de transporte
        parada = Parada actual
*/
void subirPasajeros(Unidad *unidad, Parada *parada) {
    int asientosLibres;
    int pasajerosSubieron = 0;
    int tiempoAbordaje = 0;
    Pasajero *pasajero;
    int i;
    int destinoPaso;
    
    asientosLibres = ASIENTOS_POR_UNIDAD - contarAsientosOcupados(unidad);
    
    // Mientras haya pasajeros esperando y asientos libres
    while(parada->colaPasajeros != NULL && asientosLibres > 0) {
        // Sacar al primer pasajero de la cola
        pasajero = parada->colaPasajeros;
        parada->colaPasajeros = pasajero->siguiente;
        
        // Buscar un asiento libre
        for(i = 0; i < ASIENTOS_POR_UNIDAD; i++) {
            if(!unidad->asientos[i].ocupado) {
                // Asignar asiento
                unidad->asientos[i].ocupado = 1;
                unidad->asientos[i].pasajero = pasajero;
                
                // Calcular tiempo de espera
                pasajero->tiempoEspera = unidad->tiempoActual - pasajero->tiempoLlegada;
                parada->sumaTiemposEspera += pasajero->tiempoEspera;
                
                // Registrar estadísticas por horario
                switch(pasajero->horarioTipo) {
                    case 1: parada->pasajerosManana++; break;
                    case 2: parada->pasajerosTarde++; break;
                    case 3: parada->pasajerosNoche++; break;
                }
                
                pasajerosSubieron++;
                tiempoAbordaje += TIEMPO_ABORDAJE;
                asientosLibres--;
                
                // Verificar si el destino ya pasó
                destinoPaso = 0;
                if(pasajero->paradaDestino < parada->numero) {
                    destinoPaso = 1;
                }
                
                printf("   Sube: %s [ID: %d] (destino: Parada %d, prioridad: %d)%s\n",
                       pasajero->nombre,
                       pasajero->id_P,
                       pasajero->paradaDestino,
                       pasajero->prioridad,
                       destinoPaso ? " [Vuelta extra requerida]" : "");
                
                break; // Salir del bucle de búsqueda de asiento
            }
        }
    }
    
    // Actualizar estadísticas de la parada
    if(pasajerosSubieron > 0) {
        parada->totalPasajerosSubieron += pasajerosSubieron;
        parada->tiempoTotalAbordaje += tiempoAbordaje;
        parada->totalPasajerosEsperando -= pasajerosSubieron;

        //Sumar el tiempo total
        unidad->tiempoActual += tiempoAbordaje;
        unidad->pasajerosTransportados += pasajerosSubieron;
        
        printf("   Tiempo de abordaje: %d min\n", tiempoAbordaje);
        printf("   Ocupacion actual: %d/%d asientos\n",
               contarAsientosOcupados(unidad), ASIENTOS_POR_UNIDAD);
    } else {
        printf("   No hay pasajeros esperando o unidad llena\n");
    }
    
    // Mostrar pasajeros que quedaron esperando
    if(parada->colaPasajeros != NULL) {
        int enEspera = 0;
        Pasajero *temp = parada->colaPasajeros;
        
        while(temp != NULL) {
            enEspera++;
            temp = temp->siguiente;
        }
        printf("   %d pasajero(s) quedaron esperando (unidad llena)\n", enEspera);
    }
}

// ============================================================================
// FUNCIÓN: contarAsientosOcupados
// ============================================================================
/*
    Cuenta cuántos asientos están ocupados en una unidad.
    
    Parámetros:
        unidad = Unidad de transporte
        
    Retorna:
        int = Número de asientos ocupados (0-10)
*/
int contarAsientosOcupados(Unidad *unidad) {
    int cont = 0;
    int i;
    
    for(i = 0; i < ASIENTOS_POR_UNIDAD; i++) {
        if(unidad->asientos[i].ocupado) {
            cont++;
        }
    }
    
    return cont;
}

// ============================================================================
// FUNCIÓN: unidadTienePasajeros
// ============================================================================
/*
    Verifica si una unidad tiene pasajeros a bordo.
    
    Parámetros:
        unidad = Unidad de transporte
        
    Retorna:
        int = 1 si tiene pasajeros, 0 si está vacía
*/
int unidadTienePasajeros(Unidad *unidad) {
    int i;
    
    for(i = 0; i < ASIENTOS_POR_UNIDAD; i++) {
        if(unidad->asientos[i].ocupado) {
            return 1; // Hay al menos un pasajero
        }
    }
    
    return 0; // Unidad vacía
}

// ============================================================================
// FUNCIÓN: reporte
// ============================================================================
/*
    Genera y muestra el reporte completo de la simulación:
    - Estadísticas por parada (tiempos, ocupación)
    - Estadísticas por unidad (pasajeros, ocupación promedio)
    - Análisis por horario
    - Alertas (paradas saturadas, pasajeros sin abordar)
    
    Parámetros:
        primera = Primera parada de la ruta
        unidades[] = Arreglo de unidades
*/
void reporte(Parada *primera, Unidad unidades[]) {
    Parada *p;
    int paradaMasSaturada = 1;
    int maxTiempoAbordaje = 0;
    int totalManana = 0;
    int totalTarde = 0;
    int totalNoche = 0;
    int totalEsperando = 0;
    int i;
    
    printf("========================================\n");
    printf("         REPORTE DE SIMULACION\n");
    printf("========================================\n\n");
    
    // Validar que se haya ejecutado la simulación
    if(!simulacionRealizada) {
        printf("No hay vueltas registradas.\n");
        printf("Ejecute primero una simulacion (opcion 2).\n\n");
        printf("========================================\n");
        return;
    }
    
    //ESTADÍSTICAS POR PARADA
    printf("--- ESTADISTICAS POR PARADA ---\n\n");
    
    p = primera;
    do {
        printf("Parada %d:\n", p->numero);
        printf("   Pasajeros que subieron: %d\n", p->totalPasajerosSubieron);
        printf("   Pasajeros aun esperando: %d\n", p->totalPasajerosEsperando);
        printf("   Tiempo total de abordaje: %d min\n", p->tiempoTotalAbordaje);
        
        // Calcular y mostrar tiempo promedio de espera
        if(p->totalPasajerosSubieron > 0) {
            p->tiempoPromedioEspera = (float)p->sumaTiemposEspera / p->totalPasajerosSubieron;
            printf("   Tiempo promedio de espera: %.1f min\n", p->tiempoPromedioEspera);
        } else {
            printf("   Tiempo promedio de espera: N/A (nadie subio)\n");
        }
        
        // Identificar parada más saturada
        if(p->tiempoTotalAbordaje > maxTiempoAbordaje) {
            maxTiempoAbordaje = p->tiempoTotalAbordaje;
            paradaMasSaturada = p->numero;
        }
        
        printf("\n");
        p = p->siguiente;
    } while(p != primera);
    
    //ESTADÍSTICAS POR UNIDAD
    printf("--- ESTADISTICAS POR UNIDAD ---\n\n");
    
    for(i = 0; i < NUM_UNIDADES; i++) {
        printf("Unidad %d:\n", unidades[i].id);
        printf("   Pasajeros transportados: %d\n", unidades[i].pasajerosTransportados);
        printf("   Tiempo total de recorrido: %d min\n", unidades[i].tiempoTotal);
        
        // Calcular y mostrar tasa de ocupación promedio
        if(unidades[i].paradasVisitadas > 0) {
            unidades[i].ocupacionPromedio = ((float)unidades[i].sumaOcupaciones /
                                              unidades[i].paradasVisitadas /
                                              ASIENTOS_POR_UNIDAD) * 100;
            printf("   Tasa de ocupacion promedio: %.1f%%\n", unidades[i].ocupacionPromedio);
        } else {
            printf("   Tasa de ocupacion promedio: N/A\n");
        }
        
        printf("\n");
    }
    
    // ANÁLISIS POR HORARIO
    printf("--- ANALISIS POR HORARIO ---\n\n");
    
    p = primera;
    do {
        totalManana += p->pasajerosManana;
        totalTarde += p->pasajerosTarde;
        totalNoche += p->pasajerosNoche;
        totalEsperando += p->totalPasajerosEsperando;
        p = p->siguiente;
    } while(p != primera);
    
    printf("Pasajeros en horario de Manana (6am-12pm): %d\n", totalManana);
    printf("Pasajeros en horario de Tarde (12pm-6pm): %d\n", totalTarde);
    printf("Pasajeros en horario de Noche (6pm-12am): %d\n", totalNoche);
    
    // Determinar horario con mayor demanda
    if(totalManana > totalTarde && totalManana > totalNoche) {
        printf("Mayor demanda: Manana (hora pico)\n");
    } else if(totalTarde > totalManana && totalTarde > totalNoche) {
        printf("Mayor demanda: Tarde\n");
    } else if(totalNoche > totalManana && totalNoche > totalTarde) {
        printf("Mayor demanda: Noche\n");
    } else {
        printf("Mayor demanda: Distribuida uniformemente\n");
    }
    
    printf("\n");
    
    // ALERTAS
    printf("--- ALERTAS ---\n\n");
    
    if(maxTiempoAbordaje > 0) {
        printf("Parada mas saturada: Parada %d (%d min de abordaje)\n",
               paradaMasSaturada, maxTiempoAbordaje);
    }
    
    if(totalEsperando > 0) {
        printf("Hay %d pasajero(s) que no pudieron abordar\n", totalEsperando);
        printf("Recomendacion: Agregar mas unidades en hora pico\n");
    } else {
        printf("Todos los pasajeros abordaron exitosamente\n");
    }
    
    printf("\n========================================\n");
}

// ============================================================================
// FUNCIÓN: liberarMemoria
// ============================================================================
/*
    Libera toda la memoria dinámica asignada durante la ejecución.
    1. Libera pasajeros en las colas de las paradas
    2. Libera las paradas de la lista circular
    3. Libera pasajeros que aún estén en las unidades
    
    Parámetros:
        primera = Puntero doble a la primera parada
        unidades[] = Arreglo de unidades
*/
void liberarMemoria(Parada **primera, Unidad unidades[]) {
    Parada *actual;
    Parada *siguiente;
    Pasajero *p;
    Pasajero *tempP;
    int i, j;
    
    if(*primera == NULL) {
        return; // No hay nada que liberar
    }
    
    // Liberar paradas y sus colas
    actual = *primera;
    
    do {
        // Liberar cola de pasajeros de esta parada
        p = actual->colaPasajeros;
        while(p != NULL) {
            tempP = p;
            p = p->siguiente;
            free(tempP);
        }
        
        siguiente = actual->siguiente;
        free(actual);
        actual = siguiente;
    } while(actual != *primera);
    
    *primera = NULL;
    
    // Liberar pasajeros en unidades
    for(i = 0; i < NUM_UNIDADES; i++) {
        for(j = 0; j < ASIENTOS_POR_UNIDAD; j++) {
            if(unidades[i].asientos[j].ocupado &&
               unidades[i].asientos[j].pasajero != NULL) {
                free(unidades[i].asientos[j].pasajero);
                unidades[i].asientos[j].pasajero = NULL;
            }
        }
    }
}
