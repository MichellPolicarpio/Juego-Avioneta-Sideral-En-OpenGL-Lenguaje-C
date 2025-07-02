// Graficación por Computadora - Proyecto Final: Desarrollar un videojuego básico en 2D o 3D en OpenGL
// ALumno: Policarpio Moran Michell Alexis - zS21002379 - Ingenieria en Informatica - FIEE - UV

// PARA COMPILARLO EN TERMINAL (MacOS): 
// gcc -Wall -framework OpenGL -framework GLUT  Avioneta_Sideral.c -o juego
// ./juego

// PARA COMPILARLO EN TERMINAL (Linux - Fedora):
// gcc -Wall Avioneta_Sideral.c -o juego -lGL -lGLU -lglut -lm
// ./juego

// PARA COMPILARLO EN TERMINAL (MacOS - XCode):
// gcc -Wall -framework OpenGL -framework GLUT  main.c -o juego
// ./juego


/*======================================*
 *  SECCIÓN: BIBLIOTECAS E INCLUSIONES  *
 *======================================*/
// Silenciar warnings de deprecación de OpenGL en macOS
#ifndef GL_SILENCE_DEPRECATION
#define GL_SILENCE_DEPRECATION
#endif

// Includes comunes de OpenGL
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <string.h> 
#include <stdio.h>
#include <math.h> 
#include <stdbool.h>
<<<<<<< HEAD

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
=======
>>>>>>> 790c6ee80f4f0a927a6466c98ff7301d83e9dfe9
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"             // Libreria usada para implementar imagen de fondo para el videojuego

/*==================================================*
 *        CONSTANTES Y DEFINICIONES                 *
 *==================================================*/

#define MAX_PARTICULAS_ESTELA 50      // Máximo de partículas para efectos
#define MAX_CAJAS_MUNICION 5          // Máximo de cajas de munición simultáneas
#define AMMO_REQUIRED 10              // Munición necesaria para disparar
#define MAX_SAVES 5                   // Máximo de estados guardados

#define WINDOW_WIDTH 800             //Altura de la ventana principal
#define WINDOW_HEIGHT 600            //Anchura de la ventana principal
#define WINDOW_TITLE "MI VIDEOJUEGO" //Nombre de la ventana principal
#define FPS 60
#define POINTS_PER_SECOND 100  
#define POINTS_FOR_BOOST 50   

/*==================================================*
 *              ENUMERACIONES                        *
 *==================================================*/
// Sistema de estados del juego
typedef enum {
    STATE_MENU,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_GAME_OVER,
    STATE_DIALOG
} GameState;

// Sistema de menú
typedef enum {
    MENU_STATE_MAIN,
    MENU_STATE_OPTIONS
} MenuState;

typedef enum {
    DIFFICULTY_EASY,
    DIFFICULTY_MEDIUM,
    DIFFICULTY_HARD
} GameDifficulty;

typedef enum {
    MENU_NEW_GAME,
    MENU_LOAD_GAME,
    MENU_OPTIONS,
    MENU_EXIT,
    MENU_OPTION_COUNT
} MenuOption;

// Sistema de personaje y misiones
typedef enum {
    CHAR_WALKING,
    CHAR_ENTERING,
    CHAR_HIDDEN
} CharacterState;

typedef enum {
    FASE_INFORMACION,
    FASE_INFILTRACION,
    FASE_ESCAPE
} MissionPhase;

typedef enum {
    MISION_DESPEGUE,
    MISION_COLECTAR_MUNICION,
    MISION_DESTRUIR_OBSTACULOS,
    MISION_ALZANZAR_DISTANCIA
} MissionType;

/*==================================================*
 *              ESTRUCTURAS                          *
 *==================================================*/
// Sistema de partículas (Array estatico para la estela del avion)
typedef struct {
    float x, y;
    float alpha;
    float size;
} TrailParticle;

// Sistema de la avioneta
typedef struct {
    float y_pos;
    float y_velocity;
    float normal_speed;
    float boost_speed;
    float current_speed;
    float rotation;
    struct {
        float angle;
        float speed;
    } propeller;
    float max_y;
    float min_y;
    TrailParticle estela[MAX_PARTICULAS_ESTELA];  // Array de partículas
    int estela_contador;
} Avioneta;

// Sistema de obstáculos (Lista enlazada simple)
typedef struct Obstacle {
    float x, y;
    float width, height;
    bool activo;
    struct Obstacle* sig;  // Puntero al sig obstaculo
} Obstacle;

typedef struct {
    Obstacle* head;         // Cabeza de la lista enlazada
    float spawn_timer;
    float spawn_interval;
    float speed;
    int activo_contador;
} ObstacleSystem;

// Sistema de municiones (Array estático y lista enlazada)
typedef struct {
    float x, y;
    bool activo;
} CajaMunicion;

typedef struct Bala {
    float x, y;
    bool activo;
    struct Bala* sig;  // Lista enlazada de balas
} Bala;

// Sistema de diálogos (Árbol binario)
typedef struct DialogNode {
    char* text;
    char* speaker;
    struct DialogNode* izq;       // Hijo izquierdo del árbol
    char* izq_eleccion;
    struct DialogNode* der;      // Hijo derecho del árbol
    char* der_eleccion;
    void (*action)(void);
    MissionPhase fase;
} DialogNode;

// Sistema de estado de misión
typedef struct {
    float target_x, target_y;
    float distance_to_target;
    bool defenses_alerted;
    int tiempo_restante;
    bool escaping;
    MissionPhase fase_actual;
} MissionStatus;

// Sistema de misiones (Cola)
typedef struct {
    MissionType type;
    int target_amount;
    int current_amount;
    bool completed;
    char* description;
} Mission;

typedef struct MissionNode {
    Mission mision;
    struct MissionNode* sig;   // Puntero para la cola
} MissionNode;

typedef struct {
    MissionNode* front;         // Frente de la cola
    MissionNode* rear;          // Final de la cola
    int contador;
} MissionQueue;

// Sistema de menú
typedef struct {
    MenuOption opcion_seleccionada;
    MenuState estado_actual;
    GameDifficulty dificultad;
    int dificultad_seleccionada;
    bool is_activo;
    float title_pos_y;
    float first_option_y;
    float option_spacing;
} Menu;

// Sistema de guardado (Pila)
typedef struct {
    int current_score;
    float avioneta_position;
    float avioneta_velocity;
    float avioneta_speed;
    bool game_activo;
    int obstaculos_activos;
    float *obstacle_positions;
} GameState_Save;

typedef struct {
    GameState_Save saves[MAX_SAVES];  // Array para la pila
    int top;                          // Tope de la pila
} SaveStack;

// Sistema de personaje
typedef struct {
    float x;
    float y;
    float scale;
    float walk_cycle;
    bool is_walking;
    bool has_entered;
    CharacterState state;
    float target_x;
    float enter_timer;
} Character;

/*==================================================*
 *          VARIABLES GLOBALES                       *
 *==================================================*/
GameState estado_actual;
Avioneta avioneta;
ObstacleSystem obstacle_system;
Menu game_menu;
const char* menu_options[4];
const char* dificultad_options[3];
Character pilot;
DialogNode* mision_dialog;
DialogNode* dialogo_actual;
MissionStatus mision_status;
MissionQueue mision_queue;
SaveStack save_stack;
bool has_saved_game;
CajaMunicion caja_municion[MAX_CAJAS_MUNICION];
Bala* lista_balas;
bool puede_disparar;
int municion_colectada;
int obstacles_destroyed; 

/*==================================================*
 *  SECCIÓN: DECLARACIONES DE VARIABLES DE GLOBALES *
 *==================================================*/
// Variables de estado del juego
int window_id;
GameState estado_actual = STATE_MENU;
bool running = true;
float delta_time = 0.0f;
int ultima_vez = 0;
bool has_saved_game = false;

// Variables de puntuación y progreso
int score = 0;
int high_score = 0;
float distancia_recorrida = 0.0f;
bool puede_disparar = false;
int municion_colectada = 0;
int obstacles_destroyed = 0;

// Variables de ventanas
int window_main;                // Ventana principal
int window_dialog;              // Ventana de diálogos
int window_mission_observer;    // Ventana de misionesss

// Constantes de dimensiones del personaje
static const float CABEZA_TAMANIO = 1.0f;
static const float CUERPO_ALTURA = 2.5f;
static const float CUERPO_ANCHO = 1.2f;
static const float BRAZO_LONGITUD = 1.0f;
static const float PIERNA_LONGITUD = 1.2f;

// Dimensiones de la avioneta
static float BODY_LENGTH = 8.0f;
static float BODY_WIDTH = 2.0f;
static float BODY_HEIGHT = 2.0f;
static float WING_LENGTH = 10.0f;
static float WING_WIDTH = 0.3f;
static float WING_HEIGHT = 3.0f;
static float WING_ANGLE = 15.0f;
static float TAIL_LENGTH = 2.5f;
static float TAIL_WIDTH = 0.3f;
static float TAIL_HEIGHT = 2.0f;
static float HELICE_LENGTH = 1.2f;
static float HELICE_WIDTH = 0.2f;
static float HELICE_HEIGHT = 4.0f;
static float HELICE_HUB_RADIUS = 0.4f;

// Variables de texturas y gráficos
GLuint backgroundTexture; // Textura de fondo

// Variables de sistemas de juego
Avioneta avioneta;
ObstacleSystem obstacle_system;
Menu game_menu;
MissionQueue mision_queue;
Character pilot;
SaveStack save_stack;

// Variables de diálogos y misiones
DialogNode* mision_dialog;
DialogNode* dialogo_actual;
MissionStatus mision_status;

// Variables de munición y balas
CajaMunicion caja_municion[MAX_CAJAS_MUNICION];
Bala* lista_balas = NULL;

/*==================================================*
 *          DECLARACIONES DE FUNCIONES               *
 *==================================================*/
