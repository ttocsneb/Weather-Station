//Temperature line chart
var tempElement = document.getElementById("temperature-chart");

//Chart.scaleService.registerScaleType('tempScale', tempScale, defaultConfigObject);

var tempChart = new Chart(tempElement, {
    type: 'line',
    data: {
        datasets: [
            {
                label: 'Temperature (F)',
                borderColor: '#ff2200',
                fill: false,
                yAxisID: 'temp',
                data: [
                    {
                        x: "2018-05-02 18:15:27",
                        y: 5
                    },
                    {
                        x: "2018-05-02 18:15:57",
                        y: 2
                    },
                    {
                        x: "2018-05-02 18:16:27",
                        y: 4
                    },
                    {
                        x: "2018-05-02 18:16:57",
                        y: 3
                    }
                ]
            },
            {
                label: 'Dewpoint (F)',
                borderColor: '#ff8c00',
                fill: false,
                yAxisID: 'temp',
                data: [
                    {
                        x: "2018-05-02 18:15:27",
                        y: 6
                    },
                    {
                        x: "2018-05-02 18:15:57",
                        y: 3
                    },
                    {
                        x: "2018-05-02 18:16:27",
                        y: 2
                    },
                    {
                        x: "2018-05-02 18:16:57",
                        y: 3
                    }]
            },
            {
                label: 'Humidity',
                borderColor: '#ddff00',
                fill: false,
                yAxisID: 'humidity',
                data: [
                    
                    {
                        x: "2018-05-02 18:15:27",
                        y: 3
                    },
                    {
                        x: "2018-05-02 18:15:57",
                        y: 25
                    },
                    {
                        x: "2018-05-02 18:16:27",
                        y: 75
                    },
                    {
                        x: "2018-05-02 18:16:57",
                        y: 100
                    }]
            }
        ]
    },
    options: {
        scales: {
            xAxes: [{
                type: 'time',
                time: {
                    unit: 'minute',
                    distribution: 'linear',
                    //parser: "YYYY-MM-DD HH:mm:ss"
                },
                ticks: {
                    source: 'auto'
                }
            }],
            yAxes: [
                {
                    id: 'temp',
                    type: 'linear',
                    position: 'left'
                },
                {
                    id: 'humidity',
                    type: 'linear',
                    position: 'right'
                }
            ]
        }
    }
});


/**
var myLineChart = new Chart(temperatureChart, {
    type: 'line',
    data: {
        labels: ["January", "February", "March", "April", "May", "June", "July"],
        datasets: [
            {
                label: "Temperature (F)",
                fillColor: "rgba(255,61,27,0.2)",
                strokeColor: "#ff3511",
                pointColor: "#a1240e",
                data: [65, 59, 80, 81, 56, 55, 40]
            }
        ]
    },
    options: {
        responsiv: true
    }
});*/