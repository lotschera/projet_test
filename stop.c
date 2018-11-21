#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <ev3.h>
#include <ev3_light.h>
#include <ev3_port.h>
#include <ev3_tacho.h>

#define INIT_WAIT 500000
#define TACHO_MOTORS 3

// Name the tacho motor
#define TACHO_LEFT 0
#define TACHO_RIGHT 1
#define TACHO 2

// Name the sockets according to the sensors name
#define TACHO_LEFT_PORT OUTPUT_D
#define TACHO_RIGHT_PORT OUTPUT_A
#define TACHO_PORT OUTPUT_C

/*
 * Array of sensor sequence numbers.
 */
uint8_t tacho_sn[TACHO_MOTORS + 1];

/*
 * Maximum speed for tacho motor. The maximum speed value gets assigned during
 * initialization. Default is 0.
 */
int max_spd = 0;

int init(void) {
  int i, rc, max_spd_left = 0, max_spd_right = 0;
  size_t bytes;
  unsigned char tachos = 0;
  uint8_t sn;

  for (i = 0; i < TACHO_DESC__LIMIT_; i++)
    tacho_sn[i] = DESC_LIMIT;

  // Initialisation de la brique intelligente EV3
  rc = ev3_init();
  if (rc == 1)
    zlog_info(zlog_c, "Brique intelligente EV3 trouvée");
  else {
    if (rc == 0)
      zlog_fatal(zlog_c, "Brique intelligente EV3 pas trouvée");
    else
      zlog_error(zlog_c, "ev3_init retourne erreur '%d'", rc);
    return rc;
  }
  // Initialisation des ports EV3
  rc = ev3_port_init();
  if (rc == -1) {
    zlog_error(zlog_c, "Erreur durant l'initialisation des ports EV3");
    ev3_uninit();
    return rc;
  } else if (rc != 8)
    zlog_warn(zlog_c, "'%d/8' ports EV3 retrouvé");
  else
    zlog_info(zlog_c, "Tous ports EV3 retrouvé");
  // Initialisation des servomoteurs
  rc = ev3_tacho_init();
  if (rc == -1) {
    zlog_error(zlog_c, "Erreur durant l'initialisation des servomoteurs EV3");
    ev3_uninit();
    return rc;
  } else
    zlog_info(zlog_c, "'%d' servomoteurs EV3 retrouvé", rc);
  /*
   * Assurer que les servomoteurs sont mis dans la brique et mettre en
   * correspondance les numéros de séquence.
   */
  for (i = 0; i < TACHO_DESC__LIMIT_; i++) {
    if (ev3_search_tacho_plugged_in(ev3_tacho[i].port, ev3_tacho[i].extport,
				    &sn, 0)) {
      if (ev3_tacho[i].type_inx == LEGO_EV3_L_MOTOR) {
	if (ev3_tacho[i].port == OUTPUT_A) {
	  tachos |= HAVE_TACHO_LEFT;
	  TACHO_LEFT_SN = i;
	  bytes = get_tacho_max_speed(i, &max_spd_left);
	} else {
	  tachos |= HAVE_TACHO_RIGHT;
	  TACHO_RIGHT_SN = i;
	  bytes = get_tacho_max_speed(i, &max_spd_right);
	}
      }
      if (bytes == 0) {
	zlog_error(zlog_c, "Impossible de lire la vitesse maximale pour '%s'",
		   ev3_tacho_type(ev3_tacho[i].type_inx));
	ev3_uninit();
	return 0;
      }
    } else {
      zlog_error(zlog_c, "Un des grand servomoteur n'est pas connecté à la brique intelligente EV3");
      ev3_uninit();
      return 0;
    }
  }
  if (tachos != (HAVE_TACHO_LEFT | HAVE_TACHO_RIGHT)) {
    zlog_fatal(zlog_c, "Les grand servomoteurs n'ont pas été retrouvé");
    ev3_uninit();
    return 0;
  }
  /*
   * Détermination de la vitesse maximale des deux grand servomoteurs. La
   * vitesse maximale correspond à la plus petite vitesse d'un des deux
   * servomoteurs.
   */
  bytes = multi_set_tacho_command_inx(tacho_sn, TACHO_RESET);
  if (bytes == 0) {
    zlog_error(zlog_c, "Impossible d'envoyer la commande 'TACHO_RESET' aux servomoteurs");
    ev3_uninit();
    return 0;
  }
  max_spd = MIN(max_spd_left, max_spd_right);
  /*
   * Indication aux servomoteurs d'interpréter les valeurs positives comme
   * mouvement en avant.
   */
  bytes = multi_set_tacho_polarity_inx(tacho_sn, TACHO_NORMAL);
  if (bytes == 0) {
    zlog_error(zlog_c, "Impossible de changer la polarité 'TACHO_NORMAL' pour les servomoteurs");
    ev3_uninit();
    return 0;
  }
  bytes = multi_set_tacho_duty_cycle_sp(tacho_sn, 0);
  if (bytes == 0) {
    zlog_error(zlog_c, "Impossible de changer le rapport cyclique '%d' pour les servomoteurs", 0);
    ev3_uninit();
    return 0;
  }

  return 1;
}

int main (int argc, char *argv[]) {
  int condition = 0, color_idx;
  float color;

  puts("Hello IIUN!");

  if(!init())
    return EXIT_FAILURE;

  puts("Arrêter les servomoteurs");
  // A compléter
   
  ev3_uninit();
    
  puts("Bye IIUN!");

  return EXIT_SUCCESS;
}