// Funciones principales
void init(void);
void display(void);
void update(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void special_keys(int key, int x, int y);
void cleanup(void);
void timer(int value);
void render_game(void);
void keyboard_up(unsigned char key, int x, int y);
void render_game_over(void);
void display_dialog_window(void);
void reshape_dialog(int w, int h);
void regresar_menu(int value);

// Funciones de obstáculos
void inicializar_obstaculos(void);
void actualizar_obstaculos(float delta_time);
void renderizar_obstaculos(void);
bool verificar_colision(float avioneta_x, float avioneta_y, float avioneta_radius);
void generar_obstaculo(void);
void limpiar_obstaculos_inactivos(void);
void free_todos_obstaculos(void);

// Funciones del menú
void inicializar_menu(void);
void actualizar_menu(void);
void renderizar_menu(void);
void manejar_entrada_menu(unsigned char key);
void menu_handle_special_keys(int key);
void aplicar_configuracion_dificultad(void);

// Funciones de la avioneta
void inicializar_avioneta(void);
void actualizar_avioneta(float delta_time);
void renderizar_avioneta(void);
void handle_avioneta_input(unsigned char key);
void handle_avioneta_key_up(unsigned char key);
void actualizar_helice(void);
void inicializar_estela(void);
void actualizar_estela(float delta_time);
void renderizar_estela(void);

// Funciones de la pila de guardado
void init_save_stack(void);
bool push_save(GameState_Save save);
bool pop_save(GameState_Save* save);
bool is_stack_empty(void);
bool is_stack_full(void);
void guardar_juego_actual(void);
void cargar_juego_guardado(void);

// Funciones de la cola de misiones
void inicializar_mision_queue(void);
void enqueue_mision(Mission mision);
bool dequeue_mision(Mission* mision);
void renderizar_queue_misiones(void);
bool is_queue_empty(void);

// Funciones de diálogos
void inicializar_dialogos(void);
void renderizar_dialogo(DialogNode* node);
void handle_dialog_input(unsigned char key);
static void liberar_nodo_dialogo(DialogNode* nodo);
void cleanup_dialogs(void);
void mostrar_game_over(void);
void actualizar_estado_mision(void);
void renderizar_info_mision(void);
void activar_fase_mision(MissionPhase fase);
void iniciar_mision(void);

// Funciones de municiones
void inicializar_sistema_municiones(void);
void generar_caja_municion(void);
void actualizar_caja_municion(float delta_time);
void renderizar_caja_municion(void);
void verificar_recoleccion_municion(float avioneta_x, float avioneta_y, float radius);
void disparar_bala(float avioneta_x, float avioneta_y);
void actualizar_balas(float delta_time);
void renderizar_balas(void);
void limpiar_sistema_municiones(void);

// Funciones protottipo del personaje
void inicializar_personaje(void);
void renderizar_personaje(void);
void actualizar_personaje(float delta_time);

/* ------------- FUNCIONES DEL PROGRAMA ----------- */

// Función de inicialización
void init(void) {
    inicializar_dialogos();  
    inicializar_obstaculos();
    init_save_stack();
    inicializar_sistema_municiones();
    inicializar_mision_queue();
    inicializar_personaje(); 

    distancia_recorrida = 0.0f;

    // Cargar textura
    int width, height, channels;
    unsigned char* image = stbi_load("Background1.jpeg", &width, &height, &channels, 0);

    if (image) {
        glGenTextures(1, &backgroundTexture);
        glBindTexture(GL_TEXTURE_2D, backgroundTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        printf("Textura cargada exitosamente: %dx%d con %d canales\n", width, height, channels);
        stbi_image_free(image);
    } else {
        printf("Error cargando la textura: %s\n", stbi_failure_reason());
    }

    // Configurar color de fondo
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    
    // Configurar proyección
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)WINDOW_WIDTH/WINDOW_HEIGHT, 0.1f, 100.0f);
    
    // Configurar vista
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Habilitar características
    glEnable(GL_DEPTH_TEST);
    
    // Inicializar tiempo
    ultima_vez = glutGet(GLUT_ELAPSED_TIME);
    
    // Inicializar menú y jugador
    inicializar_menu();
    inicializar_avioneta();
}

// Aqui se ctualiza la lógica del juego en cada frame y se gestiona el tiempo delta y actualiza los sistemas según el estado actual
void update(void) {
    int tiempo_actual = glutGet(GLUT_ELAPSED_TIME);
    delta_time = (tiempo_actual - ultima_vez) / 1000.0f;
    ultima_vez = tiempo_actual;
    

    switch(estado_actual) {
        case STATE_MENU:
            actualizar_menu();
            actualizar_personaje(delta_time); 
            break;
            
        case STATE_PLAYING:
            actualizar_avioneta(delta_time);
            actualizar_obstaculos(delta_time);
            actualizar_caja_municion(delta_time);
            actualizar_balas(delta_time);

            // Spawn aleatorio de cajas de munición
            if (rand() % 100 == 0) {
                generar_caja_municion();
            }

            // Actualizar estado de misiones
            MissionNode* mision_actual = mision_queue.front;
            if (mision_actual != NULL) {
                switch (mision_actual->mision.type) {
                    case MISION_DESPEGUE:
                        mision_actual->mision.current_amount = 1;
                        if (!mision_actual->mision.completed) {
                            mision_actual->mision.completed = true;
                            printf("¡Misión de despegue completada!\n");
                        }
                        break;

                    case MISION_COLECTAR_MUNICION:
                        mision_actual->mision.current_amount = municion_colectada;
                        if (municion_colectada >= mision_actual->mision.target_amount && 
                            !mision_actual->mision.completed) {
                            mision_actual->mision.completed = true;
                            printf("¡Misión de recolección completada!\n");
                        }
                        break;

                    case MISION_DESTRUIR_OBSTACULOS:
                        mision_actual->mision.current_amount = obstacles_destroyed;
                        if (obstacles_destroyed >= mision_actual->mision.target_amount && 
                            !mision_actual->mision.completed) {
                            mision_actual->mision.completed = true;
                            printf("¡Misión de destrucción completada!\n");
                        }
                        break;

                    case MISION_ALZANZAR_DISTANCIA:
                        // Actualizar distancia basado en la velocidad del jugador
                        distancia_recorrida += avioneta.current_speed * delta_time;
                        mision_actual->mision.current_amount = (int)distancia_recorrida;
                        
                        if (distancia_recorrida >= mision_actual->mision.target_amount && 
                            !mision_actual->mision.completed) {
                            mision_actual->mision.completed = true;
                            printf("¡Misión de distancia completada! Distancia recorrida: %.2f\n", 
                                distancia_recorrida);
                        }
                        break;

                    default:
                        printf("Tipo de misión no reconocido\n");
                        break;
                }
            }

            // Verificar colección de munición
            verificar_recoleccion_municion(0.0f, avioneta.y_pos, 2.0f);
            
            score += (int)(delta_time * POINTS_PER_SECOND);
            if (avioneta.current_speed > avioneta.normal_speed) {
                score += (int)(delta_time * POINTS_FOR_BOOST);
            }
            
            if (verificar_colision(0.0f, avioneta.y_pos, 2.0f)) {
                printf("¡Colisión detectada! Puntuación final: %d\n", score);
                if (score > high_score) {
                    high_score = score;
                }
                estado_actual = STATE_GAME_OVER;
                dialogo_actual = NULL;
                has_saved_game = false;
                //free_todos_obstaculos(); // liberar memoria de los obstáculos cada que se pierda.
                init_save_stack();
            }
            break;
            
        case STATE_PAUSED:
            break;

        case STATE_GAME_OVER:
            break;
        
        case STATE_DIALOG:
            actualizar_estado_mision();
            break;
    }
}

void regresar_menu(int value) {
    estado_actual = STATE_MENU;
    score = 0;
    inicializar_avioneta();
    inicializar_obstaculos();
    glutPostRedisplay();
}

// Función para renderizar el juego
void render_game(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    // Renderizar fondo con textura
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, backgroundTexture);
    
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -105.0f);  //Profundidad de 90° grados
    
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-55.0f, -40.0f, 10.0f);    // Esquina inferior izquierda
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 55.0f, -40.0f, 10.0f);    // Esquina inferior derecha
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 55.0f,  40.0f, 10.0f);    // Esquina superior izquierda
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-55.0f,  40.0f, 10.0f);    // Esquina superior derecha
    glEnd();
    
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    
    // Vista del juego
    glLoadIdentity();
    gluLookAt(0.0f, 0.0f, 70.0f,
              25.0f, 0.0f, 0.0f,
              0.0f, 1.0f, 0.0f);

    renderizar_obstaculos();
    renderizar_caja_municion();  
    renderizar_personaje(); 
    renderizar_balas();     
    renderizar_avioneta();

    // Configurar vista para HUD
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // se redenriza la puntuación en la parte donde se renderiza el score
    glColor3f(1.0f, 1.0f, 1.0f);
    char score_text[32];
    sprintf(score_text, "Score: %d  High Score: %d", score, high_score);
    glRasterPos2f(-0.9f, 0.85f);  
    
    const char* text = score_text;
    while (*text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++);
    }

    // Restaurar matrices
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void render_game_over(void) {
    // Primero renderizar el juego en el fondo
    render_game();
    
    // Ahora superponer la pantalla de Game Over
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Fondo semi-transparente negro
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.7f);  // El último valor (0.7f) controla la transparencia
    glBegin(GL_QUADS);
        glVertex2f(-1.0f, -1.0f);
        glVertex2f( 1.0f, -1.0f);
        glVertex2f( 1.0f,  1.0f);
        glVertex2f(-1.0f,  1.0f);
    glEnd();
    
    // Título "GAME OVER" grande
    glColor3f(1.0f, 0.0f, 0.0f);  // Rojo
    const char* game_over_text = "GAME OVER";
    glRasterPos2f(-0.4f, 0.6f);
    while (*game_over_text) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *game_over_text++);
    }
    
    // Estadísticas
    glColor3f(1.0f, 1.0f, 1.0f);  // Blanco
    char stats_text[128];
    
    // Puntuación final
    sprintf(stats_text, "Final Score: %d", score);
    glRasterPos2f(-0.3f, 0.3f);
    const char* text = stats_text;
    while (*text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++);
    }
    
    // High Score
    sprintf(stats_text, "High Score: %d", high_score);
    glRasterPos2f(-0.3f, 0.2f);
    text = stats_text;
    while (*text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++);
    }
    
    // Instrucciones
    glColor3f(0.7f, 0.7f, 1.0f);  // Azul claro
    const char* restart_text = "Press ENTER to Play Again";
    glRasterPos2f(-0.3f, -0.2f);
    text = restart_text;
    while (*text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++);
    }
    
    const char* menu_text = "Press ESC for Main Menu";
    glRasterPos2f(-0.3f, -0.3f);
    text = menu_text;
    while (*text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++);
    }
    
    glDisable(GL_BLEND);
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

// Función de renderizado
void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    // Renderizar según el estado
    switch(estado_actual) {
        case STATE_MENU:
            renderizar_menu();
            break;
            
        case STATE_PLAYING:
            render_game();
            break;
            
        case STATE_PAUSED:
            // Renderizar el juego pausado
            render_game();
            
            // Superponer texto de pausa
            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();
            gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            
            glColor3f(1.0f, 1.0f, 1.0f);
            const char* pause_text = "PAUSED";
            float text_length = (float)glutBitmapLength(GLUT_BITMAP_HELVETICA_18, 
                                                      (const unsigned char*)pause_text);
            float scale = 1000.0f;
            float x = -text_length / scale;
            glRasterPos2f(x, 0.0f);
            while (*pause_text) {
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *pause_text++);
            }
            
            glMatrixMode(GL_PROJECTION);
            glPopMatrix();
            glMatrixMode(GL_MODELVIEW);
            break;
        case STATE_GAME_OVER:
            render_game_over();
            break;

        case STATE_DIALOG:
            render_game();  // Mantener el juego visible
            break;
    }
    
    glutSwapBuffers();
}

// Función de reshape
void reshape(int w, int h) {
    if (h == 0) h = 1;
    float aspect = (float)w / h;
    
    glViewport(0, 0, w, h);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, aspect, 0.1f, 100.0f);
    
    glMatrixMode(GL_MODELVIEW);
}

