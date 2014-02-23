#include <stdint.h>
// TODO ifdef stuff...
typedef struct {
  //id; // startime

  sport; // ENUM ?what about 'multi'
  laps; // many points, always at least one
  data_points;
  //summary; // can include derived statistics (totalAscent, NP, avg)
} Activity;

typedef struct {
  uint32_t timestamp; // s since Unix Epoch - maybe up to 64 bits?
  int32_t latitude; // semicircles (?)
  int32_t longitude; // semicircles (?)
  uint32_t altitude; // altitude = 5 * m + 500
  uint32_t distance; // 100 *m
  uint32_t speed; // 1000* m/s
  uint16_t power; // watts
  int16_t grade; // 100 * %
  uint8_t heart_rate; // bpm
  uint8_t cadence; // rpm
  uint8_t lr_balance; // ?
  int8_t temperature; // C
} DataPoint;

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

activity: sport (RUnning, biking, other), id (dateTime), lap (starttime, total time, distance, max speed, calories, avg hr, max hr, cadence, trigger method, intensity, notes), notes,
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
