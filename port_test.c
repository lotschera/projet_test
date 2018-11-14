#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <ev3.h>
#include <ev3_light.h>
#include <ev3_port.h>
#include <ev3_sensor.h>
#include <ev3_servo.h>
#include <ev3_tacho.h>

#include "zlog.h"

// zlog specific global variable
zlog_category_t *zlog_c;

int init(void) {
  int i, rc;
  char buf[8];

  // Initialize the EV3 Intelligent Brick
  rc = ev3_init();
  if (rc == 1)
    zlog_info(zlog_c, "Found EV3 Intelligent Brick");
  else {
    if (rc == 0)
      zlog_fatal(zlog_c, "EV3 Intelligent Brick not found\n");
    else
      zlog_error(zlog_c, "ev3_init an error has occurred\n");
    return rc;
  }
  // Initialize the EV3 ports
  rc = ev3_port_init();
  if (rc == -1) {
    zlog_error(zlog_c, "Error while initializing EV3 ports");
    ev3_uninit();
    return rc;
  } else if (rc != 8)
    zlog_warn(zlog_c, "Found '%d/8' EV3 ports", rc);
  else
    zlog_info(zlog_c, "All EV3 ports found");
  // Initialize the EV3 sensors
  rc = ev3_sensor_init();
  if (rc == -1) {
    zlog_error(zlog_c, "Error while initializing EV3 sensors");
    ev3_uninit();
    return rc;
  } else
    zlog_info(zlog_c, "Found '%d' EV3 sensors", rc);
  // Initialize the EV3 tacho
  rc = ev3_tacho_init();
  if (rc == -1) {
    zlog_error(zlog_c, "Error while initializing EV3 tacho motors");
    ev3_uninit();
    return rc;
  } else
    zlog_info(zlog_c, "Found '%d' EV3 tacho motors", rc);
  // Log the sensor descriptors
  for (i = 0; i < SENSOR_DESC__LIMIT_; i++)
    if (ev3_sensor[i].type_inx != SENSOR_TYPE__NONE_) {
      const char *str = "  Sensor type:";
      zlog_info(zlog_c, "Descriptor: %d", i);
      zlog_info(zlog_c, "%s %s", str, ev3_sensor_type(ev3_sensor[i].type_inx));
      ev3_port_name(ev3_sensor[i].port, ev3_sensor[i].extport, ev3_sensor[i].addr, buf);
      zlog_info(zlog_c, "  Sensor port: %u (%s)", ev3_sensor[i].port, buf);
      zlog_info(zlog_c, "  Sensor extport: %u", ev3_sensor[i].extport);
      zlog_info(zlog_c, "  Sensor addr: %u\n", ev3_sensor[i].addr);
    }
  // Log the tacho motor descriptors
  for (i = 0; i < TACHO_DESC__LIMIT_; i++)
    if (ev3_tacho[i].type_inx != TACHO_TYPE__NONE_) {
      const char *str = "  Tacho type:";
      uint8_t sn;
      zlog_info(zlog_c, "Descriptor: %d", i);
      zlog_info(zlog_c, "%s %s", str, ev3_tacho_type(ev3_tacho[i].type_inx));
      rc = ev3_search_tacho_plugged_in(ev3_tacho[i].port, ev3_tacho[i].extport, &sn, 0);
      if (rc) {
	ev3_tacho_port_name(sn, buf);
	zlog_info(zlog_c, "  Tacho port: %u (%s)", ev3_tacho[i].port, buf);
	zlog_info(zlog_c, "  Tacho extport: %u\n", ev3_tacho[i].extport);
      } else
	zlog_warn(zlog_c, "EV3 tacho motor not plugged in");
    }

  return 1;
}

int main (int argc, char *argv[]) {
  int condition = 0, color_idx, rc;
  float color;

  // zlog specific variables
  const char *zlog_conf = "/etc/zlog.conf";
  const char *zlog_cat  = "project";

  rc = zlog_init(zlog_conf);
  if (rc) {
    printf("zlog initialization using '%s' failed\n", zlog_conf);
    return EXIT_FAILURE;
  }

  zlog_c = zlog_get_category(zlog_cat);
  if (!zlog_c) {
    printf("zlog unable to retrieve category '%s'\n", zlog_cat);
    puts("zlog logging disabled");
    zlog_fini();
  }
  
  zlog_info(zlog_c, "Hello IIUN!");

  if(!init()) {
    zlog_fini();
    return EXIT_FAILURE;
  }

  // Set lights to red
  set_light(LIT_LEFT, LIT_RED);
  set_light(LIT_RIGHT, LIT_RED);

  // Set lights to green
  set_light(LIT_LEFT, LIT_GREEN);
  set_light(LIT_RIGHT, LIT_GREEN);

  ev3_uninit();
    
  zlog_info(zlog_c, "Bye IIUN!");

  zlog_fini();

  return EXIT_SUCCESS;
}
