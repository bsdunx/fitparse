/* Remove GPS errors and interpolate positional data where the GPS device
 * did not record any data, or the data that was recorded is invalid. */

/* used to handle gaps in recording by inserting interpolated/zero samples to ensure dataPoints are contiguous in time */
bool fix_gps (Activity *a)
{
  // ignore null or files without GPS data
  /*if (!ride || ride->areDataPresent()->lat == false || ride->areDataPresent()->lon == false) // TODO*/
  /*return false;*/

  int errors=0;

  DataPoint *last_good = NULL, *fill_data;  // where did we last have decent GPS data?
  int last_good_index = -1, index = 0;
  for (data = a->data_points; data; data = data->next, index++) {
    // is this one decent?
    if (data->latitude >= -90 && data->latitude <= 90 && data->longitude >= -180 && data->longitude <= 180) {
      if (last_good && (last_good->next != data)) {
        // interpolate from last good to here
        // then set last good to here
        double delta_lat = (data->lat - last_good->lat) / (double)(index-last_good_index);
        double delta_lon = (data->lon - last_good->lon) / (double)(index-last_good_index);
        for (fill_data = last_good->next; fill_data != data; fill_data = fill_data->next) { // TODO
            ride->command->setPointValue(j, RideFile::lat, ride->dataPoints()[lastgood]->lat + (double(j-lastgood)*deltaLat));
          ride->command->setPointValue(j, RideFile::lon, ride->dataPoints()[lastgood]->lon + (double(j-lastgood)*deltaLon));
          errors++;
        }
      } else if (!last_good) {
        // fill to front
        for (fill_data = a->data_points; fill_data != data; fill_data = fill_data->next) {
          fill_data->latitude = data->latitude;
          fill_data->longitude = data->longitude;
          errors++;
        }
      }
      last_good = data;
      last_good_index = index;
    }
  }

  // fill to end...
  if (!last_good && last_good != a->last_point) {
    // fill from last_good to end with last_good
    for (fill_data = last_good->next; fill_data; fill_data = fill_data->next) {
      fill_data->latitude = last_good->latitude;
      fill_data->longitude = last_good->longitude;
      errors++;
    }
  }

  if (errors) {
    ride->setTag("GPS errors", QString("%1").arg(errors));
    return true;
  } else {
    return false;
  }
}
