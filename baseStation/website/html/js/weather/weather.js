var time = document.getElementById("weather-update-time");
var temp = document.getElementById("weather-temp");
var dew = document.getElementById("weather-dew");
var hum = document.getElementById("weather-hum");
var pres = document.getElementById("weather-pres");
var rain_hour = document.getElementById("weather-rain-hour");
var rain_day = document.getElementById("weather-rain-day");
var wind = document.getElementById("weather-wind");
var wind_dir = document.getElementById("weather-wind-dir");
var gust = document.getElementById("weather-gust");
var gust_dir = document.getElementById("weather-gust-dir");
var avg = document.getElementById("weather-avg");
var avg_dir = document.getElementById("weather-avg-dir");

function degToDir(deg) {
    var step = 22.5;
    if(deg < step) {
        return "North";
    } else if(deg < step*3) {
        return "North-East";
    } else if(deg < step*5) {
        return "East";
    } else if(deg < step*7) {
        return "South-East";
    } else if(deg < step*9) {
        return "South";
    } else if(deg < step*11) {
        return "South-West";
    } else if(deg < step*13) {
        return "West";
    } else if(deg < step*15) {
        return "North-West";
    } else {
        return "North";
    }
}

function updateWeather() {
    $.get("/php/weather/weather-status.php", function(data) {
        var obj = JSON.parse(data);

        time.textContent = obj.date;
        temp.textContent = obj.temperature;
        dew.textContent = obj.dewpoint;
        hum.textContent = obj.humidity;
        pres.textContent = obj.pressure;
        rain_hour.textContent = obj.rain_hour;
        rain_day.textContent = obj.rain_day;
        wind.textContent = obj.wind_speed;
        wind_dir.textContent = degToDir(obj.wind_dir);
        gust.textContent = obj.wind_gust;
        gust_dir.textContent = degToDir(obj.wind_gust_dir);
        avg.textContent = obj.wind_avg;
        avg_dir.textContent = degToDir(obj.wind_avg_dir);
    });
}

window.addEventListener("load",function(event) {
    setInterval(updateWeather, 30*1000);
    updateWeather();
});