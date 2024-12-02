/*COMO COMPILARLO:
ABRIR TERMINAL: 
cd /archivos/Proyecto/src/ 
coloca: gcc -Wall -framework OpenGL -framework GLUT menu.c player.c obstacle.c main.c dialog.c -o juego
*/

#include "config.h"
#include "menu.h"
#include "player.h"
#include "obstacle.h" 
#include <stdio.h>
#include <stdbool.h>
#include <math.h> 
#include "dialog.h" 
#include "stb_image.h"

// Y agregar la variable global para la textura si no la tienes:
GLuint backgroundTexture; 

// Constantes
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_TITLE "MI VIDEOJUEGO"
#define FPS 60
#define POINTS_PER_SECOND 100  // Aumentado de 10 a 100
#define POINTS_FOR_BOOST 50    // Aumentado de 5 a 50

// Variables globales
int window_id;
GameState current_state = STATE_MENU;
bool running = true;
float delta_time = 0.0f;
int last_time = 0;
int score = 0; 

// Con las variables globales
int high_score = 0;

//int window_extra;   // ID de la ventana extra

// Variables globales
int window_main;    // Ventana principal
int window_dialog;  // Ventana de diálogos

// Prototipos de funciones
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
void display_extra(void);
void display_dialog_window(void);
void reshape_dialog(int w, int h);
void return_to_menu(int value);

// Función de inicialización
void init(void) {
    init_dialogs();  // Añadir esta línea
    init_obstacles();

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
    last_time = glutGet(GLUT_ELAPSED_TIME);
    
    // Inicializar menú y jugador
    init_menu();
    init_player();
}

// Función de actualización
void update(void) {
    int current_time = glutGet(GLUT_ELAPSED_TIME);
    delta_time = (current_time - last_time) / 1000.0f;
    last_time = current_time;

    switch(current_state) {
        case STATE_MENU:
            update_menu();
            break;
            
        case STATE_PLAYING:
            update_player(delta_time);
            update_obstacles(delta_time);
            
            score += (int)(delta_time * POINTS_PER_SECOND);
            if (player.current_speed > player.normal_speed) {
                score += (int)(delta_time * POINTS_FOR_BOOST);
            }
            
            if (check_collision(0.0f, player.y_pos, 2.0f)) {
            printf("¡Colisión detectada! Puntuación final: %d\n", score);
            if (score > high_score) {
                high_score = score;
            }
            current_state = STATE_GAME_OVER;
            current_dialog = NULL;  // Limpiar diálogo actual
            //glutTimerFunc(5000, return_to_menu, 0);
        }
            break;
            
        case STATE_PAUSED:
            break;

        case STATE_GAME_OVER:
            // No actualizar nada en game over
            break;
        
        case STATE_DIALOG:
            update_mission_status();
            break;
    }
}

void return_to_menu(int value) {
    current_state = STATE_MENU;
    score = 0;
    init_player();
    init_obstacles();
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

    render_obstacles();
    render_player();

    // Configurar vista para HUD
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Renderizar puntuación
    // En la parte donde se renderiza el score
    glColor3f(1.0f, 1.0f, 1.0f);
    char score_text[32];
    sprintf(score_text, "Score: %d  High Score: %d", score, high_score);
    glRasterPos2f(-0.9f, 0.85f);  // Ajustado ligeramente la posición
    
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
    switch(current_state) {
        case STATE_MENU:
            render_menu();
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
    switch(current_state) {
        case STATE_MENU:
            menu_handle_input(key);
            break;
            
        case STATE_PLAYING:
            if(key == 27) { // ESC
                current_state = STATE_PAUSED;
            } else {
                handle_player_input(key);
            }
            break;
            
        case STATE_PAUSED:
            if (key == 27) { // ESC
                current_state = STATE_PLAYING;
            }
            break;

        case STATE_GAME_OVER:
        if (key == 13) {  // ENTER
            current_state = STATE_MENU;
            score = 0;
            init_player();
            init_obstacles();
        }
        break;
            break;

        case STATE_DIALOG:
            handle_dialog_input(key);
            break;
    }
    
    glutPostRedisplay();
}

// Función de teclas especiales
void special_keys(int key, int x, int y) {
    switch(current_state) {
        case STATE_MENU:
            menu_handle_special_keys(key);
            break;
            
        case STATE_PLAYING:
            switch(key) {
                case GLUT_KEY_UP:
                    handle_player_input('w');
                    break;
                case GLUT_KEY_DOWN:
                    handle_player_input('s');
                    break;
            }
            break;
            
        case STATE_PAUSED:
            break;

        case STATE_GAME_OVER:
            break;
        
        case STATE_DIALOG:
            break;  // No hay teclas especiales en el diálogo
    }
    
    glutPostRedisplay();
}

// Función de timer para FPS constantes
void timer(int value) {
    update();
    glutSetWindow(window_main);
    glutPostRedisplay();
    glutSetWindow(window_dialog);
    glutPostRedisplay();
    glutTimerFunc(1000/FPS, timer, 0);
}

// Función de limpieza
void cleanup(void) {
    printf("Limpiando recursos...\n");
    // TODO: Liberar recursos
}

// Implementar la función
void keyboard_up(unsigned char key, int x, int y) {
    if(current_state == STATE_PLAYING) {
        handle_player_key_up(key);
    }
    glutPostRedisplay();
}

void display_extra(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    // Aquí puedes poner lo que quieras mostrar en la ventana extra
    // Por ejemplo, una vista diferente del juego, estadísticas, etc.
    
    glutSwapBuffers();
}

// Función para la ventana de diálogos
void display_dialog_window(void) {
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glLoadIdentity();
   
   if (current_state == STATE_GAME_OVER) {
       show_game_over_dialog();
   }
   else if (current_dialog) {
       render_dialog(current_dialog);
   }
   else if (current_state == STATE_MENU) {
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
           "OBJETIVO: Infiltracion y destruccion",
           "",
           "INSTRUCCIONES TACTICAS:",
           "- Evitar colision por fuerzas enemigas",
           "- Mantener altitud optima",
           "- Usar impulso en situaciones optimas",
           "",
           "CONTROLES OPERATIVOS:",
           "W/S o FLECHAS: Control de altitud",
           "ESPACIO: Propulsor de velocidad del avion",
           "ESC: Pausa del juego",
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

int main(int argc, char** argv) {
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

   // Ventana principal
   glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
   int main_x = 20;  // Más a la izquierda
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

   // Ventana de diálogos más separada
   int dialog_width = 500;
   int dialog_height = 280;
   int dialog_x = main_x + WINDOW_WIDTH + 50;  // 50 píxeles más de separación
   int dialog_y = main_y;
   
   glutInitWindowSize(dialog_width, dialog_height);
   glutInitWindowPosition(dialog_x, dialog_y);
   window_dialog = glutCreateWindow("Terminal de Comunicaciones");

   glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
   glutDisplayFunc(display_dialog_window);
   glutReshapeFunc(reshape_dialog);

    glutSetWindow(window_dialog);  // Primero seleccionar la de diálogo
    glutPushWindow();             // Ponerla al frente temporalmente
    glutSetWindow(window_main);    // Luego seleccionar la principal
    glutPushWindow();             // Y ponerla al frente definitivamente

   printf("Iniciando juego...\n");
   glutMainLoop();
   
   cleanup();
   return 0;
}
