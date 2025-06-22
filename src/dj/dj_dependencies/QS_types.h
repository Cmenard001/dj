/*
 *  Club Robot ESEO 2006 - 2010
 *  Game Hoover, Phoboss, Archi-Tech', PACMAN
 *
 *  Fichier : QS_types.h
 *  Package : Qualité Soft
 *  Description : Définition des types pour tout code du robot
 *  Auteur : Axel Voitier (et un peu savon aussi)
 *	Revision 2008-2009 : Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *  Version 20100415
 *
 */

#ifndef QS_TYPES_H
#define QS_TYPES_H

/* Inclusions des entiers standards et du booléen standard */
#include <stdbool.h>
#include <stdint.h>

/* Type pour les SID du protocole CAN */
typedef uint16_t CAN_sid11_t;

typedef uint32_t time32_t;

/*
 * Permet de faire hériter plusieurs structure contenant un x y de manière
 * à simplement utiliser des fonctions comme "sqrt" sur des structure de type
 * différents
 */
#define ABSTRACT_POINT_2D()                                                                                                 \
    float x;                                                                                                                \
    float y

/**
 * @brief Structure très simple contenant un x y et voilà
 */
typedef struct
{
    ABSTRACT_POINT_2D();
} point_2d_t;

typedef struct
{
    ABSTRACT_POINT_2D();
} vector_2d_t;

typedef struct
{
    ABSTRACT_POINT_2D();
    float angle;
} posf_t;

typedef struct
{
    int16_t angle;
    uint16_t norme;
} vector_2d_polar_t;

typedef void (*callback_fun_t)(void); // Type pointeur sur fonction simple.

////////////////////////////////////////////////////////////////
// Familles d'énum... afin que ces enums soient non recouvrables.
#define MASK_ENUM_MEMBER 0x00FF
#define MASK_ENUM_FAMILY 0xFF00
typedef enum
{
    ENUM_FAMILY_AXIS = 0x0000,
    ENUM_FAMILY_WAY = 0x0100,
    ENUM_FAMILY_AVOIDANCE = 0x0200,
    ENUM_FAMILY_SIDE = 0x0300,
    ENUM_FAMILY_STRAT_END_CONDITION = 0x2000
} enum_family_e;
////////////////////////////////////////////////////////////////

#define BOT_COLOR_NAME "YELLOW"
#define TOP_COLOR_NAME "BLUE"

typedef enum
{
    BOT_COLOR = 0,
    YELLOW = 0,
    TOP_COLOR = 1,
    BLUE = 1,
} color_e;

typedef enum
{
    SIDE_LEFT = ENUM_FAMILY_SIDE,
    SIDE_RIGHT
} side_e;

// Attention, pour toute modification de cette liste, il faut renseigner les paramètres dans la structure telemeter_params_t
typedef enum
{
    /* Lasers de BIG */
    LASER_TELEMETER_BIG_FIRST = 0x00,
    LASER_TELEMETER_XUYPCO925L3ANSP_BIG_LEFT = LASER_TELEMETER_BIG_FIRST, // 80mm à 300mm - précision 200um
    LASER_TELEMETER_XUYPCO925L3ANSP_BIG_RIGHT,                            // 80mm à 300mm - précision 200um
    LASER_TELEMETER_BIG_LAST = LASER_TELEMETER_XUYPCO925L3ANSP_BIG_RIGHT,

    /* Lasers de SMALL */
    LASER_TELEMETER_SMALL_FIRST,
    LASER_TELEMETER_XUK8TAE2MM12_SMALL_LEFT = LASER_TELEMETER_SMALL_FIRST, // 80mm à 300mm - précision 200um
    LASER_TELEMETER_XUK8TAE2MM12_SMALL_RIGHT,                              // 80mm à 300mm - précision 200um
    LASER_TELEMETER_SMALL_LAST = LASER_TELEMETER_XUK8TAE2MM12_SMALL_RIGHT,

    /* Lasers de NORTH */
    LASER_TELEMETER_XUK8TAE2MM12_NORTH_LEFT,  // 100mm à 5000mm - précision 5mm
    LASER_TELEMETER_XUK8TAE2MM12_NORTH_RIGHT, // 100mm à 5000mm - précision 5mm

    /* Lasers de BIG */
    LASER_TELEMETER_XUK8TAE2MM12_SOUTH_LEFT,  // 100mm à 5000mm - précision 5mm
    LASER_TELEMETER_XUK8TAE2MM12_SOUTH_RIGHT, // 100mm à 5000mm - précision 5mm

    /* Nombre de laser */
    LASER_TELEMETER_NB,
    LASER_BIG_COUNT = LASER_TELEMETER_BIG_LAST - LASER_TELEMETER_BIG_FIRST + 1,
    LASER_SMALL_COUNT = LASER_TELEMETER_SMALL_LAST - LASER_TELEMETER_SMALL_FIRST + 1,
    /* Nombre de laser max par robot, à faire manuellement pour l'instant .. */
    LASER_TELEMETER_NB_MAX = 2
} laser_telemeter_e;

/* Configuration d'un laser dans un robot */
typedef struct
{
    int32_t offset_length; //(y) distance du point de départ du laser en largeur algébrique (perpendiculairement au
                           //déplacement du robot)
    int32_t offset_width; //(x) distance du point de départ du laser en longueur algébrique (dans le sens du déplacement du
                          //robot)
    int32_t offset_angle; //(a) angle du faisceau laser par rapport à l'axe des roues codeuses en rad/4096
} telemeter_params_t;

/*
 * @brief Structure de position, x, y, thêta
 */
typedef struct
{
    int16_t x;     /* mm */
    int16_t y;     /* mm */
    int16_t theta; /* rad/pi4096 */
} pos_t;

typedef enum
{
    CAN_SIDE_LEFT = SIDE_LEFT & MASK_ENUM_MEMBER,
    CAN_SIDE_RIGHT = SIDE_RIGHT & MASK_ENUM_MEMBER
} CAN_side_e;

typedef enum
{
    AXIS_X = 0,
    AXIS_Y = 1
} axis_e;

typedef enum
{
    SLOT_PROP = 0,
    SLOT_ACT,
    SLOT_STRAT,
    SLOT_INCONNU
} slot_id_e;

