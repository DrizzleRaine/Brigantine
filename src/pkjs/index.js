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

function getTides() {
 	// Construct URL
	var todayDate = "20170830";
	var endDate = "20170831";
	//var todayDate = moment().format('YYYYMMDD');
	//var endDate = moment()+1.format('YYYMMDD');
	var units = "english";
  var stationID = "9437540"; // Garibaldi, OR

  var url = 'https://tidesandcurrents.noaa.gov/api/datagetter?begin_date=' + todayDate + '&end_date=' + endDate + '&station=' + stationID + '&product=predictions&datum=MLLW&units=' + units + '&interval=h&time_zone=lst_ldt&application=BrigandineWatch&format=json';

	// Send request to NOAA
  	xhrRequest(url, 'GET', 
    	function(responseText) {
      	var json = JSON.parse(responseText);
			
				var dictionary_tides = {
					'TIME' : json.predictions[0].t,
					'TIDE' : json.predictions[0].v
			};
								
			console.log("Tide time: " + dictionary_tides.TIME + " Tide height: " + dictionary_tides.TIDE);

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
	

  // Send request to OpenWeatherMap
  xhrRequest(url, 'GET', 
    function(responseText) {
      var json = JSON.parse(responseText);
			//console.log(json.name);
			
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