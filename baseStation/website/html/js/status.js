var reporting = document.getElementById("station-reporting");
var notReporting = document.getElementById("station-not-reporting");
var battery = document.getElementById("station-battery");
var isCharging = document.getElementById("station-isCharging");
var chargeTime = document.getElementById("station-chargeTime");
var lostPackets = document.getElementById("station-lostPackets");
var resets = document.getElementById("station-resets");
var updateTime = document.getElementById("station-update-time");

function updateStatus() {
    $.get("/php/status.php", function(data) {
        var obj = JSON.parse(data);

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
        chargeTime.textContent = obj.time_charging;
        lostPackets.textContent = obj.lost_packets;
        resets.textContent = obj.resets;


    });
}

window.addEventListener("load", function(event) {
    setInterval(updateStatus, 30*1000);
    updateStatus();
});