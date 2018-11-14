/*
 * Test capteur à ultrasons.
 *
 * Auteur: Dorian Burihabwa
 * Auteur: Christian Göttel
 *
 * Matériel demandé:
 * - 1x EV3 Ultrasonic Sensor / Capteur à ultrasons EV3
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <ev3.h>
#include <ev3_light.h>
#include <ev3_port.h>
#include <ev3_sensor.h>

#include "zlog.h"

/*
 * Macro pour changer le mode d'un capteur avec impréssion de message d'erreur
 * et traitement d'erreur.
 */
#define SET_SENSOR_MODE_INX(sn,m) do {					\
    _bytes = set_sensor_mode_inx((sn), (m));				\
    if (_bytes == 0) {							\
      zlog_error(zlog_c, "Impossible de changer en mode '"#m"' pour le capteur '%s'", ev3_sensor_type(ev3_sensor[(sn)].type_inx)); \
      ev3_uninit();							\
      return 0;								\
    }									\
  } while(0);

#define GET_SENSOR_VALUE0(sn,v) do {					\
    _bytes = get_sensor_value0((sn), (v));				\
    if (_bytes == 0) {							\
      zlog_error(zlog_c,						\
		 "Impossible d'assigner une valeur du capteur '%s'",	\
		 ev3_sensor_type(ev3_sensor[(sn)].type_inx));		\
      ev3_uninit();							\
      return 0;								\
    }									\
  } while(0);

// Numéro de séquence du capteur à ultrasons
#define SENSOR_ULTRASOUND_SN sensor_sn[0]

// Drapeau pour verifier la présence des capteurs
#define HAVE_SENSOR_ULTRASOUND 0b1

// Variable globale spécifique à zlog
zlog_category_t *zlog_c;

// Tableau pour les numéros de séquence des capteurs
uint8_t sensor_sn[SENSOR_DESC__LIMIT_];

// Variable globale pour les macros
size_t _bytes;

int init(void) {
  int i, rc;
  size_t bytes;
  unsigned char sensors = 0;
  uint8_t sn;

  for (i = 0; i < SENSOR_DESC__LIMIT_; i++)
    sensor_sn[i] = DESC_LIMIT;

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
  // Initialisation des capteurs EV3
  rc = ev3_sensor_init();
  if (rc == -1) {
    zlog_error(zlog_c, "Erreur durant l'initialisation des capteurs EV3");
    ev3_uninit();
    return rc;
  } else
    zlog_info(zlog_c, "'%d' capteur(s) EV3 retrouvé", rc);
  /*
   * Assurer que le capteur de couleur est mis dans la brique et mettre en
   * correspondance les numéros de séquence.
   */
  for (i = 0; i < SENSOR_DESC__LIMIT_; i++)
    if (ev3_sensor[i].type_inx == LEGO_EV3_US) {
      sensors |= HAVE_SENSOR_ULTRASOUND;
      SENSOR_ULTRASOUND_SN = i;
      if (ev3_search_sensor_plugged_in(ev3_sensor[i].port, ev3_sensor[i].extport, &sn, 0)) {
	SET_SENSOR_MODE_INX(sn, LEGO_EV3_US_US_LISTEN);
      } else {
	zlog_error(zlog_c, "Le capteur à ultrasons n'est pas connecté à la brique intelligente EV3");
	return 0;
      }
      break;
    }

  if (sensors != HAVE_SENSOR_ULTRASOUND) {
    zlog_fatal(zlog_c, "Le capteur à ultrasons n'a pas été retrouvé");
    return 0;
  }

  return 1;
}

int continuous_test(void) {

  // A compléter
  
  return 1;
}

int single_test(void) {

  // A compléter
  
  return 1;
}

int main(int argc, char *argv[]) {
  int rc;
  size_t bytes;
  // Variables constantes spécifique à zlog
  const char *zlog_conf = "/etc/zlog.conf";
  const char *zlog_cat  = "project";

  rc = zlog_init(zlog_conf);
  if (rc) {
    printf("L'initialisation de zlog avec '%s' a échoué\n", zlog_conf);
    return EXIT_FAILURE;
  }

  zlog_c = zlog_get_category(zlog_cat);
  if (!zlog_c) {
    printf("zlog est incapable de retrouver la catégorie '%s'\n", zlog_cat);
    puts("Impression des messages par zlog est désactivé");
    zlog_fini();
  }
  
  zlog_info(zlog_c, "Hello IIUN!");

  if(!init()) {
    zlog_fini();
    return EXIT_FAILURE;
  }

  // Changer la lumière à rouge
  set_light(LIT_LEFT, LIT_RED);
  set_light(LIT_RIGHT, LIT_RED);

  // Test constamment
  zlog_info(zlog_c, "=== Test constamment ===");
  continuous_test();
  // Test à un
  zlog_info(zlog_c, "=== Test à un ===");
  single_test();

  // Changer la lumière à vert
  set_light(LIT_LEFT, LIT_GREEN);
  set_light(LIT_RIGHT, LIT_GREEN);

  bytes = set_sensor_mode_inx(SENSOR_ULTRASOUND_SN, LEGO_EV3_US_US_LISTEN);
  if (bytes == 0)
    zlog_error(zlog_c, "Impossible de changer au mode 'LEGO_EV3_US_US_LISTEN' pour le capteur à ultrasons");
  
  zlog_info(zlog_c, "Bye IIUN!");

  ev3_uninit();

  zlog_fini();

  return EXIT_SUCCESS;
}
