#ifndef _ACTIVITY_H_
#define _ACTIVITY_H_

#include <stdint.h>

typedef struct DataPoint {
  uint32_t timestamp;  /* s since Unix Epoch UTC */
  double latitude;     /* degrees */
  double longitude;    /* degrees */
  int32_t altitude;    /* m */
  uint32_t distance;   /* 100 * m */
  uint32_t speed;      /* 1000 * m/s */
  uint16_t power;      /* watts */
  int16_t grade;       /* 100 * % */
  uint8_t heart_rate;  /* bpm */
  uint8_t cadence;     /* rpm */
  uint8_t lr_balance;  /* ? */
  int8_t temperature;  /* C */
  struct DataPoint *next;
} DataPoint;

/*
 //TODO could also do these per lap
typedef struct Summary {
  cadence_avg;
  heart_rate_avg;
  temperature_avg;
  lr_balance_avg;
  // totals
  calories;
  distance;
  elevation;
  laps; // and summar per laps? not just count?
  pace;
  time;
  time_elapsed;
  ascent;
  descent;

  speed_avg;
  speed_max;

  power_avg; // 3s, 10s, 30s
  power_max;
  power_kj;

  // coggans powers - IF, NP, TSS, W/kg
} Summary;
*/


typedef enum {
  /* Swimming, */
  Running,
  Bicycling,
  UnknownSport
} Sport;

typedef struct {
  Sport sport;
  uint32_t *laps;  /* TODO array of timestamps, always at least one */
  DataPoint *data_points;
  DataPoint *last_point;
  /*
  //Summary summary; // can include derived statistics (totalAscent, NP, avg)
  //Summary * lap_summaries; // can include derived statistics (totalAscent, NP, avg)
  */
} Activity;

typedef int (*ReadFn)(char *, Activity *);
typedef int (*WriteFn)(char *, Activity *);

typedef enum {
  CSV,
  GPX,
  TCX,
  FIT,
  UnknownFileFormat
} FileFormat;

#define activity_new() \
  { UnknownSport, NULL, NULL }
void activity_destroy(Activity *a);
int activity_add_point(Activity *a, uint32_t timestamp, double latitude,
                       double longitude, int32_t altitude, uint32_t distance,
                       uint32_t speed, uint16_t power, int16_t grade,
                       uint8_t heart_rate, uint8_t cadence, uint8_t lr_balance,
                       int8_t temperature);
int activity_add_lap(uint32_t lap);

int activity_read(Activity *activity, char *filename);
int activity_write(Activity *activity, char *filename);

/* helper functions - could just call the *_read or *_write function directly */
int activity_read_format(Activity *activity, char *filename, FileFormat format);
int activity_write_format(Activity *activity, char *filename, FileFormat format);


/*****************
 * Read all individual points and compare it to summary data
 */

/* Athlete:
 *
 * name
 * gender (TRIMp)
 * preferences (metric, imperial)
 * power zones
 * hr zones (hrr, hrmax)
 * critical power
 * FTP
 * weight (kg/lbs)
 */

#endif /* _ACTIVITY_H_ */