// Función de teclado normal
void keyboard(unsigned char key, int x, int y) {
    switch(estado_actual) {
        case STATE_MENU:
            manejar_entrada_menu(key);
            break;
            
        case STATE_PLAYING:
            if(key == 27 || key == 'p' || key == 'P') { // ESC o P
                if(key == 27) { // Si es ESC
                    guardar_juego_actual();  // Guardar estado actual
                    estado_actual = STATE_MENU;
                } else { // Si es P
                    estado_actual = STATE_PAUSED;
                }
            } else if(key == 'b' || key == 'B') {  // Tecla de disparo
                if(puede_disparar) {
                    disparar_bala(0.0f, avioneta.y_pos);
                }
            } else {
                handle_avioneta_input(key);
            }
            break;
            
        case STATE_PAUSED:
            if (key == 27 || key == 'p' || key == 'P') { // ESC o P para despausar
                estado_actual = STATE_PLAYING;
            }
            break;

        case STATE_GAME_OVER:
            if (key == 13) {  // ENTER
                estado_actual = STATE_MENU;
                score = 0;
                inicializar_avioneta();
                inicializar_obstaculos();
                inicializar_sistema_municiones();  // Reiniciar sistema de munición
            }
            break;

        case STATE_DIALOG:
            handle_dialog_input(key);
            break;
    }
    
    glutPostRedisplay();
}

// Función de teclas especiales
void special_keys(int key, int x, int y) {
    switch(estado_actual) {
        case STATE_MENU:
            menu_handle_special_keys(key);
            break;
            
        case STATE_PLAYING:
            switch(key) {
                case GLUT_KEY_UP:
                    handle_avioneta_input('w');
                    break;
                case GLUT_KEY_DOWN:
                    handle_avioneta_input('s');
                    break;
            }
            break;
            
        case STATE_PAUSED:
            break;

        case STATE_GAME_OVER:
            break;
        
        case STATE_DIALOG:
            break;  
    }
    
    glutPostRedisplay();
}

// Esta función mantiene constante la tasa de actualización del juego
void timer(int value) {
    update();
    glutSetWindow(window_main);
    glutPostRedisplay();
    glutSetWindow(window_dialog);
    glutPostRedisplay();
    glutSetWindow(window_mission_observer);  
    glutPostRedisplay();           
    glutTimerFunc(1000/FPS, timer, 0);
}

// Función de limpieza
void cleanup(void) {
    printf("Limpiando recursos...\n");
    free_todos_obstaculos();    // Liberar obstáculos
    limpiar_sistema_municiones();   // Liberar municiones
    cleanup_dialogs();       // Liberar diálogos
}

//Esta función maneja cuando se sueltan las teclas durante el juego
void keyboard_up(unsigned char key, int x, int y) {
    if(estado_actual == STATE_PLAYING) {
        handle_avioneta_key_up(key);
    }
    glutPostRedisplay();
}

// Función para la ventana de diálogos
void display_dialog_window(void) {

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glLoadIdentity();
   
   if (estado_actual == STATE_GAME_OVER) {
       mostrar_game_over();
   }
   else if (dialogo_actual) {
       renderizar_dialogo(dialogo_actual);

       // Renderizar el personaje en la ventana de diálogo
       glPushMatrix();
       // Posicionar en la esquina superior derecha
       glTranslatef(0.7f, 0.5f, 0.0f);
       glScalef(0.3f, 0.3f, 0.3f);  // Escalar más pequeño para la ventana de diálogo
       renderizar_personaje();  // Renderizar el personaje
       glPopMatrix();
   }
   else if (estado_actual == STATE_MENU) {
       // Mostrar información inicial en la terminal
       glMatrixMode(GL_PROJECTION);
       glPushMatrix();
       glLoadIdentity();
       gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
       
       glMatrixMode(GL_MODELVIEW);
       glLoadIdentity();

       // Fondo negro
       glColor4f(0.05f, 0.05f, 0.05f, 1.0f);
       glBegin(GL_QUADS);
           glVertex2f(-1.0f, -1.0f);
           glVertex2f( 1.0f, -1.0f);
           glVertex2f( 1.0f, 1.0f);
           glVertex2f(-1.0f, 1.0f);
       glEnd();

       // Marco principal
       glColor3f(0.2f, 0.3f, 0.2f);
       glLineWidth(2.0f);
       glBegin(GL_LINE_LOOP);
           glVertex2f(-0.95f, 0.95f);
           glVertex2f( 0.95f, 0.95f);
           glVertex2f( 0.95f,-0.95f);
           glVertex2f(-0.95f,-0.95f);
       glEnd();

       // Título
       glColor3f(0.3f, 0.7f, 1.0f);
       glRasterPos2f(-0.6f, 0.85f);
       const char* title = "TERMINAL DE COMUNICACIONES TACTICAS MK-IV";
       while (*title) glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *title++);

       // Contexto de misión
       glColor3f(0.0f, 0.8f, 1.0f);
       const char* context[] = {
           "INFORME DE MISION:",
           "OPERACION: LIBERTAD",
           "FECHA: 13/Dic/2024",
           "ZONA: Oblast de Kursk",
           "OBJETIVO: Limpieza de obstaculos",
           "",
           "INSTRUCCIONES TACTICAS:",
           "- Evitar colision por fuerzas enemigas",
           "- Mantener altitud optima",
           "- Usar impulso en situaciones optimas",
           "",
           "CONTROLES OPERATIVOS:",
           "W/S o FLECHAS: Control de altitud",
           "ESPACIO: Propulsor de velocidad del avion",
           "ESC: Salir y guardar partida",
           "",
           "SISTEMA DESARROLLADO POR:",
           "Michell Policarpio - zS21002379",
           "Graficacion Por Computadora NRC: 18679",
           "FIEE - Universidad Veracruzana (UV) 2024"
       };

       float y_pos = 0.7f;
       for(int i = 0; i < sizeof(context)/sizeof(context[0]); i++) {
           glRasterPos2f(-0.9f, y_pos);
           const char* text = context[i];
           while (*text) glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *text++);
           y_pos -= 0.08f;
       }

       glMatrixMode(GL_PROJECTION);
       glPopMatrix();
   }
   else {
       // Terminal operativa normal
       glMatrixMode(GL_PROJECTION);
       glPushMatrix();
       glLoadIdentity();
       gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
       
       glMatrixMode(GL_MODELVIEW);
       glLoadIdentity();

       // Fondo negro
       glColor4f(0.05f, 0.05f, 0.05f, 1.0f);
       glBegin(GL_QUADS);
           glVertex2f(-1.0f, -1.0f);
           glVertex2f( 1.0f, -1.0f);
           glVertex2f( 1.0f, 1.0f);
           glVertex2f(-1.0f, 1.0f);
       glEnd();

       // Marco principal
       glColor3f(0.2f, 0.3f, 0.2f);
       glLineWidth(2.0f);
       glBegin(GL_LINE_LOOP);
           glVertex2f(-0.95f, 0.95f);
           glVertex2f( 0.95f, 0.95f);
           glVertex2f( 0.95f,-0.95f);
           glVertex2f(-0.95f,-0.95f);
       glEnd();

       // Título
       glColor3f(0.3f, 0.7f, 1.0f);
       glRasterPos2f(-0.6f, 0.85f);
       const char* title = "TERMINAL DE COMUNICACIONES TACTICAS MK-IV";
       while (*title) glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *title++);

       // Radar giratorio
       static float radar_angle = 0.0f;
       radar_angle += 0.03f;
       if (radar_angle > 360.0f) radar_angle = 0.0f;

       // Marco del radar
       glColor3f(0.2f, 0.3f, 0.2f);
       glBegin(GL_LINE_LOOP);
       for(int i = 0; i < 360; i += 10) {
           float rad = i * 3.14159f / 180.0f;
           glVertex2f(-0.75f + cos(rad)*0.15f, 0.65f + sin(rad)*0.15f);
       }
       glEnd();

       // Línea del radar
       glColor4f(0.0f, 1.0f, 0.0f, 0.7f);
       glBegin(GL_LINES);
           glVertex2f(-0.75f, 0.65f);
           float rad = radar_angle * 3.14159f / 180.0f;
           glVertex2f(-0.75f + cos(rad)*0.15f, 0.65f + sin(rad)*0.15f);
       glEnd();

       // Indicadores de estado
       glColor3f(0.0f, 0.8f, 1.0f);
       const char* indicators[] = {
           "RECEPCION SATELLITAL: 77%",
           "ESTADO: OPERATIVO",
           "CANAL: ENCRIPTADO",
           "MISION: LIBERTAD"
       };
       float y_pos = 0.35f;
       for(int i = 0; i < 4; i++) {
           glRasterPos2f(-0.93f, y_pos);
           const char* text = indicators[i];
           while (*text) glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *text++);
           y_pos -= 0.08f;
       }

       // Alerta
       static float blink = 0.0f;
       blink += 0.1f;
       if(sin(blink) > 0) {
           glColor3f(1.0f, 0.6f, 0.0f);
           glRasterPos2f(0.25f, 0.35f);
           const char* alert = "! ZONA HOSTIL !";
           while (*alert) glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *alert++);
       }

       glMatrixMode(GL_PROJECTION);
       glPopMatrix();
   }
   
   glutSwapBuffers();
}

void reshape_dialog(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    
    glMatrixMode(GL_MODELVIEW);
}