typedef enum
{
    CODE_PROP = 0,
    CODE_ACT,
    CODE_STRAT,
    CODE_BALISE
} code_id_e;

typedef enum
{
    ACT_EXPANDER_ID_00,
    ACT_EXPANDER_ID_01,
    ACT_EXPANDER_ID_10,
    ACT_EXPANDER_ID_11,
    ACT_EXPANDER_ID_ERROR = 0xFF,
    // Redefinition des id
    // ACT_EXPANDER_BIG_EXAMPLE = ACT_EXPANDER_ID_10,
    //		ACT_EXPANDER_BIG_FORWARD = ACT_EXPANDER_ID_01,
    //		ACT_EXPANDER_BIG_BACKWARD = ACT_EXPANDER_ID_10,
    //		ACT_EXPANDER_SMALL = ACT_EXPANDER_ID_01,
} actExpanderId_e;

//	typedef enum
//	{
//		BEACON_ID_MOTHER = 0,
//		BEACON_ID_CORNER = 1,
//		BEACON_ID_MIDLE = 2,
//		BEACON_ID_ROBOT_1 = 3,
//		BEACON_ID_ROBOT_2 = 4,
//		BEACONS_NUMBER
//	} beacon_id_e;

typedef enum
{
    RF_SMALL,
    RF_BIG,
    RF_FOE1,
    RF_FOE2,
    RF_BROADCAST = 7 // pourquoi a 7 ?TODO
} RF_module_e;

/**********************************************************************************************************************
 **********************************************************************************************************************
 **											Types associés à la stratégie											 **
 **********************************************************************************************************************
 *********************************************************************************************************************/

typedef enum
{
    BUZZER_DEFAULT_NOTE = 0x0, // DO : c'est la note qui fait le plus de bruit (le buzzer crache 90dB à 10cm, 4,2kHz, 3V)
    BUZZER_NOTE_DO0 = 0x1,
    BUZZER_NOTE_RE0,
    BUZZER_NOTE_MI0,
    BUZZER_NOTE_FA,
    BUZZER_NOTE_SOL,
    BUZZER_NOTE_LA,
    BUZZER_NOTE_SI,
    BUZZER_NOTE_DO,
    BUZZER_NOTE_RE,
    BUZZER_NOTE_MI
} buzzer_play_note_e;

typedef enum
{
    PROP_MEMORY_STARTUP_CHECK__UNKNOW = 0,
    PROP_MEMORY_STARTUP_CHECK__GOOD,
    PROP_MEMORY_STARTUP_CHECK__FAIL_NOT_PRESENT,
    PROP_MEMORY_STARTUP_CHECK__FAIL_NO_INITIALIZED
} prop_memory_startup_check_e;

typedef enum
{
    END_OK = 0,
    IN_PROGRESS,
    END_WITH_TIMEOUT,
    NOT_HANDLED,
    FOE_IN_PATH
} error_e;

/**
 * @brief Cette énumeration permet de savoir dans quelle direction le robot regarde lors d'un scan.
 */
typedef enum
{
    SCAN_VIEW_NORTH = 0, // Le robot regarde vers le nord, robot.x est > à observed.x
    SCAN_VIEW_SOUTH,     // Le robot regarde vers le sud, robot.x est < à observed.x
    SCAN_VIEW_EAST,      // Le robot regarde vers l'est, robot.y est < à observed.y
    SCAN_VIEW_WEST       // Le robot regarde vers l'ouest, robot.y est > à observed.y
} scan_view_e;

typedef enum
{
    // Flag synchro entre les deux robots

    F_ELEMENTS_FLAGS_END_SYNCH, // Les flags au-dessus seront synchro entre les deux robots

    // Flags pour savoir si la comm passe entre les deux robots
    // Ce flag est envoyé à intervalle de temps régulier
    // Ne pas mettre ce flag dans la partie synchro, ce flag est synchro par une machine à état spécifique
    F_COMMUNICATION_AVAILABLE,

    // Début des hardflags
    F_ELEMENTS_HARDFLAGS_START,

    // Fin des hardflags
    F_ELEMENTS_HARDFLAGS_END,

    // Eléments pris (non synchro)
    //  Flag indiquant si un stock de plantes est pris
    F_PLANT_SUPPLY_1_TAKEN,
    F_PLANT_SUPPLY_2_TAKEN,
    F_PLANT_SUPPLY_3_TAKEN,
    F_PLANT_SUPPLY_4_TAKEN,
    F_PLANT_SUPPLY_5_TAKEN,
    F_PLANT_SUPPLY_6_TAKEN,
    // Flag indiquant si un stock de pots est pris
    F_POT_OUR_SUPPLY_1_TAKEN,
    F_POT_OUR_SUPPLY_2_TAKEN,
    F_POT_OUR_SUPPLY_3_TAKEN,
    F_POT_ADV_SUPPLY_1_TAKEN,
    F_POT_ADV_SUPPLY_2_TAKEN,
    F_POT_ADV_SUPPLY_3_TAKEN,

    F_ELEMENTS_FLAGS_NB

} elements_flags_e;

/**********************************************************************************************************************
 **********************************************************************************************************************
 **											Types associés à la propulsion											 **
 **********************************************************************************************************************
 *********************************************************************************************************************/

/*sens de trajectoire - utilisé dans le code propulsion et partagé pour la stratégie... */

typedef enum
{
    ANY_WAY = ENUM_FAMILY_WAY,

    // Translation
    BACKWARD,
    FORWARD,
    BACKWARD_PREFERRED,
    FORWARD_PREFERRED,

    // Rotation
    CLOCKWISE = BACKWARD,
    TRIGOWISE = FORWARD
} way_e;

typedef enum
{
    CAN_ANY_WAY = ANY_WAY & MASK_ENUM_MEMBER,
    CAN_BACKWARD = BACKWARD & MASK_ENUM_MEMBER,
    CAN_FORWARD = FORWARD & MASK_ENUM_MEMBER,
    CAN_BACKWARD_PREFERRED = BACKWARD_PREFERRED & MASK_ENUM_MEMBER,
    CAN_FORWARD_PREFERRED = FORWARD_PREFERRED & MASK_ENUM_MEMBER
} CAN_way_e;

