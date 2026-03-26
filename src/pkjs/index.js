var Settings = require('./config');

// ============================================================
// WEATHER — Open-Meteo API (free, no API key required)
// Fetches current temperature (C, then converts to F) and WMO
// weather code. Called once on ready, then every 30 minutes.
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
        + '&temperature_unit=celsius'
        + '&forecast_days=1';

      var xhr = new XMLHttpRequest();
      xhr.onload = function() {
        try {
          var data = JSON.parse(this.responseText);
          var tempC = Math.round(data.current.temperature_2m);
          var tempF = Math.round(tempC * 9 / 5 + 32);
          var code  = data.current.weather_code;
          Pebble.sendAppMessage({
            WeatherTempF: tempF,
            WeatherTempC: tempC,
            WeatherCode:  code
          }, function() {
            console.log('Weather sent: ' + tempF + 'F / ' + tempC + 'C, code ' + code);
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
  // Refresh weather every 30 minutes, starting from when JS is ready
  setInterval(fetchWeather, 30 * 60 * 1000);
});

Pebble.addEventListener('showConfiguration', function() {
  // Detect B&W vs color platform to show the appropriate config UI
  var platform = 'color';
  try {
    if (Pebble.getActiveWatchInfo) {
      var info = Pebble.getActiveWatchInfo();
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
    }, function(e) {
      console.log('Error sending settings: ' + e.error.message);
    });
  }
});
