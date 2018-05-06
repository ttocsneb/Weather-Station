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
                        min: -20,
                        max: 120,
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
                        min: 0,
                        max: 100,
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
                        min: 27,
                        max: 33,
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
                        max: 1.5,
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
                        min: 0,
                        max: 50,
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

function loadCharts() {
    
    $.get("/php/weather-status-full.php", function(data) {
        var obj = JSON.parse(data).data;

        var hum = 0;
        var temp = 1;
        var rain = 2;
        var pres = 3;
        var wind = 4;

        var min = [999, 999, 999, 999, 999];
        var max = [-999, -999, -999, -999, -999];
        var minRange = [
            30,//Hum
            40,//Temp
            0.1,//Rain
            1,//pres
            10//wind
        ];
        var minZero = [
            true,//Hum
            false,//Temp
            true,//Rain
            true,//Pres
            true,//Wind
        ];

        var row = {};
        for(var i = 0; i < obj.length; i++) {
            
            row = obj[i];

            //Humidity
            min[hum] = Math.min(row.humidity, min[hum]);
            max[hum] = Math.max(row.humidity, max[hum]);
            tempChart.data.datasets[0].data.push({
                x: row.date,
                y: row.humidity
            });

            //Temperature
            min[temp] = Math.min(row.temperature, min[temp]);
            max[temp] = Math.max(row.temperature, max[temp]);
            tempChart.data.datasets[1].data.push({
                x: row.date,
                y: row.temperature
            });

            //Rain
            min[rain] = Math.min(row.rain_hour, min[rain]);
            max[rain] = Math.max(row.rain_hour, max[rain]);
            rainChart.data.datasets[0].data.push({
                x: row.date,
                y: row.rain_hour
            });

            //Pressure
            min[pres] = Math.min(row.pressure, min[pres]);
            max[pres] = Math.max(row.pressure, max[pres]);
            rainChart.data.datasets[1].data.push({
                x: row.date,
                y: row.pressure
            });

            //Wind
            min[wind] = Math.min(row.wind_speed, min[wind]);
            max[wind] = Math.max(row.wind_speed, max[wind]);
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

        //Calculate the mins and maxes
        for(var i=0; i < min.length; i++) {
            var range = Number(max[i]) - Number(min[i]);
            if(range < minRange[i]) {
                var addedRange = Math.abs((range - minRange[i]) / 2.0);
                min[i] = Number(min[i]) - Number(addedRange);
                max[i] = Number(max[i]) + Number(addedRange);
            } else {
                min[i] = Number(min[i]) - range * 0.1;
                max[i] = Number(max[i]) + range * 0.1;
            }

            if(minZero[i] && min[i] < 0) {
                var diff = Math.abs(min[i]);
                min[i] = 0;
                max[i] += diff;
            }
            min[i] = Math.round(min[i] * 10) / 10;
            max[i] = Math.round(max[i] * 10) / 10;

            console.log(i + ": min: " + min[i] + " max: " + max[i] + " range: " + range);
        }

        //Set the mins and maxes

        //Temp
        tempChart.options.scales.yAxes[0].ticks.min = min[temp];
        tempChart.options.scales.yAxes[0].ticks.max = max[temp];
        
        //Humidity
        tempChart.options.scales.yAxes[1].ticks.min = min[hum];
        tempChart.options.scales.yAxes[1].ticks.max = max[hum];

        
        //Pres
        rainChart.options.scales.yAxes[0].ticks.min = min[pres];
        rainChart.options.scales.yAxes[0].ticks.max = max[pres];

        //Rain
        rainChart.options.scales.yAxes[1].ticks.min = min[rain];
        rainChart.options.scales.yAxes[1].ticks.max = max[rain];

        
        //Wind
        windChart.options.scales.yAxes[0].ticks.min = min[wind];
        windChart.options.scales.yAxes[0].ticks.max = max[wind];
        

        tempChart.update();
        rainChart.update();
        windChart.update();

    });
    
}
 

window.addEventListener("load",function(event) {
    loadCharts();
});