void display_mission_observer_window(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    // Fondo negro semi-transparente
    glColor4f(0.05f, 0.05f, 0.05f, 1.0f);
    glBegin(GL_QUADS);
        glVertex2f(-1.0f, -1.0f);
        glVertex2f( 1.0f, -1.0f);
        glVertex2f( 1.0f,  1.0f);
        glVertex2f(-1.0f,  1.0f);
    glEnd();

    // Marco principal
    glColor3f(0.2f, 0.3f, 0.2f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(-0.95f, 0.95f);
        glVertex2f( 0.95f, 0.95f);
        glVertex2f( 0.95f,-0.95f);
        glVertex2f(-0.95f,-0.95f);
    glEnd();

    // Título de Misiones
    glColor3f(0.0f, 0.8f, 1.0f);
    glRasterPos2f(-0.6f, 0.85f);

    // Renderizar lista de misiones
    renderizar_queue_misiones();
    
    glutSwapBuffers();
}

void reshape_mission_observer(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    
    glMatrixMode(GL_MODELVIEW);
}



/* ╔═══════════════════════════════════════════════════════════╗
 * ║       FUNCIONES QUE DIBUJAN Y CONFIGURAN LA AVIONETA      ║
 * ╚═══════════════════════════════════════════════════════════╝ */

// Inicializar las partículas
void inicializar_estela(void) {
    avioneta.estela_contador = 0;
}

// esta funcion maneja el sistema de partículas que crea la estela de la avioneta
void actualizar_estela(float delta_time) {
    // Mover partículas existentes
    for(int i = 0; i < avioneta.estela_contador; i++) {
        avioneta.estela[i].alpha -= delta_time * 2.0f;  // Desvanecer
        avioneta.estela[i].x -= avioneta.current_speed * delta_time;  // Mover hacia atrás
        avioneta.estela[i].size += delta_time * 2.0f;  // Aumentar tamaño
    }

    // Se eliminan partículas desvanecidas
    for(int i = 0; i < avioneta.estela_contador; i++) {
        if(avioneta.estela[i].alpha <= 0) {
            // se mueve última partícula a esta posición
            if(i < avioneta.estela_contador - 1) {
                avioneta.estela[i] = avioneta.estela[avioneta.estela_contador - 1];
                i--;  // Revisar esta posición de nuevo
            }
            avioneta.estela_contador--;
        }
    }

    // Se añade nueva partícula si estamos en boost
    if(avioneta.current_speed > avioneta.normal_speed && 
       avioneta.estela_contador < MAX_PARTICULAS_ESTELA) {
        TrailParticle new_particle = {
            .x = 0,
            .y = avioneta.y_pos,
            .alpha = 1.0f,
            .size = 1.0f
        };
        avioneta.estela[avioneta.estela_contador++] = new_particle;
    }
}

// Renderizar la estela
void renderizar_estela(void) {
    glPushMatrix();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for(int i = 0; i < avioneta.estela_contador; i++) {
        glColor4f(1.0f, 0.5f, 0.0f, avioneta.estela[i].alpha);  // Naranja con transparencia
        
        glPushMatrix();
        glTranslatef(avioneta.estela[i].x, avioneta.estela[i].y, 0.0f);
        glScalef(avioneta.estela[i].size, avioneta.estela[i].size, 1.0f);
        
        // Dibujar partícula como un cuadrado o círculo
        glBegin(GL_QUADS);
        float size = 0.5f;
        glVertex3f(-size, -size, 0.0f);
        glVertex3f( size, -size, 0.0f);
        glVertex3f( size,  size, 0.0f);
        glVertex3f(-size,  size, 0.0f);
        glEnd();
        
        glPopMatrix();
    }
    
    glDisable(GL_BLEND);
    glPopMatrix();
}

// Funciones auxiliares de dibujo
static void dibujarScaledCube(float width, float height, float length) {
    glPushMatrix();
        glScalef(width, height, length);
        glutSolidCube(1.0);
    glPopMatrix();
}

static void dibujar_ala() {
    glPushMatrix();
        glColor3f(1.0f, 1.0f, 0.0f); // Amarillo
        glRotatef(WING_ANGLE, 0.0f, 0.0f, 0.0f);
        
        // Parte central del ala
        dibujarScaledCube(WING_LENGTH, WING_WIDTH, WING_HEIGHT);
        
        // Borde de ataque
        glPushMatrix();
            glTranslatef(0.0f, 0.0f, WING_HEIGHT/2);
            glRotatef(-30.0f, 1.0f, 0.0f, 0.0f);
            dibujarScaledCube(WING_LENGTH, WING_WIDTH, WING_HEIGHT * 0.3f);
        glPopMatrix();
        
        // Borde de fuga
        glPushMatrix();
            glTranslatef(0.0f, 0.0f, -WING_HEIGHT/2);
            glRotatef(15.0f, 1.0f, 0.0f, 0.0f);
            dibujarScaledCube(WING_LENGTH, WING_WIDTH, WING_HEIGHT * 0.2f);
        glPopMatrix();
    glPopMatrix();
}

static void dibujar_helice() {
    glPushMatrix();
        // Hub de la hélice
        glColor3f(1.0f, 1.0f, 0.0f); // Amarillo
        GLUquadric *quadric = gluNewQuadric();
        gluQuadricDrawStyle(quadric, GLU_FILL);
        gluCylinder(quadric, HELICE_HUB_RADIUS, HELICE_HUB_RADIUS, 0.5f, 20, 1);
        gluDeleteQuadric(quadric);

        // Aspas rotando
        glTranslatef(0.0f, 0.0f, 0.25f);
        glRotatef(avioneta.propeller.angle, 0.0f, 0.0f, 1.0f);
        
        // Primera aspa
        glPushMatrix();
            glColor3f(0.4f, 0.4f, 0.4f);
            dibujarScaledCube(HELICE_HEIGHT, HELICE_WIDTH, HELICE_LENGTH);
        glPopMatrix();
        
        // Segunda aspa
        glPushMatrix();
            glRotatef(90, 0.0f, 0.0f, 1.0f);
            glColor3f(0.4f, 0.4f, 0.4f);
            dibujarScaledCube(HELICE_HEIGHT, HELICE_WIDTH, HELICE_LENGTH);
        glPopMatrix();
    glPopMatrix();
}

void inicializar_avioneta(void) {
    inicializar_estela();

    avioneta.y_pos = -20.0f;
    avioneta.y_velocity = 0.0f;
    
    // Aplicar velocidades según dificultad actual
    float speed_multiplier = 1.0f;
    switch(game_menu.dificultad) {
        case DIFFICULTY_MEDIUM:
            speed_multiplier = 2.0f;
            break;
        case DIFFICULTY_HARD:
            speed_multiplier = 3.0f;
            break;
        default:
            speed_multiplier = 1.0f;
    }
    
    avioneta.normal_speed = 30.0f * speed_multiplier;
    avioneta.boost_speed = 60.0f * speed_multiplier;
    avioneta.current_speed = avioneta.normal_speed;  // Importante
    
    avioneta.rotation = 0.0f;
    avioneta.propeller.angle = 0.0f;
    avioneta.propeller.speed = 5.0f;
    avioneta.max_y = 20.0f;
    avioneta.min_y = -20.0f;
}

void actualizar_avioneta(float delta_time) {
    actualizar_estela(delta_time);

    // Actualizar posición vertical
    avioneta.y_pos += avioneta.y_velocity * delta_time;
    
    // Limitar posición vertical
    if (avioneta.y_pos > avioneta.max_y) {
        avioneta.y_pos = avioneta.max_y;
        avioneta.y_velocity = 0;
    }
    if (avioneta.y_pos < avioneta.min_y) {
        avioneta.y_pos = avioneta.min_y;
        avioneta.y_velocity = 0;
    }
    
    // Actualizar rotación basada en la velocidad vertical
    avioneta.rotation = -avioneta.y_velocity * 2.0f;
    
    // Actualizar hélice
    actualizar_helice();
}

void renderizar_avioneta(void) {
    renderizar_estela(); 
    glPushMatrix();
        // Posicionar la avioneta
        glTranslatef(0.0f, avioneta.y_pos, 0.0f);
        
        // Rotar 90 grados para mirar hacia la derecha
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        
        // Rotar según el movimiento vertical
        glRotatef(avioneta.rotation, 1.0f, 0.0f, 0.0f);
        
        // Fuselaje principal
        glColor3f(0.0f, 0.0f, 1.0f); // Azul (cambia de 0.8f, 0.8f, 0.8f a 0.0f, 0.0f, 1.0f)

        dibujarScaledCube(BODY_WIDTH, BODY_HEIGHT, BODY_LENGTH);
        
        // Nariz y hélice
        glPushMatrix();
            glTranslatef(0.0f, 0.0f, BODY_LENGTH/2);
            
            // Nariz cónica
            glColor3f(0.7f, 0.7f, 0.7f);
            GLUquadric *quadric = gluNewQuadric();
            gluQuadricDrawStyle(quadric, GLU_FILL);
            gluCylinder(quadric, BODY_WIDTH/2, BODY_WIDTH/4, 2.0f, 20, 1);
            gluDeleteQuadric(quadric);
            
            // Hélice
            glTranslatef(0.0f, 0.0f, 1.0f);
            dibujar_helice();
        glPopMatrix();
        
        // Alas y cola
        dibujar_ala();
        
        // Cola vertical
        glPushMatrix();
            glTranslatef(0.0f, BODY_HEIGHT/2 + TAIL_HEIGHT/2, -BODY_LENGTH/2 + TAIL_LENGTH/2);
            glColor3f(0.7f, 0.7f, 0.7f);
            dibujarScaledCube(TAIL_WIDTH, TAIL_HEIGHT, TAIL_LENGTH);
        glPopMatrix();
        
        // Cola horizontal
        glPushMatrix();
            glTranslatef(0.0f, 0.0f, -BODY_LENGTH/2 + TAIL_LENGTH/2);
            glColor3f(0.7f, 0.7f, 0.7f);
            dibujarScaledCube(WING_LENGTH/2, WING_WIDTH, WING_HEIGHT/2);
        glPopMatrix();
    glPopMatrix();
}

void handle_avioneta_input(unsigned char key) {
    switch(key) {
        case 'w':
        case 'W':
            avioneta.y_velocity = 17.0f;
            break;
        case 's':
        case 'S':
            avioneta.y_velocity = -17.0f;
            break;
        case ' ': // Espacio para boost
            avioneta.current_speed = avioneta.boost_speed;
            avioneta.propeller.speed = 20.0f;
            break;
        case 13: // Enter (acelerador permanente)
            avioneta.normal_speed += 20.0f;
            if(avioneta.normal_speed > avioneta.boost_speed) {
                avioneta.normal_speed = avioneta.boost_speed;
            }
            avioneta.current_speed = avioneta.normal_speed;
            break;
    }
}

void handle_avioneta_key_up(unsigned char key) {
    switch(key) {
        case ' ': // Cuando se suelta el espacio
            avioneta.current_speed = avioneta.normal_speed;
            avioneta.propeller.speed = 15.0f;
            break;
        case 'w':
        case 'W':
        case 's':
        case 'S':
            avioneta.y_velocity = 0.0f;
            break;
    }
}

void actualizar_helice(void) {
    avioneta.propeller.angle += avioneta.propeller.speed;
    if (avioneta.propeller.angle > 360.0f) {
        avioneta.propeller.angle -= 360.0f;
    }
}



/* ╔═══════════════════════════════════════════════════════════╗
 * ║       FUNCIONES DE MANEJO DE OBSTÁCULOS                   ║
 * ╚═══════════════════════════════════════════════════════════╝ */

void inicializar_obstaculos(void) {
    obstacle_system.head = NULL;
    obstacle_system.spawn_timer = 0.0f;
    obstacle_system.spawn_interval = 1.0f;
    obstacle_system.speed = 30.0f;
    obstacle_system.activo_contador = 0;
}

void generar_obstaculo(void) {
    Obstacle* new_obstacle = (Obstacle*)malloc(sizeof(Obstacle));
    if (new_obstacle) {
        new_obstacle->x = 100.0f;
        new_obstacle->y = avioneta.min_y + (rand() % (int)((avioneta.max_y - avioneta.min_y) * 10)) / 10.0f;
        new_obstacle->width = 2.0f;
        new_obstacle->height = 8.0f + (rand() % 40) / 10.0f;
        new_obstacle->activo = true;
        new_obstacle->sig = obstacle_system.head;
        obstacle_system.head = new_obstacle;
        obstacle_system.activo_contador++;
    }
}

void limpiar_obstaculos_inactivos(void) {
    Obstacle* current = obstacle_system.head;
    Obstacle* prev = NULL;

    while (current != NULL) {
        if (!current->activo) {
            Obstacle* to_delete = current;
            
            if (prev == NULL) {
                obstacle_system.head = current->sig;
                current = obstacle_system.head;
            } else {
                prev->sig = current->sig;
                current = current->sig;
            }
            
            free(to_delete);
            obstacle_system.activo_contador--;
        } else {
            prev = current;
            current = current->sig;
        }
    }
}

void actualizar_obstaculos(float delta_time) {
    obstacle_system.spawn_timer += delta_time;
    if (obstacle_system.spawn_timer >= obstacle_system.spawn_interval) {
        generar_obstaculo();
        obstacle_system.spawn_timer = 0.0f;
    }

    Obstacle* current = obstacle_system.head;
    while (current != NULL) {
        if (current->activo) {
            current->x -= avioneta.current_speed * delta_time;

            if (current->x < -50.0f) {
                current->activo = false;
            }
        }
        current = current->sig;
    }

    limpiar_obstaculos_inactivos();
}

void renderizar_obstaculos(void) {
    glColor3f(1.0f, 0.0f, 0.0f);  // Color rojo para obstáculos
    
    Obstacle* current = obstacle_system.head;
    while (current != NULL) {
        if (current->activo) {
            glPushMatrix();
                glTranslatef(current->x, current->y, 0.0f);
                glScalef(current->width, current->height, 1.0f);
                glutSolidCube(1.0f);
            glPopMatrix();
        }
        current = current->sig;
    }
}

bool verificar_colision(float avioneta_x, float avioneta_y, float avioneta_radius) {
    Obstacle* current = obstacle_system.head;
    while (current != NULL) {
        if (current->activo) {
            float dx = avioneta_x - current->x;
            float dy = avioneta_y - current->y;
            float distance = sqrt(dx * dx + dy * dy);
            
            if (distance < avioneta_radius + current->width) {
                return true;
            }
        }
        current = current->sig;
    }
    return false;
}

void free_todos_obstaculos(void) {
    Obstacle* current = obstacle_system.head;
    while (current != NULL) {
        Obstacle* sig = current->sig;
        free(current);
        current = sig;
    }
    obstacle_system.head = NULL;
    obstacle_system.activo_contador = 0;
}



/* ╔═══════════════════════════════════════════════════════════╗
 * ║       FUNCIONES DE MANEJO DEL MENÚ                        ║
 * ╚═══════════════════════════════════════════════════════════╝ */

const char* menu_options[4] = {
    "Nuevo Juego",
    "Cargar Juego",
    "Opciones",
    "Salir"
};

// Opciones de dificultad
const char* dificultad_options[3] = {
    "Facil - Velocidad Normal",
    "Medio - Doble Velocidad",
    "Dificil - Triple Velocidad"
};

// Inicializar menú
void inicializar_menu(void) {
    game_menu.opcion_seleccionada = MENU_NEW_GAME;
    game_menu.estado_actual = MENU_STATE_MAIN;
    game_menu.dificultad = DIFFICULTY_EASY;
    game_menu.dificultad_seleccionada = 0;
    game_menu.is_activo = true;
    game_menu.title_pos_y = 0.5f;
    game_menu.first_option_y = 0.2f;
    game_menu.option_spacing = 0.15f;
}

// Actualizar menú
void actualizar_menu(void) {
    // aqui animaciones o actualizaciones después
<<<<<<< HEAD
// Animación del título
static float title_animation_offset = 0.0f;
title_animation_offset += delta_time * 0.5f;
if (title_animation_offset > 1.0f) {
    title_animation_offset -= 1.0f;
}
game_menu.title_pos_y = 0.5f + sin(title_animation_offset * 2.0f * M_PI) * 0.05f;
=======
>>>>>>> 790c6ee80f4f0a927a6466c98ff7301d83e9dfe9
}

void renderizar_menu(void) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    if (game_menu.estado_actual == MENU_STATE_MAIN) {
        // Título del juego
        glColor3f(0.0f, 0.7f, 1.0f); // Azul claro para título
        float title_length = (float)glutBitmapLength(GLUT_BITMAP_HELVETICA_18, 
                           (const unsigned char*)"JUEGO DE LA AVIONETA SIDERAL");
        float title_scale = 1000.0f;
        float title_x = -title_length / title_scale;
        glRasterPos2f(title_x, game_menu.title_pos_y);
        const char* title = "JUEGO DE LA AVIONETA SIDERAL";
        while (*title) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *title++);
        }
        
        // Renderizar todas las opciones del menú principal
        for (int i = 0; i < MENU_OPTION_COUNT; i++) {
            float y = game_menu.first_option_y - ((float)i * game_menu.option_spacing);
            bool is_selected = (i == (int)game_menu.opcion_seleccionada);
            
            if (is_selected) {
                // Dibujar la flecha
                float arrow_x = -0.3f;
                glColor3f(1.0f, 1.0f, 0.0f); // Amarillo para la flecha
                glRasterPos2f(arrow_x, y);
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, '>');
                
                // Dibujar texto seleccionado en amarillo
                glColor3f(1.0f, 1.0f, 0.0f);
            } else {
                // Dibujar texto no seleccionado en blanco
                glColor3f(1.0f, 1.0f, 1.0f);
            }

            // Dibujar el texto de la opción
            float text_length = (float)glutBitmapLength(GLUT_BITMAP_HELVETICA_18, 
                              (const unsigned char*)menu_options[i]);
            float scale = 1000.0f;
            float x = -text_length / scale;
            glRasterPos2f(x, y);
            
            const char* text = menu_options[i];
            while (*text) {
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++);
            }
        }
    }
    else if (game_menu.estado_actual == MENU_STATE_OPTIONS) {
        // Título del menú de opciones
        glColor3f(0.0f, 0.7f, 1.0f);
        float title_x = -0.4f;
        glRasterPos2f(title_x, game_menu.title_pos_y);
        const char* title = "SELECCIONAR DIFICULTAD";
        while (*title) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *title++);
        }

        // Instrucciones
        glColor3f(0.7f, 0.7f, 0.7f);
        glRasterPos2f(-0.4f, game_menu.title_pos_y - 0.1f);
        const char* instr = "Presiona ESC para volver";
        while (*instr) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *instr++);
        }

        // Opciones de dificultad
        for (int i = 0; i < 3; i++) {
            float y = game_menu.first_option_y - ((float)i * game_menu.option_spacing);
            bool is_selected = (i == game_menu.dificultad_seleccionada);
            
            if (is_selected) {
                glColor3f(1.0f, 1.0f, 0.0f);
                glRasterPos2f(-0.3f, y);
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, '>');
            } else {
                glColor3f(1.0f, 1.0f, 1.0f);
            }

            glRasterPos2f(-0.2f, y);
            const char* text = dificultad_options[i];
            while (*text) {
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++);
            }
        }
    }
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

