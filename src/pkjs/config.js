module.exports = {
  buildUrl: function(platform, currentSettings) {

    // ----------------------------------------------------------------
    // PRESETS
    // Each preset defines a complete color theme.
    //
    // Key:
    //   bg   = background        obg  = overlay background
    //   tt   = time text (TimeColor)
    //   lH   = lit hour ticks    lM   = lit minute ticks
    //   lB   = lit battery ring  lS   = lit steps ring
    //   dH   = dim hour ticks    dM   = dim minute ticks
    //   dB   = dim battery ring  dS   = dim steps ring
    //   tH   = hour tip highlight  tM = minute tip highlight
    //   l1/4 = outer info line colors  l2/3 = inner info line colors
    //
    // Design mix: ~half have active tips; ~1/3 have split lH/lM colors
    // ----------------------------------------------------------------
    var presets = [
      // ---- DARK (8) -- Radium first, then the 2015 tweet schemes ----
      // Radium (classic): ticks/ring=#aaffaa (GColorMintGreen), tips=white
      // Radium+ (v2.2 bright): ticks/ring=#00ff00 (GColorGreen), tips=#aaffaa
      { label:'Radium',  bg:'#000000',obg:'#000000',tt:'#ffffff',
        lH:'#aaffaa',lM:'#aaffaa',lB:'#aaffaa',lS:'#aaffaa',
        dH:'#005500',dM:'#005500',dB:'#005500',dS:'#005500',
        tH:'#ffffff',tM:'#ffffff',
        l1:'#aaaaaa',l2:'#aaffaa',l3:'#aaffaa',l4:'#aaaaaa' },
      { label:'Scarlet', bg:'#000000',obg:'#000000',tt:'#ffffff',
        lH:'#ff0000',lM:'#ff0000',lB:'#ff0000',lS:'#ff0000',
        dH:'#550000',dM:'#550000',dB:'#550000',dS:'#550000',
        tH:'#ffaaaa',tM:'#ffaaaa',
        l1:'#aaaaaa',l2:'#ff5555',l3:'#ff5555',l4:'#aaaaaa' },
      { label:'Ember',   bg:'#000000',obg:'#000000',tt:'#ffffff',
        lH:'#ff5500',lM:'#ff5500',lB:'#ff5500',lS:'#ff5500',
        dH:'#555555',dM:'#555555',dB:'#555555',dS:'#555555',
        tH:'#ffaa55',tM:'#ffff00',
        l1:'#aaaaaa',l2:'#ffaa55',l3:'#ffaa55',l4:'#aaaaaa' },
      { label:'Cobalt',  bg:'#000000',obg:'#000000',tt:'#ffffff',
        lH:'#0055ff',lM:'#0055ff',lB:'#0055ff',lS:'#0055ff',
        dH:'#555555',dM:'#555555',dB:'#555555',dS:'#555555',
        tH:'#aaaaff',tM:'#aaaaff',
        l1:'#aaaaaa',l2:'#aaaaff',l3:'#aaaaff',l4:'#aaaaaa' },
      { label:'Crimson', bg:'#000000',obg:'#000000',tt:'#ffffff',
        lH:'#ff5555',lM:'#ff5555',lB:'#ff5555',lS:'#ff5555',
        dH:'#550000',dM:'#550000',dB:'#550000',dS:'#550000',
        tH:'#ffaaaa',tM:'#ffaaaa',
        l1:'#aa5555',l2:'#ffaaaa',l3:'#ffaaaa',l4:'#aa5555' },
      { label:'Volt',    bg:'#000000',obg:'#000000',tt:'#ffffff',
        lH:'#ffff00',lM:'#ffff00',lB:'#ffff00',lS:'#ffff00',
        dH:'#555500',dM:'#555500',dB:'#555500',dS:'#555500',
        tH:'#ffff00',tM:'#ffff00',
        l1:'#aaaa55',l2:'#ffffaa',l3:'#ffffaa',l4:'#aaaa55' },
      { label:'Slate',   bg:'#000000',obg:'#000000',tt:'#ffffff',
        lH:'#ffffff',lM:'#aaaaaa',lB:'#aaaaaa',lS:'#aaaaaa',
        dH:'#555555',dM:'#555555',dB:'#555555',dS:'#555555',
        tH:'#ffffff',tM:'#ffffff',
        l1:'#aaaaaa',l2:'#aaaaaa',l3:'#aaaaaa',l4:'#aaaaaa' },
      { label:'Dusk',    bg:'#000000',obg:'#000000',tt:'#ffffff',
        lH:'#ff55ff',lM:'#ff55ff',lB:'#ff55ff',lS:'#ff55ff',
        dH:'#550055',dM:'#550055',dB:'#550055',dS:'#550055',
        tH:'#ffaaff',tM:'#ffaaff',
        l1:'#aa55aa',l2:'#ffaaff',l3:'#ffaaff',l4:'#aa55aa' },

      // ---- DARK+ (8) ----
      { label:'Ocean',   bg:'#000000',obg:'#000000',tt:'#ffffff',
        lH:'#00aaff',lM:'#00ffff',lB:'#00aaff',lS:'#00aaff',
        dH:'#0055aa',dM:'#005555',dB:'#0055aa',dS:'#0055aa',
        tH:'#aaffff',tM:'#ffffff',
        l1:'#0055aa',l2:'#aaffff',l3:'#aaffff',l4:'#0055aa' },
      { label:'Aurora',  bg:'#000000',obg:'#000000',tt:'#ffffff',
        lH:'#00ff55',lM:'#00ffff',lB:'#00aaff',lS:'#aaffaa',
        dH:'#005500',dM:'#005555',dB:'#005555',dS:'#005500',
        tH:'#aaffaa',tM:'#aaffff',
        l1:'#aaaaaa',l2:'#aaffff',l3:'#aaffaa',l4:'#aaaaaa' },
      { label:'Solar',   bg:'#000000',obg:'#000000',tt:'#ffaa55',
        lH:'#ff5500',lM:'#ffaa00',lB:'#ffff00',lS:'#aaff00',
        dH:'#550000',dM:'#555500',dB:'#555500',dS:'#005500',
        tH:'#ffff00',tM:'#ffff00',
        l1:'#aa5500',l2:'#ffaa00',l3:'#ffaa00',l4:'#aa5500' },
      { label:'Venom',   bg:'#000000',obg:'#000000',tt:'#ffffff',
        lH:'#aaff00',lM:'#aa00ff',lB:'#55ff00',lS:'#5500aa',
        dH:'#005500',dM:'#550055',dB:'#005500',dS:'#550055',
        tH:'#aaff00',tM:'#aa00ff',
        l1:'#aaaaaa',l2:'#aaff55',l3:'#aa55ff',l4:'#aaaaaa' },
      { label:'Reactor', bg:'#000000',obg:'#000000',tt:'#ffffff',
        lH:'#00ffff',lM:'#ff0000',lB:'#00aaff',lS:'#ff5500',
        dH:'#005555',dM:'#550000',dB:'#005555',dS:'#550000',
        tH:'#aaffff',tM:'#ffaaaa',
        l1:'#aaaaaa',l2:'#aaffff',l3:'#ffaaaa',l4:'#aaaaaa' },
      { label:'Neon',    bg:'#000000',obg:'#000000',tt:'#ffffff',
        lH:'#00ff00',lM:'#ff00ff',lB:'#00ffff',lS:'#ff55ff',
        dH:'#005500',dM:'#550055',dB:'#005555',dS:'#550055',
        tH:'#00ff00',tM:'#ff00ff',
        l1:'#aaaaaa',l2:'#aaffaa',l3:'#ffaaff',l4:'#aaaaaa' },
      { label:'Blossom', bg:'#000000',obg:'#000000',tt:'#ffffff',
        lH:'#aaffaa',lM:'#ff55aa',lB:'#55ffaa',lS:'#ff0055',
        dH:'#005500',dM:'#550055',dB:'#005500',dS:'#aa0055',
        tH:'#ffffff',tM:'#ffffff',
        l1:'#aaaaaa',l2:'#aaffaa',l3:'#ff55aa',l4:'#aaaaaa' },
      { label:'Hearth',  bg:'#000000',obg:'#000000',tt:'#ffaa55',
        lH:'#ff5500',lM:'#ff5500',lB:'#ffaa00',lS:'#ffaa00',
        dH:'#550000',dM:'#550000',dB:'#aa5500',dS:'#550000',
        tH:'#ffff00',tM:'#ffff00',
        l1:'#ffaa55',l2:'#ffaa55',l3:'#ffaa55',l4:'#ffaa55' },

      // ---- LIGHT (8) ----
      { label:'Paper',    bg:'#ffffff',obg:'#ffffff',tt:'#000000',
        lH:'#000000',lM:'#000000',lB:'#000000',lS:'#000000',
        dH:'#aaaaaa',dM:'#aaaaaa',dB:'#aaaaaa',dS:'#aaaaaa',
        tH:'#555555',tM:'#555555',
        l1:'#555555',l2:'#555555',l3:'#555555',l4:'#555555' },
      { label:'Jade',     bg:'#ffffff',obg:'#ffffff',tt:'#000000',
        lH:'#00aa55',lM:'#00aa55',lB:'#00aa55',lS:'#00aa55',
        dH:'#aaaaaa',dM:'#aaaaaa',dB:'#aaaaaa',dS:'#aaaaaa',
        tH:'#005500',tM:'#005500',
        l1:'#aaaaaa',l2:'#00aa55',l3:'#00aa55',l4:'#aaaaaa' },
      { label:'Sapphire', bg:'#ffffff',obg:'#ffffff',tt:'#000000',
        lH:'#0055aa',lM:'#0055aa',lB:'#0055aa',lS:'#0055aa',
        dH:'#aaaaaa',dM:'#aaaaaa',dB:'#aaaaaa',dS:'#aaaaaa',
        tH:'#000055',tM:'#000055',
        l1:'#aaaaaa',l2:'#0055aa',l3:'#0055aa',l4:'#aaaaaa' },
      { label:'Ruby',     bg:'#ffffff',obg:'#ffffff',tt:'#000000',
        lH:'#aa0000',lM:'#aa0000',lB:'#aa0000',lS:'#aa0000',
        dH:'#aaaaaa',dM:'#aaaaaa',dB:'#aaaaaa',dS:'#aaaaaa',
        tH:'#550000',tM:'#550000',
        l1:'#aaaaaa',l2:'#aa0000',l3:'#aa0000',l4:'#aaaaaa' },
      { label:'Mint',     bg:'#ffffff',obg:'#ffffff',tt:'#000000',
        lH:'#005500',lM:'#005500',lB:'#005500',lS:'#005500',
        dH:'#aaffaa',dM:'#aaffaa',dB:'#aaffaa',dS:'#aaffaa',
        tH:'#000000',tM:'#000000',
        l1:'#000000',l2:'#005500',l3:'#005500',l4:'#000000' },
      { label:'Navy',     bg:'#ffffff',obg:'#ffffff',tt:'#000000',
        lH:'#0000aa',lM:'#ffffff',lB:'#0000aa',lS:'#0000aa',
        dH:'#aaaaff',dM:'#aaaaaa',dB:'#aaaaff',dS:'#aaaaff',
        tH:'#000055',tM:'#ffffff',
        l1:'#aaaaaa',l2:'#0000aa',l3:'#0000aa',l4:'#aaaaaa' },
      { label:'Rose',     bg:'#ffffff',obg:'#ffffff',tt:'#550000',
        lH:'#550000',lM:'#550000',lB:'#550000',lS:'#550000',
        dH:'#ffaaaa',dM:'#ffaaaa',dB:'#ffaaaa',dS:'#ffaaaa',
        tH:'#000000',tM:'#000000',
        l1:'#000000',l2:'#550000',l3:'#550000',l4:'#000000' },
      { label:'Sepia',    bg:'#ffffaa',obg:'#ffffaa',tt:'#000000',
        lH:'#550000',lM:'#550000',lB:'#550000',lS:'#550000',
        dH:'#ffaa55',dM:'#ffaa55',dB:'#ffaa55',dS:'#ffaa55',
        tH:'#000000',tM:'#000000',
        l1:'#aa5500',l2:'#aa5500',l3:'#aa5500',l4:'#aa5500' },

      // ---- COLOR (8) ----
      { label:'Teal',        bg:'#00aaaa',obg:'#00aaaa',tt:'#ffffff',
        lH:'#ffffff',lM:'#ffffff',lB:'#ffffff',lS:'#ffffff',
        dH:'#005555',dM:'#005555',dB:'#005555',dS:'#005555',
        tH:'#aaffff',tM:'#aaffff',
        l1:'#aaffff',l2:'#ffffff',l3:'#ffffff',l4:'#aaffff' },
      { label:'Flame',       bg:'#ff5500',obg:'#ff5500',tt:'#ffffff',
        lH:'#ffffff',lM:'#ffffff',lB:'#ffffff',lS:'#ffffff',
        dH:'#aa5500',dM:'#aa5500',dB:'#aa5500',dS:'#aa5500',
        tH:'#ffff00',tM:'#ffff00',
        l1:'#ffff00',l2:'#ffffff',l3:'#ffffff',l4:'#ffff00' },
      { label:'Midnight',    bg:'#0000aa',obg:'#0000aa',tt:'#aaaaff',
        lH:'#00ffff',lM:'#aaaaff',lB:'#00aaff',lS:'#aaaaff',
        dH:'#0055aa',dM:'#0055aa',dB:'#0055aa',dS:'#0055aa',
        tH:'#ffffff',tM:'#ffffff',
        l1:'#00ffff',l2:'#aaaaff',l3:'#aaaaff',l4:'#00ffff' },
      { label:'Forest',      bg:'#005500',obg:'#005500',tt:'#ffffff',
        lH:'#55ff00',lM:'#55ff00',lB:'#55ff00',lS:'#55ff00',
        dH:'#55aa00',dM:'#55aa00',dB:'#55aa00',dS:'#55aa00',
        tH:'#ffffff',tM:'#ffffff',
        l1:'#aaaaaa',l2:'#55ff00',l3:'#55ff00',l4:'#aaaaaa' },
      { label:'Plum',        bg:'#550055',obg:'#550055',tt:'#ffffff',
        lH:'#ff55ff',lM:'#ff55ff',lB:'#ff55ff',lS:'#ff55ff',
        dH:'#aa00aa',dM:'#aa00aa',dB:'#aa00aa',dS:'#aa00aa',
        tH:'#ffffff',tM:'#ffffff',
        l1:'#aa55aa',l2:'#ffaaff',l3:'#ffaaff',l4:'#aa55aa' },
      { label:'Cinnabar',    bg:'#550000',obg:'#550000',tt:'#ffffff',
        lH:'#ffaa55',lM:'#ff5500',lB:'#ffaa00',lS:'#ff5500',
        dH:'#aa0000',dM:'#aa0000',dB:'#aa5500',dS:'#aa0000',
        tH:'#ffffff',tM:'#ffff00',
        l1:'#aaaaaa',l2:'#ffaa55',l3:'#ffaa55',l4:'#aaaaaa' },
      { label:'Ultraviolet', bg:'#550055',obg:'#550055',tt:'#ffffff',
        lH:'#aa00ff',lM:'#ff00ff',lB:'#ff55ff',lS:'#aa00ff',
        dH:'#5500aa',dM:'#550055',dB:'#5500aa',dS:'#550055',
        tH:'#ffffff',tM:'#ffffff',
        l1:'#aa55ff',l2:'#ffaaff',l3:'#ffaaff',l4:'#aa55ff' },
      { label:'Ash',         bg:'#555555',obg:'#555555',tt:'#ffffff',
        lH:'#ffffff',lM:'#ffffff',lB:'#ffffff',lS:'#ffffff',
        dH:'#aaaaaa',dM:'#aaaaaa',dB:'#aaaaaa',dS:'#aaaaaa',
        tH:'#ffffff',tM:'#ffffff',
        l1:'#aaaaaa',l2:'#ffffff',l3:'#ffffff',l4:'#aaaaaa' },

      // ---- SPECIAL (8) ----
      { label:'Boreal',   bg:'#000000',obg:'#0000aa',tt:'#aaaaff',
        lH:'#00aaff',lM:'#55ffff',lB:'#00ffff',lS:'#55aaff',
        dH:'#0000aa',dM:'#005555',dB:'#005555',dS:'#0000aa',
        tH:'#ffffff',tM:'#ffffff',
        l1:'#aaaaff',l2:'#55ffff',l3:'#55ffff',l4:'#aaaaff' },
      { label:'Cosmos',   bg:'#0000aa',obg:'#0000aa',tt:'#aaaaff',
        lH:'#aa00ff',lM:'#ff00ff',lB:'#ff55aa',lS:'#ff0055',
        dH:'#550055',dM:'#550055',dB:'#550055',dS:'#550000',
        tH:'#ffffff',tM:'#ffffff',
        l1:'#aa55ff',l2:'#ff55ff',l3:'#ff55ff',l4:'#aa55ff' },
      { label:'Horizon',  bg:'#000000',obg:'#0000aa',tt:'#ffaa55',
        lH:'#0055ff',lM:'#ff5500',lB:'#0055aa',lS:'#aa5500',
        dH:'#0000aa',dM:'#550000',dB:'#0000aa',dS:'#550000',
        tH:'#aaaaff',tM:'#ffff00',
        l1:'#aaaaaa',l2:'#aaaaaa',l3:'#aaaaaa',l4:'#aaaaaa' },
      { label:'Inferno',  bg:'#000000',obg:'#000000',tt:'#ffffff',
        lH:'#ff0000',lM:'#ff5500',lB:'#ffaa00',lS:'#aaff00',
        dH:'#550000',dM:'#550000',dB:'#555500',dS:'#005500',
        tH:'#ffff00',tM:'#ffff00',
        l1:'#aaaaaa',l2:'#ffaa00',l3:'#ffaa00',l4:'#aaaaaa' },
      { label:'Triadic',  bg:'#000000',obg:'#000000',tt:'#ffffff',
        lH:'#ff0000',lM:'#0055ff',lB:'#00ff00',lS:'#aa00ff',
        dH:'#550000',dM:'#0000aa',dB:'#005500',dS:'#550055',
        tH:'#ffaa55',tM:'#aaaaff',
        l1:'#aaaaaa',l2:'#ffaa55',l3:'#aaaaff',l4:'#aaaaaa' },
      { label:'GoldEye',  bg:'#000000',obg:'#005500',tt:'#ffffff',
        lH:'#ff5500',lM:'#00aaff',lB:'#aaff00',lS:'#aaff00',
        dH:'#550000',dM:'#0000aa',dB:'#005500',dS:'#005500',
        tH:'#ffff00',tM:'#aaffff',
        l1:'#aaff00',l2:'#aaff00',l3:'#aaff00',l4:'#aaff00' },
      { label:'Rainbow',  bg:'#000000',obg:'#000000',tt:'#ffffff',
        lH:'#ff0000',lM:'#ffaa00',lB:'#00ff00',lS:'#00ffff',
        dH:'#550000',dM:'#aa5500',dB:'#005500',dS:'#005555',
        tH:'#ffff00',tM:'#ffff00',
        l1:'#aaaaaa',l2:'#ffaa55',l3:'#aaaaaa',l4:'#aaaaaa' },
      // Radium+: v2.2 bright green -- ticks=#00ff00, tips=#aaffaa, outer lines=#00ff00
      { label:'Radium+',  bg:'#000000',obg:'#000000',tt:'#ffffff',
        lH:'#00ff00',lM:'#00ff00',lB:'#00ff00',lS:'#00ff00',
        dH:'#005500',dM:'#005500',dB:'#005500',dS:'#005500',
        tH:'#aaffaa',tM:'#aaffaa',
        l1:'#00ff00',l2:'#aaffaa',l3:'#aaffaa',l4:'#00ff00' },
    ];

    var palette = [
      '#000000','#555555','#aaaaaa','#ffffff','#550000','#aa5555','#aa0000','#ffaaaa',
      '#ff5555','#ff0000','#aa5500','#ffaa55','#ff5500','#ffaa00','#555500','#aaaa55',
      '#aaaa00','#ffffaa','#ffff55','#ffff00','#55aa00','#aaff55','#55ff00','#aaff00',
      '#005500','#55aa55','#00aa00','#aaffaa','#55ff55','#00ff00','#00aa55','#55ffaa',
      '#00ff55','#00ffaa','#005555','#55aaaa','#00aaaa','#aaffff','#55ffff','#00ffff',
      '#0055aa','#55aaff','#0055ff','#00aaff','#000055','#5555aa','#0000aa','#aaaaff',
      '#5555ff','#0000ff','#5500aa','#aa55ff','#5500ff','#aa00ff','#550055','#aa55aa',
      '#aa00aa','#ffaaff','#ff55ff','#ff00ff','#aa0055','#ff55aa','#ff0055','#ff00aa',
    ];
    var paletteNames = {
      '#000000':'GColorBlack','#000055':'GColorOxfordBlue','#0000aa':'GColorDukeBlue','#0000ff':'GColorBlue',
      '#005500':'GColorDarkGreen','#005555':'GColorMidnightGreen','#0055aa':'GColorCobaltBlue','#0055ff':'GColorBlueMoon',
      '#00aa00':'GColorIslamicGreen','#00aa55':'GColorJaegerGreen','#00aaaa':'GColorTiffanyBlue','#00aaff':'GColorVividCerulean',
      '#00ff00':'GColorGreen','#00ff55':'GColorMalachite','#00ffaa':'GColorMediumSpringGreen','#00ffff':'GColorCyan',
      '#550000':'GColorBulgarianRose','#550055':'GColorImperialPurple','#5500aa':'GColorIndigo','#5500ff':'GColorElectricUltramarine',
      '#555500':'GColorArmyGreen','#555555':'GColorDarkGray','#5555aa':'GColorLiberty','#5555ff':'GColorVeryLightBlue',
      '#55aa00':'GColorKellyGreen','#55aa55':'GColorMayGreen','#55aaaa':'GColorCadetBlue','#55aaff':'GColorPictonBlue',
      '#55ff00':'GColorBrightGreen','#55ff55':'GColorScreaminGreen','#55ffaa':'GColorMediumAquamarine','#55ffff':'GColorElectricBlue',
      '#aa0000':'GColorDarkCandyAppleRed','#aa0055':'GColorJazzberryJam','#aa00aa':'GColorPurple','#aa00ff':'GColorVividViolet',
      '#aa5500':'GColorWindsorTan','#aa5555':'GColorRoseVale','#aa55aa':'GColorPurpureus','#aa55ff':'GColorLavenderIndigo',
      '#aaaa00':'GColorLimerick','#aaaa55':'GColorBrass','#aaaaaa':'GColorLightGray','#aaaaff':'GColorBabyBlueEyes',
      '#aaff00':'GColorSpringBud','#aaff55':'GColorInchworm','#aaffaa':'GColorMintGreen','#aaffff':'GColorCeleste',
      '#ff0000':'GColorRed','#ff0055':'GColorFolly','#ff00aa':'GColorFashionMagenta','#ff00ff':'GColorMagenta',
      '#ff5500':'GColorOrange','#ff5555':'GColorSunsetOrange','#ff55aa':'GColorBrilliantRose','#ff55ff':'GColorShockingPink',
      '#ffaa00':'GColorChromeYellow','#ffaa55':'GColorRajah','#ffaaaa':'GColorMelon','#ffaaff':'GColorRichBrilliantLavender',
      '#ffff00':'GColorYellow','#ffff55':'GColorIcterine','#ffffaa':'GColorPastelYellow','#ffffff':'GColorWhite'
    };

    // Field options for the 4 info line dropdowns.
    // Inner lines (2 & 3) get all options; outer lines (1 & 4) omit text-only fields.
    var fieldOptionsInner = [
      { value: 0, label: 'None' },
      { value: 1, label: 'Day' },
      { value: 2, label: 'Date' },
      { value: 3, label: 'Day + Date' },
      { value: 4, label: 'Steps' },
      { value: 5, label: 'Temp (F)' },
      { value: 6, label: 'Temp (C)' },
      { value: 7, label: 'Battery' },
      { value: 8, label: 'Distance' },
      { value: 9, label: 'Calories' },
    ];
    var fieldOptionsOuter = [
      { value: 0, label: 'None' },
      { value: 2, label: 'Date' },
      { value: 4, label: 'Steps' },
      { value: 5, label: 'Temp (F)' },
      { value: 6, label: 'Temp (C)' },
      { value: 7, label: 'Battery' },
      { value: 8, label: 'Distance' },
      { value: 9, label: 'Calories' },
    ];

    function makeSelect(id, defaultVal, options) {
      var opts = options.map(function(o) {
        return '<option value="' + o.value + '"' + (o.value === defaultVal ? ' selected' : '') + '>' + o.label + '</option>';
      }).join('');
      return '<select id="' + id + '" style="background:#242424;color:#ddd;border:1px solid #333;border-radius:6px;padding:6px 8px;font-size:14px;flex:1">' + opts + '</select>';
    }

    // Large overlay toggle shown only for emery and chalk (chalk = gabbro in CloudPebble)
    var isLargePlatform = (platform === 'emery' || platform === 'chalk');
    var isAplite = (platform === 'aplite'); // aplite has no health service
    var overlayLargeRow = isLargePlatform
      ? '<div class="row"><label>Large overlay</label>' +
        '<label class="toggle"><input type="checkbox" id="OverlaySize"><span class="knob"></span></label></div>'
      : '';

    var platformData = 'var PLATFORM=' + JSON.stringify(platform || 'color') + ';'
      + 'var CURRENT=' + JSON.stringify(currentSettings || null) + ';';
    var presetsData  = 'var PRESETS=' + JSON.stringify(presets) + ';';
    var paletteData  = 'var PALETTE=' + JSON.stringify(palette) + ';var PALETTE_NAMES=' + JSON.stringify(paletteNames) + ';';

    var presetRows = [
      { label: 'Dark',    presets: presets.slice(0,  8)  },
      { label: 'Dark+',   presets: presets.slice(8,  16) },
      { label: 'Light',   presets: presets.slice(16, 24) },
      { label: 'Color',   presets: presets.slice(24, 32) },
      { label: 'Special', presets: presets.slice(32, 40) },
    ];
    var presetsHtml = presetRows.map(function(row) {
      var rowItems = row.presets.map(function(p) {
        var i = presets.indexOf(p);
        var pipBg = (p.lH === p.lM)
          ? p.lH
          : 'linear-gradient(90deg,' + p.lH + ' 50%,' + p.lM + ' 50%)';
        return '<div class="preset" onclick="applyPreset(' + i + ')" style="background:' + p.bg + ';border:2px solid ' + p.lM + '">'
          + '<div class="preset-pip" style="background:' + pipBg + '"></div>'
          + '<div class="preset-label" style="color:' + p.tt + '">' + p.label + '</div>'
          + '</div>';
      }).join('');
      return '<div class="preset-row-label">' + row.label + '</div>'
        + '<div class="preset-row">' + rowItems + '</div>';
    }).join('');

    var html = '<!DOCTYPE html><html><head>'
      + '<meta name="viewport" content="width=device-width,initial-scale=1">'
      + '<title>Radium 2</title>'
      + '<style>'
      + '*{box-sizing:border-box}'
      + 'body{font-family:sans-serif;background:#111;color:#fff;margin:0;padding:16px;max-width:480px}'
      + 'h1{font-size:24px;margin:0 0 2px;letter-spacing:-0.5px}'
      + 'h1 span{color:#aaffaa}'
      + 'p.sub{color:#555;font-size:13px;margin:0 0 20px}'
      + 'h2{font-size:11px;text-transform:uppercase;color:#555;letter-spacing:1.5px;margin:20px 0 6px}'
      + '.card{background:#1a1a1a;border-radius:10px;overflow:hidden;margin-bottom:8px}'
      + '.row{display:flex;align-items:center;justify-content:space-between;padding:11px 14px;border-bottom:1px solid #222}'
      + '.row:last-child{border-bottom:none}'
      + '.row label{font-size:15px;color:#ddd;flex:1}'
      + '.field-row{display:flex;align-items:center;justify-content:space-between;padding:10px 14px;border-bottom:1px solid #222;gap:12px}'
      + '.field-row:last-child{border-bottom:none}'
      + '.field-row label{font-size:14px;color:#aaa;white-space:nowrap;min-width:60px}'
      + '.swatch{width:32px;height:24px;border-radius:4px;cursor:pointer;border:2px solid #333;flex-shrink:0}'
      + '.expand-row{display:flex;align-items:center;justify-content:space-between;padding:9px 14px;border-bottom:1px solid #222;cursor:pointer;user-select:none}'
      + '.expand-row:last-child{border-bottom:none}'
      + '.expand-row label{font-size:13px;font-weight:bold;text-transform:uppercase;letter-spacing:0.06em;color:#fff;flex:1;cursor:pointer}'
      + '.expand-row .right{display:flex;align-items:center;gap:8px}'
      + '.expand-btn{font-size:18px;color:#555;line-height:1;width:24px;text-align:center;transition:transform .2s}'
      + '.expand-btn.open{transform:rotate(45deg);color:#aaffaa}'
      + '.sub-rows{display:none;background:#141414}'
      + '.sub-rows.open{display:block}'
      + '.sub-row{display:flex;align-items:center;justify-content:space-between;padding:7px 14px 7px 28px;border-bottom:1px solid #1e1e1e}'
      + '.sub-row:last-child{border-bottom:none}'
      + '.sub-row label{font-size:14px;color:#aaa;flex:1}'
      + '.sub-expand-row{display:flex;align-items:center;justify-content:space-between;padding:7px 14px 7px 28px;border-bottom:1px solid #1e1e1e;cursor:pointer;user-select:none}'
      + '.sub-expand-row label{font-size:14px;color:#aaa;flex:1;cursor:pointer}'
      + '.sub-expand-row .right{display:flex;align-items:center;gap:8px}'
      + '.sub-expand-btn{font-size:15px;color:#444;line-height:1;width:20px;text-align:center;transition:transform .15s}'
      + '.sub-expand-btn.open{transform:rotate(45deg);color:#aaffaa}'
      + '.sub-sub-rows{display:none;background:#0e0e0e}'
      + '.sub-sub-rows.open{display:block}'
      + '.sub-sub-row{display:flex;align-items:center;justify-content:space-between;padding:6px 14px 6px 42px;border-bottom:1px solid #181818}'
      + '.sub-sub-row:last-child{border-bottom:none}'
      + '.sub-sub-row label{font-size:13px;color:#888;flex:1}'
      + '.sub-sub-expand-row{display:flex;align-items:center;justify-content:space-between;padding:6px 14px 6px 42px;border-bottom:1px solid #181818;cursor:pointer;user-select:none}'
      + '.sub-sub-expand-row label{font-size:13px;color:#888;flex:1;cursor:pointer}'
      + '.sub-sub-expand-row .right{display:flex;align-items:center;gap:8px}'
      + '.sub-sub-expand-btn{font-size:13px;color:#333;line-height:1;width:18px;text-align:center;transition:transform .15s}'
      + '.sub-sub-expand-btn.open{transform:rotate(45deg);color:#aaffaa}'
      + '.sub-sub-sub-rows{display:none;background:#090909}'
      + '.sub-sub-sub-rows.open{display:block}'
      + '.sub-sub-sub-row{display:flex;align-items:center;justify-content:space-between;padding:5px 14px 5px 56px;border-bottom:1px solid #141414}'
      + '.sub-sub-sub-row:last-child{border-bottom:none}'
      + '.sub-sub-sub-row label{font-size:12px;color:#666;flex:1}'
      + '.radio-group{display:flex;gap:6px;padding:10px 14px}'
      + '.radio-group label{flex:1;text-align:center;padding:8px 4px;border-radius:7px;background:#242424;font-size:13px;cursor:pointer;color:#aaa;border:2px solid transparent}'
      + '.radio-group input{display:none}'
      + '.radio-group input:checked+label{background:#aaffaa;color:#000;font-weight:bold}'
      + '.toggle{position:relative;width:44px;height:26px;flex-shrink:0}'
      + '.toggle input{opacity:0;width:0;height:0}'
      + '.knob{position:absolute;cursor:pointer;top:0;left:0;right:0;bottom:0;background:#333;border-radius:13px;transition:.2s}'
      + '.knob:before{content:"";position:absolute;width:20px;height:20px;left:3px;bottom:3px;background:#666;border-radius:50%;transition:.2s}'
      + 'input:checked+.knob{background:#aaffaa}'
      + 'input:checked+.knob:before{transform:translateX(18px);background:#000}'
      + '.slider-wrap{padding:10px 14px}'
      + '.slider-lbl{font-size:15px;color:#ddd;display:flex;justify-content:space-between;margin-bottom:8px}'
      + '.slider-lbl span{color:#aaffaa;font-weight:bold}'
      + 'input[type=range]{width:100%;accent-color:#aaffaa}'
      + '.note{font-size:12px;color:#444;padding:4px 14px 10px;display:block}'
      + '.presets{padding:4px 14px 10px}'
      + '.preset-row-label{font-size:10px;text-transform:uppercase;letter-spacing:0.08em;color:#888;padding:8px 0 4px}'
      + '.preset-row{display:flex;gap:6px;margin-bottom:4px}'
      + '.preset{flex:1;border-radius:8px;padding:8px 2px;cursor:pointer;display:flex;flex-direction:column;align-items:center;gap:5px}'
      + '.preset:active{opacity:0.7}'
      + '.preset-pip{width:20px;height:3px;border-radius:2px}'
      + '.preset-label{font-size:10px}'
      + '.modal-bg{display:none;position:fixed;top:0;left:0;right:0;bottom:0;background:rgba(0,0,0,.8);z-index:100;align-items:center;justify-content:center}'
      + '.modal-bg.open{display:flex}'
      + '.modal{background:#1a1a1a;border-radius:12px;padding:16px;width:90%;max-width:340px}'
      + '.modal h3{margin:0 0 12px;font-size:14px;color:#aaa;text-transform:uppercase;letter-spacing:1px}'
      + '.palette{display:grid;grid-template-columns:repeat(8,1fr);gap:4px}'
      + '.pal-swatch{width:100%;aspect-ratio:1;border-radius:3px;cursor:pointer;border:2px solid transparent}'
      + '.pal-swatch:hover,.pal-swatch.selected{border-color:#fff}'
      + '.modal-cancel{margin-top:12px;width:100%;padding:10px;background:#333;color:#aaa;border:none;border-radius:7px;font-size:14px;cursor:pointer}'
      + 'button.save{display:block;width:100%;padding:14px;background:#aaffaa;color:#000;border:none;border-radius:8px;font-size:17px;font-weight:bold;cursor:pointer;margin-top:24px}'
      + 'button.save:active{opacity:0.8}'
      + '</style></head><body>'

      + '<h1>Radium <span>2</span></h1><p class="sub">Watchface Configuration</p>'

      + '<h2>Info Overlay</h2><div class="card">'
      + '<div class="radio-group">'
      + '<input type="radio" name="overlay" id="ov0" value="0"><label for="ov0">Always On</label>'
      + '<input type="radio" name="overlay" id="ov1" value="1"><label for="ov1">Always Off</label>'
      + '<input type="radio" name="overlay" id="ov2" value="2" checked><label for="ov2">Shake</label>'
      + '<input type="radio" name="overlay" id="ov3" value="3"><label for="ov3">1 min</label>'
      + '</div>'
      + overlayLargeRow
      + '</div>'

      + '<h2>Info Lines</h2><div class="card">'
      + '<div class="field-row"><label>Line 1</label>' + makeSelect('Line1Field', 0, fieldOptionsOuter) + '</div>'
      + '<div class="field-row"><label>Line 2</label>' + makeSelect('Line2Field', 1, fieldOptionsInner) + '</div>'
      + '<div class="field-row"><label>Line 3</label>' + makeSelect('Line3Field', 2, fieldOptionsInner) + '</div>'
      + '<div class="field-row"><label>Line 4</label>' + makeSelect('Line4Field', 0, fieldOptionsOuter) + '</div>'
      + '</div>'

      + '<div id="color-section">'
      + '<h2>Presets</h2><div class="card"><div class="presets">' + presetsHtml + '</div></div>'
      + '<h2>Colors</h2><div class="card">'

      + '<div class="expand-row" onclick="toggle(\'text\')"><label>Text</label><div class="right"><div class="swatch" id="sw-TextAll" onclick="openPicker(\'TextAll\');event.stopPropagation()"></div><span class="expand-btn" id="btn-text">+</span></div></div>'
      + '<div class="sub-rows" id="sub-text">'
      + '<div class="sub-row"><label>Time</label><div class="swatch" id="sw-TimeColor" onclick="openPicker(\'TimeColor\')"></div></div>'
      + '<div class="sub-expand-row" onclick="toggle2(\'infolines\')"><label>Info Lines</label><div class="right"><div class="swatch" id="sw-InfoLinesAll" onclick="openPicker(\'InfoLinesAll\');event.stopPropagation()"></div><span class="sub-expand-btn" id="btn2-infolines">+</span></div></div>'
      + '<div class="sub-sub-rows" id="sub2-infolines">'
      + '<div class="sub-sub-row"><label>Line 1</label><div class="swatch" id="sw-Line1Color" onclick="openPicker(\'Line1Color\')"></div></div>'
      + '<div class="sub-sub-row"><label>Line 2</label><div class="swatch" id="sw-Line2Color" onclick="openPicker(\'Line2Color\')"></div></div>'
      + '<div class="sub-sub-row"><label>Line 3</label><div class="swatch" id="sw-Line3Color" onclick="openPicker(\'Line3Color\')"></div></div>'
      + '<div class="sub-sub-row"><label>Line 4</label><div class="swatch" id="sw-Line4Color" onclick="openPicker(\'Line4Color\')"></div></div>'
      + '</div>'
      + '</div>'

      + '<div class="expand-row" onclick="toggle(\'lit\')"><label>Lit</label><div class="right"><div class="swatch" id="sw-LitAll" onclick="openPicker(\'LitAll\');event.stopPropagation()"></div><span class="expand-btn" id="btn-lit">+</span></div></div>'
      + '<div class="sub-rows" id="sub-lit">'
      + '<div class="sub-expand-row" onclick="toggle2(\'litticks\')"><label>Time Ticks</label><div class="right"><div class="swatch" id="sw-LitTicks" onclick="openPicker(\'LitTicks\');event.stopPropagation()"></div><span class="sub-expand-btn" id="btn2-litticks">+</span></div></div>'
      + '<div class="sub-sub-rows" id="sub2-litticks">'
      + '<div class="sub-sub-expand-row" onclick="toggle3(\'lithours\')"><label>Hours</label><div class="right"><div class="swatch" id="sw-LitHourColor" onclick="openPicker(\'LitHourColor\');event.stopPropagation()"></div><span class="sub-sub-expand-btn" id="btn3-lithours">+</span></div></div>'
      + '<div class="sub-sub-sub-rows" id="sub3-lithours"><div class="sub-sub-sub-row"><label>Leading tick</label><div class="swatch" id="sw-HourTipColor" onclick="openPicker(\'HourTipColor\')"></div></div></div>'
      + '<div class="sub-sub-expand-row" onclick="toggle3(\'litminutes\')"><label>Minutes</label><div class="right"><div class="swatch" id="sw-LitMinuteColor" onclick="openPicker(\'LitMinuteColor\');event.stopPropagation()"></div><span class="sub-sub-expand-btn" id="btn3-litminutes">+</span></div></div>'
      + '<div class="sub-sub-sub-rows" id="sub3-litminutes"><div class="sub-sub-sub-row"><label>Leading tick</label><div class="swatch" id="sw-MinuteTipColor" onclick="openPicker(\'MinuteTipColor\')"></div></div></div>'
      + '</div>'
      + '<div class="sub-expand-row" onclick="toggle2(\'litring\')"><label>Outer Ring</label><div class="right"><div class="swatch" id="sw-LitRing" onclick="openPicker(\'LitRing\');event.stopPropagation()"></div><span class="sub-expand-btn" id="btn2-litring">+</span></div></div>'
      + '<div class="sub-sub-rows" id="sub2-litring">'
      + '<div class="sub-sub-row"><label>Battery</label><div class="swatch" id="sw-LitBatteryColor" onclick="openPicker(\'LitBatteryColor\')"></div></div>'
      + '<div class="sub-sub-row"><label>Steps</label><div class="swatch" id="sw-LitStepsColor" onclick="openPicker(\'LitStepsColor\')"></div></div>'
      + '</div></div>'

      + '<div class="expand-row" onclick="toggle(\'dim\')"><label>Unlit</label><div class="right"><div class="swatch" id="sw-DimAll" onclick="openPicker(\'DimAll\');event.stopPropagation()"></div><span class="expand-btn" id="btn-dim">+</span></div></div>'
      + '<div class="sub-rows" id="sub-dim">'
      + '<div class="sub-expand-row" onclick="toggle2(\'dimticks\')"><label>Time Ticks</label><div class="right"><div class="swatch" id="sw-DimTicks" onclick="openPicker(\'DimTicks\');event.stopPropagation()"></div><span class="sub-expand-btn" id="btn2-dimticks">+</span></div></div>'
      + '<div class="sub-sub-rows" id="sub2-dimticks">'
      + '<div class="sub-sub-row"><label>Hours</label><div class="swatch" id="sw-DimHourColor" onclick="openPicker(\'DimHourColor\')"></div></div>'
      + '<div class="sub-sub-row"><label>Minutes</label><div class="swatch" id="sw-DimMinuteColor" onclick="openPicker(\'DimMinuteColor\')"></div></div>'
      + '</div>'
      + '<div class="sub-expand-row" onclick="toggle2(\'dimring\')"><label>Outer Ring</label><div class="right"><div class="swatch" id="sw-DimRing" onclick="openPicker(\'DimRing\');event.stopPropagation()"></div><span class="sub-expand-btn" id="btn2-dimring">+</span></div></div>'
      + '<div class="sub-sub-rows" id="sub2-dimring">'
      + '<div class="sub-sub-row"><label>Battery</label><div class="swatch" id="sw-DimBatteryColor" onclick="openPicker(\'DimBatteryColor\')"></div></div>'
      + '<div class="sub-sub-row"><label>Steps</label><div class="swatch" id="sw-DimStepsColor" onclick="openPicker(\'DimStepsColor\')"></div></div>'
      + '</div></div>'

      + '<div class="expand-row" onclick="toggle(\'base\')"><label>Base</label><div class="right"><div class="swatch" id="sw-BaseAll" onclick="openPicker(\'BaseAll\');event.stopPropagation()"></div><span class="expand-btn" id="btn-base">+</span></div></div>'
      + '<div class="sub-rows" id="sub-base">'
      + '<div class="sub-row"><label>Overlay</label><div class="swatch" id="sw-OverlayColor" onclick="openPicker(\'OverlayColor\')"></div></div>'
      + '<div class="sub-row"><label>Background</label><div class="swatch" id="sw-BackgroundColor" onclick="openPicker(\'BackgroundColor\')"></div></div>'
      + '</div></div></div>'

      + '<div id="bw-section"><h2>Display</h2><div class="card">'
      + '<div class="row"><label>Invert (white bg, black ticks)</label><label class="toggle"><input type="checkbox" id="InvertBW"><span class="knob"></span></label></div>'
      + '</div></div>'

      + '<h2>Outer Ring</h2><div class="card">'
      + '<div class="row"><label>Show battery &amp; steps ring</label>'
      + '<label class="toggle"><input type="checkbox" id="ShowRing"' + (isAplite ? '' : ' checked') + '><span class="knob"></span></label></div>'
      + '<span class="note">When hidden, tick art extends to screen edge</span>'
      + '</div>'

      + (isAplite ? '' :
          '<h2>Health</h2><div class="card"><div class="slider-wrap">'
        + '<div class="slider-lbl">Daily Step Goal <span id="goalVal">10,000</span></div>'
        + '<input type="range" id="StepGoal" min="1000" max="30000" step="500" value="10000"'
        + ' oninput="document.getElementById(\'goalVal\').textContent=parseInt(this.value).toLocaleString()">'
        + '</div></div>')

      + '<button class="save" onclick="save()">Save to Watch</button>'

      + '<div class="modal-bg" id="modal"><div class="modal">'
      + '<h3 id="modal-title">Pick a color</h3>'
      + '<div class="palette" id="palette-grid"></div>'
      + '<button class="modal-cancel" onclick="closePicker()">Cancel</button>'
      + '</div></div>'

      + '<script>'
      + platformData + presetsData + paletteData

      // Default colors = Radium classic preset
      + 'var colors={'
      + 'BackgroundColor:"#000000",OverlayColor:"#000000",TimeColor:"#ffffff",'
      + 'LitHourColor:"#aaffaa",LitMinuteColor:"#aaffaa",LitBatteryColor:"#aaffaa",LitStepsColor:"#aaffaa",'
      + 'DimHourColor:"#005500",DimMinuteColor:"#005500",DimBatteryColor:"#005500",DimStepsColor:"#005500",'
      + 'HourTipColor:"#ffffff",MinuteTipColor:"#ffffff",'
      + 'Line1Color:"#aaaaaa",Line2Color:"#aaffaa",Line3Color:"#aaffaa",Line4Color:"#aaaaaa"'
      + '};'

      + 'function updateSwatches(key,hex){'
      + 'colors[key]=hex;'
      + 'var el=document.getElementById("sw-"+key);if(el)el.style.background=hex;'
      + 'function setSplit(id,a,b){var e=document.getElementById(id);if(e)e.style.background="linear-gradient(135deg,"+a+" 50%,"+b+" 50%)"}'
      + 'function setQuad(id,a,b,c,d){var e=document.getElementById(id);if(e)e.style.background="conic-gradient("+a+" 0 25%,"+b+" 0 50%,"+c+" 0 75%,"+d+" 0 100%)"}'
      + 'setSplit("sw-LitTicks",colors.LitHourColor,colors.LitMinuteColor);'
      + 'setSplit("sw-LitRing",colors.LitBatteryColor,colors.LitStepsColor);'
      + 'setQuad("sw-LitAll",colors.LitHourColor,colors.LitMinuteColor,colors.LitBatteryColor,colors.LitStepsColor);'
      + 'setSplit("sw-DimTicks",colors.DimHourColor,colors.DimMinuteColor);'
      + 'setSplit("sw-DimRing",colors.DimBatteryColor,colors.DimStepsColor);'
      + 'setQuad("sw-DimAll",colors.DimHourColor,colors.DimMinuteColor,colors.DimBatteryColor,colors.DimStepsColor);'
      + 'setSplit("sw-TextAll",colors.TimeColor,colors.Line2Color);'
      + 'setSplit("sw-BaseAll",colors.BackgroundColor,colors.OverlayColor);'
      + 'setQuad("sw-InfoLinesAll",colors.Line1Color,colors.Line2Color,colors.Line3Color,colors.Line4Color);'
      + '}'

      + 'var cascadeMap={'
      + '"LitAll":["LitHourColor","LitMinuteColor","LitBatteryColor","LitStepsColor","HourTipColor","MinuteTipColor"],'
      + '"LitTicks":["LitHourColor","LitMinuteColor","HourTipColor","MinuteTipColor"],'
      + '"LitRing":["LitBatteryColor","LitStepsColor"],'
      + '"LitHourColor":["LitHourColor","HourTipColor"],'
      + '"LitMinuteColor":["LitMinuteColor","MinuteTipColor"],'
      + '"DimAll":["DimHourColor","DimMinuteColor","DimBatteryColor","DimStepsColor"],'
      + '"DimTicks":["DimHourColor","DimMinuteColor"],"DimRing":["DimBatteryColor","DimStepsColor"],'
      + '"TextAll":["TimeColor","Line1Color","Line2Color","Line3Color","Line4Color"],'
      + '"BaseAll":["BackgroundColor","OverlayColor"],'
      + '"InfoLinesAll":["Line1Color","Line2Color","Line3Color","Line4Color"]'
      + '};'

      + 'var pickerTarget=null,pickerKeys=null;'

      + 'function openPicker(key){'
      + 'pickerTarget=key;pickerKeys=cascadeMap[key]||[key];'
      + 'var labels={"LitAll":"Lit","LitTicks":"Time Ticks","LitRing":"Outer Ring","LitHourColor":"Hours","LitMinuteColor":"Minutes","DimAll":"Unlit","DimTicks":"Time Ticks","DimRing":"Outer Ring","TextAll":"Text","BaseAll":"Base","InfoLinesAll":"Info Lines"};'
      + 'document.getElementById("modal-title").textContent=labels[key]||key.replace(/([A-Z])/g," $1").trim();'
      + 'var grid=document.getElementById("palette-grid");grid.innerHTML="";'
      + 'PALETTE.forEach(function(hex){'
      + 'var d=document.createElement("div");'
      + 'var cur=colors[pickerTarget]||(pickerKeys&&colors[pickerKeys[0]]);'
      + 'd.className="pal-swatch"+(cur===hex?" selected":"");'
      + 'd.style.background=hex;d.title=PALETTE_NAMES[hex]||hex;'
      + 'd.onclick=function(){pickColor(hex);};grid.appendChild(d);});'
      + 'document.getElementById("modal").classList.add("open");}'

      + 'function pickColor(hex){if(!pickerKeys)return;pickerKeys.forEach(function(k){updateSwatches(k,hex);});closePicker();}'
      + 'function closePicker(){document.getElementById("modal").classList.remove("open");pickerTarget=null;pickerKeys=null;}'
      + 'function initSwatches(){Object.keys(colors).forEach(function(k){updateSwatches(k,colors[k]);});}'

      + 'function loadSettings(){'
      + 'try{if(!CURRENT)return;'
      + 'Object.keys(CURRENT).forEach(function(k){'
      + 'if(k==="OverlayMode"){var el=document.getElementById("ov"+CURRENT[k]);if(el)el.checked=true;}'
      + 'else if(k==="OverlaySize"){var el=document.getElementById("OverlaySize");if(el)el.checked=(CURRENT[k]===1);}'
      + 'else if(k==="InvertBW"){document.getElementById("InvertBW").checked=!!CURRENT[k];}'
      + 'else if(k==="ShowRing"){document.getElementById("ShowRing").checked=!!CURRENT[k];}'
      + 'else if(k==="StepGoal"){var el=document.getElementById("StepGoal");if(el){el.value=CURRENT[k];document.getElementById("goalVal").textContent=parseInt(CURRENT[k]).toLocaleString();}}'
      + 'else if(k==="Line1Field"||k==="Line2Field"||k==="Line3Field"||k==="Line4Field"){var el=document.getElementById(k);if(el)el.value=CURRENT[k];}'
      + 'else if(colors[k]!==undefined){var hex="#"+(CURRENT[k]>>>0).toString(16).padStart(6,"0");updateSwatches(k,hex);}'
      + '});'
      + '}catch(e){}}'

      + 'function applyPlatform(){var c=(PLATFORM!=="bw");document.getElementById("color-section").style.display=c?"":"none";document.getElementById("bw-section").style.display=c?"none":""; }'
      + 'function toggle(id){var s=document.getElementById("sub-"+id);var b=document.getElementById("btn-"+id);var o=s.classList.toggle("open");b.classList.toggle("open",o);}'
      + 'function toggle2(id){var s=document.getElementById("sub2-"+id);var b=document.getElementById("btn2-"+id);var o=s.classList.toggle("open");b.classList.toggle("open",o);}'
      + 'function toggle3(id){var s=document.getElementById("sub3-"+id);var b=document.getElementById("btn3-"+id);var o=s.classList.toggle("open");b.classList.toggle("open",o);}'

      + 'function applyPreset(i){var p=PRESETS[i];'
      + 'updateSwatches("BackgroundColor",p.bg);updateSwatches("OverlayColor",p.obg);'
      + 'updateSwatches("TimeColor",p.tt);'
      + 'updateSwatches("Line1Color",p.l1);updateSwatches("Line2Color",p.l2);'
      + 'updateSwatches("Line3Color",p.l3);updateSwatches("Line4Color",p.l4);'
      + 'updateSwatches("LitHourColor",p.lH);updateSwatches("LitMinuteColor",p.lM);'
      + 'updateSwatches("LitBatteryColor",p.lB);updateSwatches("LitStepsColor",p.lS);'
      + 'updateSwatches("DimHourColor",p.dH);updateSwatches("DimMinuteColor",p.dM);'
      + 'updateSwatches("DimBatteryColor",p.dB);updateSwatches("DimStepsColor",p.dS);'
      + 'updateSwatches("HourTipColor",p.tH);updateSwatches("MinuteTipColor",p.tM);}'

      + 'function h(hex){return parseInt(hex.slice(1),16);}'
      + 'function tog(id){var el=document.getElementById(id);return el&&el.checked?1:0;}'
      + 'function sel(id){var el=document.getElementById(id);return el?parseInt(el.value)||0:0;}'

      + 'function save(){'
      + 'var ov=parseInt(document.querySelector(\'input[name="overlay"]:checked\').value);'
      + 'var s={OverlayMode:ov,'
      + 'BackgroundColor:h(colors.BackgroundColor),OverlayColor:h(colors.OverlayColor),'
      + 'TimeColor:h(colors.TimeColor),'
      + 'LitHourColor:h(colors.LitHourColor),LitMinuteColor:h(colors.LitMinuteColor),'
      + 'LitBatteryColor:h(colors.LitBatteryColor),LitStepsColor:h(colors.LitStepsColor),'
      + 'DimHourColor:h(colors.DimHourColor),DimMinuteColor:h(colors.DimMinuteColor),'
      + 'DimBatteryColor:h(colors.DimBatteryColor),DimStepsColor:h(colors.DimStepsColor),'
      + 'HourTipColor:h(colors.HourTipColor),MinuteTipColor:h(colors.MinuteTipColor),'
      + 'Line1Color:h(colors.Line1Color),Line2Color:h(colors.Line2Color),'
      + 'Line3Color:h(colors.Line3Color),Line4Color:h(colors.Line4Color),'
      + 'InvertBW:tog("InvertBW"),ShowRing:tog("ShowRing"),OverlaySize:tog("OverlaySize"),'
      + 'StepGoal:parseInt((document.getElementById("StepGoal")||{value:"10000"}).value),'
      + 'Line1Field:sel("Line1Field"),Line2Field:sel("Line2Field"),'
      + 'Line3Field:sel("Line3Field"),Line4Field:sel("Line4Field")'
      + '};'
      + 'window.location="pebblejs://close#"+encodeURIComponent(JSON.stringify(s));}'

      + 'loadSettings();initSwatches();applyPlatform();'
      + '</script></body></html>';
    return 'data:text/html,' + encodeURIComponent(html);
  }
};
