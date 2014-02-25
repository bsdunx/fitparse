#include <stdint.h>
// TODO ifdef stuff...

typedef struct DataPoint {
  uint32_t timestamp;  // s since Unix Epoch UTC
  double latitude;     // semicircles (?)
  double longitude;    // semicircles (?)
  int32_t altitude;    // m
  uint32_t distance;   // 100 *m
  uint32_t speed;      // 1000* m/s
  uint16_t power;      // watts
  int16_t grade;       // 100 * %
  uint8_t heart_rate;  // bpm
  uint8_t cadence;     // rpm
  uint8_t lr_balance;  // ?
  int8_t temperature;  // C
  struct DataPoint *next;
} DataPoint;

typedef enum {
  NA,
  /* Swimming, */
  Running,
  Bicycling
} Sport;

typedef struct {
  Sport sport;
  uint32_t *laps;  // array of timestamps, always at least one
  DataPoint *data_points;
  DataPoint *last_point;
  // summary; // can include derived statistics (totalAscent, NP, avg)
} Activity;

#define activity_new() \
  { NA, NULL, NULL }
void activity_destroy(Activity *a);
int activity_add_point(Activity *a, uint32_t timestamp, double latitude,
                       double longitude, int32_t altitude, uint32_t distance,
                       uint32_t speed, uint16_t power, int16_t grade,
                       uint8_t heart_rate, uint8_t cadence, uint8_t lr_balance,
                       int8_t temperature);
int activity_add_lap(uint32_t lap);

/*
 * torque, headwind, calories
 *
  uint16_t vertical_oscillation; // 10 * mm,
  uint16_t stance_time_percent; // 100 * percent,
  uint16_t stance_time; // 10 * ms,
  uint8_t left_torque_effectiveness; // 2 * percent,
  uint8_t right_torque_effectiveness; // 2 * percent,
  uint8_t left_pedal_smoothness; // 2 * percent,
  uint8_t right_pedal_smoothness; // 2 * percent,
  uint8_t combined_pedal_smoothness; // 2 * percent,
*/

/*

TCX:

activity: sport (RUnning, biking, other), id (dateTime), lap (starttime, total
time, distance, max speed, calories, avg hr, max hr, cadence, trigger method,
intensity, notes), notes,
multi sport session (first sport, next sport)
course point (time, position, alt metres)




*/

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