// Manejar input del teclado normal
void manejar_entrada_menu(unsigned char key) {
    if (game_menu.estado_actual == MENU_STATE_MAIN) {
        if (key == 13) { // ENTER
            switch(game_menu.opcion_seleccionada) {
                case MENU_NEW_GAME:
                    // Reiniciar todo
                    free_todos_obstaculos();
                    init_save_stack();                     // se inicializa la pila de guardados
                    has_saved_game = false;                // Marca que no hay juego guardado
                    inicializar_avioneta();                // Reinicia jugador
                    inicializar_obstaculos();              // Reinicia obstáculos
                    inicializar_sistema_municiones();      // Reinicia sistema de munición
                    inicializar_mision_queue();            // Reinicia misiones
                    inicializar_personaje();
                    
                    // E inicia diálogo
                    dialogo_actual = mision_dialog;
                    estado_actual = STATE_DIALOG;
                    break;
                    
                case MENU_LOAD_GAME:
                    printf("Cargando juego...\n");
                    if (has_saved_game) {
                        cargar_juego_guardado();
                        estado_actual = STATE_PLAYING;
                    } else {
                        printf("No hay juego guardado\n");
                    }
                    break;
                    
                case MENU_OPTIONS:
                    game_menu.estado_actual = MENU_STATE_OPTIONS;
                    game_menu.dificultad_seleccionada = (int)game_menu.dificultad;
                    break;
                    
                case MENU_EXIT:
                    printf("Saliendo del juego...\n");
                    exit(0);
                    break;
                    
                case MENU_OPTION_COUNT:
                    break;
            }
        }
    }
    else if (game_menu.estado_actual == MENU_STATE_OPTIONS) {
        if (key == 13) { // ENTER
            game_menu.dificultad = (GameDifficulty)game_menu.dificultad_seleccionada;
            aplicar_configuracion_dificultad();
            game_menu.estado_actual = MENU_STATE_MAIN;
        }
        else if (key == 27) { // ESC
            game_menu.estado_actual = MENU_STATE_MAIN;
        }
    }
}

// Manejar teclas especiales
void menu_handle_special_keys(int key) {
    if (game_menu.estado_actual == MENU_STATE_MAIN) {
        switch(key) {
            case GLUT_KEY_UP:
                if (game_menu.opcion_seleccionada > 0) {
                    game_menu.opcion_seleccionada--;
                    glutPostRedisplay();
                }
                break;
                
            case GLUT_KEY_DOWN:
                if (game_menu.opcion_seleccionada < MENU_OPTION_COUNT - 1) {
                    game_menu.opcion_seleccionada++;
                    glutPostRedisplay();
                }
                break;
        }
    }
    else if (game_menu.estado_actual == MENU_STATE_OPTIONS) {
        switch(key) {
            case GLUT_KEY_UP:
                if (game_menu.dificultad_seleccionada > 0) {
                    game_menu.dificultad_seleccionada--;
                    glutPostRedisplay();
                }
                break;
            case GLUT_KEY_DOWN:
                if (game_menu.dificultad_seleccionada < 2) {
                    game_menu.dificultad_seleccionada++;
                    glutPostRedisplay();
                }
                break;
        }
    }
}

void aplicar_configuracion_dificultad(void) {
    float speed_multiplier = 1.0f;
    switch(game_menu.dificultad) {
        case DIFFICULTY_MEDIUM:
            speed_multiplier = 2.0f;
            break;
        case DIFFICULTY_HARD:
            speed_multiplier = 3.0f;
            break;
        default: // DIFFICULTY_EASY
            speed_multiplier = 1.0f;
            break;
    }
    
    // Aplicar multiplicador a las velocidades
    avioneta.normal_speed = 30.0f * speed_multiplier;
    avioneta.boost_speed = 60.0f * speed_multiplier;
    avioneta.current_speed = avioneta.normal_speed;  // Añadir esta línea
    obstacle_system.speed = 30.0f * speed_multiplier;
    
    printf("Dificultad cambiada - Multiplicador: %.1f\n", speed_multiplier);
}



/* ╔═══════════════════════════════════════════════════════════╗
 * ║       FUNCIONES QUE GESTIONA COLA DE MISIONES             ║
 * ╚═══════════════════════════════════════════════════════════╝ */