/*état de la carte propulsion - utilisé dans le code propulsion et partagé pour la stratégie... */
typedef enum
{
    NO_ERROR = 0,
    UNABLE_TO_GO_ERROR_TRANSLATION,
    UNABLE_TO_GO_ERROR_ROTATION,
    IMMOBILITY_ERROR,
    ROUNDS_RETURNS_ERROR,
    NOT_ARRIVED_ON_THE_DESTINATION_POINT_ERROR,
    SHOCK_ERROR,
    UNKNOW_ERROR
} SUPERVISOR_error_source_e;

/*type de trajectoire - utilisé dans le code propulsion et partagé pour la stratégie... */
typedef enum
{
    TRAJECTORY_TRANSLATION = 0,
    TRAJECTORY_ROTATION,
    TRAJECTORY_STOP,
    TRAJECTORY_AUTOMATIC_CURVE,
    TRAJECTORY_NONE,
    TRAJECTORY_SPLINE,
    TRAJECTORY_CLOTOIDE,
    WAIT_FOREVER
} trajectory_e;

/*type d'évitement - utilisé dans le code propulsion et partagé pour la stratégie... */
typedef enum
{
    AVOIDANCE_DISABLED = ENUM_FAMILY_AVOIDANCE,
    AVOIDANCE_WITHOUT_WAIT,                 // aucune attente, aucune esquive, on détecte, on NOT_HANDLED !
    AVOIDANCE_WITH_WAIT,                    // attente normale, puis NOT_HANDLED
    AVOIDANCE_WITH_WAIT_WITHOUT_EXTRACTION, // attente normale mais sans extraction
    AVOIDANCE_DODGE_WITH_WAIT,              // tentative d'évitement immédiate, puis attente
    AVOIDANCE_DODGE_WITHOUT_WAIT,           // tentative d'évitement immédiate, puis NOT_HANDLED
} avoidance_e;

/*type d'évitement - utilisé dans le code propulsion et partagé pour la stratégie... */
typedef enum
{
    CAN_AVOIDANCE_DISABLED = AVOIDANCE_DISABLED & MASK_ENUM_MEMBER,
    CAN_AVOIDANCE_ENABLED = AVOIDANCE_WITHOUT_WAIT & MASK_ENUM_MEMBER,
    CAN_AVOIDANCE_ENABLED_AND_WAIT = AVOIDANCE_WITH_WAIT & MASK_ENUM_MEMBER
    // les autres cas de figures de l'enum avoidance_e restent en strat... (pas de transport de l'info de DODGE via le CAN)
} CAN_avoidance_e;

typedef enum
{
    FAST = 0,
    SLOW,
    SLOW_TRANSLATION_AND_FAST_ROTATION,
    FAST_TRANSLATION_AND_SLOW_ROTATION,
    EXTREMELY_VERY_SLOW,
    CUSTOM, // Les valeurs suivantes sont également valables (jusqu'à 4095... et indiquent un choix de vitesse personnalisé
            // !)
    MAX_SPEED = 0x0FFF,

    SPEED_SLOW = SLOW, // noms équivalents utiles trouvables facilement en commençant par SPEED.
    SPEED_FAST = FAST,
} PROP_speed_e;

typedef enum
{
    PROP_NO_BORDER_MODE = 0,
    PROP_BORDER_MODE
} prop_border_mode_e;

typedef enum
{
    PROP_NO_CURVE = 0,
    PROP_CURVE
} prop_curve_e;

typedef enum
{
    PROP_END_AT_POINT = 0,
    PROP_END_AT_BRAKE
} propEndCondition_e;

typedef enum
{
    PROP_ABSOLUTE = 0,
    PROP_RELATIVE
} prop_referential_e;

typedef enum
{
    PROP_NOW = 0,
    PROP_END_OF_BUFFER
} prop_buffer_mode_e;

typedef enum
{
    PROP_NO_ACKNOWLEDGE = 0,
    PROP_ACKNOWLEDGE
} prop_acknowledge_e;

typedef enum
{
    PROP_SET_POS_NONE = 0b000,

    PROP_SET_POS_X_ONLY = 0b001,
    PROP_SET_POS_Y_ONLY = 0b010,
    PROP_SET_POS_TETA_ONLY = 0b100,

    PROP_SET_POS_X_AND_Y = 0b011,
    PROP_SET_POS_X_AND_TETA = 0b101,
    PROP_SET_POS_Y_AND_TETA = 0b110,

    PROP_SET_POS_X_Y_AND_TETA = 0b111,
} prop_set_pos_e;

typedef enum
{
    WARNING_NO = (0b00000000),
    WARNING_TIMER = (0b00000010),
    WARNING_TRANSLATION = (0b00000100),
    WARNING_ROTATION = (0b00001000),
    WARNING_REACH_X = (0b00010000),
    WARNING_REACH_Y = (0b00100000),
    WARNING_REACH_TETA = (0b01000000)
} prop_warning_reason_e;

typedef enum
{
    ODOMETRY_COEF_TRANSLATION = 0,
    ODOMETRY_COEF_SYM,             // 1
    ODOMETRY_COEF_ROTATION,        // 2
    ODOMETRY_COEF_CENTRIFUGAL,     // 3		//attention, la valeur de ODOMETRY_COEF_CENTRIFUGAL est utilisé comme borne dans le
                                   // code de propulsion, il faut le laisser en dernier dans les coefs d'odométrie !
    CORRECTOR_COEF_KP_TRANSLATION, // 4
    CORRECTOR_COEF_KD_TRANSLATION, // 5
    CORRECTOR_COEF_KV_TRANSLATION, // 6
    CORRECTOR_COEF_KA_TRANSLATION, // 7
    CORRECTOR_COEF_KP_ROTATION,    // 8
    CORRECTOR_COEF_KI_ROTATION,    // 9
    CORRECTOR_COEF_KD_ROTATION,    // 10
    CORRECTOR_COEF_KV_ROTATION,    // 11
    CORRECTOR_COEF_KA_ROTATION,    // 12
    CORRECTOR_COEF_KP_ROTATION_PURE, // 13
    CORRECTOR_COEF_KI_ROTATION_PURE, // 14
    CORRECTOR_COEF_KD_ROTATION_PURE, // 15
    CORRECTOR_COEF_KV_ROTATION_PURE, // 16
    CORRECTOR_COEF_KA_ROTATION_PURE, // 17
    CORRECTOR_COEF_BALANCING,        // 18
    GYRO_COEF_GAIN,                  // 19
    PROPULSION_NUMBER_COEFS
} PROPULSION_coef_e;

