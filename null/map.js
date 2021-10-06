var map;
var directions;

/*
var vv = [["三内丸山遺跡", 40.811225, 140.695649], 
          ["東京大学", 35.712940, 139.759590], 
          ["京都リサーチパーク", 34.994707, 135.740076]];
*/

/*
var vv = [["札幌時計台",43.062603,141.353642], 
          ["青葉城跡",38.251127,140.855294], 
          ["国会正門前",35.676293,139.746927], 
          ["秋吉台",34.234753,131.310094], 
          ["海ほたる",35.464380,139.874407], 
          ["京都リサーチパーク",34.994707,135.740076]];
*/

var vv = [["東京駅", 35.681382, 139.766084],
          ["札幌時計台", 43.062603, 141.353642],
          ["首里城", 26.216991, 127.719362],
          ["偕楽園", 36.374693, 140.452653],
          ["熊本城", 32.804722, 130.707835],
          ["法隆寺", 34.614756, 135.734254],
          ["佐多岬", 30.994560, 130.660638],
          ["秋吉台", 34.234753, 131.310094],
          ["海ほたる", 35.464380, 139.874407],
          ["日本科学未来館", 35.619415, 139.776550]];

var BEGIN = 9;
var END = 10;

function initialize() {
  if (GBrowserIsCompatible()) {
    map = new GMap2(document.getElementById("map_canvas"));
    directions = new GDirections(map, document.getElementById("route"));
    GEvent.addListener(directions, "load", onGDirectionsLoad);
    directions.clear();
    var pointArray = [];
    
    for (var i = BEGIN; i < END; i++) {
      for (var j = 0; j < vv.length; j++) {
        pointArray.push(new GLatLng(vv[i][1], vv[i][2]), 
                        new GLatLng(vv[j][1], vv[j][2]));
      }
    }
    directions.loadFromWaypoints(pointArray, {locale: "ja_JP"});
  }
}

function onGDirectionsLoad(){ 
  var ot = directions.getDuration();
  var routeNum = directions.getNumRoutes();
  //GLog.write(routeNum);
  
  var c = 0;
  var html = "";
  for (var i = BEGIN; i < END; i++) {
    for (var j = 0; j < vv.length; j++) {
      var route = directions.getRoute(c);
      var ot = route.getDuration();
      var str = vv[i][0] + " " + vv[j][0] + " " + ot.seconds + "<br />";
      html += str;
      c += 2;
    }
  }
  
  document.getElementById("html").innerHTML = html;
}