void inicializar_mision_queue(void) {
    mision_queue.front = NULL;
    mision_queue.rear = NULL;
    mision_queue.contador = 0;

    // Inicializar misiones predefinidas
    Mission takeoff = {
        .type = MISION_DESPEGUE,
        .target_amount = 1,
        .current_amount = 0,
        .completed = false,
        .description = "Mision 1: Despegar"
    };
    enqueue_mision(takeoff);

    Mission collect_ammo = {
        .type = MISION_COLECTAR_MUNICION,
        .target_amount = 10,
        .current_amount = 0,
        .completed = false,
        .description = "Mision 2: Recolectar 10 municiones"
    };
    enqueue_mision(collect_ammo);

    Mission destroy_obstacles = {
        .type = MISION_DESTRUIR_OBSTACULOS,
        .target_amount = 10,
        .current_amount = 0,
        .completed = false,
        .description = "Mision 3: Destruir 10 obstaculos"
    };
    enqueue_mision(destroy_obstacles);
}

void enqueue_mision(Mission mision) {
    MissionNode* newNode = (MissionNode*)malloc(sizeof(MissionNode));
    newNode->mision = mision;
    newNode->sig = NULL;

    if (is_queue_empty()) {
        mision_queue.front = newNode;
    } else {
        mision_queue.rear->sig = newNode;
    }
    mision_queue.rear = newNode;
    mision_queue.contador++;
}

bool dequeue_mision(Mission* mision) {
    if (is_queue_empty()) return false;

    MissionNode* temp = mision_queue.front;
    *mision = temp->mision;
    mision_queue.front = mision_queue.front->sig;
    free(temp);

    if (mision_queue.front == NULL) {
        mision_queue.rear = NULL;
    }
    mision_queue.contador--;
    return true;
}

bool is_queue_empty(void) {
    return mision_queue.front == NULL;
}

void renderizar_queue_misiones(void) {
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();
   gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
   
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   // Título
   glColor3f(0.0f, 0.8f, 1.0f);
   glRasterPos2f(0.0f, 0.8f);
   const char* title = "MISIONES ACTIVAS";
   while (*title) glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *title++);

   // Listar misiones
   float y_pos = 0.6f;
   MissionNode* current = mision_queue.front;
   while (current != NULL) {
       // Determinar el color según el estado de la misión
       if (current->mision.completed) {
           glColor3f(0.0f, 1.0f, 0.0f);  // Verde para completada
       }
       else if (current->mision.current_amount > 0) {
           glColor3f(1.0f, 0.0f, 0.0f);  // Rojo para en progreso
       }
       else {
           glColor3f(0.7f, 0.7f, 0.7f);  // Gris para no iniciada
       }

       glRasterPos2f(-0.9f, y_pos);
       const char* desc = current->mision.description;
       while (*desc) glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *desc++);

       // Mostrar progreso actualizado
       char progress[32];
       int current_progress = 0;
       
       switch(current->mision.type) {
           case MISION_DESPEGUE:
               current_progress = current->mision.completed ? 1 : 0;
               break;
           case MISION_COLECTAR_MUNICION:
               current_progress = municion_colectada;
               if (current_progress >= current->mision.target_amount) {
                   current->mision.completed = true;
               }
               break;
           case MISION_DESTRUIR_OBSTACULOS:
               current_progress = obstacles_destroyed;
               if (current_progress >= current->mision.target_amount) {
                   current->mision.completed = true;
               }
               break;
           default:
               current_progress = 0;
       }
       
       current->mision.current_amount = current_progress;  // Actualizar el progreso
       
       sprintf(progress, "(%d/%d)", current_progress, current->mision.target_amount);
       glRasterPos2f(0.5f, y_pos);
       const char* prog = progress;
       while (*prog) glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *prog++);

       y_pos -= 0.2f;
       current = current->sig;
   }

   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
}



/* ╔═══════════════════════════════════════════════════════════╗
 * ║       FUNCIONES QUE GESTIONAN ARBOL BINARIO DE DIALOGO    ║
 * ╚═══════════════════════════════════════════════════════════╝ */

// Función para crear un nuevo nodo de diálogo
DialogNode* create_dialog_node(const char* speaker, const char* text, 
                             const char* izq_eleccion, const char* der_eleccion,
                             MissionPhase fase) {
    DialogNode* node = (DialogNode*)malloc(sizeof(DialogNode));
    node->speaker = strdup(speaker);
    node->text = strdup(text);
    node->izq_eleccion = izq_eleccion ? strdup(izq_eleccion) : NULL;
    node->der_eleccion = der_eleccion ? strdup(der_eleccion) : NULL;
    node->izq = NULL;
    node->der = NULL;
    node->action = NULL;
    node->fase = fase;
    return node;
}

// Acciones de la misión
void action_iniciar_mision(void) {
    mision_status.fase_actual = FASE_INFILTRACION;
    printf("Iniciando infiltracion...\n");
    printf("Fase actual: %d\n", mision_status.fase_actual);
}

// Inicializar diálogos
void inicializar_dialogos(void) {
    dialogo_actual = NULL;
    // Inicializar estado de la misión
    mision_status.target_x = 1000.0f;
    mision_status.target_y = 0.0f;
    mision_status.escaping = false;
    mision_status.fase_actual = FASE_INFORMACION;

    // Nodo inicial
    mision_dialog = create_dialog_node(
        "Comandante Eagle",
        "¡Atencion piloto! Los cielos estan repletos de obstaculos.\nSu mision: recolectar municion y destruir todos los\nobjetivos posibles. ¡Debemos limpiar esta region!",
        "Entendido, comandante",
        "Detalles de la mision",
        FASE_INFORMACION
    );

    // Nodo para opción 1
    DialogNode* start_node = create_dialog_node(
        "Comandante Eagle",
        "Excelente actitud, piloto. Recolecte 10 cajas de\nmunicion para activar el sistema de disparo.\nUse el boost para evadir obstaculos.",
        "Iniciar operacion",
        NULL,
        FASE_INFORMACION
    );
    start_node->action = action_iniciar_mision;

    // Nodo final para la opción 1
    DialogNode* final_node = create_dialog_node(
        "Comandante Eagle",
        "¡Buena suerte, piloto! Mantenga los cielos seguros.",
        NULL,
        NULL,
        FASE_INFILTRACION
    );
    start_node->izq = final_node;
    mision_dialog->izq = start_node;

    // Rama de detalles
    DialogNode* details_node = create_dialog_node(
        "Oficial de Inteligencia",
        "Objetivos de mision:\n- Recolectar cajas de municion (10)\n- Destruir obstaculos con disparos\n- Evitar colisiones fatales",
        "Entendido, proceder",
        "Mas informacion",
        FASE_INFORMACION
    );
    mision_dialog->der = details_node;

    // Información adicional
    DialogNode* extra_details = create_dialog_node(
        "Oficial de Inteligencia",
        "Datos tecnicos:\n- Sistema de disparo requiere municion\n- Use boost para evitar obstaculos\n- Disparos destruyen objetivos",
        "Iniciar mision",
        "Controles",
        FASE_INFORMACION
    );
    details_node->der = extra_details;

    // Información de controles
    DialogNode* escape_info = create_dialog_node(
        "Oficial de Operaciones",
        "Controles basicos:\n- W/S: Control de altura\n- Espacio: Activar boost\n- B: Disparar (requiere municion)",
        "Entendido, iniciando operacion",
        NULL,
        FASE_INFORMACION
    );
    extra_details->der = escape_info;

    // Conectar todos los endpoints al inicio de la misión
    details_node->izq = start_node;
    extra_details->izq = start_node;
    escape_info->izq = start_node;

    dialogo_actual = mision_dialog;
}

// Renderizar diálogo actual
void renderizar_dialogo(DialogNode* node) {
    if (!node) return;

    // Configurar vista ortográfica para el diálogo
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Fondo semi-transparente con degradado
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Degradado del fondo
    glBegin(GL_QUADS);
        glColor4f(0.0f, 0.1f, 0.2f, 0.9f);  // Azul oscuro arriba
        glVertex2f(-0.95f, 0.95f);
        glVertex2f( 0.95f, 0.95f);
        glColor4f(0.0f, 0.2f, 0.3f, 0.9f);  // Azul más claro abajo
        glVertex2f( 0.95f, -0.95f);
        glVertex2f(-0.95f, -0.95f);
    glEnd();

    // Borde del panel de diálogo
    glColor4f(0.0f, 0.8f, 1.0f, 0.7f);  // Azul brillante
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(-0.95f, 0.95f);
        glVertex2f( 0.95f, 0.95f);
        glVertex2f( 0.95f, -0.95f);
        glVertex2f(-0.95f, -0.95f);
    glEnd();

    // Línea separadora después del nombre
    glBegin(GL_LINES);
        glVertex2f(-0.95f, 0.75f);
        glVertex2f( 0.95f, 0.75f);
    glEnd();

    // Renderizar nombre del hablante con efecto de "brillo"
    glColor4f(0.0f, 1.0f, 1.0f, 1.0f);  // Cyan brillante
    glRasterPos2f(-0.90f, 0.80f);
    const char* text = node->speaker;
    while (*text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++);
    }

    // Renderizar texto principal con mejor espaciado
    glColor4f(0.8f, 0.8f, 0.8f, 1.0f);  // Blanco suave
    glRasterPos2f(-0.90f, 0.60f);
    text = node->text;
    float y_offset = 0.60f;
    while (*text) {
        if (*text == '\n') {
            y_offset -= 0.15f;  // Mayor espaciado entre líneas
            glRasterPos2f(-0.90f, y_offset);
        } else {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text);
        }
        text++;
    }

    // Línea separadora antes de las opciones
    glColor4f(0.0f, 0.8f, 1.0f, 0.7f);
    glBegin(GL_LINES);
        glVertex2f(-0.95f, -0.6f);
        glVertex2f( 0.95f, -0.6f);
    glEnd();

    // Renderizar opciones con resaltado
    if (node->izq_eleccion) {
        glColor4f(0.0f, 0.8f, 0.4f, 1.0f);  // Verde brillante
        glRasterPos2f(-0.90f, -0.75f);
        text = "1: ";
        while (*text) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++);
        
        glColor4f(0.7f, 1.0f, 0.7f, 1.0f);  // Verde más claro
        text = node->izq_eleccion;
        while (*text) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++);
    }

    if (node->der_eleccion) {
        glColor4f(0.0f, 0.8f, 0.4f, 1.0f);  // Verde brillante
        glRasterPos2f(-0.90f, -0.90f);
        text = "2: ";
        while (*text) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++);
        
        glColor4f(0.7f, 1.0f, 0.7f, 1.0f);  // Verde más claro
        text = node->der_eleccion;
        while (*text) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++);
    }

    glDisable(GL_BLEND);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void mostrar_game_over(void) {
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   
   // Fondo negro semi-transparente
   glColor4f(0.0f, 0.0f, 0.0f, 0.8f);
   glBegin(GL_QUADS);
       glVertex2f(-1.0f, -1.0f);
       glVertex2f( 1.0f, -1.0f);
       glVertex2f( 1.0f,  1.0f);
       glVertex2f(-1.0f,  1.0f);
   glEnd();

   // GAME OVER
   glColor3f(1.0f, 0.0f, 0.0f);
   const char* text = "GAME OVER";
   glRasterPos2f(-0.4f, 0.2f);
   while (*text) {
       glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *text++);
       glTranslatef(0.15f, 0, 0);
   }

   // Mensaje de aliento
   glLoadIdentity();
   glColor3f(1.0f, 1.0f, 1.0f);
   const char* msg = "¡El cielo te espera de nuevo, piloto!";
   glRasterPos2f(-0.4f, -0.1f);
   while (*msg) {
       glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *msg++);
   }

   glDisable(GL_BLEND);
}