typedef enum
{
    CORRECTOR_ENABLE = 0,
    CORRECTOR_ROTATION_ONLY,
    CORRECTOR_TRANSLATION_ONLY,
    CORRECTOR_DISABLE
} corrector_e;

// Lorsque l'on observe un objet depuis un laser, permet de préciser
// Quelle coté (relativement à l'objet dans le repère du terrain)
// On "voit" de l'objet
typedef enum
{
    SCAN_SIDE_TOP = 0,
    SCAN_SIDE_RIGHT,
    SCAN_SIDE_BOT,
    SCAN_SIDE_LEFT
} SCAN_side_e;

/**
 * @brief Les 4 drivers
 */
typedef enum
{
    STEPPER_MOTOR_0 = 0,
    STEPPER_MOTOR_1,
    STEPPER_MOTOR_2,
    STEPPER_MOTOR_3,

    // Ascenseurs de BIG
    STEPPER_MOTOR_BIG_LIFT_FRONT = STEPPER_MOTOR_2,
    STEPPER_MOTOR_BIG_LIFT_BACK = STEPPER_MOTOR_3,
} stepper_motor_e;

#define STEPPER_MOTOR_COUNT 4

/**
 * @brief Définition des niveaux de courant pour le can
 */
typedef enum
{
    STEPPER_CURRENT_STRONG = 0,
    STEPPER_CURRENT_MIDDLE_STRONG,
    STEPPER_CURRENT_MIDDLE_WEAK,
    STEPPER_CURRENT_WEAK
} stepper_current_e;

/**
 * @brief Résolution du moteur pas à pas en demi, quart, huitième ou seizième de pas
 */
typedef enum
{
    STEPPER_RES_1_2 = 0,
    STEPPER_RES_1_4,
    STEPPER_RES_1_8,
    STEPPER_RES_1_16
} stepper_res_e;

/**
 * @brief Définition des status possibles pour la carte stepper
 */
typedef enum
{
    // OK
    STEPPER_STATUS__GOAL_REACHED = 0,
    STEPPER_STATUS__CALIBRATED,
    STEPPER_STATUS__CONFIGURED,

    // Pas ok
    STEPPER_STATUS__DRIVER_ERROR,                // STEPPER indique qu'un driver est en erreur
    STEPPER_STATUS__IN_PROGRESS,                 // STEPPER indique qu'un moteur est en mouvement
    STEPPER_STATUS__ORDER_DENIED_DRIVER_ERROR,   // Le moteur est en erreur, l'ordre est refusé
    STEPPER_STATUS__ORDER_DENIED_BOARD_DISABLED, // La strat a désactivée la carte, on refuse l'ordre. Un reset est requis
    STEPPER_STATUS__ORDER_DENIED_ALREADY_MOVING, // On refuse l'ordre si l'on est déjà en déplacement
    STEPPER_STATUS__ORDER_TIMEOUT,

    // Action en cours
    STEPPER_STATUS__UNKNOWN = 255 // Pas de réponse de stepper pour l'instant
} stepper_status_e;

/**********************************************************************************************************************
 **********************************************************************************************************************
 **												Types associés à l'évitement										 **
 **********************************************************************************************************************
 *********************************************************************************************************************/

/*Type d'evitement pour construction du message de debug*/
typedef enum
{
    FORCED_BY_USER = 0,
    FOE1,
    FOE2
} foe_origin_e;

typedef enum
{
    HOKUYO_DEBUG_EVENT_INIT,
    HOKUYO_DEBUG_DISCONNECTED,
    HOKUYO_DEBUG_INITIALIZED,
    HOKUYO_DEBUG_BUFFER_READ_TIMEOUT,
    HOKUYO_DEBUG_FIRST_TRAME_RECEIVED,
    HOKUYO_DEBUG_EVENT_ERROR
} hokuyo_debug_event_e;

typedef enum
{
    ADVERSARY_DETECTION_FIABILITY_NONE = 0b0000,
    ADVERSARY_DETECTION_FIABILITY_X = 0b0001,
    ADVERSARY_DETECTION_FIABILITY_Y = 0b0010,
    ADVERSARY_DETECTION_FIABILITY_TETA = 0b0100,
    ADVERSARY_DETECTION_FIABILITY_DISTANCE = 0b1000,
    ADVERSARY_DETECTION_FIABILITY_ALL = 0b1111
} adversary_detection_fiability_leg_e;

typedef enum
{                               // Plusieurs erreurs peuvent se cumuler
    AUCUNE_ERREUR = 0b00000000, // COMPORTEMENT : le résultat délivré semble bon, il peut être utilisé.

    AUCUN_SIGNAL = 0b00000001, // survenue de l'interruption timer 3 car strictement aucun signal reçu depuis au moins deux
                               // tours moteurs cette erreur peut se produire si l'on est très loin COMPORTEMENT : pas
                               // d'évittement par balise, prise en compte des télémètres !

    SIGNAL_INSUFFISANT
        = 0b00000010, // il peut y avoir un peu de signal, mais pas assez pour estimer une position fiable (se produit
    // typiquement si l'on est trop loin) cette erreur n'est pas grave, on peut considérer que le robot est
    // LOIN ! COMPORTEMENT : pas d'évittement, pas de prise en compte des télémètres !

    TACHE_TROP_GRANDE = 0b00000100, // Ce cas se produit si trop de récepteurs ont vu du signal.
    //  Ce seuil est STRICTEMENT supérieur au cas normal d'un robot très pret. Il y a donc probablement un
    //  autre émetteur quelque part, ou on est entouré de miroir.
    // COMPORTEMENT : La position obtenue n'est pas fiable, il faut se référer aux télémètres...

    TROP_DE_SIGNAL = 0b00001000, // Le récepteur ayant reçu le plus de signal en à trop recu
                                 //	cas 1, peu probable, le moteur est bloqué (cas de test facile pour vérifier cette
                                 //fonctionnalité !) 	cas 2, probable, il y a un autre émetteur quelque part !!!
                                 //  	cas 3, on est dans une enceinte fermée et on capte trop
    // COMPORTEMENT : La position obtenue n'est pas fiable, il faut se référer aux télémètres...

    ERREUR_POSITION_INCOHERENTE
        = 0b00010000, // La position obtenue en x/y est incohérente, le robot semble être franchement hors du terrain
                      // COMPORTEMENT : si la position obtenue indique qu'il est loin, on ne fait pas d'évitement !
                      // sinon, on fait confiance à nos télémètres (?)

    OBSOLESCENCE
        = 0b10000000 // La position adverse connue est obsolète compte tenu d'une absence de résultat valide depuis un certain
                     // temps. COMPORTEMENT : La position obtenue n'est pas fiable, il faut se référer aux télémètres...
} beacon_ir_error_e;

