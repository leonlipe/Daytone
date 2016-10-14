/* === KIEZELPAY === SET TO false BEFORE RELEASING === */
var KIEZELPAY_LOGGING = false;
/* === KIEZELPAY === SET TO false BEFORE RELEASING === */

/* initiate KiezelPay functionality */
var KiezelPay = require('kiezelpay-core');
var kiezelpay = new KiezelPay(KIEZELPAY_LOGGING);


/**
 * Clay config
 * 
 */

var Clay = require('pebble-clay');
var clayConfig = require('./config.json');
var clay = new Clay(clayConfig);

/**
 * Library for weather requests
 * @type {[type]}
 */
// var OWMWeather = require('lib/owm_weather.js');
// var owmWeather = new OWMWeather();
var GenericWeather = require('pebble-generic-weather');
var genericWeather = new GenericWeather();


Pebble.addEventListener('ready', function(e) {
  console.log('PebbleKit JS ready!');
  var msg = "OK";
    var dictionary = {"READY": msg};
    Pebble.sendAppMessage(dictionary, function(e) {console.log("Ready sent to Pebble successfully!");}, function(e) {console.log('Error sending ready info to Pebble.');});
});


Pebble.addEventListener('appmessage', function(e) {
  genericWeather.appMessageHandler(e);
});