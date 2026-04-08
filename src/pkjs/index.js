var Settings = require('./config');

// ============================================================
// WEATHER + SOLAR — Open-Meteo API (free, no API key required)
//
// Fetches:
//   - current temperature (C and F) and WMO weather code
//   - today's sunrise/sunset and tomorrow's sunrise (for solar ring)
//
// Called once on ready, then every 30 minutes.
// Solar timestamps are sent as Unix integers (int32-safe through 2038).
// ============================================================
function fetchWeather() {
  navigator.geolocation.getCurrentPosition(
    function(pos) {
      var lat = pos.coords.latitude;
      var lon = pos.coords.longitude;
      var url = 'https://api.open-meteo.com/v1/forecast'
        + '?latitude='  + lat
        + '&longitude=' + lon
        + '&current=temperature_2m,weather_code'
        + '&daily=sunrise,sunset'
        + '&temperature_unit=celsius'
        + '&timezone=auto'
        + '&forecast_days=2';

      var xhr = new XMLHttpRequest();
      xhr.onload = function() {
        try {
          var data = JSON.parse(this.responseText);

          // Current weather
          var tempC = Math.round(data.current.temperature_2m);
          var tempF = Math.round(tempC * 9 / 5 + 32);
          var code  = data.current.weather_code;

          // Solar times -- Open-Meteo returns ISO strings like "2026-04-07T06:23"
          // Convert to Unix timestamps (seconds). JS Date parses ISO strings correctly
          // when timezone=auto is set (times are in the local timezone of the location).
          var sunriseTime = 0, sunsetTime = 0, sunriseTomorrow = 0;
          try {
            var daily = data.daily;
            if (daily && daily.sunrise && daily.sunset) {
              sunriseTime     = Math.round(new Date(daily.sunrise[0]).getTime() / 1000);
              sunsetTime      = Math.round(new Date(daily.sunset[0]).getTime()  / 1000);
              sunriseTomorrow = Math.round(new Date(daily.sunrise[1]).getTime() / 1000);
            }
          } catch(solarErr) {
            console.log('Solar parse error: ' + solarErr);
          }

          var msg = {
            WeatherTempF:    tempF,
            WeatherTempC:    tempC,
            WeatherCode:     code
          };
          // Only include solar data if successfully parsed
          if (sunriseTime > 0) {
            msg.SunriseTime     = sunriseTime;
            msg.SunsetTime      = sunsetTime;
            msg.SunriseTomorrow = sunriseTomorrow;
          }

          Pebble.sendAppMessage(msg, function() {
            console.log('Weather+solar sent: ' + tempF + 'F / ' + tempC + 'C,'
              + ' code ' + code
              + ', sunrise ' + sunriseTime + ', sunset ' + sunsetTime
              + ', tomorrow sunrise ' + sunriseTomorrow);
          }, function(e) {
            console.log('Weather send error: ' + e.error.message);
          });
        } catch(err) {
          console.log('Weather parse error: ' + err);
        }
      };
      xhr.open('GET', url);
      xhr.send();
    },
    function(err) {
      console.log('Geolocation error: ' + err.message);
    },
    { timeout: 15000, maximumAge: 300000 }
  );
}

// ============================================================
// PEBBLEKIT JS LIFECYCLE
// ============================================================
Pebble.addEventListener('ready', function() {
  console.log('PebbleKit JS ready');
  fetchWeather();
  // Refresh weather + solar every 30 minutes
  setInterval(fetchWeather, 30 * 60 * 1000);
});

Pebble.addEventListener('showConfiguration', function() {
  // Detect B&W vs color platform to show the appropriate config UI
  var platform = 'color';
  try {
    if (Pebble.getActiveWatchInfo) {
      var info = Pebble.getActiveWatchInfo();
      // gabbro (Pebble Round 2) is color -- only aplite/diorite/flint are B&W
      var bwPlatforms = ['aplite', 'diorite', 'flint'];
      platform = (bwPlatforms.indexOf(info.platform) >= 0) ? 'bw' : 'color';
    }
  } catch(e) {}
  var saved = localStorage.getItem('radium2_settings');
  Pebble.openURL(Settings.buildUrl(platform, saved ? JSON.parse(saved) : null));
});

Pebble.addEventListener('webviewclosed', function(e) {
  if (e.response && e.response !== 'CANCELLED') {
    var settings = JSON.parse(decodeURIComponent(e.response));
    localStorage.setItem('radium2_settings', JSON.stringify(settings));
    Pebble.sendAppMessage(settings, function() {
      console.log('Settings sent successfully');
    }, function(err) {
      console.log('Error sending settings: ' + err.error.message);
    });
  }
});
