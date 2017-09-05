var Clay = require('pebble-clay');
var clayConfig = require('./config');
var clay = new Clay(clayConfig);

var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function getTides() {
	var today = new Date();
	var todayDate = today.toISOString().substring(0,4)+today.toISOString().substring(5,7)+today.toISOString().substring(8,10);
	//todayDate = todayDate[0,9];
	var units = "english";
  var stationID = "9437540"; // Garibaldi, OR

  var url = 'https://tidesandcurrents.noaa.gov/api/datagetter?begin_date=' + todayDate + '&end_date=' + todayDate + '&station=' + stationID + '&product=predictions&datum=MLLW&units=' + units + '&interval=h&time_zone=lst_ldt&application=BrigandineWatch&format=json';

	//console.log("Date: " + todayDate);
	
	// Send request to NOAA
  	xhrRequest(url, 'GET', 
    	function(responseText) {
      	var json = JSON.parse(responseText);
				var i;
				
				var buffer = "";
				var buffer1 = "";
				var parsedInt = 0;
				for (i = 0; i < 24; i++) {
					parsedInt = parseInt(Math.round(json.predictions[i].v*10));
					if (parsedInt >= 10) {
						buffer = '' + parsedInt;
					}
					else if (parsedInt >= 0) {
						buffer = '0' + parsedInt;
					}
					else {
						buffer = '00';
					}
					//console.log("Input: " + json.predictions[i].v + " Output: " + buffer);
					buffer1 += buffer;
				}
				
				var dictionary_tides = {
					'TIDES' : buffer1
				};

			Pebble.sendAppMessage(dictionary_tides,
  			function(e) {
    			//console.log('Tide info from sent to Pebble successfully!');
  			},
  			function(e) {
    			console.log('Error sending tide info to Pebble!');
  			});
    });
}

function locationSuccess(pos) {
  // Construct URL
	var myAPIKey = "db0f8d947d39c6e0aacbd78159e1830d";
	var units = "imperial";
  var url = 'http://api.openweathermap.org/data/2.5/weather?lat=' + pos.coords.latitude + '&lon=' + pos.coords.longitude + '&units=' + units + '&appid=' + myAPIKey;
	
	console.log("Lat: " + pos.coords.latitude + " " + pos.coords.longitude);

  // Send request to OpenWeatherMap
  xhrRequest(url, 'GET', 
    function(responseText) {
      var json = JSON.parse(responseText);
			
			var dictionary_weather = {
				'LOCATION_NAME' : json.name,
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
		getTides();
  }
);