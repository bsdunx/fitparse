#ifndef _ACTIVITY_H_
#define _ACTIVITY_H_

#include <stdint.h>
#include <stdio.h> /* TODO for fprintf in print data point */
#include <float.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

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

typedef struct {
  double data[DataFieldCount];
} DataPoint;
static const char *DATA_FIELDS[DataFieldCount] = {
    "timestamp",  "latitude", "longitude",  "altitude",
    "distance",   "speed",    "power",      "grade",
    "heart_rate", "cadence",  "lr_balance", "temperature"};

/* TODO convert to array...
//typedef struct Summary {
  //unsigned lap; [> lap number 0 vs 1? <]

  //uint8_t cadence_avg;
  //uint8_t heart_rate_avg;
  //int8_t temperature_avg;
  //uint8_t lr_balance_avg;

  //uint16_t calories;
  //uint32_t distance; [> 100 * m <]
  //uint32_t elevation; [> 100 * m <]
  //uint32_t time; [> s <]
  //uint32_t time_elapsed; [> s <]
  //uint32_t ascent; [> 100 * m <]
  //uint32_t descent; [> 100 * m <]
  //[> function to calc pace from speed <]
  //uint32_t speed_avg; [> 1000 * m/s <]
  //uint32_t speed_max; [> 1000 * m/s <]

  //uint16_t power_avg; [> TODO 3s, 10s, 30s - for speed as well? <]
  //uint16_t power_max;
  //uint16_t power_kj; [> TODO correct? <]

  //[> TODO coggans powers - IF, NP, TSS, W/kg <]
//} Summary;
*/

#define UNSET_FIELD DBL_MAX

static inline void unset_data_point(DataPoint *dp) {
  unsigned char i;
  for (i = 0; i < DataFieldCount; i++) dp->data[i] = UNSET_FIELD;
}

/* TODO debug */
static inline void print_data_point(DataPoint *dp) {
  double *d = dp->data;
  fprintf(stderr,
          "time: %.0f, lat: %.15f, lon: %.15f, alt: %.2f, dist: %.2f, speed: "
          "%.2f, pow: %.0f, "
          "grd: %.2f, hr: %.0f, cad: %.0f, bal: %.0f, temp: %.0f\n",
          d[Timestamp], d[Latitude], d[Longitude], d[Altitude], d[Distance],
          d[Speed], d[Power], d[Grade], d[HeartRate], d[Cadence], d[LRBalance],
          d[Temperature]);
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
  size_t num_points;
  size_t points_alloc;
  bool has_data[DataFieldCount];
  unsigned errors[DataErrorCount];
  /*
  //Summary *summaries; // laps + total can include derived statistics
  */
} Activity;

Activity *activity_new(void);
void activity_destroy(Activity *a);
int activity_add_point(Activity *a, DataPoint *dp);
int activity_add_lap(Activity *a, uint32_t lap);

#endif /* _ACTIVITY_H_ */
