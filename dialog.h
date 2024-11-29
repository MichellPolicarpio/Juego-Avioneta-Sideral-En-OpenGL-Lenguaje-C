#ifndef DIALOG_H
#define DIALOG_H

#include "config.h"
#include <stdbool.h>
#include "menu.h"

// Fases de la misión
typedef enum {
    PHASE_BRIEFING,
    PHASE_INFILTRATION,
    PHASE_APPROACH,
    PHASE_PLANT_EXPLOSIVES,
    PHASE_ESCAPE,
    PHASE_COMPLETE
} MissionPhase;

// Estado de la misión
typedef struct {
    float target_x, target_y;     // Posición del almacén
    float distance_to_target;     // Distancia actual al objetivo
    bool defenses_alerted;        // Si las defensas están en alerta
    int time_remaining;           // Tiempo para completar la misión
    bool explosives_planted;      // Si se colocaron los explosivos
    bool escaping;               // Si estamos en fase de escape
    MissionPhase current_phase;   // Fase actual de la misión
} MissionStatus;

// Estructura para un nodo del árbol de diálogo
typedef struct DialogNode {
    char* text;                    // Texto del diálogo
    char* speaker;                 // Nombre de quien habla
    struct DialogNode* left;       // Opción izquierda
    char* left_choice;            // Texto de la opción izquierda
    struct DialogNode* right;      // Opción derecha
    char* right_choice;           // Texto de la opción derecha
    void (*action)(void);         // Función a ejecutar al llegar a este nodo
    MissionPhase phase;           // Fase asociada con este diálogo
} DialogNode;

// Variables externas
extern DialogNode* mission_dialog;
extern DialogNode* current_dialog;
extern MissionStatus mission_status;

// Funciones principales
void init_dialogs(void);
void render_dialog(DialogNode* node);
void handle_dialog_input(unsigned char key);
void cleanup_dialogs(void);

// Funciones de la misión
void start_mission(void);
void update_mission_status(void);
void render_mission_info(void);
void trigger_mission_phase(MissionPhase phase);

#endif // DIALOG_H