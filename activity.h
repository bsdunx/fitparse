typedef struct {
  //id; // startime

  sport; // what about 'multi'
  laps; // many points, always at least one
  //summary; // can include derived statistics (totalAscent, NP, avg)
} Activity;

typedef struct {
  timestamp;
  latitude;
  longitude;
  elevation; // altitude

  heart_rate;
  cadence;
  distance; // speed cadence sensor
  // speed
  // headwind
  // slope; (gradient)

  temperature;

  watts;
  torque; // nm
  balance;

  notes;
} Point; // TODO name

/*
 double secs, cad, hr, km, kph, nm, watts, alt, lon, lat, headwind, slope, temp, lrbalance;

GPX:

elevation
timestamp
lat
lon
name
desc

GPX extension:
HR
cadence
temp
distance
run
sensor
dataPt
lap


sportType
laps



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