/**********************************************************************************************************************
 **********************************************************************************************************************
 **											Types associés à la communication XBEE									 **
 **********************************************************************************************************************
 *********************************************************************************************************************/

typedef enum
{
    XBEE_ZONE_TRY_LOCK = 0, // La réponse de l'autre robot sera envoyé avec XBEE_ZONE_LOCK_RESULT
    XBEE_ZONE_LOCK_RESULT,  // La réponse dans data[2]: true/false suivant si le verouillage à été accepté ou non
    XBEE_ZONE_UNLOCK,       // Libère une zone qui a été verouillée
    XBEE_ZONE_LOCK
} xbee_zone_order_e;

// Zones pour gérer les déplacements des robots
typedef enum
{
    ZONE_NO_ZONE = 0,
    NB_ZONES
} zone_id_e;

typedef enum
{
    EVENT_NO_EVENT = 0b00000000,
    EVENT_GET_IN = 0b00000001,
    EVENT_GET_OUT = 0b00000010,
    EVENT_TIME_IN = 0b00000100
} ZONE_event_t;

// Zones pour la balise fixe
typedef enum
{
    ZONE_NUMBER // Nombre de zones...
} ZONE_zoneId_e;

/**********************************************************************************************************************
 **********************************************************************************************************************
 **												Types associés à l'IHM												 **
 **********************************************************************************************************************
 *********************************************************************************************************************/

typedef enum
{
    POWER_UNKNOW = 0b000000000,          // aucune info reçue
    POWER_BATTERY_DISABLE = 0b000000001, // le 24V permanent semble absent
    POWER_BATTERY_ENABLE = 0b000000010,  // le 24V permanent est ok
    POWER_BATTERY_LOW = 0b000000100,     // le 24V permanent semble faible
    POWER_ARU_24V_DISABLE = 0b000001000, // le 24V n'est pas ou plus disponible
    POWER_ARU_24V_ENABLE = 0b000010000,  // le 24V est disponible
    POWER_HOKUYO_DISABLE = 0b000100000,  // l'alim de l'hokuyo est désactivée
    POWER_HOKUYO_ENABLE = 0b001000000    // l'alim de l'hokuyo est activée
} power_event_e;

// Switch de la carte IHM, pour rajouter des switchs (voir IHM switch.c/h)
typedef enum
{
    BIROUTE_IHM = 0,
    SWITCH_COLOR_IHM,
    SWITCH_LCD_IHM,
    SWITCH0_IHM,
    SWITCH1_IHM,
    SWITCH2_IHM,
    SWITCH3_IHM,
    SWITCH4_IHM,
    SWITCH5_IHM,
    SWITCH6_IHM,
    SWITCH7_IHM,
    SWITCH8_IHM,
    SWITCH9_IHM,
    SWITCH10_IHM,
    SWITCH11_IHM,
    SWITCH12_IHM,
    SWITCH13_IHM,
    SWITCH14_IHM,
    SWITCH15_IHM,
    SWITCH16_IHM,
    SWITCH17_IHM,
    SWITCH18_IHM,
    SWITCHS_NUMBER_IHM
} switch_ihm_e;

// Button de la carte ihm
typedef enum
{
    BP_SELFTEST_IHM = 0,
    BP_CALIBRATION_IHM,
    BP_PRINTMATCH_IHM,
    BP_SUSPEND_RESUME_MATCH_IHM,
    BP_0_IHM,
    BP_1_IHM,
    BP_2_IHM,
    BP_3_IHM,
    BP_4_IHM,
    BP_5_IHM,
    BP_NUMBER_IHM
} button_ihm_e;

// Leds de la carte IHM
typedef enum
{
    LED_OK_IHM = 0,
    LED_UP_IHM,
    LED_DOWN_IHM,
    LED_SET_IHM,
    LED_COLOR_IHM,
    LED_0_IHM,
    LED_1_IHM,
    LED_2_IHM,
    LED_3_IHM,
    LED_4_IHM,
    LED_5_IHM,
    LED_NUMBER_IHM
} led_ihm_e;

// Ne mettre que 8 états max
// Si rajout état le faire aussi dans la fonction get_blink_state de led.c de la carte IHM
typedef enum
{
    OFF = 0,
    ON,
    BLINK_1HZ,
    SPEED_BLINK_4HZ,
    FLASH_BLINK_10MS
} blinkLED_e;

typedef enum
{
    LED_COLOR_BLACK = 0b000,
    LED_COLOR_BLUE = 0b001,
    LED_COLOR_GREEN = 0b010,
    LED_COLOR_CYAN = 0b011,
    LED_COLOR_RED = 0b100,
    LED_COLOR_MAGENTA = 0b101,
    LED_COLOR_YELLOW = 0b110,
    LED_COLOR_WHITE = 0b111
} led_color_e;

typedef struct
{
    led_ihm_e id;
    blinkLED_e blink;
} led_ihm_t;

typedef enum
{
    IHM_ERROR_HOKUYO = 0b00000001,
    IHM_ERROR_ASSER = 0b00000010
} ihm_error_e;

/**********************************************************************************************************************
 **********************************************************************************************************************
 **												Types associés au selftest											 **
 **********************************************************************************************************************
 *********************************************************************************************************************/

