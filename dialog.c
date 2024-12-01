#include "dialog.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "menu.h"

DialogNode* mission_dialog;
DialogNode* current_dialog;
MissionStatus mission_status;

// Función para crear un nuevo nodo de diálogo
DialogNode* create_dialog_node(const char* speaker, const char* text, 
                             const char* left_choice, const char* right_choice,
                             MissionPhase phase) {
    DialogNode* node = (DialogNode*)malloc(sizeof(DialogNode));
    node->speaker = strdup(speaker);
    node->text = strdup(text);
    node->left_choice = left_choice ? strdup(left_choice) : NULL;
    node->right_choice = right_choice ? strdup(right_choice) : NULL;
    node->left = NULL;
    node->right = NULL;
    node->action = NULL;
    node->phase = phase;
    return node;
}

// Acciones de la misión
void action_start_mission(void) {
    mission_status.current_phase = PHASE_INFILTRATION;
    printf("Iniciando infiltracion...\n");
    printf("Fase actual: %d\n", mission_status.current_phase);
    //printf("Estado del juego: %d\n", current_state);
}

void action_start_approach(void) {
    mission_status.current_phase = PHASE_APPROACH;
    printf("Iniciando aproximacion al objetivo...\n");
}

void action_start_planting(void) {
    mission_status.current_phase = PHASE_PLANT_EXPLOSIVES;
    printf("Preparando colocación de explosivos...\n");
}

void action_start_escape(void) {
    mission_status.current_phase = PHASE_ESCAPE;
    mission_status.time_remaining = 60; // 60 segundos para escapar
    printf("¡Explosivos colocados! ¡Inicie escape!\n");
}

// Inicializar diálogos
void init_dialogs(void) {
    // Inicializar estado de la misión
    mission_status.target_x = 1000.0f;
    mission_status.target_y = 0.0f;
    mission_status.distance_to_target = 1000.0f;
    mission_status.defenses_alerted = false;
    mission_status.time_remaining = 0;
    mission_status.explosives_planted = false;
    mission_status.escaping = false;
    mission_status.current_phase = PHASE_BRIEFING;

    // Nodo inicial
    mission_dialog = create_dialog_node(
        "Comandante Eagle",
        "¡Atencion piloto! Hemos identificado un almacen de \narmas enemigo. Su mision: infiltrarse y destruirlo. \nEs vital para la operacion.",
        "Entendido, comandante",
        "Detalles de la mision",
        PHASE_BRIEFING
    );

    // Nodo para opción 1 (modificado)
    DialogNode* start_node = create_dialog_node(
        "Comandante Eagle",
        "Excelente actitud, piloto. Las defensas estan activas.\nUse el boost para atravesar zonas de alto riesgo.",
        "Iniciar infiltracion",  // Esta será la última opción
        NULL,
        PHASE_BRIEFING
    );
    start_node->action = action_start_mission;

    // Nodo final para la opción 1
    DialogNode* final_node = create_dialog_node(
        "Comandante Eagle",
        "¡Buena suerte, piloto! Comienza la infiltracion.",
        NULL,  // Sin más opciones
        NULL,
        PHASE_INFILTRATION
    );
    start_node->left = final_node;  // Conectar el nodo final
    mission_dialog->left = start_node;  // Conectar al árbol principal

    // Rama de detalles
    DialogNode* details_node = create_dialog_node(
        "Oficial de Inteligencia",
        "Defensas identificadas:\n- Torres anti-aereas\n- Sistemas de radar\n- Campos de minas\nEvite deteccion a toda costa.",
        "Entendido, proceder",
        "Mas informacion",
        PHASE_BRIEFING
    );
    mission_dialog->right = details_node;

    // Información adicional
    DialogNode* extra_details = create_dialog_node(
        "Oficial de Inteligencia",
        "Datos adicionales:\n- Objetivo a 1000m\n- Defensas automatizadas\n- Sin apoyo aéreo disponible",
        "Iniciar mision",
        "Ruta de escape",
        PHASE_BRIEFING
    );
    details_node->right = extra_details;

    // Información de escape
    DialogNode* escape_info = create_dialog_node(
        "Oficial de Operaciones",
        "Post-detonacion: 60 segundos para evacuar.\nRuta de escape estara marcada.\nUse el boost estrategicamente.",
        "Entendido, iniciando operacion",
        NULL,
        PHASE_BRIEFING
    );
    extra_details->right = escape_info;

    // Conectar todos los endpoints al inicio de la misión
    details_node->left = start_node;
    extra_details->left = start_node;
    escape_info->left = start_node;

    current_dialog = mission_dialog;
}

// Renderizar diálogo actual
void render_dialog(DialogNode* node) {
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
    if (node->left_choice) {
        glColor4f(0.0f, 0.8f, 0.4f, 1.0f);  // Verde brillante
        glRasterPos2f(-0.90f, -0.75f);
        text = "1: ";
        while (*text) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++);
        
        glColor4f(0.7f, 1.0f, 0.7f, 1.0f);  // Verde más claro
        text = node->left_choice;
        while (*text) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++);
    }

    if (node->right_choice) {
        glColor4f(0.0f, 0.8f, 0.4f, 1.0f);  // Verde brillante
        glRasterPos2f(-0.90f, -0.90f);
        text = "2: ";
        while (*text) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++);
        
        glColor4f(0.7f, 1.0f, 0.7f, 1.0f);  // Verde más claro
        text = node->right_choice;
        while (*text) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++);
    }

    glDisable(GL_BLEND);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void show_game_over_dialog(void) {
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
    if (!current_dialog) return;

    DialogNode* next_node = NULL;

    switch(key) {
        case '1':
            if (current_dialog->left) {
                if (current_dialog->action) {
                    current_dialog->action();
                }
                next_node = current_dialog->left;
            }
            break;

        case '2':
            if (current_dialog->right) {
                if (current_dialog->action) {
                    current_dialog->action();
                }
                next_node = current_dialog->right;
            }
            break;
    }

    // Si hay un siguiente nodo, avanzamos a él
    if (next_node) {
        current_dialog = next_node;
    }
    // Si no hay siguiente nodo, significa que es el final del diálogo
    else if (current_dialog && (current_dialog->left == NULL && current_dialog->right == NULL)) {
        // Cambiar al estado de juego
        current_state = STATE_PLAYING;
        // Limpiar el diálogo actual
        current_dialog = NULL;
        printf("Transicion a estado de juego\n"); // Debug
    }
}

// Actualizar estado de la misión
void update_mission_status(void) {
    if (mission_status.current_phase == PHASE_ESCAPE && mission_status.time_remaining > 0) {
        mission_status.time_remaining--;
    }
}

// Renderizar información de la misión
void render_mission_info(void) {
    if (mission_status.current_phase == PHASE_BRIEFING) return;

    char info[128];
    sprintf(info, "Distancia al objetivo: %.0fm", mission_status.distance_to_target);
    
    if (mission_status.current_phase == PHASE_ESCAPE) {
        sprintf(info, "¡ESCAPE! Tiempo restante: %ds", mission_status.time_remaining);
    }

    // TODO: Renderizar información en pantalla
}

// Liberar memoria
void cleanup_dialogs(void) {
    // TODO: Implementar liberación recursiva del árbol de diálogos
}