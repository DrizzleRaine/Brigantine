var Clay = require('pebble-clay');
var clayConfig = require('./config');
//var clay = new Clay(clayConfig, null, { autoHandleEvents: false });
var clay = new Clay(clayConfig);

var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function locationSuccess(pos) {
  // Construct URL
	var myAPIKey = "db0f8d947d39c6e0aacbd78159e1830d";
	var units = "imperial";
  var url = 'http://api.openweathermap.org/data/2.5/weather?lat=' + pos.coords.latitude + '&lon=' + pos.coords.longitude + '&units=' + units + '&appid=' + myAPIKey;

  // Send request to OpenWeatherMap
  xhrRequest(url, 'GET', 
    function(responseText) {
      var json = JSON.parse(responseText);
			
			var dictionary_weather = {
  			'TEMPERATURE': Math.round(json.main.temp),
  			'CONDITIONS': json.weather[0].main
			};

			Pebble.sendAppMessage(dictionary_weather,
  			function(e) {
    			//console.log('Weather info sent to Pebble successfully!');
  			},
  			function(e) {
    			console.log('Error sending weather info to Pebble!');
  			});
    });
}

function locationError(err) {
  console.log('Error requesting location!');
}

function getWeather() {
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {timeout: 15000, maximumAge: 60000}
  );
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    getWeather();
  }
);