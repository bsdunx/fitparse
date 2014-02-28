#ifndef _ACTIVITY_H_
#define _ACTIVITY_H_

#include <stdint.h>
#include <stdio.h> /* TODO for fprintf in print data point */
#include <float.h>

typedef enum {
  false,
  true
} bool;

typedef enum {
  Timestamp,
  Latitude,
  Longitude,
  Altitude,
  Distance,
  Speed,
  Power,
  Grade,
  HeartRate,
  Cadence,
  LRBalance,
  Temperature,
  DataFieldCount
} DataField;

typedef struct DataPoint {
  uint32_t timestamp; /* s since Unix Epoch UTC */
  double latitude;    /* degrees */
  double longitude;   /* degrees */
  int32_t altitude;   /* 1000 * m */
  uint32_t distance;  /* 100 * m */
  uint32_t speed;     /* 1000 * m/s */
  uint16_t power;     /* watts */
  int16_t grade;      /* 100 * % */
  uint8_t heart_rate; /* bpm */
  uint8_t cadence;    /* rpm */
  uint8_t lr_balance; /* ? */
  int8_t temperature; /* C */
  struct DataPoint *next;
} DataPoint;

typedef struct Summary {
  unsigned lap; /* lap number 0 vs 1? */

  uint8_t cadence_avg;
  uint8_t heart_rate_avg;
  int8_t temperature_avg;
  uint8_t lr_balance_avg;

  uint16_t calories;
  uint32_t distance; /* 100 * m */
  uint32_t elevation; /* 100 * m */
  uint32_t time; /* s */
  uint32_t time_elapsed; /* s */
  uint32_t ascent; /* 100 * m */
  uint32_t descent; /* 100 * m */
  /* function to calc pace from speed */
  uint32_t speed_avg; /* 1000 * m/s */
  uint32_t speed_max; /* 1000 * m/s */

  uint16_t power_avg; /* TODO 3s, 10s, 30s - for speed as well? */
  uint16_t power_max;
  uint16_t power_kj; /* TODO correct? */

  /* TODO coggans powers - IF, NP, TSS, W/kg */
} Summary;

#define UNSET_TIMESTAMP UINT32_MAX
#define UNSET_LATITUDE DBL_MAX
#define UNSET_LONGITUDE DBL_MAX
#define UNSET_ALTITUDE INT32_MAX
#define UNSET_DISTANCE UINT32_MAX
#define UNSET_SPEED UINT32_MAX
#define UNSET_POWER UINT16_MAX
#define UNSET_GRADE INT16_MAX
#define UNSET_HEART_RATE UINT8_MAX
#define UNSET_CADENCE UINT8_MAX
#define UNSET_LR_BALANCE UINT8_MAX
#define UNSET_TEMPERATURE INT8_MAX

#define UNSET_DATA_POINT(d)              \
  do {                                   \
    (d).timestamp = UNSET_TIMESTAMP;     \
    (d).latitude = UNSET_LATITUDE;       \
    (d).longitude = UNSET_LONGITUDE;     \
    (d).altitude = UNSET_ALTITUDE;       \
    (d).distance = UNSET_DISTANCE;       \
    (d).speed = UNSET_SPEED;             \
    (d).power = UNSET_POWER;             \
    (d).grade = UNSET_GRADE;             \
    (d).heart_rate = UNSET_HEART_RATE;   \
    (d).lr_balance = UNSET_LR_BALANCE;   \
    (d).temperature = UNSET_TEMPERATURE; \
    (d).next = NULL;                     \
  } while (0)

/* TODO debug */
static inline void print_data_point(DataPoint *d) {
  fprintf(stderr,
          "time: %u, lat: %f, lon: %f, alt: %d, dist: %u, speed: %u, pow: %u, "
          "grd: %d, hr: %u, cad: %u, bal: %u, temp: %d, next: %p\n",
          d->timestamp, d->latitude, d->longitude, d->altitude, d->distance,
          d->speed, d->power, d->grade, d->heart_rate, d->cadence,
          d->lr_balance, d->temperature, (void *)d->next);
}

typedef enum {
  /* Swimming, */
  Running,
  Bicycling,
  UnknownSport
} Sport;


typedef enum {
  InvalidGPS,
  Dropouts,
  PowerSpikes,
  HeartRateSpikes,
  HeartRateDropouts,
  DataErrorCount
} DataError;


/*****************
 * TODO Read all individual points and compare it to summary data
 */
typedef struct {
  Sport sport;
  uint32_t *laps; /* TODO array of timestamps, always at least one */
  DataPoint *data_points;
  DataPoint *last_point;
  bool has_data[DataFieldCount];
  unsigned errors[DataErrorCount];
  /*
  //Summary *summaries; // laps + total can include derived statistics
  */
} Activity;

Activity *activity_new(void);
void activity_destroy(Activity *a);
int activity_add_point(Activity *a, DataPoint *point);
int activity_add_lap(Activity *a, uint32_t lap);

#endif /* _ACTIVITY_H_ */
