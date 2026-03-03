var Settings = require('./config');

Pebble.addEventListener('showConfiguration', function() {
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