// Manejar input del diálogo
void handle_dialog_input(unsigned char key) {
    if (!dialogo_actual) return;

    DialogNode* sig_node = NULL;

    switch(key) {
        case '1':
            if (dialogo_actual->izq) {
                if (dialogo_actual->action) {
                    dialogo_actual->action();
                }
                sig_node = dialogo_actual->izq;
            }
            break;

        case '2':
            if (dialogo_actual->der) {
                if (dialogo_actual->action) {
                    dialogo_actual->action();
                }
                sig_node = dialogo_actual->der;
            }
            break;
    }

    // Si hay un siguiente nodo, avanzamos a él
    if (sig_node) {
        dialogo_actual = sig_node;
    }
    // Si no hay siguiente nodo, significa que es el final del diálogo
    else if (dialogo_actual && (dialogo_actual->izq == NULL && dialogo_actual->der == NULL)) {
        // Cambiar al estado de juego
        estado_actual = STATE_PLAYING;
        // Limpiar el diálogo actual
        dialogo_actual = NULL;
        printf("Transicion a estado de juego\n"); // Debug
    }
}

// Actualizar estado de la misión
void actualizar_estado_mision(void) {
    if (mision_status.fase_actual == FASE_ESCAPE && mision_status.tiempo_restante > 0) {
        mision_status.tiempo_restante--;
    }
}

// Renderizar información de la misión
void renderizar_info_mision(void) {
    if (mision_status.fase_actual == FASE_INFORMACION) return;

    char info[128];
    sprintf(info, "Distancia al objetivo: %.0fm", mision_status.distance_to_target);
    
    if (mision_status.fase_actual == FASE_ESCAPE) {
        sprintf(info, "¡ESCAPE! Tiempo restante: %ds", mision_status.tiempo_restante);
    }

    // TODO: Renderizar información en pantalla
}

// Esta función libera recursivamente un nodo y sus hijos del árbol de diálogos
static void liberar_nodo_dialogo(DialogNode* nodo) {
    if (nodo == NULL) return;
    
    // Primero libera subarboles
    liberar_nodo_dialogo(nodo->izq);
    liberar_nodo_dialogo(nodo->der);
    
    // Luego libera strings del nodo actual
    if (nodo->text) free(nodo->text);
    if (nodo->speaker) free(nodo->speaker);
    if (nodo->izq_eleccion) free(nodo->izq_eleccion);
    if (nodo->der_eleccion) free(nodo->der_eleccion);
    
    // Finalmente libera el nodo
    free(nodo);
}

// Ey aqui inicia la limpieza del sistema de diálogos
void cleanup_dialogs(void) {
    liberar_nodo_dialogo(mision_dialog);
    mision_dialog = NULL;
    dialogo_actual = NULL;
}



/* ╔═══════════════════════════════════════════════════════════╗
 * ║       FUNCIONES QUE GESTIONAN MUNICIONES                  ║
 * ╚═══════════════════════════════════════════════════════════╝ */

void inicializar_sistema_municiones(void) {
    obstacles_destroyed = 0;  // Reiniciar contador
    // Inicializar cajas de munición
    for(int i = 0; i < MAX_CAJAS_MUNICION; i++) {
        caja_municion[i].activo = false;
    }
    
    // Limpiar lista de balas
    while(lista_balas != NULL) {
        Bala* temp = lista_balas;
        lista_balas = lista_balas->sig;
        free(temp);
    }
    
    municion_colectada = 0;
    //obstacles_destroyed = 0;
    puede_disparar = false;
}

void generar_caja_municion(void) {
    for(int i = 0; i < MAX_CAJAS_MUNICION; i++) {
        if(!caja_municion[i].activo) {
            caja_municion[i].x = 100.0f;  // Aparecen a la derecha
            caja_municion[i].y = -15.0f + (rand() % 300) / 10.0f;  // Posición Y aleatoria
            caja_municion[i].activo = true;
            break;
        }
    }
}

void actualizar_caja_municion(float delta_time) {
    // Mover cajas hacia la izquierda
    for(int i = 0; i < MAX_CAJAS_MUNICION; i++) {
        if(caja_municion[i].activo) {
            caja_municion[i].x -= 30.0f * delta_time;  // Velocidad de movimiento
            
            // Desactivar si sale de la pantalla
            if(caja_municion[i].x < -50.0f) {
                caja_municion[i].activo = false;
            }
        }
    }
}

void renderizar_caja_municion(void) {
    for(int i = 0; i < MAX_CAJAS_MUNICION; i++) {
        if(caja_municion[i].activo) {
            glPushMatrix();
            glTranslatef(caja_municion[i].x, caja_municion[i].y, 0.0f);
            
            // Dibujar caja de munición (cubo amarillo)
            glColor3f(1.0f, 1.0f, 0.0f);
            glutSolidCube(2.0f);
            
            glPopMatrix();
        }
    }
}

// Aqui se verifica si una bala impacta con algún obstáculo usando cálculo de distancia
// por lo que incrementa el contador de obstáculos destruidos y retorna true si hubo colisión
bool check_bullet_collision(float bullet_x, float bullet_y) {
    Obstacle* current = obstacle_system.head;
    while (current != NULL) {
        if (current->activo) {
            float dx = bullet_x - current->x;
            float dy = bullet_y - current->y;
            float distance = sqrt(dx * dx + dy * dy);
            
            if (distance < 5.0f) {
                current->activo = false;
                obstacle_system.activo_contador--;
                obstacles_destroyed++;
                printf("Obstáculos destruidos: %d\n", obstacles_destroyed);
                return true;
            }
        }
        current = current->sig;
    }
    return false;
}

// revisa si la avioneta está lo suficientemente cerca de una caja de munición
void verificar_recoleccion_municion(float avioneta_x, float avioneta_y, float radius) {
    for(int i = 0; i < MAX_CAJAS_MUNICION; i++) {
        if(caja_municion[i].activo) {
            float dx = avioneta_x - caja_municion[i].x;
            float dy = avioneta_y - caja_municion[i].y;
            float distance = sqrt(dx * dx + dy * dy);
            
            if(distance < radius + 1.0f) {  // Radio de colección
                caja_municion[i].activo = false;
                municion_colectada++;
                printf("Munición recogida: %d/10\n", municion_colectada);
                
                if(municion_colectada >= AMMO_REQUIRED) {
                    puede_disparar = true;
                    printf("¡Sistema de disparo activado!\n");
                }
            }
        }
    }
}

void disparar_bala(float avioneta_x, float avioneta_y) {
    if(!puede_disparar) return;
    
    Bala* new_bullet = (Bala*)malloc(sizeof(Bala));
    new_bullet->x = avioneta_x;
    new_bullet->y = avioneta_y;
    new_bullet->activo = true;
    new_bullet->sig = lista_balas;
    lista_balas = new_bullet;
}

// Esta función mueve las balas hacia adelante y verifica colisiones con obstáculos
// y actualiza el contador de munición y activa el sistema de disparo al alcanzar 10 unidades
void actualizar_balas(float delta_time) {
    Bala* current = lista_balas;
    Bala* prev = NULL;
    
    while(current != NULL) {
        if(current->activo) {
            current->x += 100.0f * delta_time;  // Velocidad de la bala
            
            // Verificar colisión con obstáculos
            if(check_bullet_collision(current->x, current->y)) {
                current->activo = false;  // Desactivar bala si golpea
            }
            
            // Desactivar si sale de la pantalla
            if(current->x > 100.0f) {
                current->activo = false;
            }
        }
        
        // Eliminar balas inactivas
        if(!current->activo) {
            if(prev == NULL) {
                lista_balas = current->sig;
                free(current);
                current = lista_balas;
            } else {
                prev->sig = current->sig;
                free(current);
                current = prev->sig;
            }
        } else {
            prev = current;
            current = current->sig;
        }
    }
}

// Esta función dibuja todas las balas activas en el juego usando esferas rojas pequeñas
void renderizar_balas(void) {
    Bala* current = lista_balas;
    
    glColor3f(1.0f, 0.0f, 0.0f);  // Color rojo para las balas
    while(current != NULL) {
        if(current->activo) {
            glPushMatrix();
            glTranslatef(current->x, current->y, 0.0f);
            glutSolidSphere(0.5f, 8, 8);  // Bala pequeña
            glPopMatrix();
        }
        current = current->sig;
    }
}

void limpiar_sistema_municiones(void) {
    // Liberar memoria de las balas
    while(lista_balas != NULL) {
        Bala* temp = lista_balas;
        lista_balas = lista_balas->sig;
        free(temp);
    }
}



/* ╔═══════════════════════════════════════════════════════════╗
 * ║       FUNCIONES QUE GESTIONAN AL PERSONAJE                ║
 * ╚═══════════════════════════════════════════════════════════╝ */

void inicializar_personaje(void) {
    pilot.x = 0.0f;            
    pilot.y = -18.0f;          // Nivel del suelo
    pilot.scale = 1.0f;
    pilot.walk_cycle = 0.0f;
    pilot.is_walking = false;
    pilot.has_entered = false;
    pilot.state = CHAR_HIDDEN;  // Inicia oculto hasta que despega
    pilot.target_x = -5.0f;
    pilot.enter_timer = 0.0f;
}

