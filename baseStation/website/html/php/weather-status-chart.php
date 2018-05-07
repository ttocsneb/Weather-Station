<?php
$servername = "localhost";
$username = "web";
$password = "";
$dbname = "weather";

mysqli_report(MYSQLI_REPORT_STRICT);

$conn = new mysqli($servername, $username, $password, $dbname);

if($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

class Data {
    public $date = "";
    public $temperature = 0.0;
    public $dewpoint = 0.0;
    public $humidity = 0.0;
    public $rain_hour = 0.0;
    public $rain_day = 0.0;
    public $pressure = 0.0;
    public $wind_speed = 0.0;
    public $wind_dir = 0.0;
    public $wind_gust = 0.0;
    public $wind_gust_dir = 0.0;
    public $wind_avg = 0.0;
    public $wind_avg_dir = 0.0;
}

$data = new Data();

$qry = "SELECT * FROM graphdata WHERE date = (SELECT MAX(date) FROM graphdata)";
$result = $conn->query($qry);

if($result->num_rows > 0) {
    $row = $result->fetch_assoc();
    $data->date = $row["date"];
    $data->temperature = $row["temperature"];
    $data->dewpoint = $row["dewpoint"];
    $data->humidity = $row["humidity"];
    $data->rain_hour = $row["rain_hour"];
    $data->rain_day = $row["rain_day"];
    $data->pressure = $row["pressure"];
    $data->wind_speed = $row["wind_speed"];
    $data->wind_dir = $row["wind_dir"];
    $data->wind_gust = $row["wind_gust"];
    $data->wind_gust_dir = $row["wind_gust_dir"];
    $data->wind_avg = $row["wind_avg"];
    $data->wind_avg_dir = $row["wind_avg_dir"];
}

$jsonWeather = json_encode($data);

echo $jsonWeather;

$conn->close();

?>