typedef enum
{
    SELFTEST_NOT_DONE = 0,
    SELFTEST_FAIL_UNKNOW_REASON,
    SELFTEST_TIMEOUT,

    SELFTEST_BEACON_ADV1_NOT_SEEN, // Ne rien mettre avant ceci sans synchroniser le QS_CANmsgList dsPIC pour la balise !!!
    SELFTEST_BEACON_ADV2_NOT_SEEN,
    SELFTEST_BEACON_SYNCHRO_NOT_RECEIVED,
    SELFTEST_BEACON_ADV1_BATTERY_LOW,
    SELFTEST_BEACON_ADV2_BATTERY_LOW,
    SELFTEST_BEACON_ADV1_RF_UNREACHABLE,
    SELFTEST_BEACON_ADV2_RF_UNREACHABLE,

    SELFTEST_PROP_FAILED,
    SELFTEST_PROP_HOKUYO_FAILED,
    SELFTEST_PROP_IN_SIMULATION_MODE,
    SELFTEST_PROP_SWITCH_ASSER_DISABLE,
    SELFTEST_PROP_LASER_SENSOR_RIGHT,
    SELFTEST_PROP_LASER_SENSOR_LEFT, // 0xF

    SELFTEST_STRAT_XBEE_OR_PAMI_SWITCH_DISABLE,
    SELFTEST_STRAT_XBEE_DESTINATION_UNREACHABLE,
    SELFTEST_STRAT_RTC,
    SELFTEST_STRAT_BATTERY_NO_24V,
    SELFTEST_STRAT_BATTERY_LOW,
    SELFTEST_STRAT_WHO_AM_I_ARE_NOT_THE_SAME,
    SELFTEST_STRAT_BIROUTE_FORGOTTEN,
    SELFTEST_STRAT_SD_WRITE_FAIL,

    SELFTEST_IHM_BATTERY_NO_24V,
    SELFTEST_IHM_BATTERY_LOW,
    SELFTEST_IHM_BIROUTE_FORGOTTEN, // 0x1F
    SELFTEST_IHM_POWER_HOKUYO_FAILED,

    SELFTEST_ACT_UNREACHABLE,
    SELFTEST_PROP_UNREACHABLE,
    SELFTEST_BEACON_UNREACHABLE,
    SELFTEST_IHM_UNREACHABLE,

    // Self test de la carte actionneur (si actionneur indiqué, alors il n'a pas fonctionné comme prévu, pour plus d'info
    // voir la sortie uart de la carte actionneur) :
    SELFTEST_ACT_MISSING_TEST, // Test manquant après un timeout du selftest actionneur, certains actionneur n'ont pas le
                               // selftest d'implémenté ou n'ont pas terminé leur action (ou plus rarement, la pile était
                               // pleine et le selftest n'a pas pu se faire)
    SELFTEST_ACT_UNKNOWN_ACT,  // Un actionneur inconnu a fail son selftest. Pour avoir le nom, ajoutez un SELFTEST_ACT_xxx
                               // ici et gérez l'actionneur dans selftest.c de la carte actionneur
    SELFTEST_ACT_SIMU,         // Actionneur en mode simu

    // Switchs
    SELFTEST_STRAT_SWITCH_NOMINAL_MODE,
    SELFTEST_STRAT_SWITCH_HOMOLOGATION,
    SELFTEST_STRAT_SWITCH_DISABLE_SUCKERS,
    SELFTEST_STRAT_SWITCH_DISABLE_CAPACITIVE_SENSORS,
    SELFTEST_STRAT_SWITCH_DISABLE_LASER_SENSORS,
    SELFTEST_STRAT_SWITCH18_DISABLE_ASSER,

    // Capteurs capacitifs
    SELFTEST_STRAT_RECALAGE_AR,
    SELFTEST_STRAT_RECALAGE_AV,
    SELFTEST_STRAT_RECALAGE_AV_G,
    SELFTEST_STRAT_RECALAGE_AV_D,
    SELFTEST_STRAT_RECALAGE_AR_G,
    SELFTEST_STRAT_RECALAGE_AR_D,

    // Pompes
    SELFTEST_ACT_PUMPS,

    // BIG_ROBOT
    //SELFTEST_ACT_EXEMPLE,
    SELFTEST_ACT_BIG_ELECTROMAGNET_ARM_FRONT_LEFT,
    SELFTEST_ACT_BIG_ELECTROMAGNET_ARM_FRONT_RIGHT,
    SELFTEST_ACT_BIG_ELECTROMAGNET_ARM_BACK_LEFT,
    SELFTEST_ACT_BIG_ELECTROMAGNET_ARM_BACK_RIGHT,
    SELFTEST_ACT_BIG_MAGNETS_ARM_FRONT_LEFT,
    SELFTEST_ACT_BIG_MAGNETS_ARM_FRONT_RIGHT,
    SELFTEST_ACT_BIG_MAGNETS_ARM_BACK_LEFT,
    SELFTEST_ACT_BIG_MAGNETS_ARM_BACK_RIGHT,
    SELFTEST_ACT_BIG_PLATFORM_HOLDER_FRONT_ABOVE,
    SELFTEST_ACT_BIG_PLATFORM_HOLDER_FRONT_BELOW,
    SELFTEST_ACT_BIG_BANNER_ARM,
    SELFTEST_ACT_BIG_BANNER_LOCKER,
    SELFTEST_ACT_BIG_ELECTROMAGNET_FRONT_LEFT,
    SELFTEST_ACT_BIG_ELECTROMAGNET_FRONT_RIGHT,
    SELFTEST_ACT_BIG_ELECTROMAGNET_BACK_LEFT,
    SELFTEST_ACT_BIG_ELECTROMAGNET_BACK_RIGHT,
    SELFTEST_ACT_BIG_PUMP_BIG_LIFT_FRONT_VERY_LEFT,
    SELFTEST_ACT_BIG_ELECTROVANNE_BIG_LIFT_FRONT_VERY_LEFT,
    SELFTEST_ACT_BIG_VACUOSTAT_BIG_LIFT_FRONT_VERY_LEFT,
    SELFTEST_ACT_BIG_PUMP_BIG_LIFT_FRONT_LEFT,
    SELFTEST_ACT_BIG_ELECTROVANNE_BIG_LIFT_FRONT_LEFT,
    SELFTEST_ACT_BIG_VACUOSTAT_BIG_LIFT_FRONT_LEFT,
    SELFTEST_ACT_BIG_PUMP_BIG_LIFT_FRONT_RIGHT,
    SELFTEST_ACT_BIG_ELECTROVANNE_BIG_LIFT_FRONT_RIGHT,
    SELFTEST_ACT_BIG_VACUOSTAT_BIG_LIFT_FRONT_RIGHT,
    SELFTEST_ACT_BIG_PUMP_BIG_LIFT_FRONT_VERY_RIGHT,
    SELFTEST_ACT_BIG_ELECTROVANNE_BIG_LIFT_FRONT_VERY_RIGHT,
    SELFTEST_ACT_BIG_VACUOSTAT_BIG_LIFT_FRONT_VERY_RIGHT,
    SELFTEST_ACT_BIG_PUMP_BIG_LIFT_BACK_VERY_LEFT,
    SELFTEST_ACT_BIG_ELECTROVANNE_BIG_LIFT_BACK_VERY_LEFT,
    SELFTEST_ACT_BIG_VACUOSTAT_BIG_LIFT_BACK_VERY_LEFT,
    SELFTEST_ACT_BIG_PUMP_BIG_LIFT_BACK_LEFT,
    SELFTEST_ACT_BIG_ELECTROVANNE_BIG_LIFT_BACK_LEFT,
    SELFTEST_ACT_BIG_VACUOSTAT_BIG_LIFT_BACK_LEFT,
    SELFTEST_ACT_BIG_PUMP_BIG_LIFT_BACK_RIGHT,
    SELFTEST_ACT_BIG_ELECTROVANNE_BIG_LIFT_BACK_RIGHT,
    SELFTEST_ACT_BIG_VACUOSTAT_BIG_LIFT_BACK_RIGHT,
    SELFTEST_ACT_BIG_PUMP_BIG_LIFT_BACK_VERY_RIGHT,
    SELFTEST_ACT_BIG_ELECTROVANNE_BIG_LIFT_BACK_VERY_RIGHT,
    SELFTEST_ACT_BIG_VACUOSTAT_BIG_LIFT_BACK_VERY_RIGHT,
    SELFTEST_ACT_BIG_PUMP_BIG_PLATEFORM_HOLDER_FRONT_ABOVE,
    SELFTEST_ACT_BIG_ELECTROVANNE_BIG_PLATEFORM_HOLDER_FRONT_ABOVE,
    SELFTEST_ACT_BIG_VACUOSTAT_BIG_PLATEFORM_HOLDER_FRONT_ABOVE,
    SELFTEST_ACT_BIG_PUMP_BIG_PLATEFORM_HOLDER_BACK_ABOVE,
    SELFTEST_ACT_BIG_ELECTROVANNE_BIG_PLATEFORM_HOLDER_BACK_ABOVE,
    SELFTEST_ACT_BIG_VACUOSTAT_BIG_PLATEFORM_HOLDER_BACK_ABOVE,

    // SMALL_ROBOT
    // SELFTEST_ACT_EXEMPLE,

    SELFTEST_ERROR_NB,
    SELFTEST_ERROR = 0xFE,
    SELFTEST_NO_ERROR = 0xFF
    //... ajouter ici d'éventuels nouveaux code d'erreur.
} SELFTEST_error_code_e;

