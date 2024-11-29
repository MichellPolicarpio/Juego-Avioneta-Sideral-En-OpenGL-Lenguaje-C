#include "menu.h"
#include <stdio.h>
#include <stdlib.h>
#include "dialog.h"

// Definición de variables globales del menú
Menu game_menu;
const char* menu_options[4] = {
    "Nuevo Juego",
    "Cargar Juego",
    "Opciones",
    "Salir"
};

// Inicializar menú
void init_menu(void) {
    game_menu.selected_option = MENU_NEW_GAME;
    game_menu.is_active = true;
    game_menu.title_pos_y = 0.5f;
    game_menu.first_option_y = 0.2f;
    game_menu.option_spacing = 0.15f;
}

// Actualizar menú
void update_menu(void) {
    // aqui anicomaciones o actualizaciones depsues
}

void render_menu(void) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    // Título del juego
    glColor3f(0.0f, 0.7f, 1.0f); // Azul claro para título
    float title_length = (float)glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)"JUEGO DE LA AVIONETA SIDERAL");
    float title_scale = 1000.0f;
    float title_x = -title_length / title_scale;
    glRasterPos2f(title_x, game_menu.title_pos_y);
    const char* title = "JUEGO DE LA AVIONETA SIDERAL";
    while (*title) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *title++);
    }
    
    // Renderizar todas las opciones del menú
    for (int i = 0; i < MENU_OPTION_COUNT; i++) {
        float y = game_menu.first_option_y - ((float)i * game_menu.option_spacing);
        bool is_selected = (i == (int)game_menu.selected_option);
        
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
        float text_length = (float)glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)menu_options[i]);
        float scale = 1000.0f;
        float x = -text_length / scale;
        glRasterPos2f(x, y);
        
        const char* text = menu_options[i];
        while (*text) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++);
        }
    }
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

// Manejar input del teclado normal
void menu_handle_input(unsigned char key) {
    if (key == 13) { // ENTER
        switch(game_menu.selected_option) {
            case MENU_NEW_GAME:
                current_dialog = mission_dialog;  // Iniciar el diálogo
                current_state = STATE_DIALOG;     // Cambiar al estado de diálogo
                break;
                
            case MENU_LOAD_GAME:
                printf("Cargando juego...\n");
                // TODO: Implementar carga de juego
                break;
                
            case MENU_OPTIONS:
                printf("Abriendo opciones...\n");
                // TODO: Implementar menú de opciones
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

// Manejar teclas especiales
void menu_handle_special_keys(int key) {
    switch(key) {
        case GLUT_KEY_UP:
            if (game_menu.selected_option > 0) {
                game_menu.selected_option--;
                glutPostRedisplay();
            }
            break;
            
        case GLUT_KEY_DOWN:
            if (game_menu.selected_option < MENU_OPTION_COUNT - 1) { // Ajustado para incluir la última opción
                game_menu.selected_option++;
                glutPostRedisplay();
            }
            break;
    }
}