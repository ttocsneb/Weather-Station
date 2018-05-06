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
                    distribution: 'linear'
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
                        min: -20,
                        max: 120
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
                        min: 0,
                        max: 100
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
                    distribution: 'linear'
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
                        min: 27,
                        max: 33
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
                        max: 1.5
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
                    distribution: 'linear'
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
                        min: 0,
                        max: 50
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
                        max: 360
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



function loadWeather() {
    
    $.get("/php/weather-status-full.php", function(data) {
        var obj = JSON.parse(data).data;

        var temperature = [];
        var dewpoint = [];
        var humidity = [];


        var row = {};
        for(var i = 0; i < obj.length; i++) {
            
            row = obj[i];

            //Humidity
            tempChart.data.datasets[0].data.push({
                x: row.date,
                y: row.humidity
            });

            //Temperature
            tempChart.data.datasets[1].data.push({
                x: row.date,
                y: row.temperature
            });

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

        tempChart.update();
        rainChart.update();
        windChart.update();

    });
    
}

window.onload = function() {
    loadWeather();
};