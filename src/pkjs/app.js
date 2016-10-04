var myAPIKey = '6d15f970ce4711aa49b77813973edf2f';
var temperature=0;
var lowTemp=0;
var highTemp=0;
var sunrise=0;
var sunset=0;
var conditions="";
var today="";
var returnMessage="";
var moreBlocks=true;
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
  var url = 'http://api.openweathermap.org/data/2.5/weather?lat=' +
      pos.coords.latitude + '&lon=' + pos.coords.longitude + '&appid=' + myAPIKey;

  // Send request to OpenWeatherMap
  xhrRequest(url, 'GET', 
    function(responseText) {
      // responseText contains a JSON object with weather info
      var json = JSON.parse(responseText);
			console.log(responseText);
      // Temperature in Kelvin requires adjustment
      temperature = Math.round((json.main.temp - 273.15)*1.8+32);
      console.log('Temperature is ' + temperature);
			lowTemp = Math.round((json.main.temp_min-273.15)*1.8+32);
			console.log('Low is '+lowTemp);
			highTemp = Math.round((json.main.temp_max-273.15)*1.8+32);
			console.log('High is '+highTemp);
		  sunrise=json.sys.sunrise;
			sunset=json.sys.sunset;
      // Conditions
      conditions = json.weather[0].description;      
      console.log('Conditions are ' + conditions);
      var dictionary = {
      'KEY_TEMPERATURE': temperature,
      'KEY_CONDITIONS': conditions,
			'KEY_LOW_TEMP': lowTemp,
			'KEY_HIGH_TEMP': highTemp,
				'KEY_SUNRISE': sunrise,
				'KEY_SUNSET': sunset
      };
      // Send to Pebble
      Pebble.sendAppMessage(dictionary,
        function(e) {
          console.log('Weather info sent to Pebble successfully!');
        },
        function(e) {
          console.log('Error sending weather info to Pebble!');
        }
      );
    }      
  );
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

function getSchedule(){
	var url = "https://api.myjson.com/bins/59bdk";
	xhrRequest(url, 'GET', 
    function(responseText) {
			var json = JSON.parse(responseText);
			var index=0;
			console.log("Getting Schedule");
			for(i=0;i<json.days.length;i++){
				console.log(json.days[i].cdate.substring(0, 10));
				if(json.days[i].cdate.substring(0, 10)==today){
					console.log("Found");
					index=i;
					break;
				}
			}
			if(json.days[index].special_identifier=="Exam"){
				returnMessage = "Exam";
			}else{
				returnMessage=returnMessage+json.days[index].blocks[0].block_number.toString();
				returnMessage=returnMessage+json.days[index].blocks[0].start_time.substring(11, 16);
				returnMessage=returnMessage+json.days[index].blocks[0].end_time.substring(11, 16);
				for(i=1;i<6;i++){
					try{
						returnMessage=returnMessage+json.days[index].blocks[i].block_number.toString();
						returnMessage=returnMessage+json.days[index].blocks[i].start_time.substring(11, 16);
						returnMessage=returnMessage+json.days[index].blocks[i].end_time.substring(11, 16);
					}catch(error){
	
					}
				}
			}
			if(index==0){
				returnMessage="No School";
			}
			console.log("returnMessage:");
			console.log(returnMessage);
			var dictionary = {
      'KEY_TEMPERATURE': returnMessage
      };
      // Send to Pebble
      Pebble.sendAppMessage(dictionary,
        function(e) {
          console.log('Schedule info sent to Pebble successfully!');
        },
        function(e) {
          console.log('Error sending schedule info to Pebble!');
        }
      );
			returnMessage="";
			moreBlocks=true;
	});
}

Pebble.addEventListener('ready', 
  function(e) {
    console.log('PebbleKit JS ready!');
    getWeather();
		//getSchedule();
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
		if(e.payload.KEY_TEMPERATURE=="weather"){
			getWeather();
		}else{
			today=e.payload.KEY_TEMPERATURE;
			console.log("Today is:");
			console.log(today);
			getSchedule();
		}
  }                     
);