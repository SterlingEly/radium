module.exports = {
  buildUrl: function(platform, currentSettings) {

    var presets = [
      // ---- DARK (8) ----
      // tH = leading hour tick (slightly brighter than lH)
      // tM = leading minute tick (noticeably brighter/whiter than lM)
      { label:'Radium',  bg:'#000000',obg:'#000000',tt:'#ffffff',dt:'#aaffaa', lH:'#55ff00',lM:'#55ff00',lB:'#55ff00',lS:'#55ff00', dH:'#005500',dM:'#005500',dB:'#005500',dS:'#005500', tH:'#aaffaa',tM:'#ffffff' },
      { label:'Ember',   bg:'#000000',obg:'#000000',tt:'#ffffff',dt:'#aaaaaa', lH:'#ff5500',lM:'#ff5500',lB:'#ff5500',lS:'#ff5500', dH:'#555555',dM:'#555555',dB:'#555555',dS:'#555555', tH:'#ffaa55',tM:'#ffffff' },
      { label:'Cobalt',  bg:'#000000',obg:'#000000',tt:'#ffffff',dt:'#aaaaaa', lH:'#0055ff',lM:'#0055ff',lB:'#0055ff',lS:'#0055ff', dH:'#555555',dM:'#555555',dB:'#555555',dS:'#555555', tH:'#aaaaff',tM:'#ffffff' },
      { label:'Slate',   bg:'#000000',obg:'#000000',tt:'#ffffff',dt:'#aaaaaa', lH:'#ffffff',lM:'#aaaaaa',lB:'#aaaaaa',lS:'#aaaaaa', dH:'#555555',dM:'#555555',dB:'#555555',dS:'#555555', tH:'#ffffff',tM:'#ffffff' },
      { label:'Radium+', bg:'#000000',obg:'#000000',tt:'#ffffff',dt:'#55aa55', lH:'#00ff00',lM:'#00ff00',lB:'#00ff00',lS:'#00ff00', dH:'#005500',dM:'#005500',dB:'#005500',dS:'#005500', tH:'#aaffaa',tM:'#ffffff' },
      { label:'Crimson', bg:'#000000',obg:'#000000',tt:'#ffffff',dt:'#aa5555', lH:'#ff5555',lM:'#ff5555',lB:'#ff5555',lS:'#ff5555', dH:'#550000',dM:'#550000',dB:'#550000',dS:'#550000', tH:'#ffaaaa',tM:'#ffffff' },
      { label:'Ocean',   bg:'#000000',obg:'#000000',tt:'#ffffff',dt:'#0055aa', lH:'#00aaff',lM:'#00aaff',lB:'#00aaff',lS:'#00aaff', dH:'#0055aa',dM:'#0055aa',dB:'#0055aa',dS:'#0055aa', tH:'#aaffff',tM:'#ffffff' },
      { label:'Volt',    bg:'#000000',obg:'#000000',tt:'#ffffff',dt:'#aaaa55', lH:'#ffff00',lM:'#ffff00',lB:'#ffff00',lS:'#ffff00', dH:'#555500',dM:'#555500',dB:'#555500',dS:'#555500', tH:'#ffffff',tM:'#ffffff' },

      // ---- DARK+ (8) ----
      { label:'Dusk',    bg:'#000000',obg:'#000000',tt:'#ffffff',dt:'#aa00aa', lH:'#ff55ff',lM:'#ff55ff',lB:'#ff55ff',lS:'#ff55ff', dH:'#550055',dM:'#550055',dB:'#550055',dS:'#550055', tH:'#ffaaff',tM:'#ffffff' },
      { label:'Horizon', bg:'#000000',obg:'#0000aa',tt:'#ffaa55',dt:'#aaaaaa', lH:'#0055ff',lM:'#ff5500',lB:'#0055aa',lS:'#aa5500', dH:'#0000aa',dM:'#550000',dB:'#0000aa',dS:'#550000', tH:'#aaaaff',tM:'#ffff00' },
      { label:'Reactor', bg:'#000000',obg:'#000000',tt:'#ffffff',dt:'#aaaaaa', lH:'#00ffff',lM:'#ff0000',lB:'#00aaff',lS:'#ff5500', dH:'#005555',dM:'#550000',dB:'#005555',dS:'#550000', tH:'#aaffff',tM:'#ffaaaa' },
      { label:'Venom',   bg:'#000000',obg:'#000000',tt:'#ffffff',dt:'#aaaaaa', lH:'#aaff00',lM:'#aa00ff',lB:'#55ff00',lS:'#5500aa', dH:'#005500',dM:'#550055',dB:'#005500',dS:'#550055', tH:'#ffffff',tM:'#ffffff' },
      { label:'Blossom', bg:'#000000',obg:'#000000',tt:'#ffffff',dt:'#aaaaaa', lH:'#aaffaa',lM:'#ff55aa',lB:'#55ffaa',lS:'#ff0055', dH:'#005500',dM:'#550055',dB:'#005500',dS:'#aa0055', tH:'#ffffff',tM:'#ffffff' },
      { label:'Solar',   bg:'#000000',obg:'#000000',tt:'#ffffff',dt:'#ffaa00', lH:'#ff5500',lM:'#ffaa00',lB:'#ffff00',lS:'#aaff00', dH:'#550000',dM:'#555500',dB:'#555500',dS:'#005500', tH:'#ffff00',tM:'#ffffff' },
      { label:'Aurora',  bg:'#000000',obg:'#000000',tt:'#ffffff',dt:'#aaffff', lH:'#00ffff',lM:'#00ff55',lB:'#00aaff',lS:'#aaffaa', dH:'#005555',dM:'#005500',dB:'#005555',dS:'#005500', tH:'#ffffff',tM:'#ffffff' },
      { label:'Neon',    bg:'#000000',obg:'#000000',tt:'#ffffff',dt:'#aaaaaa', lH:'#00ff00',lM:'#ff00ff',lB:'#00ffff',lS:'#ff55ff', dH:'#005500',dM:'#550055',dB:'#005555',dS:'#550055', tH:'#aaffaa',tM:'#ffffff' },

      // ---- LIGHT (8) ----
      { label:'Paper',    bg:'#ffffff',obg:'#ffffff',tt:'#000000',dt:'#555555', lH:'#000000',lM:'#000000',lB:'#000000',lS:'#000000', dH:'#aaaaaa',dM:'#aaaaaa',dB:'#aaaaaa',dS:'#aaaaaa', tH:'#555555',tM:'#000000' },
      { label:'Jade',     bg:'#ffffff',obg:'#ffffff',tt:'#000000',dt:'#555555', lH:'#00aa55',lM:'#00aa55',lB:'#00aa55',lS:'#00aa55', dH:'#aaaaaa',dM:'#aaaaaa',dB:'#aaaaaa',dS:'#aaaaaa', tH:'#005500',tM:'#005500' },
      { label:'Sapphire', bg:'#ffffff',obg:'#ffffff',tt:'#000000',dt:'#555555', lH:'#0055aa',lM:'#0055aa',lB:'#0055aa',lS:'#0055aa', dH:'#aaaaaa',dM:'#aaaaaa',dB:'#aaaaaa',dS:'#aaaaaa', tH:'#000055',tM:'#000055' },
      { label:'Ruby',     bg:'#ffffff',obg:'#ffffff',tt:'#000000',dt:'#555555', lH:'#aa0000',lM:'#aa0000',lB:'#aa0000',lS:'#aa0000', dH:'#aaaaaa',dM:'#aaaaaa',dB:'#aaaaaa',dS:'#aaaaaa', tH:'#550000',tM:'#550000' },
      { label:'Mint',     bg:'#ffffff',obg:'#ffffff',tt:'#000000',dt:'#005500', lH:'#005500',lM:'#005500',lB:'#005500',lS:'#005500', dH:'#aaffaa',dM:'#aaffaa',dB:'#aaffaa',dS:'#aaffaa', tH:'#000000',tM:'#000000' },
      { label:'Rose',     bg:'#ffffff',obg:'#ffffff',tt:'#550000',dt:'#550000', lH:'#550000',lM:'#550000',lB:'#550000',lS:'#550000', dH:'#ffaaaa',dM:'#ffaaaa',dB:'#ffaaaa',dS:'#ffaaaa', tH:'#000000',tM:'#000000' },
      { label:'Sky',      bg:'#ffffff',obg:'#ffffff',tt:'#000000',dt:'#0000aa', lH:'#0000aa',lM:'#0000aa',lB:'#0000aa',lS:'#0000aa', dH:'#aaaaff',dM:'#aaaaff',dB:'#aaaaff',dS:'#aaaaff', tH:'#000000',tM:'#000000' },
      { label:'Sepia',    bg:'#ffffaa',obg:'#ffffaa',tt:'#000000',dt:'#aa5500', lH:'#550000',lM:'#550000',lB:'#550000',lS:'#550000', dH:'#ffaa55',dM:'#ffaa55',dB:'#ffaa55',dS:'#ffaa55', tH:'#000000',tM:'#000000' },

      // ---- COLOR (8) ----
      { label:'Teal',         bg:'#00aaaa',obg:'#00aaaa',tt:'#ffffff',dt:'#ffffff', lH:'#ffffff',lM:'#ffffff',lB:'#ffffff',lS:'#ffffff', dH:'#005555',dM:'#005555',dB:'#005555',dS:'#005555', tH:'#aaffff',tM:'#ffffff' },
      { label:'Flame',        bg:'#ff5500',obg:'#ff5500',tt:'#ffffff',dt:'#ffffff', lH:'#ffffff',lM:'#ffffff',lB:'#ffffff',lS:'#ffffff', dH:'#aa5500',dM:'#aa5500',dB:'#aa5500',dS:'#aa5500', tH:'#ffff00',tM:'#ffffff' },
      { label:'Plum',         bg:'#550055',obg:'#550055',tt:'#ffffff',dt:'#ff55ff', lH:'#ff55ff',lM:'#ff55ff',lB:'#ff55ff',lS:'#ff55ff', dH:'#aa00aa',dM:'#aa00aa',dB:'#aa00aa',dS:'#aa00aa', tH:'#ffffff',tM:'#ffffff' },
      { label:'Forest',       bg:'#005500',obg:'#005500',tt:'#ffffff',dt:'#55ff00', lH:'#55ff00',lM:'#55ff00',lB:'#55ff00',lS:'#55ff00', dH:'#55aa00',dM:'#55aa00',dB:'#55aa00',dS:'#55aa00', tH:'#ffffff',tM:'#ffffff' },
      { label:'Midnight',     bg:'#0000aa',obg:'#0000aa',tt:'#aaaaff',dt:'#aaaaff', lH:'#00ffff',lM:'#aaaaff',lB:'#00aaff',lS:'#aaaaff', dH:'#0055aa',dM:'#0055aa',dB:'#0055aa',dS:'#0055aa', tH:'#ffffff',tM:'#ffffff' },
      { label:'Cinnabar',     bg:'#550000',obg:'#550000',tt:'#ffffff',dt:'#ffaa55', lH:'#ffaa55',lM:'#ff5500',lB:'#ffaa00',lS:'#ff5500', dH:'#aa0000',dM:'#aa0000',dB:'#aa5500',dS:'#aa0000', tH:'#ffffff',tM:'#ffff00' },
      { label:'Ultraviolet',  bg:'#550055',obg:'#550055',tt:'#ffffff',dt:'#ff55ff', lH:'#aa00ff',lM:'#ff00ff',lB:'#ff55ff',lS:'#aa00ff', dH:'#5500aa',dM:'#550055',dB:'#5500aa',dS:'#550055', tH:'#ffffff',tM:'#ffffff' },
      { label:'Ash',          bg:'#555555',obg:'#555555',tt:'#ffffff',dt:'#ffffff', lH:'#ffffff',lM:'#ffffff',lB:'#ffffff',lS:'#ffffff', dH:'#aaaaaa',dM:'#aaaaaa',dB:'#aaaaaa',dS:'#aaaaaa', tH:'#ffffff',tM:'#ffffff' },

      // ---- SPECIAL (8) ----
      { label:'Hearth',   bg:'#000000',obg:'#000000',tt:'#ffaa55',dt:'#aa5500', lH:'#ff5500',lM:'#ff5500',lB:'#ffaa00',lS:'#ffaa00', dH:'#550000',dM:'#550000',dB:'#aa5500',dS:'#550000', tH:'#ffff00',tM:'#ffffff' },
      { label:'Boreal',   bg:'#000000',obg:'#0000aa',tt:'#aaaaff',dt:'#0055aa', lH:'#00aaff',lM:'#55ffff',lB:'#00ffff',lS:'#55aaff', dH:'#0000aa',dM:'#005555',dB:'#005555',dS:'#0000aa', tH:'#ffffff',tM:'#ffffff' },
      { label:'GoldEye',  bg:'#000000',obg:'#005500',tt:'#ffffff',dt:'#00ff00', lH:'#ff5500',lM:'#00aaff',lB:'#00ff00',lS:'#00ff00', dH:'#550000',dM:'#0000aa',dB:'#005500',dS:'#005500', tH:'#ffff00',tM:'#ffffff' },
      { label:'Viper',    bg:'#000000',obg:'#0000aa',tt:'#ffffff',dt:'#aaffff', lH:'#00ffff',lM:'#00ff55',lB:'#0055ff',lS:'#aaffaa', dH:'#005555',dM:'#005500',dB:'#0000aa',dS:'#005500', tH:'#ffffff',tM:'#ffffff' },
      { label:'Inferno',  bg:'#000000',obg:'#000000',tt:'#ffffff',dt:'#ffaa00', lH:'#ff0000',lM:'#ff5500',lB:'#ffaa00',lS:'#aaff00', dH:'#550000',dM:'#550000',dB:'#555500',dS:'#005500', tH:'#ffff00',tM:'#ffffff' },
      { label:'Cosmos',   bg:'#0000aa',obg:'#0000aa',tt:'#aaaaff',dt:'#ff55ff', lH:'#aa00ff',lM:'#ff00ff',lB:'#ff55aa',lS:'#ff0055', dH:'#550055',dM:'#550055',dB:'#550055',dS:'#550000', tH:'#ffffff',tM:'#ffffff' },
      { label:'Triadic',  bg:'#000000',obg:'#000000',tt:'#ffffff',dt:'#aaaaaa', lH:'#ff0000',lM:'#0055ff',lB:'#00ff00',lS:'#aa00ff', dH:'#550000',dM:'#0000aa',dB:'#005500',dS:'#550055', tH:'#ffaa55',tM:'#ffffff' },
      { label:'Rainbow',  bg:'#000000',obg:'#000000',tt:'#ffffff',dt:'#aaaaaa', lH:'#ff0000',lM:'#ffaa00',lB:'#00ff00',lS:'#00ffff', dH:'#550000',dM:'#aa5500',dB:'#005500',dS:'#005555', tH:'#ffff00',tM:'#ffffff' },
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

    var fieldOptionsInner = [
      { value: 0, label: 'None' },
      { value: 1, label: 'Day' },
      { value: 2, label: 'Date' },
      { value: 3, label: 'Day + Date' },
      { value: 4, label: 'Steps' },
      { value: 5, label: 'Temp (F)' },
      { value: 6, label: 'Temp (C)' },
      { value: 7, label: 'Battery' },
    ];

    var fieldOptionsOuter = [
      { value: 0, label: 'None' },
      { value: 2, label: 'Date' },
      { value: 4, label: 'Steps' },
      { value: 5, label: 'Temp (F)' },
      { value: 6, label: 'Temp (C)' },
      { value: 7, label: 'Battery' },
    ];

    function makeSelect(id, defaultVal, options) {
      var opts = options.map(function(o) {
        return '<option value="' + o.value + '"' + (o.value === defaultVal ? ' selected' : '') + '>' + o.label + '</option>';
      }).join('');
      return '<select id="' + id + '" style="background:#242424;color:#ddd;border:1px solid #333;border-radius:6px;padding:6px 8px;font-size:14px;flex:1">' + opts + '</select>';
    }

    // Large overlay toggle only shown for emery and gabbro (chalk = gabbro round)
    var isLargePlatform = (platform === 'emery' || platform === 'chalk');
    var overlayLargeRow = isLargePlatform
      ? '<div class="row"><label>Large overlay</label><label class="toggle"><input type="checkbox" id="OverlaySize"><span class="knob"></span></label></div>'
      : '';

    var platformData = 'var PLATFORM=' + JSON.stringify(platform || 'color') + ';'
      + 'var CURRENT=' + JSON.stringify(currentSettings || null) + ';';
    var presetsData = 'var PRESETS=' + JSON.stringify(presets) + ';';
    var paletteData = 'var PALETTE=' + JSON.stringify(palette) + ';var PALETTE_NAMES=' + JSON.stringify(paletteNames) + ';';

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
      + '.row .right{display:flex;align-items:center;gap:8px}'
      + '.field-row{display:flex;align-items:center;justify-content:space-between;padding:10px 14px;border-bottom:1px solid #222;gap:12px}'
      + '.field-row:last-child{border-bottom:none}'
      + '.field-row label{font-size:14px;color:#aaa;white-space:nowrap;min-width:60px}'
      + '.swatch{width:36px;height:28px;border-radius:5px;cursor:pointer;border:2px solid #333;flex-shrink:0}'
      + '.expand-row{display:flex;align-items:center;justify-content:space-between;padding:11px 14px;border-bottom:1px solid #222;cursor:pointer;user-select:none}'
      + '.expand-row:last-child{border-bottom:none}'
      + '.expand-row label{font-size:15px;color:#ddd;flex:1;cursor:pointer}'
      + '.expand-row .right{display:flex;align-items:center;gap:8px}'
      + '.expand-btn{font-size:18px;color:#555;line-height:1;width:24px;text-align:center;transition:transform .2s}'
      + '.expand-btn.open{transform:rotate(45deg);color:#aaffaa}'
      + '.sub-rows{display:none;background:#141414}'
      + '.sub-rows.open{display:block}'
      + '.sub-row{display:flex;align-items:center;justify-content:space-between;padding:9px 14px 9px 28px;border-bottom:1px solid #1e1e1e}'
      + '.sub-row:last-child{border-bottom:none}'
      + '.sub-row label{font-size:14px;color:#aaa;flex:1}'
      + '.sub-expand-row{display:flex;align-items:center;justify-content:space-between;padding:9px 14px 9px 28px;border-bottom:1px solid #1e1e1e;cursor:pointer;user-select:none}'
      + '.sub-expand-row label{font-size:14px;color:#aaa;flex:1;cursor:pointer}'
      + '.sub-expand-row .right{display:flex;align-items:center;gap:8px}'
      + '.sub-expand-btn{font-size:15px;color:#444;line-height:1;width:20px;text-align:center;transition:transform .15s}'
      + '.sub-expand-btn.open{transform:rotate(45deg);color:#aaffaa}'
      + '.sub-sub-rows{display:none;background:#0e0e0e}'
      + '.sub-sub-rows.open{display:block}'
      + '.sub-sub-row{display:flex;align-items:center;justify-content:space-between;padding:8px 14px 8px 42px;border-bottom:1px solid #181818}'
      + '.sub-sub-row:last-child{border-bottom:none}'
      + '.sub-sub-row label{font-size:13px;color:#888;flex:1}'
      + '.sub-sub-expand-row{display:flex;align-items:center;justify-content:space-between;padding:8px 14px 8px 42px;border-bottom:1px solid #181818;cursor:pointer;user-select:none}'
      + '.sub-sub-expand-row label{font-size:13px;color:#888;flex:1;cursor:pointer}'
      + '.sub-sub-expand-row .right{display:flex;align-items:center;gap:8px}'
      + '.sub-sub-expand-btn{font-size:13px;color:#333;line-height:1;width:18px;text-align:center;transition:transform .15s}'
      + '.sub-sub-expand-btn.open{transform:rotate(45deg);color:#aaffaa}'
      + '.sub-sub-sub-rows{display:none;background:#090909}'
      + '.sub-sub-sub-rows.open{display:block}'
      + '.sub-sub-sub-row{display:flex;align-items:center;justify-content:space-between;padding:7px 14px 7px 56px;border-bottom:1px solid #141414}'
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
      + '<input type="radio" name="overlay" id="ov0" value="0" checked><label for="ov0">Always On</label>'
      + '<input type="radio" name="overlay" id="ov1" value="1"><label for="ov1">Always Off</label>'
      + '<input type="radio" name="overlay" id="ov2" value="2"><label for="ov2">Shake</label>'
      + '</div>'
      + overlayLargeRow
      + '</div>'

      + '<h2>Info Lines</h2><div class="card">'
      + '<div class="field-row"><label>Line 1</label>' + makeSelect('TopOuterField',    0, fieldOptionsOuter) + '</div>'
      + '<div class="field-row"><label>Line 2</label>' + makeSelect('TopInnerField',    1, fieldOptionsInner) + '</div>'
      + '<div class="field-row"><label>Line 3</label>' + makeSelect('BottomInnerField', 2, fieldOptionsInner) + '</div>'
      + '<div class="field-row"><label>Line 4</label>' + makeSelect('BottomOuterField', 0, fieldOptionsOuter) + '</div>'
      + '</div>'

      + '<div id="color-section">'
      + '<h2>Presets</h2><div class="card"><div class="presets">' + presetsHtml + '</div></div>'
      + '<h2>Colors</h2><div class="card">'
      + '<div class="expand-row" onclick="toggle(\'text\')"><label>Text</label><div class="right"><div class="swatch" id="sw-TextAll" onclick="openPicker(\'TextAll\');event.stopPropagation()"></div><span class="expand-btn" id="btn-text">+</span></div></div>'
      + '<div class="sub-rows" id="sub-text">'
      + '<div class="sub-row"><label>Time</label><div class="swatch" id="sw-TimeTextColor" onclick="openPicker(\'TimeTextColor\')"></div></div>'
      + '<div class="sub-row"><label>Date &amp; Day</label><div class="swatch" id="sw-DateTextColor" onclick="openPicker(\'DateTextColor\')"></div></div>'
      + '<div class="sub-expand-row" onclick="toggle2(\'infolines\')"><label>Info Lines</label><div class="right"><div class="swatch" id="sw-InfoLinesAll" onclick="openPicker(\'InfoLinesAll\');event.stopPropagation()"></div><span class="sub-expand-btn" id="btn2-infolines">+</span></div></div>'
      + '<div class="sub-sub-rows" id="sub2-infolines">'
      + '<div class="sub-sub-row"><label>Line 1</label><div class="swatch" id="sw-InfoLine1Color" onclick="openPicker(\'InfoLine1Color\')"></div></div>'
      + '<div class="sub-sub-row"><label>Line 2</label><div class="swatch" id="sw-InfoLine2Color" onclick="openPicker(\'InfoLine2Color\')"></div></div>'
      + '<div class="sub-sub-row"><label>Line 3</label><div class="swatch" id="sw-InfoLine3Color" onclick="openPicker(\'InfoLine3Color\')"></div></div>'
      + '<div class="sub-sub-row"><label>Line 4</label><div class="swatch" id="sw-InfoLine4Color" onclick="openPicker(\'InfoLine4Color\')"></div></div>'
      + '</div>'
      + '</div>'
      + '<div class="expand-row" onclick="toggle(\'lit\')"><label>Lit</label><div class="right"><div class="swatch" id="sw-LitAll" onclick="openPicker(\'LitAll\');event.stopPropagation()"></div><span class="expand-btn" id="btn-lit">+</span></div></div>'
      + '<div class="sub-rows" id="sub-lit">'
      + '<div class="sub-expand-row" onclick="toggle2(\'litticks\')"><label>Time Ticks</label><div class="right"><div class="swatch" id="sw-LitTicks" onclick="openPicker(\'LitTicks\');event.stopPropagation()"></div><span class="sub-expand-btn" id="btn2-litticks">+</span></div></div>'
      + '<div class="sub-sub-rows" id="sub2-litticks">'
      + '<div class="sub-sub-expand-row" onclick="toggle3(\'lithours\')"><label>Hours</label><div class="right"><div class="swatch" id="sw-LitHourColor" onclick="openPicker(\'LitHourColor\');event.stopPropagation()"></div><span class="sub-sub-expand-btn" id="btn3-lithours">+</span></div></div>'
      + '<div class="sub-sub-sub-rows" id="sub3-lithours"><div class="sub-sub-sub-row"><label>Leading tick</label><div class="swatch" id="sw-LitHourTipColor" onclick="openPicker(\'LitHourTipColor\')"></div></div></div>'
      + '<div class="sub-sub-expand-row" onclick="toggle3(\'litminutes\')"><label>Minutes</label><div class="right"><div class="swatch" id="sw-LitMinuteColor" onclick="openPicker(\'LitMinuteColor\');event.stopPropagation()"></div><span class="sub-sub-expand-btn" id="btn3-litminutes">+</span></div></div>'
      + '<div class="sub-sub-sub-rows" id="sub3-litminutes"><div class="sub-sub-sub-row"><label>Leading tick</label><div class="swatch" id="sw-LitMinuteTipColor" onclick="openPicker(\'LitMinuteTipColor\')"></div></div></div>'
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
      + '<div class="sub-row"><label>Overlay</label><div class="swatch" id="sw-OverlayBgColor" onclick="openPicker(\'OverlayBgColor\')"></div></div>'
      + '<div class="sub-row"><label>Background</label><div class="swatch" id="sw-BackgroundColor" onclick="openPicker(\'BackgroundColor\')"></div></div>'
      + '</div></div></div>'

      + '<div id="bw-section"><h2>Display</h2><div class="card">'
      + '<div class="row"><label>Invert (white bg, black ticks)</label><label class="toggle"><input type="checkbox" id="InvertBW"><span class="knob"></span></label></div>'
      + '</div></div>'

      + '<h2>Outer Ring</h2><div class="card">'
      + '<div class="row"><label>Show battery &amp; steps ring</label><label class="toggle"><input type="checkbox" id="ShowRing" checked><span class="knob"></span></label></div>'
      + '<span class="note">When hidden, tick art extends to the screen edge</span>'
      + '</div>'

      + '<h2>Health</h2><div class="card"><div class="slider-wrap">'
      + '<div class="slider-lbl">Daily Step Goal <span id="goalVal">10,000</span></div>'
      + '<input type="range" id="StepGoal" min="1000" max="30000" step="500" value="10000"'
      + ' oninput="document.getElementById(\'goalVal\').textContent=parseInt(this.value).toLocaleString()">'
      + '</div></div>'

      + '<button class="save" onclick="save()">Save to Watch</button>'

      + '<div class="modal-bg" id="modal"><div class="modal">'
      + '<h3 id="modal-title">Pick a color</h3>'
      + '<div class="palette" id="palette-grid"></div>'
      + '<button class="modal-cancel" onclick="closePicker()">Cancel</button>'
      + '</div></div>'

      + '<script>'
      + platformData + presetsData + paletteData

      + 'var colors={'
      + 'BackgroundColor:"#000000",OverlayBgColor:"#000000",TimeTextColor:"#ffffff",DateTextColor:"#aaaaaa",'
      + 'LitHourColor:"#aaffaa",LitMinuteColor:"#aaffaa",LitBatteryColor:"#aaffaa",LitStepsColor:"#aaffaa",'
      + 'DimHourColor:"#555555",DimMinuteColor:"#555555",DimBatteryColor:"#555555",DimStepsColor:"#555555",'
      + 'LitHourTipColor:"#ffffff",LitMinuteTipColor:"#ffffff",'
      + 'InfoLine1Color:"#aaaaaa",InfoLine2Color:"#aaaaaa",InfoLine3Color:"#aaaaaa",InfoLine4Color:"#aaaaaa"'
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
      + 'setSplit("sw-TextAll",colors.TimeTextColor,colors.DateTextColor);'
      + 'setSplit("sw-BaseAll",colors.BackgroundColor,colors.OverlayBgColor);'
      + 'setQuad("sw-InfoLinesAll",colors.InfoLine1Color,colors.InfoLine2Color,colors.InfoLine3Color,colors.InfoLine4Color);'
      + '}'

      + 'var cascadeMap={'
      + '"LitAll":["LitHourColor","LitMinuteColor","LitBatteryColor","LitStepsColor","LitHourTipColor","LitMinuteTipColor"],'
      + '"LitTicks":["LitHourColor","LitMinuteColor"],"LitRing":["LitBatteryColor","LitStepsColor"],'
      + '"DimAll":["DimHourColor","DimMinuteColor","DimBatteryColor","DimStepsColor"],'
      + '"DimTicks":["DimHourColor","DimMinuteColor"],"DimRing":["DimBatteryColor","DimStepsColor"],'
      + '"TextAll":["TimeTextColor","DateTextColor"],"BaseAll":["BackgroundColor","OverlayBgColor"],'
      + '"InfoLinesAll":["InfoLine1Color","InfoLine2Color","InfoLine3Color","InfoLine4Color"]'
      + '};'

      + 'var pickerTarget=null,pickerKeys=null;'

      + 'function openPicker(key){'
      + 'pickerTarget=key;pickerKeys=cascadeMap[key]||[key];'
      + 'var labels={"LitAll":"Lit","LitTicks":"Time Ticks","LitRing":"Outer Ring","DimAll":"Unlit","DimTicks":"Time Ticks","DimRing":"Outer Ring","TextAll":"Text","BaseAll":"Base","InfoLinesAll":"Info Lines"};'
      + 'document.getElementById("modal-title").textContent=labels[key]||key.replace(/([A-Z])/g," $1").trim();'
      + 'var grid=document.getElementById("palette-grid");grid.innerHTML="";'
      + 'PALETTE.forEach(function(hex){'
      + 'var d=document.createElement("div");'
      + 'var curColor=colors[pickerTarget]||(pickerKeys&&colors[pickerKeys[0]]);'
      + 'd.className="pal-swatch"+(curColor===hex?" selected":"");'
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
      + 'else if(k==="StepGoal"){var el=document.getElementById("StepGoal");el.value=CURRENT[k];document.getElementById("goalVal").textContent=parseInt(CURRENT[k]).toLocaleString();}'
      + 'else if(k==="TopOuterField"||k==="TopInnerField"||k==="BottomInnerField"||k==="BottomOuterField"){var el=document.getElementById(k);if(el)el.value=CURRENT[k];}'
      + 'else if(colors[k]!==undefined){var hex="#"+(CURRENT[k]>>>0).toString(16).padStart(6,"0");updateSwatches(k,hex);}'
      + '});'
      + '}catch(e){}}'

      + 'function applyPlatform(){var isColor=(PLATFORM!=="bw");document.getElementById("color-section").style.display=isColor?"":"none";document.getElementById("bw-section").style.display=isColor?"none":""; }'
      + 'function toggle(id){var sub=document.getElementById("sub-"+id);var btn=document.getElementById("btn-"+id);var open=sub.classList.toggle("open");btn.classList.toggle("open",open);}'
      + 'function toggle2(id){var sub=document.getElementById("sub2-"+id);var btn=document.getElementById("btn2-"+id);var open=sub.classList.toggle("open");btn.classList.toggle("open",open);}'
      + 'function toggle3(id){var sub=document.getElementById("sub3-"+id);var btn=document.getElementById("btn3-"+id);var open=sub.classList.toggle("open");btn.classList.toggle("open",open);}'

      + 'function applyPreset(i){var p=PRESETS[i];'
      + 'updateSwatches("BackgroundColor",p.bg);updateSwatches("OverlayBgColor",p.obg);'
      + 'updateSwatches("TimeTextColor",p.tt);updateSwatches("DateTextColor",p.dt);'
      + 'updateSwatches("LitHourColor",p.lH);updateSwatches("LitMinuteColor",p.lM);'
      + 'updateSwatches("LitBatteryColor",p.lB);updateSwatches("LitStepsColor",p.lS);'
      + 'updateSwatches("DimHourColor",p.dH);updateSwatches("DimMinuteColor",p.dM);'
      + 'updateSwatches("DimBatteryColor",p.dB);updateSwatches("DimStepsColor",p.dS);'
      + 'updateSwatches("LitHourTipColor",p.tH);updateSwatches("LitMinuteTipColor",p.tM);'
      + 'updateSwatches("InfoLine1Color",p.dt);updateSwatches("InfoLine2Color",p.dt);'
      + 'updateSwatches("InfoLine3Color",p.dt);updateSwatches("InfoLine4Color",p.dt);}'

      + 'function h(hex){return parseInt(hex.slice(1),16);}'
      + 'function tog(id){var el=document.getElementById(id);return el&&el.checked?1:0;}'
      + 'function sel(id){return parseInt(document.getElementById(id).value)||0;}'

      + 'function save(){'
      + 'var ov=parseInt(document.querySelector(\'input[name="overlay"]:checked\').value);'
      + 'var s={OverlayMode:ov,'
      + 'BackgroundColor:h(colors.BackgroundColor),OverlayBgColor:h(colors.OverlayBgColor),'
      + 'TimeTextColor:h(colors.TimeTextColor),DateTextColor:h(colors.DateTextColor),'
      + 'LitHourColor:h(colors.LitHourColor),LitMinuteColor:h(colors.LitMinuteColor),'
      + 'LitBatteryColor:h(colors.LitBatteryColor),LitStepsColor:h(colors.LitStepsColor),'
      + 'DimHourColor:h(colors.DimHourColor),DimMinuteColor:h(colors.DimMinuteColor),'
      + 'DimBatteryColor:h(colors.DimBatteryColor),DimStepsColor:h(colors.DimStepsColor),'
      + 'LitHourTipColor:h(colors.LitHourTipColor),LitMinuteTipColor:h(colors.LitMinuteTipColor),'
      + 'InfoLine1Color:h(colors.InfoLine1Color),InfoLine2Color:h(colors.InfoLine2Color),'
      + 'InfoLine3Color:h(colors.InfoLine3Color),InfoLine4Color:h(colors.InfoLine4Color),'
      + 'InvertBW:tog("InvertBW"),ShowRing:tog("ShowRing"),OverlaySize:tog("OverlaySize"),'
      + 'StepGoal:parseInt(document.getElementById("StepGoal").value),'
      + 'TopOuterField:sel("TopOuterField"),TopInnerField:sel("TopInnerField"),'
      + 'BottomInnerField:sel("BottomInnerField"),BottomOuterField:sel("BottomOuterField")'
      + '};'
      + 'window.location="pebblejs://close#"+encodeURIComponent(JSON.stringify(s));}'

      + 'loadSettings();initSwatches();applyPlatform();'
      + '</script></body></html>';
    return 'data:text/html,' + encodeURIComponent(html);
  }
};
