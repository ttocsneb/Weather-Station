var reporting = document.getElementById("station-reporting");
var notReporting = document.getElementById("station-not-reporting");
var battery = document.getElementById("station-battery");
var isCharging = document.getElementById("station-isCharging");
var chargeTime = document.getElementById("station-chargeTime");
var lostPackets = document.getElementById("station-lostPackets");
var resets = document.getElementById("station-resets");
var updateTime = document.getElementById("station-update-time");
var uptime = document.getElementById("station-uptime");

var base_uptime = document.getElementById("base-uptime");
var base_resets = document.getElementById("base-resets");

function getTime(seconds) {
    var str = "";

    const minute = 60;
    const hour = minute * 60;
    const day = hour * 24;
    const year = day * 365;

    if(seconds > year) {
        var y = Math.floor(seconds / year)
        str += y;
        str += (y == 1 ? " year " : " years ");
    }

    if(seconds > day) {
        var d = Math.floor(seconds / day) % (year / day);
        str += y;
        str += (d == 1 ? " day " : " days ");
    }

    if(seconds > hour) {
        var h = Math.floor(seconds / hour) % (day / hour);
        str += h;
        str += (h == 1 ? " hour " : " hours ");
    }

    if(seconds > minute) {
        var m = Math.floor(seconds / minute) % (hour / minute);
        str += m;
        str += (m == 1 ? " minute " : " minutes ");
    }

    var s = seconds % minute;
    str += s;
    str += (s == 1 ? " second" : " seconds");

    return str;
}

var uptime_seconds = 0;
var base_uptime_seconds = 0;
var uptime_orig = 0;
var base_uptime_orig = 0;

function setTime() {
    uptime.textContent = getTime(uptime_seconds);
    base_uptime.textContent = getTime(base_uptime_seconds);

    uptime_seconds++;
    base_uptime_seconds++;
}

function updateStatus() {
    $.get("/php/status.php", function(data) {
        var obj = JSON.parse(data);
        
        if(obj.uptime != uptime_orig || obj.base_uptime != base_uptime_orig) {
            
            base_uptime_orig = obj.base_uptime;
            uptime_orig = obj.uptime;

            uptime_seconds = obj.uptime;
            base_uptime_seconds = obj.base_uptime;
            
            setTime();
        }

        if(obj.reporting) {
            notReporting.style.display='none';
            reporting.style.display='block';
        } else {
            reporting.style.display='none';
            notReporting.style.display='block';
        }


        updateTime.textContent = obj.date;
        battery.textContent = Number(obj.battery).toFixed(2);
        isCharging.textContent = obj.is_charging ? "Yes" : "No";
        chargeTime.textContent = getTime(obj.time_charging);
        lostPackets.textContent = obj.lost_packets;
        resets.textContent = obj.resets;
        base_resets.textContent = obj.base_resets;


    });
}

window.addEventListener("load", function(event) {
    setInterval(updateStatus, 30*1000);
    updateStatus();
    setInterval(setTime, 1000);
});