/**********************************************************************************************************************
 **********************************************************************************************************************
 **											Types associés à l'actionneur											 **
 **********************************************************************************************************************
 *********************************************************************************************************************/

typedef enum
{
    SENSOR_EXEMPLE,
    SENSOR_LONG_DISTANCE_BIG_LEFT,
    SENSOR_LONG_DISTANCE_BIG_RIGHT,
    NB_ACT_SENSOR
} act_sensor_id_e;

// Mettre toujours l'ordre de STOP à la valeur 0 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
typedef enum
{

    // Ne pas toucher
    ACT_DEFAULT_STOP = 0,
    ACT_CUSTOM_ORDER = 0xFF,

    // Mosfet
    ACT_MOSFET_STOP = 0,
    ACT_MOSFET_NORMAL,

    // Pompe
    ACT_POMPE_STOP = 0,
    ACT_POMPE_NORMAL,
    ACT_POMPE_REVERSE,

    // Moteur
    ACT_MOTOR_STOP = 0,
    ACT_MOTOR_RUN,

    // Asservissement en couple des servos
    ACT_SERVO_ASSER_TORQUE = 0xFE,

    // BIG_ROBOT

    ACT_BIG_SERVO_STOP = 0,

    ACT_BIG_ELECTROMAGNET_ARM_STOP = 0,
    ACT_BIG_ELECTROMAGNET_ARM_OPEN,
    ACT_BIG_ELECTROMAGNET_ARM_EXTRACT_EXTREMITY,
    ACT_BIG_ELECTROMAGNET_ARM_TAKE_CENTER,
    ACT_BIG_ELECTROMAGNET_ARM_TAKE_EXTREMITY,
    ACT_BIG_ELECTROMAGNET_ARM_DISPLACEMENTS,

    ACT_BIG_MAGNETS_ARM_STOP = 0,
    ACT_BIG_MAGNETS_ARM_TAKE,
    ACT_BIG_MAGNETS_ARM_RELEASE,

    ACT_BIG_PLATFORM_HOLDER_ABOVE_STOP = 0,
    ACT_BIG_PLATFORM_HOLDER_ABOVE_OPEN,
    ACT_BIG_PLATFORM_HOLDER_ABOVE_CLOSE,

    ACT_BIG_PLATFORM_HOLDER_BELOW_STOP = 0,
    ACT_BIG_PLATFORM_HOLDER_BELOW_OPEN,
    ACT_BIG_PLATFORM_HOLDER_BELOW_CLOSE,

    ACT_BIG_BANNER_ARM_STOP = 0,
    ACT_BIG_BANNER_ARM_FOLDED,
    ACT_BIG_BANNER_ARM_OUT,
    ACT_BIG_BANNER_ARM_FREE,
    ACT_BIG_BANNER_ARM_DROP,
    ACT_BIG_BANNER_ARM_AFTER_DISPOSE,
    ACT_BIG_BANNER_ARM_INIT_POS,

    ACT_BIG_BANNER_LOCKER_STOP = 0,
    ACT_BIG_BANNER_LOCKER_LOCK,
    ACT_BIG_BANNER_LOCKER_MIDDLE,
    ACT_BIG_BANNER_LOCKER_UNLOCK,
    ACT_BIG_BANNER_LOCKER_AFTER_DISPOSE,
    ACT_BIG_BANNER_LOCKER_INIT_POS,

    // SMALL_ROBOT

    ACT_SMALL_SERVO_STOP = 0,

    // STRAT_EASY

    POS_STRATEASY_SERVO_STOP = 0,
    POS_NORTH_TIDY,
    POS_NORTH_DISPENSER,
    POS_NORTH_DISPENSER_LAST_SAMPLE,
    POS_NORTH_TRANSFER,
    POS_NORTH_INSIDE_WITH_SAMPLE,
    POS_NORTH_INSIDE_MAX,
    POS_NORTH_TRANSFER_FORK,
    POS_NORTH_UP_TO_PROMENADE,
    POS_NORTH_DISPOSE,
    POS_NORTH_DISPOSE_HIGH,
    POS_NORTH_DISPOSE_DECREASE,

    POS_SOUTH_SERVO_STOP = 0,
    POS_SOUTH_TIDY,
    POS_SOUTH_BOT,
    POS_SOUTH_ALMOST_BOT,
    POS_SOUTH_UP_WITH_SAMPLE,
    POS_SOUTH_CRUSH_ON_CONSTRUCTION_SITE,
    POS_SOUTH_JUST_HIGHER_THAN_SAMPLE,
    POS_SOUTH_STATUETTE_CLOSE_WITH_REPLICA,
    POS_SOUTH_STATUETTE_ALMOST_CLOSE_WITH_REPLICA,
    POS_SOUTH_STATUETTE_ALMOST_CLOSE_TO_PUSH_REPLICA,
    POS_SOUTH_STATUETTE_OPEN_DISPOSE_REPLICA,
    POS_SOUTH_STATUETTE_FULL_OPEN_MEASURING_RESISTORS,
    POS_SOUTH_STATUETTE_INSIDE_RELEASE,
    POS_EXCAVATION_SQUARE_TIDY,
    POS_EXCAVATION_SQUARE_OPEN,
    POS_EXCAVATION_SQUARE_ALMOST_OPEN

} ACT_order_e;

