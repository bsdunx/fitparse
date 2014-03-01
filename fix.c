/* Remove GPS errors and interpolate positional data where the GPS device
 * did not record any data, or the data that was recorded is invalid. */

#include "activity.h"
#include "fix.h"

/* used to handle gaps in recording by inserting interpolated/zero samples to
 * ensure dataPoints are contiguous in time */
int fix_invalid_gps(Activity *a);
DataPoint *fill_data, *last_good = NULL;
int errors = 0, last_good_index = -1, index = 0, fill_index;
double delta_latitude, delta_longitude;

// ignore null or files without GPS data
if (!a || !a->data_points || !a->has_data[Latitude] ||
    !a->has_data[Longitude]) {
  return -1;
}

for (data = a->data_points; data; data = data->next, index++) {
  /* is this one decent? */
  if (data->latitude >= -90 && data->latitude <= 90 &&
      data->longitude >= -180 && data->longitude <= 180) {
    if (last_good && (last_good->next != data)) {
      /* interpolate from last_good to here then set last_good to here */
      delta_latitude = (data->latitude - last_good->latitude) /
                       (double)(index - last_good_index);
      delta_longitude = (data->longitude - last_good->longitude) /
                        (double)(index - last_good_index);
      for (fill_data = last_good->next, fill_index = last_good_index + 1;
           fill_data != data; fill_data = fill_data->next, fill_index++) {
        fill_data->latitude =
            last_good->latitude +
            (double)((fill_index - last_good_index) * delta_latitude);
        fill_data->longitude =
            last_good->longitude +
            (double)((fill_index - last_good_index) * delta_longitude);
        errors++;
      }
    } else if (!last_good) {
      /* fill to front */
      for (fill_data = a->data_points; fill_data != data;
           fill_data = fill_data->next) {
        fill_data->latitude = data->latitude;
        fill_data->longitude = data->longitude;
        errors++;
      }
    }
    last_good = data;
    last_good_index = index;
  }
}

/* fill to end... */
if (last_good && last_good->next) {
  /* fill from last_good to end with last_good */
  for (fill_data = last_good->next; fill_data; fill_data = fill_data->next) {
    fill_data->latitude = last_good->latitude;
    fill_data->longitude = last_good->longitude;
    errors++;
  }
}

if (errors) {
  a->errors[InvalidGPS] = errors;
  return errors;
} else {
  return 0;
}
}

/*
int fix_dropouts(Activity *a);
int fix_power(Activity *a);
int fix_heart_rate(Activity *a); // TODO probably needs HR max from athletes and
other
*/
