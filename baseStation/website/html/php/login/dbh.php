<?php

$dbServername = "localhost";
$dbUsername = "login";
$dbPassword = "";
$dbName = "loginsystem";

mysqli_report(MYSQLI_REPORT_STRICT);

$conn = new mysqli($dbServername, $dbUsername, $dbPassword, $dbName);

if($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}