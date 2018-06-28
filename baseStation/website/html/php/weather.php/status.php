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

class Status {
    public $date = "";
    public $battery = 0.0;
    public $is_charging = false;
    public $time_charging = 0;
    public $uptime = 0;
    public $resets = 0;
    public $lost_packets = 0;
    public $reporting = false;
    public $base_uptime = 0;
    public $base_resets = 0;
}

$data = new Status();

$qry = "SELECT * FROM status";
$result = $conn->query($qry);

if($result->num_rows > 0) {
    $row = $result->fetch_assoc();
    $data->date = $row["date"];
    $data->battery = $row["battery"];
    $data->is_charging = (bool)$row["is_charging"];
    $data->time_charging = $row["time_charging"];
    $data->uptime = $row["uptime"];
    $data->resets = $row["resets"];
    $data->lost_packets = $row["lost_packets"];
    $data->reporting = (bool)$row["reporting"];
    $data->base_uptime = $row["base_uptime"];
    $data->base_resets = $row["base_resets"];
    
    #Calculate the string to display for the last updated time in the format
    # [today/yesterday/X days ago], at h:m [am:pm]
    $then = new DateTime($row["date"]);
    $now = new DateTime();
    $updateTime = $now->diff($then);

    if($updateTime->d < 1) {
        $data->date = "today";
    } else if($updateTime->d < 2) {
        $data->date = "yesterday";
    } else {
        $data->date = $updateTime->d . " days ago";
    }

    $data->date = $data->date . " at " . $then->format('h:i:s a');
}

$jsonStatus = json_encode($data);

echo $jsonStatus;

$conn->close();

?>