typedef enum
{
    VACUOSTAT_EXEMPLE = 1,

} act_vacuostat_id;

typedef enum
{
    ACT_RESULT_DONE = 0, // Tout s'est bien passé
    ACT_RESULT_FAILED, // La commande s'est mal passé et on ne sait pas dans quel état est l'actionneur (par: les capteurs ne
                       // fonctionnent plus)
    ACT_RESULT_NOT_HANDLED, // La commande ne s'est pas effectué correctement et on sait que l'actionneur n'a pas bougé (ou
                            // quand la commande a été ignorée)
    ACT_RESULT_IN_PROGRESS  // La commande est en cours de réalisation.
} act_result_state_e;

typedef enum
{
    ACT_RESULT_ERROR_OK = 0,   // Quand pas d'erreur
    ACT_RESULT_ERROR_TIMEOUT,  // Il y a eu timeout, par ex l'asservissement prend trop de temps
    ACT_RESULT_ERROR_OTHER,    // La commande était lié à une autre qui ne s'est pas effectué correctement, utilisé avec
                               // ACT_RESULT_NOT_HANDLED
    ACT_RESULT_ERROR_NOT_HERE, // L'actionneur ou le capteur ne répond plus (on le sait par un autre moyen que le timeout,
                               // par exemple l'AX12 ne renvoie plus d'info après l'envoi d'une commande.)
    ACT_RESULT_ERROR_LOGIC,    // Erreur de logique interne à la carte actionneur, probablement une erreur de codage (par
                               // exemple un état qui n'aurait jamais dû arrivé)
    ACT_RESULT_ERROR_NO_RESOURCES, // La carte n'a pas assez de resource pour gérer la commande. Commande à renvoyer plus
                                   // tard.
    ACT_RESULT_ERROR_INVALID_ARG, // La commande ne peut pas être effectuée, l'argument n'est pas valide ou est en dehors des
                                  // valeurs acceptés
    ACT_RESULT_ERROR_CANCELED,    // L'action a été annulé
    ACT_RESULT_ERROR_OVERHEATING,                 // Actioneur en surchauffe
    ACT_RESULT_ERROR_OVERLOAD,                    // Actionneur en surcharge
    ACT_RESULT_ERROR_OVERVOLTAGE_OR_UNDERVOLTAGE, // Actionneur en surtension ou sous tension
    ACT_RESULT_ERROR_NOT_INITIALIZED,             // Actionneur non initialisé

    ACT_RESULT_ERROR_UNKNOWN = 255 // Erreur inconnue ou qui ne correspond pas aux précédentes.

} act_result_error_code_e;

typedef enum
{
    SPEED_CONFIG = 0,
    TORQUE_CONFIG,
    POSITION_CONFIG,
    TEMPERATURE_CONFIG,
    LOAD_CONFIG,
    PUNCH_CONFIG,
} act_config_e;

typedef struct
{
    int16_t warner_value;
    int16_t last_value;
    bool activated;
} act_warner_s;

/**********************************************************************************************************************
 **********************************************************************************************************************
 **												Types associés à la carte mosfet									 **
 **********************************************************************************************************************
 *********************************************************************************************************************/

typedef enum
{
    ACT_EXPANDER_PUMP_STATUS_NO_PUMPING,
    ACT_EXPANDER_PUMP_STATUS_PUMPING_NOTHING,
    ACT_EXPANDER_PUMP_STATUS_PUMPING_OBJECT,
    ACT_EXPANDER_PUMP_STATUS_VACUOSTAT_INEXISTANT
} ACT_EXPANDER_pumpStatus_e;

typedef enum
{
    ACT_EXPANDER_COLOR_SENSOR_NONE,
    ACT_EXPANDER_COLOR_SENSOR_RED,
    ACT_EXPANDER_COLOR_SENSOR_GREEN,
    ACT_EXPANDER_COLOR_SENSOR_BLUE,
    ACT_EXPANDER_COLOR_SENSOR_WHITE,
    ACT_EXPANDER_COLOR_SENSOR_BLACK,
    ACT_EXPANDER_COLOR_SENSOR_NB
} ACT_EXPANDER_colorSensor_e;

typedef enum
{
    ACT_EXPANDER_DISTANCE_THRESHOLD_WAY_LOWER,
    ACT_EXPANDER_DISTANCE_THRESHOLD_WAY_UPPER
} ACT_EXPANDER_DistanceThresholdWay_e;

#endif /* ndef QS_TYPES_H */