static void dibujar_cabeza(void) {
    glPushMatrix();
        // Color piel para la cabeza
        glColor3f(0.96f, 0.80f, 0.69f);  // Un tono de piel claro
        glutSolidSphere(CABEZA_TAMANIO * 0.8f, 16, 16);  
        
        // Ojos
        glColor3f(1.0f, 1.0f, 1.0f);  // Blanco para los ojos (why not? :p)
        glPushMatrix();
            glTranslatef(CABEZA_TAMANIO/2, CABEZA_TAMANIO/4, CABEZA_TAMANIO/2);
            glutSolidSphere(CABEZA_TAMANIO * 0.15f, 8, 8);
            // Pupila
            glColor3f(0.0f, 0.0f, 0.0f);
            glTranslatef(0.0f, 0.0f, CABEZA_TAMANIO * 0.1f);
            glutSolidSphere(CABEZA_TAMANIO * 0.05f, 8, 8);
        glPopMatrix();
        
        // Otro ojo
        glColor3f(1.0f, 1.0f, 1.0f);
        glPushMatrix();
            glTranslatef(CABEZA_TAMANIO/2, CABEZA_TAMANIO/4, -CABEZA_TAMANIO/2);
            glutSolidSphere(CABEZA_TAMANIO * 0.15f, 8, 8);
            // Pupila
            glColor3f(0.0f, 0.0f, 0.0f);
            glTranslatef(0.0f, 0.0f, -CABEZA_TAMANIO * 0.1f);
            glutSolidSphere(CABEZA_TAMANIO * 0.05f, 8, 8);
        glPopMatrix();
        
        // Nariz
        glColor3f(0.92f, 0.75f, 0.65f);  //color de la nariz un poco mas sobresaliente (casi no se aprecia igual :()
        glPushMatrix();
            glTranslatef(CABEZA_TAMANIO * 0.7f, 0.0f, 0.0f);
            glScalef(0.3f, 0.4f, 0.3f);
            glutSolidCube(CABEZA_TAMANIO);
        glPopMatrix();
        
        // Boca
        glColor3f(0.8f, 0.3f, 0.3f);  // Rojo oscuro para los labios
        glPushMatrix();
            glTranslatef(CABEZA_TAMANIO * 0.6f, -CABEZA_TAMANIO/4, 0.0f);
            glScalef(0.1f, 0.2f, 0.4f);
            glutSolidCube(CABEZA_TAMANIO);
        glPopMatrix();

        // cabellito
        glColor3f(0.2f, 0.1f, 0.0f);  // Marrón oscuro para el pelo
        glPushMatrix();
            glTranslatef(0.0f, CABEZA_TAMANIO/2, 0.0f);
            glScalef(0.9f, 0.4f, 1.0f);
            glutSolidCube(CABEZA_TAMANIO);
        glPopMatrix();
    glPopMatrix();
}

static void dibujar_cuerpo(void) {
    glPushMatrix();
        // Traje
        glColor3f(0.2f, 0.2f, 0.8f);
        glScalef(CUERPO_ANCHO, CUERPO_ALTURA, CUERPO_ANCHO);
        glutSolidCube(1.0f);
        
        // Detalles del traje
        glColor3f(0.8f, 0.8f, 0.8f);
        glTranslatef(0.0f, 0.2f, 0.0f);
        glScalef(1.1f, 0.1f, 1.1f);
        glutSolidCube(1.0f);
    glPopMatrix();
}

static void dibujar_limb(float length, float angle) {
    glPushMatrix();
        glRotatef(angle, 0.0f, 0.0f, 1.0f);
        glColor3f(0.2f, 0.2f, 0.8f);
        glScalef(0.4f, length, 0.4f);
        glutSolidCube(1.0f);
    glPopMatrix();
}

void renderizar_personaje(void) {
    if (estado_actual == STATE_PLAYING) {
        // Dibujar el personaje encima del avión
        glPushMatrix();
            // Se ajusta posición
            glTranslatef(1.0f, avioneta.y_pos + 1.5f, 0.0f);  
            glScalef(0.6f, 0.6f, 0.6f);  // aqui ajusto al personaje para hacerlo mas grande
            
            // Se inclina el personaje segun la rotación del avión
            glRotatef(avioneta.rotation, 0.0f, 0.0f, 1.0f);
            
            // se manda a llamar el dinujo de cabeza y cuerpo
            dibujar_cabeza();
            dibujar_cuerpo();
            
            // Brazos en posición de "pilotaje"
            glPushMatrix();
                glTranslatef(-CUERPO_ANCHO/2 - 0.2f, CUERPO_ALTURA/4, 0.0f);
                dibujar_limb(BRAZO_LONGITUD, -45.0f);
            glPopMatrix();
            
            glPushMatrix();
                glTranslatef(CUERPO_ANCHO/2 + 0.2f, CUERPO_ALTURA/4, 0.0f);
                dibujar_limb(BRAZO_LONGITUD, 45.0f);
            glPopMatrix();
            
            // Se coloca a las piernas sentadas para que no se sobresalgan del avion
            glPushMatrix();
                glTranslatef(-CUERPO_ANCHO/4, -CUERPO_ALTURA/2, 0.0f);
                dibujar_limb(PIERNA_LONGITUD * 0.7f, -90.0f);
            glPopMatrix();
            
            glPushMatrix();
                glTranslatef(CUERPO_ANCHO/4, -CUERPO_ALTURA/2, 0.0f);
                dibujar_limb(PIERNA_LONGITUD * 0.7f, -90.0f);
            glPopMatrix();
        glPopMatrix();
    }
}

void actualizar_personaje(float delta_time) {
   if (estado_actual == STATE_PLAYING) {

    if (pilot.state == CHAR_WALKING) {
        pilot.is_walking = true;
        pilot.walk_cycle += delta_time * 10.0f;
        if (pilot.walk_cycle > 2.0f * M_PI) {
            pilot.walk_cycle -= 2.0f * M_PI;
        }
        
        pilot.x += delta_time * 20.0f;
        
        if (pilot.x >= pilot.target_x) {
            pilot.state = CHAR_ENTERING;
            pilot.enter_timer = 0.0f;
        }
    }
    else if (pilot.state == CHAR_ENTERING) {
        pilot.is_walking = false;
        pilot.enter_timer += delta_time;
        pilot.y += delta_time * 5.0f;
        pilot.scale -= delta_time * 0.5f;
        
        if (pilot.enter_timer >= 1.0f) {
            pilot.state = CHAR_HIDDEN;
            estado_actual = STATE_DIALOG;
            dialogo_actual = mision_dialog;
        }
    }
}
}



/* ╔═══════════════════════════════════════════════════════════╗
 * ║   FUNCIONES QUE GESTIONAN LA PILA DE GUARDADO DE JUEGO    ║
 * ╚═══════════════════════════════════════════════════════════╝ */

void init_save_stack(void) {
    save_stack.top = -1;
    has_saved_game = false;   //aqui aseguro de que cuando inicialice la pila, no hay partida preguardada
    // limpiar completamente la memoria
    memset(save_stack.saves, 0, sizeof(save_stack.saves));
}

bool is_stack_empty(void) {
    return save_stack.top == -1;
}

bool is_stack_full(void) {
    return save_stack.top >= MAX_SAVES - 1;
}

bool push_save(GameState_Save save) {
    if (is_stack_full()) {
        // Si está llena, desplazar todos los elementos una posición
        for(int i = 0; i < MAX_SAVES - 1; i++) {
            save_stack.saves[i] = save_stack.saves[i + 1];
        }
        save_stack.saves[MAX_SAVES - 1] = save;
        return true;
    }
    
    save_stack.top++;
    save_stack.saves[save_stack.top] = save;
    return true;
}

bool pop_save(GameState_Save* save) {
    if (is_stack_empty()) return false;
    
    *save = save_stack.saves[save_stack.top];
    save_stack.top--;
    
    if (save_stack.top < 0) {
        has_saved_game = false;
    }
    
    return true;
}

void guardar_juego_actual(void) {
    GameState_Save nuevo_guardado = {
        .current_score = score,
        .avioneta_position = avioneta.y_pos,
        .avioneta_velocity = avioneta.y_velocity,
        .avioneta_speed = avioneta.current_speed,
        .game_activo = true,
        .obstaculos_activos = obstacle_system.activo_contador
    };
    
    // Asignar memoria para las posiciones de obstáculos
    nuevo_guardado.obstacle_positions = malloc(obstacle_system.activo_contador * 2 * sizeof(float));
    
    int pos = 0;
    Obstacle* current = obstacle_system.head;
    while (current != NULL) {
        if (current->activo) {
            nuevo_guardado.obstacle_positions[pos*2] = current->x;
            nuevo_guardado.obstacle_positions[pos*2 + 1] = current->y;
            pos++;
        }
        current = current->sig;
    }
    
    push_save(nuevo_guardado);
    has_saved_game = true;
    printf("Juego guardado exitosamente\n");
}

void cargar_juego_guardado(void) {
    if (!has_saved_game) {
        printf("No hay juegos guardados\n");
        return;
    }
    
    GameState_Save guardado_cargado;
    if (pop_save(&guardado_cargado)) {
        score = guardado_cargado.current_score;
        avioneta.y_pos = guardado_cargado.avioneta_position;
        avioneta.y_velocity = guardado_cargado.avioneta_velocity;
        avioneta.current_speed = guardado_cargado.avioneta_speed;
        
        // Limpiar obstáculos existentes
        free_todos_obstaculos();
        
        // Recrear obstáculos guardados
        for(int i = 0; i < guardado_cargado.obstaculos_activos; i++) {
            Obstacle* new_obstacle = (Obstacle*)malloc(sizeof(Obstacle));
            if (new_obstacle) {
                new_obstacle->x = guardado_cargado.obstacle_positions[i*2];
                new_obstacle->y = guardado_cargado.obstacle_positions[i*2 + 1];
                new_obstacle->width = 2.0f;
                new_obstacle->height = 8.0f;
                new_obstacle->activo = true;
                new_obstacle->sig = obstacle_system.head;
                obstacle_system.head = new_obstacle;
                obstacle_system.activo_contador++;
            }
        }
        
        free(guardado_cargado.obstacle_positions);
        printf("Juego cargado exitosamente\n");
    }
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // Se habilita RGB para redenderizado de color, tmb profundida y doble buffer para evitar parpadeo

  // Ventana principal
  glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
  int main_x = 20;  
  int main_y = 50;
  glutInitWindowPosition(main_x, main_y);
  window_main = glutCreateWindow(WINDOW_TITLE);

  init();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutSpecialFunc(special_keys);
  glutTimerFunc(0, timer, 0);
  glutKeyboardFunc(keyboard);
  glutKeyboardUpFunc(keyboard_up);

  // Ventana de diálogos
  int dialog_width = 500;
  int dialog_height = 280;
  int dialog_x = main_x + WINDOW_WIDTH + 50;  // Separacion entre la ventana de terminal de comuniq y la ventana principal
  int dialog_y = main_y;
  
  glutInitWindowSize(dialog_width, dialog_height);
  glutInitWindowPosition(dialog_x, dialog_y);
  window_dialog = glutCreateWindow("Terminal de Comunicaciones");

  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glutDisplayFunc(display_dialog_window);
  glutReshapeFunc(reshape_dialog);

  // Ventana de mission_observer
  int mission_observer_width = 500;
  int mission_observer_height = 280;
  int mission_observer_x = dialog_x;
  int mission_observer_y = dialog_y + dialog_height + 30;  // Definido justo para que este debajo de la ventana de diálogo

  glutInitWindowSize(mission_observer_width, mission_observer_height);
  glutInitWindowPosition(mission_observer_x, mission_observer_y);
  window_mission_observer = glutCreateWindow("Estado de Misiones");

  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glutDisplayFunc(display_mission_observer_window);
  glutReshapeFunc(reshape_mission_observer);

  // Configurar orden de ventanas
  glutSetWindow(window_dialog);
  glutPushWindow();
  glutSetWindow(window_mission_observer);
  glutPushWindow();
  glutSetWindow(window_main);
  glutPushWindow();

  printf("Iniciando juego...\n");
  glutMainLoop();
  
  cleanup();
  return 0;
}
