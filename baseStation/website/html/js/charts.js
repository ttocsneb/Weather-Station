//Temperature line chart
var tempElement = document.getElementById("temperature-chart");
var rainElement = document.getElementById("rain-chart");
var windElement = document.getElementById("wind-chart");

var tempChart = new Chart(tempElement, {
    type: 'line',
    data: {
        datasets: [
            {
                label: 'Humidity',
                borderColor: '#0099ff',
                pointBackgroundColor: '#0099ff',
                backgroundColor: 'rgba(0,153,255,0.3)',
                yAxisID: 'humidity',
                data: []
            },
            {
                label: 'Temperature',
                borderColor: '#ff1e00',
                pointBackgroundColor: '#ff1e00',
                backgroundColor: 'rgba(255,30,0,0.3)',
                
                yAxisID: 'temp',
                data: []
            }
            
        ]
    },
    options: {
        responsive: true,
        maintainAspectRatio: false,
        scales: {
            xAxes: [{
                type: 'time',
                time: {
                    unit: 'minute',
                    distribution: 'linear',
                    stepSize: 120
                },
                ticks: {
                    source: 'auto'
                }
            }],
            yAxes: [
                {
                    id: 'temp',
                    scaleLabel: {
                        display: true,
                        labelString: 'Temperature (F)'
                    },
                    type: 'linear',
                    position: 'left',
                    ticks: {
                        suggestedMin: 50,
                        suggestedMax: 70,
                        maxTicksLimit: 5
                    }
                },
                {
                    id: 'humidity',
                    scaleLabel: {
                        display: true,
                        labelString: 'Humidity (%)'
                    },
                    type: 'linear',
                    position: 'right',
                    ticks: {
                        suggestedMin: 20,
                        suggestedMax: 40,
                        maxTicksLimit: 5
                    },
                    gridLines: {
                        display: false
                    }
                }
            ]
        },
        elements: {
            point: {
                radius: 0,
                hitRadius: 5
            },
            line: {
                tension: 0.3
            }
        }
    }
});

var rainChart = new Chart(rainElement, {
    type: 'line',
    data: {
        datasets: [
            {
                label: 'Rainfall',
                borderColor: '#03b3ff',
                pointBackgroundColor: '#03b3ff',
                backgroundColor: 'rgba(3,179,255,0.3)',
                yAxisID: 'rain',
                data: []
            },
            {
                label: 'Pressure',
                borderColor: '#ffd503',
                pointBackgroundColor: '#ffd503',
                backgroundColor: 'rgba(255,213,3,0.3)',
                yAxisID: 'pres',
                data: []
            }
        ]
    },
    options: {
        responsive: true,
        maintainAspectRatio: false,
        scales: {
            xAxes: [{
                type: 'time',
                time: {
                    unit: 'minute',
                    distribution: 'linear',
                    stepSize: 120
                },
                ticks: {
                    source: 'auto'
                }
            }],
            yAxes: [
                {
                    id: 'pres',
                    scaleLabel: {
                        display: true,
                        labelString: 'Pressure (inHg)'
                    },
                    type: 'linear',
                    position: 'right',
                    ticks: {
                        suggestedMin: 29.6,
                        suggestedMax: 30.4,
                        maxTicksLimit: 5
                    }
                },
                {
                    id: 'rain',
                    scaleLabel: {
                        display: true,
                        labelString: 'Rainfall (in)'
                    },
                    type: 'linear',
                    position: 'left',
                    ticks: {
                        min: 0,
                        maxTicksLimit: 5
                    },
                    gridLines: {
                        display: false
                    }
                }
            ]
        },
        elements: {
            point: {
                radius: 0,
                hitRadius: 5
            },
            line: {
                tension: 0.3
            }
        }
    }
});

var windChart = new Chart(windElement, {
    type: 'line',
    data: {
        datasets: [
            {
                label: 'Wind',
                borderColor: '#14db02',
                pointBackgroundColor: '#14db02',
                backgroundColor: 'rgba(20,219,2,0.3)',
                yAxisID: 'wind',
                data: []
            },
            {
                label: 'Direction',
                borderColor: '#bb02cc',
                pointBackgroundColor: '#bb02cc',
                backgroundColor: 'rgba(187,2,204,0.3)',
                yAxisID: 'direction',
                data: []
            }
        ]
    },
    options: {
        responsive: true,
        maintainAspectRatio: false,
        scales: {
            xAxes: [{
                type: 'time',
                time: {
                    unit: 'minute',
                    distribution: 'linear',
                    stepSize: 120
                },
                ticks: {
                    source: 'auto'
                }
            }],
            yAxes: [
                {
                    id: 'wind',
                    scaleLabel: {
                        display: true,
                        labelString: 'Wind (mph)'
                    },
                    type: 'linear',
                    position: 'left',
                    ticks: {
                        maxTicksLimit: 5
                    }
                },
                {
                    id: 'direction',
                    scaleLabel: {
                        display: true,
                        labelString: 'Direction (deg)'
                    },
                    type: 'linear',
                    position: 'right',
                    ticks: {
                        min: 0,
                        max: 360,
                        maxTicksLimit: 5
                    },
                    gridLines: {
                        display: false
                    }
                }
            ]
        },
        elements: {
            point: {
                radius: 0,
                hitRadius: 5
            },
            line: {
                tension: 0.3
            }
        }
    }
});
var minRange = [
    30,//Hum
    40,//Temp
    0.1,//Rain
    1,//pres
    10//wind
];


function addRow(row) {
    
    //Humidity
    tempChart.data.datasets[0].data.push({
        x: row.date,
        y: row.humidity
    });

    console.log("temp: " + row.temperature);
    //Temperature
    tempChart.data.datasets[1].data.push({
        x: row.date,
        y: row.temperature
    });

    console.log("rain: " + row.rain);
    //Rain
    rainChart.data.datasets[0].data.push({
        x: row.date,
        y: row.rain_hour
    });

    //Pressure
    rainChart.data.datasets[1].data.push({
        x: row.date,
        y: row.pressure
    });

    //Wind
    windChart.data.datasets[0].data.push({
        x: row.date,
        y: row.wind_speed
    });

    //Direction
    windChart.data.datasets[1].data.push({
        x: row.date,
        y: row.wind_avg_dir
    });
}

function setRanges() {
    tempChart.update();
    rainChart.update();
    windChart.update();
}

function loadCharts() {
    
    $.get("/php/weather-status-full.php", function(data) {
        var obj = JSON.parse(data).data;

        for(var i = 0; i < obj.length; i++) {
            addRow(obj[i]);
        }

        setRanges();
    });
    
}

function updateChart() {
    $.get("/php/weather-status-chart.php", function(data) {
        var obj = JSON.parse(data);

        addRow(obj);

        setRanges();
    });
}

window.addEventListener("load",function(event) {
    loadCharts();
    setInterval(updateChart, 5 * 60*1000);
});