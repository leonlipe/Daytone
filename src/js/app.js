/**
 * Clay config
 * 
 */

var Clay = require('pebble-clay');
var clayConfig = require('./config.json');
var clay = new Clay(clayConfig);


function fetchWeather(latitude, longitude) {
  console.log("Obten temperatura");
  var req = new XMLHttpRequest();
  req.open('GET', "http://api.wunderground.com/api/4f3cd4a8c1f8b357/conditions/q/"+latitude+","+longitude+".json", true);
  req.onload = function(e) {
    if (req.readyState == 4) {
      if(req.status == 200) {
       // console.log(req.responseText);

        var response = JSON.parse(req.responseText);
        var temperature = response.current_observation.temp_c;
        //var temperature = Math.round(response.main.temp - 273.15);
        //var icon = iconFromWeatherId(response.weather[0].id);
        //var city = response.name;
        //console.log(temperature);
        //console.log(icon);
        //console.log(city);

        Pebble.sendAppMessage({
          "WEATHER_TEMPERATURE_KEY":temperature
        }
        );

      } else {
        console.log("Error");
      }
    }
  }
  req.send(null);
  var req_fore = new XMLHttpRequest();
  req_fore.open('GET', "http://api.wunderground.com/api/4f3cd4a8c1f8b357/hourly/q/"+latitude+","+longitude+".json", true);
  req_fore.onload = function(e) {
    if (req_fore.readyState == 4) {
      if(req_fore.status == 200) {
       // console.log(req.responseText);

        var response_fore = JSON.parse(req_fore.responseText);
        var forecast = "";
        var temperature_fore = response_fore.hourly_forecast;
         
         for(x = 0; x<5; x++){
           if (forecast != ""){
             forecast = forecast + "|";
           }
           
           forecast = forecast + temperature_fore[x].temp.metric;
         }
        
        //var temperature = Math.round(response.main.temp - 273.15);
        //var icon = iconFromWeatherId(response.weather[0].id);
        //var city = response.name;
        //console.log(temperature);
        //console.log(icon);
        //console.log(city);

        Pebble.sendAppMessage({
          "WEATHER_TEMPERATURE_FORE_KEY":forecast
        }
        );

      } else {
        console.log("Error");
      }
    }
  }
  req_fore.send(null);
}

function locationSuccess(pos) {
  var coordinates = pos.coords;
  fetchWeather(coordinates.latitude, coordinates.longitude);
}

function locationError(err) {
  console.warn('location error (' + err.code + '): ' + err.message);
  Pebble.sendAppMessage({
    "WEATHER_TEMPERATURE_KEY":"..."
  });
}

var locationOptions = { "timeout": 15000, "maximumAge": 60000 }; 

Pebble.addEventListener("ready", function(e) {
  console.log("connect!" + e.ready);
  locationWatcher = window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
  console.log(e.type);
});

Pebble.addEventListener("appmessage", function(e) {
  window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
  console.log(e.type);
  console.log(e.payload.temperature);
  console.log("message!");
});

Pebble.addEventListener("webviewclosed", function(e) {
  console.log("webview closed");
  console.log(e.type);
  console.log(e.response);
});


